## Process this file with automake to produce Makefile.in

INCLUDES = \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	@PACKAGE_CFLAGS@

bin_PROGRAMS = walltalk

walltalk_SOURCES = \
	main.c main.h \
	support.c support.h \
	interface.c interface.h \
	callbacks.c callbacks.h \
	gtkmeter.c gtkmeter.h \
	gtkmeterscale.c gtkmeterscale.h \
	meters.c meters.h \
	threads.c threads.h \
	usb_manage.c usb_manage.h


walltalk_LDADD = @PACKAGE_LIBS@
