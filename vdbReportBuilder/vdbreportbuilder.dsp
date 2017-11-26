# Microsoft Developer Studio Project File - Name="vdbReportBuilder" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=vdbReportBuilder - Win32 AfxDebug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vdbReportBuilder.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vdbReportBuilder.mak" CFG="vdbReportBuilder - Win32 AfxDebug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vdbReportBuilder - Win32 AfxDebug" (based on "Win32 (x86) Static Library")
!MESSAGE "vdbReportBuilder - Win32 AfxRelease" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vdbReportBuilder - Win32 AfxDebug"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vdbReportBuilder___Win32_AfxDebug"
# PROP BASE Intermediate_Dir "vdbReportBuilder___Win32_AfxDebug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "L:\AfxDebug"
# PROP Intermediate_Dir "AfxDebug"
# PROP Target_Dir ""
F90=df.exe
# ADD BASE F90 /check:bounds /compile_only /debug:full /include:"vdbReportBuilder___Win32_AfxDebug/" /nologo /warn:argument_checking /warn:nofileopt
# ADD F90 /check:bounds /compile_only /debug:full /include:"AfxDebug/" /nologo /warn:argument_checking /warn:nofileopt
# ADD BASE CPP /nologo /MTd /W3 /Gm /GR /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "USE_AFX" /D "_MBCS" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GR /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "USE_AFX" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "vdbReportBuilder - Win32 AfxRelease"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vdbReportBuilder___Win32_AfxRelease"
# PROP BASE Intermediate_Dir "vdbReportBuilder___Win32_AfxRelease"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "L:\AfxRelease"
# PROP Intermediate_Dir "AfxRelease"
# PROP Target_Dir ""
F90=df.exe
# ADD BASE F90 /compile_only /include:"vdbReportBuilder___Win32_AfxRelease/" /nologo /warn:nofileopt
# ADD F90 /compile_only /include:"AfxRelease/" /nologo /warn:nofileopt
# ADD BASE CPP /nologo /MT /W3 /GR /GX /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "USE_AFX" /D "_MBCS" /FD /c
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /nologo /MT /W3 /GR /GX /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "USE_AFX" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
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

# Name "vdbReportBuilder - Win32 AfxDebug"
# Name "vdbReportBuilder - Win32 AfxRelease"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;f90;for;f;fpp"
# Begin Source File

SOURCE=.\MapTemplates.cpp
# End Source File
# Begin Source File

SOURCE=.\ReportBuilderPage1.cpp
# End Source File
# Begin Source File

SOURCE=.\ReportBuilderPage10.cpp
# End Source File
# Begin Source File

SOURCE=.\ReportBuilderPage2.cpp
# End Source File
# Begin Source File

SOURCE=.\ReportBuilderPage3.cpp
# End Source File
# Begin Source File

SOURCE=.\ReportBuilderPage4.cpp
# End Source File
# Begin Source File

SOURCE=.\ReportBuilderPage5.cpp
# End Source File
# Begin Source File

SOURCE=.\ReportBuilderPage6.cpp
# End Source File
# Begin Source File

SOURCE=.\ReportBuilderPage7.cpp
# End Source File
# Begin Source File

SOURCE=.\ReportBuilderPage8.cpp
# End Source File
# Begin Source File

SOURCE=.\ReportBuilderPage9.cpp
# End Source File
# Begin Source File

SOURCE=.\ReportBuilderPageHolder.cpp
# End Source File
# Begin Source File

SOURCE=.\ReportChangeColumn.cpp
# End Source File
# Begin Source File

SOURCE=.\ReportColumn.cpp
# End Source File
# Begin Source File

SOURCE=.\ReportColumnAssignment.cpp
# End Source File
# Begin Source File

SOURCE=.\ReportDefinition.cpp
# End Source File
# Begin Source File

SOURCE=.\ReportFontSpecs.cpp
# End Source File
# Begin Source File

SOURCE=.\ReportLayout.cpp
# End Source File
# Begin Source File

SOURCE=.\ReportLookupColumn.cpp
# End Source File
# Begin Source File

SOURCE=.\ReportNormalColumn.cpp
# End Source File
# Begin Source File

SOURCE=.\ReportOIDColumn.cpp
# End Source File
# Begin Source File

SOURCE=.\ReportPreview.cpp
# End Source File
# Begin Source File

SOURCE=.\ReportSelectionCriteria.cpp
# End Source File
# Begin Source File

SOURCE=.\ReportSortCriteria.cpp
# End Source File
# Begin Source File

SOURCE=.\ReportStyleDefinition.cpp
# End Source File
# Begin Source File

SOURCE=.\SampleWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\SetTemplates.cpp
# End Source File
# Begin Source File

SOURCE=.\SimpleDatabase.cpp
# End Source File
# Begin Source File

SOURCE=.\vdbExportData.cpp
# End Source File
# Begin Source File

SOURCE=.\vdbFormatStyles.cpp
# End Source File
# Begin Source File

SOURCE=.\vdbPrintReport.cpp
# End Source File
# Begin Source File

SOURCE=.\vdbReportManager.cpp
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
