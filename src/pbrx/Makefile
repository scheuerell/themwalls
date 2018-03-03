
# -DRPI Defines the RPI variable which is needed by RCSwitch.h
CXXFLAGS=-Wall -DRPI
	
all: librfpbrx_api.so

librfpbrx_api.so: RCSwitch.o rfpbrx_api.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -shared -fPIC $+ -o $@ -lwiringPi

print_vars:
	echo $(CXX); echo $(CXXFLAGS); echo $(LDFLAGS); 	

clean:
	$(RM) *.a *.o *.so rfpbrx

install:
	cp -p -f librfpbrx_api.so /usr/lib