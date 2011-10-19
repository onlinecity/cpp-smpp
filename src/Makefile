VERSION=0.1
MAJOR_VERSION=1
MINOR_VERSION=0

CC=g++
OBJ=sms.o timeformat.o pdu.o smpp.o smppclient.o 
PROGRAM=smpp

LIB_NAME=lib$(PROGRAM).so
LIB_MAJOR_NAME=$(LIB_NAME).$(MAJOR_VERSION)
LIB_MINOR_NAME=$(LIB_MAJOR_NAME).$(MINOR_VERSION)

OPT?=2
CFLAGS=-fPIC -c -Wall -Weffc++ -I/usr/local/include -O${OPT}
LDFLAGS=-L/usr/local/lib -lboost_system -lboost_regex -lboost_date_time
CXXFLAGS=${CFLAGS}

INSTALL=cp -a
PREFIX=/usr/local
INCLUDE_PATH?=include/$(PROGRAM)
LIBRARY_PATH?=lib
INSTALL_INCLUDE_PATH= $(PREFIX)/$(INCLUDE_PATH)
INSTALL_LIBRARY_PATH=$(PREFIX)/$(LIBRARY_PATH)

all: $(OBJ) lib 
sms.o: sms.cpp sms.h smpp.h pdu.h tlv.h
pdu.o: pdu.cpp pdu.h smpp.h tlv.h
timeformat.o: timeformat.cpp timeformat.h exceptions.h
smpp.o: smpp.cpp smpp.h
smppclient.o: smppclient.cpp smpp.h smppclient.h tlv.h pdu.h sms.h exceptions.h timeformat.h

lib: $(OBJ)	
	gcc -shared -Wl,-soname,$(LIB_MINOR_NAME) -o $(LIB_NAME) $(OBJ) $(LDFLAGS)

clean:
	rm -rf $(OBJ) $(LIB_NAME) *.o 

install:
	mkdir -p $(INSTALL_INCLUDE_PATH) $(INSTALL_LIBRARY_PATH) 
	$(INSTALL) *.h $(INSTALL_INCLUDE_PATH)
	$(INSTALL) $(LIB_NAME) $(INSTALL_LIBRARY_PATH)/$(LIB_MINOR_NAME)
	cd $(INSTALL_LIBRARY_PATH) && ln -sf $(LIB_MINOR_NAME) $(LIB_MAJOR_NAME)
	cd $(INSTALL_LIBRARY_PATH) && ln -sf $(LIB_MAJOR_NAME) $(LIB_NAME)

deinstall:
	rm -r $(INSTALL_INCLUDE_PATH)
	rm $(INSTALL_LIBRARY_PATH)/$(LIB_MINOR_NAME)
	rm $(INSTALL_LIBRARY_PATH)/$(LIB_MAJOR_NAME)
	rm $(INSTALL_LIBRARY_PATH)/$(LIB_NAME)
	

dep:
	$(CC) -MM *.cpp
