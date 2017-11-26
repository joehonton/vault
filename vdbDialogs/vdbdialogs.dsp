# Microsoft Developer Studio Project File - Name="vdbDialogs" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=vdbDialogs - Win32 AfxDebug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vdbDialogs.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vdbDialogs.mak" CFG="vdbDialogs - Win32 AfxDebug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vdbDialogs - Win32 AfxDebug" (based on "Win32 (x86) Static Library")
!MESSAGE "vdbDialogs - Win32 AfxRelease" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
F90=df.exe
RSC=rc.exe

!IF  "$(CFG)" == "vdbDialogs - Win32 AfxDebug"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vdbDialogs___Win32_AfxDebug"
# PROP BASE Intermediate_Dir "vdbDialogs___Win32_AfxDebug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "L:\AfxDebug"
# PROP Intermediate_Dir "AfxDebug"
# PROP Target_Dir ""
# ADD BASE F90 /check:bounds /compile_only /debug:full /include:"vdbDialogs___Win32_AfxDebug/" /nologo /warn:argument_checking /warn:nofileopt
# ADD F90 /check:bounds /compile_only /debug:full /include:"AfxDebug/" /nologo /warn:argument_checking /warn:nofileopt
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "USE_AFX" /FD /GZ /c
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "USE_AFX" /D "WIN32" /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "vdbDialogs - Win32 AfxRelease"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vdbDialogs___Win32_AfxRelease"
# PROP BASE Intermediate_Dir "vdbDialogs___Win32_AfxRelease"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "L:\AfxRelease"
# PROP Intermediate_Dir "AfxRelease"
# PROP Target_Dir ""
# ADD BASE F90 /compile_only /include:"vdbDialogs___Win32_AfxRelease/" /nologo /warn:nofileopt
# ADD F90 /compile_only /include:"AfxRelease/" /nologo /warn:nofileopt
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "USE_AFX" /FD /c
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /nologo /MT /W3 /GX /D "_WINDOWS" /D "USE_AFX" /D "NDEBUG" /D "WIN32" /FD /c
# SUBTRACT CPP /O<none> /YX /Yc /Yu
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "vdbDialogs - Win32 AfxDebug"
# Name "vdbDialogs - Win32 AfxRelease"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;f90;for;f;fpp"
# Begin Source File

SOURCE=.\SetTemplates.cpp
# End Source File
# Begin Source File

SOURCE=.\VaultResourceInstance.cpp
# End Source File
# Begin Source File

SOURCE=.\vdbDatabaseAttach.cpp
# End Source File
# Begin Source File

SOURCE=.\vdbDatabaseBackup.cpp
# End Source File
# Begin Source File

SOURCE=.\vdbdatabasedelete.cpp
# End Source File
# Begin Source File

SOURCE=.\vdbdatabasenew.cpp
# End Source File
# Begin Source File

SOURCE=.\vdbDatabaseOpen.cpp
# End Source File
# Begin Source File

SOURCE=.\vdbdatabaserestore.cpp
# End Source File
# Begin Source File

SOURCE=.\vdbDataSource.cpp
# End Source File
# Begin Source File

SOURCE=.\vdbdirectorydialog.cpp
# End Source File
# Begin Source File

SOURCE=.\vdbDLLBitmapButton.cpp
# End Source File
# Begin Source File

SOURCE=.\vdbDLLDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\vdbFileDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\vdbfiledialogx.cpp
# End Source File
# Begin Source File

SOURCE=.\vdbListCtrlColors.cpp
# End Source File
# Begin Source File

SOURCE=.\vdbMessageBox.cpp
# End Source File
# Begin Source File

SOURCE=.\vdbPrinterDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\vdbStatusWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\vdbtroubleshootlogindialog.cpp
# End Source File
# Begin Source File

SOURCE=.\vdbUserInput.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# End Target
# End Project
