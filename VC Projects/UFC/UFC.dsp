# Microsoft Developer Studio Project File - Name="UFC" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=UFC - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "UFC.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "UFC.mak" CFG="UFC - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "UFC - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "UFC - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "UFC - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GR /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x404 /d "NDEBUG"
# ADD RSC /l 0x404 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\Build\VC\UFC.lib"

!ELSEIF  "$(CFG)" == "UFC - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x404 /d "_DEBUG"
# ADD RSC /l 0x404 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\Build\VC\UFC.lib"

!ENDIF 

# Begin Target

# Name "UFC - Win32 Release"
# Name "UFC - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "Utility"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\UFC\MD5.cpp
# End Source File
# Begin Source File

SOURCE=..\..\UFC\UDateTime.cpp
# End Source File
# End Group
# Begin Group "Kernel"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\UFC\Exception.cpp
# End Source File
# Begin Source File

SOURCE=..\..\UFC\FileStream.cpp
# End Source File
# Begin Source File

SOURCE=..\..\UFC\List.cpp
# End Source File
# Begin Source File

SOURCE=..\..\UFC\MemoryStream.cpp
# End Source File
# Begin Source File

SOURCE=..\..\UFC\PThread.cpp
# End Source File
# Begin Source File

SOURCE=..\..\UFC\ThreadPool.cpp
# End Source File
# End Group
# Begin Group "String"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\UFC\AnsiString.cpp
# End Source File
# Begin Source File

SOURCE=..\..\UFC\StringBuffer.cpp
# End Source File
# End Group
# Begin Group "Network"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\UFC\MultiplexingServer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\UFC\NetUtility.cpp
# End Source File
# Begin Source File

SOURCE=..\..\UFC\PClientSocket.cpp
# End Source File
# Begin Source File

SOURCE=..\..\UFC\PServerSocket.cpp
# End Source File
# Begin Source File

SOURCE=..\..\UFC\PSocket.cpp
# End Source File
# Begin Source File

SOURCE=..\..\UFC\PUDPClient.cpp
# End Source File
# Begin Source File

SOURCE=..\..\UFC\SocketHeartBeat.cpp
# End Source File
# End Group
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "Network header"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\UFC\MultiplexingServer.h
# End Source File
# Begin Source File

SOURCE=..\..\UFC\NetUtility.h
# End Source File
# Begin Source File

SOURCE=..\..\UFC\PClientSocket.h
# End Source File
# Begin Source File

SOURCE=..\..\UFC\PServerSocket.h
# End Source File
# Begin Source File

SOURCE=..\..\UFC\PSocket.h
# End Source File
# Begin Source File

SOURCE=..\..\UFC\PUDPClient.h
# End Source File
# Begin Source File

SOURCE=..\..\UFC\SocketHeartBeat.h
# End Source File
# End Group
# Begin Group "String header"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\UFC\AnsiString.h
# End Source File
# Begin Source File

SOURCE=..\..\UFC\StringBuffer.h
# End Source File
# End Group
# Begin Group "Kernel header"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\UFC\Exception.h
# End Source File
# Begin Source File

SOURCE=..\..\UFC\FileStream.h
# End Source File
# Begin Source File

SOURCE=..\..\UFC\List.h
# End Source File
# Begin Source File

SOURCE=..\..\UFC\MemoryStream.h
# End Source File
# Begin Source File

SOURCE=..\..\UFC\PThread.h
# End Source File
# Begin Source File

SOURCE=..\..\UFC\Stream.h
# End Source File
# Begin Source File

SOURCE=..\..\UFC\ThreadPool.h
# End Source File
# End Group
# Begin Group "Utility header"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\UFC\MD5.h
# End Source File
# Begin Source File

SOURCE=..\..\UFC\UDateTime.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\UFC\Type.h
# End Source File
# Begin Source File

SOURCE=..\..\UFC\UFC.h
# End Source File
# End Group
# End Target
# End Project
