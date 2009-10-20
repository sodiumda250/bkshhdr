# ---------------------------------------------------------------------------
VERSION = BCB.03
# ---------------------------------------------------------------------------
!ifndef BCB
BCB = $(MAKEDIR)\..
!endif
# ---------------------------------------------------------------------------
PROJECT = BKSHHDR.dll
OBJFILES = BKSHHDR.obj ..\BkCommon.obj ..\BeckyAPI.obj ShowText.obj TAboutForm.obj \
  TDummyForm.obj
RESFILES = BKSHHDR.res
RESDEPEN = $(RESFILES) ShowText.dfm TAboutForm.dfm TDummyForm.dfm
LIBFILES = 
LIBRARIES = vcldbx35.lib vcldb35.lib vclx35.lib vcl35.lib
SPARELIBS = vcl35.lib vclx35.lib vcldb35.lib vcldbx35.lib
PACKAGES =
PATHASM = .;
PATHCPP = .;..\..\PlugInSDK
PATHPAS = .;
PATHRC = .;
DEBUGLIBPATH = $(BCB)\lib\debug
RELEASELIBPATH = $(BCB)\lib\release
DEFFILE = ..\Template.def
# ---------------------------------------------------------------------------
CFLAG1 = -WD -O2 -Hc -w -Ve -d -k- -vi -c -b- -w-par -w-inl -Vx
CFLAG2 = -I..\..\pluginsdk;c:\usr\woods\pluginsdk;.;c:\usr\woods\pluginsdk\BkShHdr;c:\usr\woods\plugin~1;..;c:\usr\woods\plugin~1;$(BCB)\include;$(BCB)\include\vcl \
  -H=$(BCB)\lib\vcl.csm
CFLAG3 =
PFLAGS = -U..\..\pluginsdk;c:\usr\woods\pluginsdk;.;c:\usr\woods\pluginsdk\BkShHdr;c:\usr\woods\plugin~1;..;c:\usr\woods\plugin~1;$(BCB)\lib\obj;$(BCB)\lib;$(DEBUGLIBPATH) \
  -I..\..\pluginsdk;c:\usr\woods\pluginsdk;.;c:\usr\woods\pluginsdk\BkShHdr;c:\usr\woods\plugin~1;..;c:\usr\woods\plugin~1;$(BCB)\include;$(BCB)\include\vcl \
  -AWinTypes=Windows;WinProcs=Windows;DbiTypes=BDE;DbiProcs=BDE;DbiErrs=BDE -$L- \
  -$D- -JPHNV -M
RFLAGS = -i..\..\pluginsdk;c:\usr\woods\pluginsdk;.;c:\usr\woods\pluginsdk\BkShHdr;c:\usr\woods\plugin~1;..;c:\usr\woods\plugin~1;$(BCB)\include;$(BCB)\include\vcl
AFLAGS = /ic:\usr\woods\pluginsdk /i. /ic:\usr\woods\pluginsdk\BkShHdr \
  /ic:\usr\woods\plugin~1 /i.. /ic:\usr\woods\plugin~1 /i$(BCB)\include \
  /i$(BCB)\include\vcl /mx /w2 /zd
LFLAGS = -L..\..\pluginsdk;c:\usr\woods\pluginsdk;.;c:\usr\woods\pluginsdk\BkShHdr;c:\usr\woods\plugin~1;..;c:\usr\woods\plugin~1;$(BCB)\lib\obj;$(BCB)\lib;$(DEBUGLIBPATH) \
  -aa -Tpd -s
IFLAGS = -g
LINKER = ilink32
# ---------------------------------------------------------------------------
ALLOBJ = c0d32.obj sysinit.obj $(OBJFILES)
ALLRES = $(RESFILES)
ALLLIB = $(LIBFILES) $(LIBRARIES) import32.lib cp32mt.lib
# ---------------------------------------------------------------------------
.autodepend

!ifdef IDEOPTIONS

[Version Info]
IncludeVerInfo=0
AutoIncBuild=0
MajorVer=1
MinorVer=0
Release=0
Build=0
Debug=0
PreRelease=0
Special=0
Private=0
DLL=0
Locale=1041
CodePage=932

[Debugging]
DebugSourceDirs=

[Parameters]
RunParams=
HostApplication=

!endif

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
