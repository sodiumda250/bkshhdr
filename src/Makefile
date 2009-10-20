#!/usr/bin/make -f
# $Id: Makefile,v 1.54 2005/05/30 02:53:45 woods Exp $

MAJOR = 0
MINOR = 9
TEENY = 1

COPYRIGHT = '"Copyright (C) 2000-2005 woods"'

RELEASE_VERSION = $(MAJOR).$(MINOR).$(TEENY)

TARGET = bkshhdr.dll
OBJS = bkshhdr.$(OBJ) PluginSDK/BeckyAPI.$(OBJ) charbuf.$(OBJ) hdrwin.$(OBJ) \
	dialog.$(OBJ) hdrlist.$(OBJ) hookproc.$(OBJ)
RESOBJS = bkshhdr_res.$(OBJ)
LIBS = -lstdc++
SRCS = $(OBJS:.$(OBJ)=.cpp)
HDRS = bkshhdr.h charbuf.h dialog.h hdrlist.h hdrwin.h hookproc.h resource.h

# Suffix
OBJ = o
#OBJ= obj # for bcc/cl(?)

# Commands
MAKE = make
CC = gcc
LD = gcc
RM = rm -f
CP = cp -f
STRIP = strip
MKDIR = mkdir -p
ZIP = zip
LHA = lha
TOUCH = touch
FIND = find
INSTALL = install

# Compile and Link option
CFLAGS = -g -O -Wall -pipe -mno-cygwin -I/usr/inlcude/w32api \
 -DRELEASE_VERSION=\"$(RELEASE_VERSION)\" \
 -DMAJOR=$(MAJOR) -DMINOR=$(MINOR) -DTEENY=$(TEENY) \
 -DCOPYRIGHT=$(COPYRIGHT)
LDFLAGS = -pipe -Wl,--enable-stdcall-fixup -mno-cygwin -mwindows -mingw \
 -mdll -mshared \
 -L/usr/lib/w32api

INSTALLPATH = /cygdrive/c/Becky!/`whoami`/Plugins
INSTALLNAME = bkshhdr.dll

DOCDIR = doc

RELEASE_DIR = bkshhdr$(RELEASE_VERSION)

all: $(TARGET)

install: all
	mkdir -p $(INSTALLPATH)
	$(INSTALL) -p -s $(TARGET) $(INSTALLPATH)/$(INSTALLNAME)

clean:
	$(RM) $(OBJS) $(RESOBJS) exports.$(OBJ) bkshhdr.lib bkshhdr.res
	$(RM) -r $(RELEASE_DIR) release
	if [ -d test ]; then (cd test; $(MAKE) clean); fi
	if [ -d exe ]; then (cd exe; $(MAKE) clean); fi

test: all
	if [ -d test ]; then (cd test; $(MAKE) all); fi
	if [ -d exe ]; then (cd exe; $(MAKE) all); fi

testrun: test
	if [ -f exe/shhdr01.exe ]; then exe/shhdr01.exe; fi

tags: $(SRCS) $(HDRS)
	ctags $(SRCS) $(HDRS)

strip:
	$(STRIP) $(TARGET)

debug: install
	./b2.sh

cleanall: clean
	if [ -d test ]; then (cd test; $(MAKE) cleanall); fi
	if [ -d exe ]; then (cd exe; $(MAKE) cleanall); fi
	$(RM) $(TARGET) bkshhdr$(RELEASE_VERSION).lzh bkshhdr$(RELEASE_VERSION).zip tags release $(RELEASE_DIR)
	$(RM) -r $(DOCDIR)

release: $(TARGET)
	mkdir -p $(RELEASE_DIR)
	mkdir -p $(RELEASE_DIR)/src
	mkdir -p $(RELEASE_DIR)/src/PlugInSDK
	$(INSTALL) -p -s $(TARGET) $(RELEASE_DIR)
	$(CP) bkshhdr.txt $(RELEASE_DIR)
	$(CP) *.c* *.h *.def *.rc Makefile bkshhdr.doxygen $(RELEASE_DIR)/src
	$(CP) PlugInSDK/*.* $(RELEASE_DIR)/src/PlugInSDK
	$(FIND) $(RELEASE_DIR) -name '*.$(OBJ)' -exec $(RM) -f {} \;
	$(ZIP) -9 -v -r bkshhdr$(RELEASE_VERSION).zip $(RELEASE_DIR)
	$(LHA) av bkshhdr$(RELEASE_VERSION).lzh $(RELEASE_DIR)
	$(RM) bkshhdr$(RELEASE_VERSION).bak
	$(TOUCH) release

docs:
	doxygen bkshhdr.doxygen

doc: docs

$(TARGET): $(OBJS) $(RESOBJS) bkshhdr.def
	dlltool -d bkshhdr.def -e exports.$(OBJ) -l bkshhdr.lib $(OBJS)
	$(LD) $(LDFLAGS) -o bkshhdr.dll exports.$(OBJ) $(OBJS) $(RESOBJS) $(LIBS)

PluginSDK/BeckyAPI.$(OBJ): PluginSDK/BeckyAPI.cpp PluginSDK/BeckyAPI.h
	$(CC) -c -o PluginSDK/BeckyAPI.$(OBJ) $(CFLAGS) PluginSDK/BeckyAPI.cpp

bkshhdr_res.$(OBJ): bkshhdr.res
	windres -v -i bkshhdr.res -o bkshhdr_res.$(OBJ) --language=00000411

bkshhdr.res: bkshhdr.rc Makefile
	rc /r /dMAJOR=$(MAJOR) /dMINOR=$(MINOR) /dTEENY=$(TEENY) /dRELEASE_VERSION='"$(RELEASE_VERSION)\0"' /dVERSION_STR='"bkshhdr version $(RELEASE_VERSION)"' /dCOPYRIGHT=$(COPYRIGHT) bkshhdr.rc
#windres -v -i shhdr01.rc -o shhdr01.res --language=00000411 -DMAJOR=$(MAJOR) -DMINOR=$(MINOR) -DTEENY=$(TEENY)
#nkf -S --utf16 shhdr01.rc | windres -v --input-format=rc -o shhdr01.res --language=00000411

.cpp.$(OBJ):
	$(CC) -c $(CFLAGS) $<

bkshhdr.$(OBJ): bkshhdr.cpp bkshhdr.h hdrwin.h hdrlist.h dialog.h hookproc.h resource.h Makefile
charbuf.$(OBJ): charbuf.cpp charbuf.h
hdrwin.$(OBJ): hdrwin.cpp hdrwin.h charbuf.h bkshhdr.h hdrlist.h resource.h
hdrlist.$(OBJ): hdrlist.cpp hdrlist.h
dialog.$(OBJ): dialog.cpp dialog.h bkshhdr.h resource.h
hookproc.$(OBJ): hookproc.cpp hookproc.h hdrwin.h

bkshhdr.h: PlugInSDK/BeckyAPI.h
	$(TOUCH) -c $<

hdrwin.h: charbuf.h
	$(TOUCH) -c $<
