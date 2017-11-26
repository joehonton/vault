# Microsoft Developer Studio Project File - Name="vdbKernel" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=vdbKernel - Win32 AfxDebug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vdbKernel.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vdbKernel.mak" CFG="vdbKernel - Win32 AfxDebug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vdbKernel - Win32 AfxDebug" (based on "Win32 (x86) Static Library")
!MESSAGE "vdbKernel - Win32 AfxRelease" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
F90=df.exe
RSC=rc.exe

!IF  "$(CFG)" == "vdbKernel - Win32 AfxDebug"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vdbKernel___Win32_AfxDebug"
# PROP BASE Intermediate_Dir "vdbKernel___Win32_AfxDebug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "L:\AfxDebug"
# PROP Intermediate_Dir "AfxDebug"
# PROP Target_Dir ""
# ADD BASE F90 /include:"vdbKernel___Win32_AfxDebug/"
# ADD F90 /include:"AfxDebug/"
# ADD BASE CPP /nologo /MTd /W3 /GX /Z7 /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "USE_AFX" /YX /FD /c
# ADD CPP /nologo /MTd /W3 /GX /Z7 /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "USE_AFX" /YX /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "vdbKernel - Win32 AfxRelease"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vdbKernel___Win32_AfxRelease"
# PROP BASE Intermediate_Dir "vdbKernel___Win32_AfxRelease"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "L:\AfxRelease"
# PROP Intermediate_Dir "AfxRelease"
# PROP Target_Dir ""
# ADD BASE F90 /include:"vdbKernel___Win32_AfxRelease/"
# ADD F90 /include:"AfxRelease/"
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "USE_AFX" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "USE_AFX" /YX /FD /c
# SUBTRACT CPP /O<none>
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

# Name "vdbKernel - Win32 AfxDebug"
# Name "vdbKernel - Win32 AfxRelease"
# Begin Group "Source files"

# PROP Default_Filter "*.cpp"
# Begin Source File

SOURCE=.\vdbColumn.cpp
# End Source File
# Begin Source File

SOURCE=.\vdbDatabase.cpp
# End Source File
# Begin Source File

SOURCE=.\vdbIndex.cpp
# End Source File
# Begin Source File

SOURCE=.\vdbLinkSet.cpp
# End Source File
# Begin Source File

SOURCE=.\vdbMicrosoftSQLServer.cpp
# End Source File
# Begin Source File

SOURCE=.\vdbObject.cpp
# End Source File
# Begin Source File

SOURCE=.\vdbOmniscienceORDBMS.cpp
# End Source File
# Begin Source File

SOURCE=.\vdbPool.cpp
# End Source File
# Begin Source File

SOURCE=.\vdbQuery.cpp
# End Source File
# Begin Source File

SOURCE=.\vdbServer.cpp
# End Source File
# Begin Source File

SOURCE=.\vdbServerInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\vdbServerInfoTemplates.cpp
# End Source File
# Begin Source File

SOURCE=.\vdbSetOfServers.cpp
# End Source File
# Begin Source File

SOURCE=.\vdbSetOfVersions.CPP
# End Source File
# Begin Source File

SOURCE=.\vdbSetSystemInstantiation.cpp
# End Source File
# Begin Source File

SOURCE=.\vdbStatement.cpp
# End Source File
# Begin Source File

SOURCE=.\vdbTable.cpp
# End Source File
# Begin Source File

SOURCE=.\vdbUnique.cpp
# End Source File
# Begin Source File

SOURCE=.\vdbUniqueTemplates.cpp
# End Source File
# Begin Source File

SOURCE=.\vdbVersion.cpp
# End Source File
# Begin Source File

SOURCE=.\vdbVersionTemplates.cpp
# End Source File
# Begin Source File

SOURCE=.\vdbWatcomSQLServer.cpp
# End Source File
# End Group
# End Target
# End Project
