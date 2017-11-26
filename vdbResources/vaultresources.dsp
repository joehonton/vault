# Microsoft Developer Studio Project File - Name="VaultResources" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=VaultResources - Win32 AfxRelease
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "VaultResources.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "VaultResources.mak" CFG="VaultResources - Win32 AfxRelease"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "VaultResources - Win32 AfxRelease" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "VaultResources - Win32 AfxDebug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
F90=df.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "VaultResources - Win32 AfxRelease"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\VaultRe1"
# PROP BASE Intermediate_Dir ".\VaultRe1"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "L:\AfxRelease"
# PROP Intermediate_Dir "AfxRelease"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE F90 /include:".\VaultRe1/"
# ADD F90 /include:"AfxRelease/"
# ADD BASE CPP /nologo /MT /W3 /GX /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /MT /W3 /GX /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "USE_AFX" /D "_WINDLL" /D "_MBCS" /YX /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x11000000" /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /base:"0x11000000" /subsystem:windows /dll /machine:I386

!ELSEIF  "$(CFG)" == "VaultResources - Win32 AfxDebug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\VaultRe2"
# PROP BASE Intermediate_Dir ".\VaultRe2"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "L:\AfxDebug"
# PROP Intermediate_Dir "AfxDebug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE F90 /include:".\VaultRe2/"
# ADD F90 /include:"AfxDebug/"
# ADD BASE CPP /nologo /MTd /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /MTd /W3 /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "USE_AFX" /D "_WINDLL" /D "_MBCS" /YX /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x11000000" /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 /nologo /base:"0x11000000" /subsystem:windows /dll /debug /machine:I386

!ENDIF 

# Begin Target

# Name "VaultResources - Win32 AfxRelease"
# Name "VaultResources - Win32 AfxDebug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\DummyExport.cpp
# End Source File
# Begin Source File

SOURCE=.\Startup.cpp
# End Source File
# Begin Source File

SOURCE=.\vault.rc
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\accept.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\checkmark.bmp
# End Source File
# Begin Source File

SOURCE=.\close.bmp
# End Source File
# Begin Source File

SOURCE=.\connection_failed.bmp
# End Source File
# Begin Source File

SOURCE=.\create.bmp
# End Source File
# Begin Source File

SOURCE=.\create_disabled.bmp
# End Source File
# Begin Source File

SOURCE=.\create_new.bmp
# End Source File
# Begin Source File

SOURCE=.\database.bmp
# End Source File
# Begin Source File

SOURCE=.\directory_disabled.bmp
# End Source File
# Begin Source File

SOURCE=.\directory_down.bmp
# End Source File
# Begin Source File

SOURCE=.\directory_focus.bmp
# End Source File
# Begin Source File

SOURCE=.\directory_icons.bmp
# End Source File
# Begin Source File

SOURCE=.\directory_up.bmp
# End Source File
# Begin Source File

SOURCE=.\file_icons.bmp
# End Source File
# Begin Source File

SOURCE=.\file_icons1.bmp
# End Source File
# Begin Source File

SOURCE=.\new.bmp
# End Source File
# Begin Source File

SOURCE=.\odbc_manager.bmp
# End Source File
# Begin Source File

SOURCE=.\ok.bmp
# End Source File
# Begin Source File

SOURCE=.\reject.bmp
# End Source File
# Begin Source File

SOURCE=.\report_filter.bmp
# End Source File
# Begin Source File

SOURCE=.\report_sort.bmp
# End Source File
# Begin Source File

SOURCE=.\report_tables.bmp
# End Source File
# Begin Source File

SOURCE=.\sql_server.bmp
# End Source File
# Begin Source File

SOURCE=.\vdb_acce.bmp
# End Source File
# Begin Source File

SOURCE=.\vdb_new_disabled.bmp
# End Source File
# End Group
# End Target
# End Project
