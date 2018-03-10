/*
 *  Copyright (C) 2018 Mike Scheuerell
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  $Id: main.c,v 1.7 2005/01/22 17:17:39 swh Exp $
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <jack/jack.h>
#include <getopt.h>
#include <sndfile.h>
#include <gtk/gtk.h>


#include "threads.h"
#include "interface.h"
#include "meters.h"
#include "support.h"
#include "main.h"
#include <wiringPi.h>
#include "usb_manage.h"
#include "pbrx/rfpbrx_api.h"

#define DEBUG(lvl, txt...) \
    if (verbosity >= lvl) fprintf(stderr, PACKAGE ": " txt)

const static int verbosity = 0;

gboolean idle_cb(gpointer data);
void cleanup();

GtkWidget *main_window;

int num_ports = DEFAULT_NUM_PORTS;
unsigned int buf_length = DEFAULT_BUF_LENGTH; /* in seconds */

char *client_name = DEFAULT_CLIENT_NAME;
char *prefix = DEFAULT_PREFIX;
char *format_name = DEFAULT_FORMAT;
int format_sf = 0;
int safe_filename = 0;
int auto_record = 0;
float auto_begin_threshold = 0.0;
float auto_end_threshold = 0.0;
unsigned int auto_end_time = DEFAULT_AUTO_END_TIME;
char dev_path[100];

jack_port_t *ports[MAX_PORTS];
jack_client_t *client;

GdkPixbuf *img_on, *img_off, *img_busy;
GdkPixbuf *icon_on, *icon_off;


int main(int argc, char *argv[])
{
    unsigned int i;
    int opt;
    int help = 0;
    //int console = 0;
    char port_name[32];
    pthread_t dt;
    pthread_t bt;

    auto_begin_threshold = db2lin(DEFAULT_AUTO_BEGIN_THRESHOLD);
    auto_end_threshold = db2lin(DEFAULT_AUTO_END_THRESHOLD);

    while ((opt = getopt(argc, argv, "hic:t:n:p:f:sab:e:T:")) != -1) {
	switch (opt) {
	case 'h':
	    help = 1;
	    break;
	/*
	case 'i':
	    console = 1;
	    break;
	*/
	case 'c':
	    num_ports = atoi(optarg);
	    DEBUG(1, "ports: %d\n", num_ports);
	    break;
	case 't':
	    buf_length = atoi(optarg);
	    DEBUG(1, "buffer: %ds\n", buf_length);
	    break;
	case 'n':
	    client_name = optarg;
	    DEBUG(1, "client name: %s\n", client_name);
	    break;
	case 'p':
	    prefix = optarg;
	    DEBUG(1, "prefix: %s\n", prefix);
	    break;
	case 'f':
	    format_name = optarg;
	    break;
	case 's':
	    safe_filename = 1;
	    break;
	case 'a':
	    auto_record = 1;
	    break;
	case 'b':
	    auto_begin_threshold = db2lin(atof(optarg));
	    break;
	case 'e':
	    auto_end_threshold = db2lin(atof(optarg));
	    break;
	case 'T':
	    auto_end_time = atoi(optarg);
	    break;
	default:
	    num_ports = 0;
	    break;
	}
    }

    if (optind != argc) {
	num_ports = argc - optind;
    }

    if (num_ports < 1 || num_ports > MAX_PORTS || help) {
	fprintf(stderr, "Usage %s: [-h] [-i] [-c channels] [-n jack-name]\n\t"
			"[-t buffer-length] [-p file prefix] [-f format]\n\t"
			"[-a] [-b begin-threshold] [-e end-threshold] [-T end-time]\n\t"
			"[port-name ...]\n\n", argv[0]);
	fprintf(stderr, "\t-h\tshow this help\n");
	fprintf(stderr, "\t-i\tinteractive mode (console instead of X11) also enabled\n\t\tif DISPLAY is unset\n");
	fprintf(stderr, "\t-c\tspecify number of recording channels\n");
	fprintf(stderr, "\t-n\tspecify the JACK name walltalk will use\n");
	fprintf(stderr, "\t-t\tspecify the pre-recording buffer length\n");
	fprintf(stderr, "\t-p\tspecify the saved file prefix, may include path\n");
	fprintf(stderr, "\t-s\tuse safer characters in filename (windows compatibility)\n");
	fprintf(stderr, "\t-f\tspecify the saved file format\n");
	fprintf(stderr, "\t-a\tenable automatic sound-triggered recording\n");
	fprintf(stderr, "\t-b\tspecify threshold above which automatic recording will begin\n");
	fprintf(stderr, "\t-e\tspecify threshold below which automatic recording will end\n");
	fprintf(stderr, "\t-T\tspecify silence length before automatic recording ends\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "\tchannels must be in the range 1-8, default %d\n",
			DEFAULT_NUM_PORTS);
	fprintf(stderr, "\tjack-name, default \"%s\"\n", DEFAULT_CLIENT_NAME);
	fprintf(stderr, "\tfile-prefix, default \"%s\"\n", DEFAULT_PREFIX);
	fprintf(stderr, "\tbuffer-length, default %d secs\n", DEFAULT_BUF_LENGTH);
	fprintf(stderr, "\tformat, default '%s', options: wav, w64\n", DEFAULT_FORMAT);
	fprintf(stderr, "\tbegin-threshold, default %.1f dB\n", DEFAULT_AUTO_BEGIN_THRESHOLD);
	fprintf(stderr, "\tend-threshold, default %.1f dB\n", DEFAULT_AUTO_END_THRESHOLD);
	fprintf(stderr, "\tend-time, default %d secs\n", DEFAULT_AUTO_END_TIME);
	fprintf(stderr, "\n");
	fprintf(stderr, "specifying port names to connect to on the command line overrides -c\n\n");
	exit(1);
    }

    if (!strcasecmp(format_name, "wav")) {
	format_sf = SF_FORMAT_WAV | SF_FORMAT_FLOAT;
    }
#ifdef HAVE_W64
    if (!strcasecmp(format_name, "w64")) {
	format_sf = SF_FORMAT_W64 | SF_FORMAT_FLOAT;
    }
#endif

    if (format_sf == 0) {
	fprintf(stderr, "Unknown format '%s'\n", format_name);
    }

    /* Register with jack */
    if ((client = jack_client_open(client_name, 0, NULL)) == 0) {
	DEBUG(0, "jack server not running?\n");
	exit(1);
    }
    DEBUG(1, "registering as %s\n", client_name);

    process_init(buf_length);

    jack_set_process_callback(client, process, 0);

    if (jack_activate(client)) {
	DEBUG(0, "cannot activate JACK client");
	exit(1);
    }

    /* Create the jack ports */
    for (i = 0; i < num_ports; i++) {
	jack_port_t *port;

	snprintf(port_name, 31, "in_%d", i + 1);
	ports[i] = jack_port_register(client, port_name,
				      JACK_DEFAULT_AUDIO_TYPE,
				      JackPortIsInput, 0);
	if (optind != argc) {
	    port = jack_port_by_name(client, argv[optind+i]);
	    if (port == NULL) {
		fprintf(stderr, "Can't find port '%s'\n", port_name);
		continue;
	    }
	    if (jack_connect(client, argv[optind+i], jack_port_name(ports[i]))) {
		fprintf(stderr, "Cannot connect port '%s' to '%s'\n",
			argv[optind+i], jack_port_name(ports[i]));
	    }
	}
    }

    /* Start the disk thread */
    pthread_create(&dt, NULL, (void *)&writer_thread, NULL);
    
    /* mjs setup GPIO */
    wiringPiSetup ();
	pinMode (LED, OUTPUT);
	pinMode (PB, INPUT);
	pullUpDnControl(PB, PUD_DOWN);
	digitalWrite (LED, 1) ; 
	//out = wiringPiISR(PB, INT_EDGE_RISING);
	
	// set timezone
	system("TZ='America/Menominee'; export TZ");
	
	/* mjs setup RF pusshbutton*/
	if ( rfpb_init () == 0) 
	{
		printf("init failed\n");
		exit(0);
	} 
	
	/* find the first USB flash drive, unmount it, then re-mount to /media/pi/USB_DISK */
	usb_removable_prep(dev_path);			
    
    /* mjs start the blink thread */
    pthread_create(&bt, NULL, (void *)&blink_thread, NULL);

	// wait indefinately if display is not detected, otherwise start the interface
	if (!getenv("DISPLAY") || getenv("DISPLAY")[0] == '\0') {
		pause();
	}
	else {
		// start graphical display
		gtk_init(&argc, &argv);
		add_pixmap_directory(PACKAGE_DATA_DIR "/" PACKAGE "/pixmaps");
		add_pixmap_directory("pixmaps");
		add_pixmap_directory("../pixmaps");
		img_on = create_pixbuf("on.png");
		img_off = create_pixbuf("off.png");
		img_busy = create_pixbuf("busy.png");
		icon_on = create_pixbuf("on-icon.png");
		icon_off = create_pixbuf("off-icon.png");
		main_window = create_window(client_name);
		gtk_window_set_icon(GTK_WINDOW(main_window), icon_off);
		gtk_widget_show(main_window);
		bind_meters();
		g_timeout_add(100, meter_tick, NULL);
		gtk_main();
	}

    cleanup();

    /* We can't ever get here, but it keeps gcc quiet */
    return 0;
}

void cleanup()
{
    /* Leave the jack graph */
    jack_client_close(client);

    recording_quit();

    while(!recording_done) {
	usleep(1000);
    }

    DEBUG(0, "exiting\n");
    fflush(stderr);

    /* And were done */
    exit(0);
}

/* vi:set ts=8 sts=4 sw=4: */
