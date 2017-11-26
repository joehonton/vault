# Microsoft Developer Studio Project File - Name="vdbLibrary" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=vdbLibrary - Win32 AfxDebug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vdbLibrary.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vdbLibrary.mak" CFG="vdbLibrary - Win32 AfxDebug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vdbLibrary - Win32 AfxRelease" (based on "Win32 (x86) Static Library")
!MESSAGE "vdbLibrary - Win32 AfxDebug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vdbLibrary - Win32 AfxRelease"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vdbLibrary___Win32_AfxRelease"
# PROP BASE Intermediate_Dir "vdbLibrary___Win32_AfxRelease"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "L:\AfxRelease"
# PROP Intermediate_Dir "AfxRelease"
# PROP Target_Dir ""
F90=df.exe
# ADD BASE F90 /include:"vdbLibrary___Win32_AfxRelease/"
# ADD F90 /include:"AfxRelease/"
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "USE_AFX" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /D "_WINDOWS" /D "USE_AFX" /D "NDEBUG" /D "WIN32" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "vdbLibrary - Win32 AfxDebug"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vdbLibrary___Win32_AfxDebug"
# PROP BASE Intermediate_Dir "vdbLibrary___Win32_AfxDebug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "L:\AfxDebug"
# PROP Intermediate_Dir "AfxDebug"
# PROP Target_Dir ""
F90=df.exe
# ADD BASE F90 /include:"vdbLibrary___Win32_AfxDebug/"
# ADD F90 /include:"AfxDebug/"
# ADD BASE CPP /nologo /MTd /W3 /GX /Z7 /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "USE_AFX" /YX /FD /c
# ADD CPP /nologo /MTd /W3 /GX /Zi /Od /D "_DEBUG" /D "_WINDOWS" /D "USE_AFX" /D "WIN32" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "vdbLibrary - Win32 AfxRelease"
# Name "vdbLibrary - Win32 AfxDebug"
# Begin Group "Source files"

# PROP Default_Filter "*.cpp"
# Begin Source File

SOURCE=.\vdbApp.cpp
# End Source File
# Begin Source File

SOURCE=.\vdbAppRegistry.cpp
# End Source File
# Begin Source File

SOURCE=.\vdbArray.cpp
# End Source File
# Begin Source File

SOURCE=.\vdbAssert.cpp
# End Source File
# Begin Source File

SOURCE=.\vdbBinaryFixed.cpp
# End Source File
# Begin Source File

SOURCE=.\vdbBinaryLong.cpp
# End Source File
# Begin Source File

SOURCE=.\vdbBinaryObject.cpp
# End Source File
# Begin Source File

SOURCE=.\vdbBinaryStreamHeader.cpp
# End Source File
# Begin Source File

SOURCE=.\vdbBinaryTree.cpp
# End Source File
# Begin Source File

SOURCE=.\vdbBinaryVar.cpp
# End Source File
# Begin Source File

SOURCE=.\vdbBitmask16.cpp
# End Source File
# Begin Source File

SOURCE=.\vdbBitmask32.cpp
# End Source File
# Begin Source File

SOURCE=.\vdbBitmask8.cpp
# End Source File
# Begin Source File

SOURCE=.\vdbColorConstants.cpp
# End Source File
# Begin Source File

SOURCE=.\vdbColorConversions.cpp
# End Source File
# Begin Source File

SOURCE=.\vdbexception.cpp
# End Source File
# Begin Source File

SOURCE=.\vdbFilename.cpp
# End Source File
# Begin Source File

SOURCE=.\vdbGDIPlus.cpp
# End Source File
# Begin Source File

SOURCE=.\vdbGeneric.cpp
# End Source File
# Begin Source File

SOURCE=.\vdbLong.cpp
# End Source File
# Begin Source File

SOURCE=.\vdbMap.cpp
# End Source File
# Begin Source File

SOURCE=.\vdbMeasurement.cpp
# End Source File
# Begin Source File

SOURCE=.\vdbNewHandler.cpp
# End Source File
# Begin Source File

SOURCE=.\vdbProfile.cpp
# End Source File
# Begin Source File

SOURCE=.\vdbProfileString.cpp
# End Source File
# Begin Source File

SOURCE=.\vdbProperty.cpp
# End Source File
# Begin Source File

SOURCE=.\vdbPtr.cpp
# End Source File
# Begin Source File

SOURCE=.\vdbRegistrationKey.cpp
# End Source File
# Begin Source File

SOURCE=.\vdbRowColumn.cpp
# End Source File
# Begin Source File

SOURCE=.\vdbset.cpp
# End Source File
# Begin Source File

SOURCE=.\vdbSetSort.cpp
# End Source File
# Begin Source File

SOURCE=.\vdbSetTemplates.cpp
# End Source File
# Begin Source File

SOURCE=.\vdbstreamheader.cpp
# End Source File
# Begin Source File

SOURCE=.\vdbstring.cpp
# End Source File
# Begin Source File

SOURCE=.\vdbtimestamp.cpp
# End Source File
# Begin Source File

SOURCE=.\vdbTimeStampDB.cpp
# End Source File
# Begin Source File

SOURCE=.\vdbToken.cpp
# End Source File
# Begin Source File

SOURCE=.\vdbTypedef.cpp
# End Source File
# Begin Source File

SOURCE=.\vdbURL.cpp
# End Source File
# Begin Source File

SOURCE=.\vdbUtility.cpp
# End Source File
# Begin Source File

SOURCE=.\vdbVarCharLong.cpp
# End Source File
# Begin Source File

SOURCE=.\vdbWideChar.cpp
# End Source File
# Begin Source File

SOURCE=.\vdbWindowSupport.cpp
# End Source File
# End Group
# End Target
# End Project
