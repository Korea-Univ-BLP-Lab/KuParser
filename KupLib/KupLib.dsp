# Microsoft Developer Studio Project File - Name="KupLib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=KupLib - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "KupLib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "KupLib.mak" CFG="KupLib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "KupLib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "KupLib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "KupLib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../lib"
# PROP Intermediate_Dir "../lib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GR /GX /O1 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "_STLP_USE_STATICX_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x412 /d "NDEBUG"
# ADD RSC /l 0x412 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../lib/libKup.lib" /NODEFAULTLIB:LIBCMT.LIB

!ELSEIF  "$(CFG)" == "KupLib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../lib"
# PROP Intermediate_Dir "../lib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GR /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "_STLP_USE_STATICX_LIB" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x412 /d "_DEBUG"
# ADD RSC /l 0x412 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../lib/libKupd.lib" /NODEFAULTLIB:LIBCMT.LIB

!ENDIF 

# Begin Target

# Name "KupLib - Win32 Release"
# Name "KupLib - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\header_src\BTree.cpp
# End Source File
# Begin Source File

SOURCE=..\header_src\BTree.h
# End Source File
# Begin Source File

SOURCE=..\header_src\DataType.h
# End Source File
# Begin Source File

SOURCE=..\header_src\DynamicRule.cpp
# End Source File
# Begin Source File

SOURCE=..\header_src\DynamicRule.h
# End Source File
# Begin Source File

SOURCE=..\header_src\KomaWrap.cpp
# End Source File
# Begin Source File

SOURCE=..\header_src\KomaWrap.h
# End Source File
# Begin Source File

SOURCE=..\header_src\Kup.cpp
# End Source File
# Begin Source File

SOURCE=..\header_src\Kup.h
# End Source File
# Begin Source File

SOURCE=..\header_src\KupError.h
# End Source File
# Begin Source File

SOURCE=..\header_src\Lexicon.cpp
# End Source File
# Begin Source File

SOURCE=..\header_src\Lexicon.h
# End Source File
# Begin Source File

SOURCE=..\header_src\ParseTree.cpp
# End Source File
# Begin Source File

SOURCE=..\header_src\ParseTree.h
# End Source File
# Begin Source File

SOURCE=..\header_src\ReadTree.cpp
# End Source File
# Begin Source File

SOURCE=..\header_src\ReadTree.h
# End Source File
# Begin Source File

SOURCE=..\header_src\RscPath.h
# End Source File
# Begin Source File

SOURCE=..\header_src\StaticRule.cpp
# End Source File
# Begin Source File

SOURCE=..\header_src\StaticRule.h
# End Source File
# Begin Source File

SOURCE=..\header_src\WriteTree.cpp
# End Source File
# Begin Source File

SOURCE=..\header_src\WriteTree.h
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Koma\Koma.lib
# End Source File
# Begin Source File

SOURCE=..\Koma\HanTag.lib
# End Source File
# End Group
# End Target
# End Project
