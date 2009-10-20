# ---------------------------------------------------------------------------
VERSION = BCB.01
# ---------------------------------------------------------------------------
!ifndef BCB
BCB = $(MAKEDIR)\..
!endif
# ---------------------------------------------------------------------------
PROJECT = BKSHHDR.dll
OBJFILES = BKSHHDR.obj \usr\woods\PlugInSDK\BkCommon.obj \
   \usr\woods\PlugInSDK\BeckyAPI.obj ShowText.obj TAboutForm.obj
RESFILES = 
RESDEPEN = $(RESFILES) ShowText.dfm TAboutForm.dfm
LIBFILES = 
DEFFILE = \usr\woods\PlugInSDK\Template.def
# ---------------------------------------------------------------------------
CFLAG1 = -WD -O2 -Hc -w -d -k- -r -vi -c -a4 -b- -w-par -w-inl -Vx -Ve -x
CFLAG2 = -Ic:\usr\woods\pluginsdk;.;c:\usr\woods\pluginsdk\BkShHdrE;c:\usr\woods\plugin~1;..;c:\usr\woods\plugin~1;$(BCB)\include;$(BCB)\include\vcl \
   -H=$(BCB)\lib\vcl.csm 
PFLAGS = -AWinTypes=Windows;WinProcs=Windows;DbiTypes=BDE;DbiProcs=BDE;DbiErrs=BDE \
   -Uc:\usr\woods\pluginsdk;.;c:\usr\woods\pluginsdk\BkShHdrE;c:\usr\woods\plugin~1;..;c:\usr\woods\plugin~1;$(BCB)\lib\obj;$(BCB)\lib \
   -Ic:\usr\woods\pluginsdk;.;c:\usr\woods\pluginsdk\BkShHdrE;c:\usr\woods\plugin~1;..;c:\usr\woods\plugin~1;$(BCB)\include;$(BCB)\include\vcl \
   -$L- -$D- -JPHNV -M     
RFLAGS = -ic:\usr\woods\pluginsdk;.;c:\usr\woods\pluginsdk\BkShHdrE;c:\usr\woods\plugin~1;..;c:\usr\woods\plugin~1;$(BCB)\include;$(BCB)\include\vcl 
LFLAGS = -Lc:\usr\woods\pluginsdk;.;c:\usr\woods\pluginsdk\BkShHdrE;c:\usr\woods\plugin~1;..;c:\usr\woods\plugin~1;$(BCB)\lib\obj;$(BCB)\lib \
   -aa -Tpd -s -V4.0 
IFLAGS = -g 
LINKER = ilink32
# ---------------------------------------------------------------------------
ALLOBJ = c0d32.obj $(OBJFILES)
ALLRES = $(RESFILES)
ALLLIB = $(LIBFILES) vcl.lib import32.lib cp32mt.lib 
# ---------------------------------------------------------------------------
.autodepend

$(PROJECT): $(OBJFILES) $(RESDEPEN) $(DEFFILE)
    $(BCB)\BIN\$(LINKER) @&&!
    $(LFLAGS) +
    $(ALLOBJ), +
    $(PROJECT),, +
    $(ALLLIB), +
    $(DEFFILE), +
    $(ALLRES) 
!

.pas.hpp:
    $(BCB)\BIN\dcc32 $(PFLAGS) { $** }

.pas.obj:
    $(BCB)\BIN\dcc32 $(PFLAGS) { $** }

.cpp.obj:
    $(BCB)\BIN\bcc32 $(CFLAG1) $(CFLAG2) -o$* $* 

.c.obj:
    $(BCB)\BIN\bcc32 $(CFLAG1) $(CFLAG2) -o$* $**

.rc.res:
    $(BCB)\BIN\brcc32 $(RFLAGS) $<
#-----------------------------------------------------------------------------
