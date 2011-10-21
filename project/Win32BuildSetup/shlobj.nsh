!include LogicLib.nsh
;**********************************************************************
; Inspired by http://portableapps.com/node/11936                      *
;**********************************************************************
;* SHGetFolderPath Constants                                          *
;**********************************************************************
!define CSIDL_DESKTOP "0x0000"
!define CSIDL_INTERNET "0x0001"
!define CSIDL_PROGRAMS "0x0002"
!define CSIDL_CONTROLS "0x0003"
!define CSIDL_PRINTERS "0x0004"
!define CSIDL_PERSONAL "0x0005"
!define CSIDL_FAVORITES "0x0006"
!define CSIDL_STARTUP "0x0007"
!define CSIDL_RECENT "0x0008"
!define CSIDL_SENDTO "0x0009"
!define CSIDL_BITBUCKET "0x000A"
!define CSIDL_STARTMENU "0x000B"
!define CSIDL_MYDOCUMENTS "0x000C"
!define CSIDL_MYMUSIC "0x000D"
!define CSIDL_MYVIDEO "0x000E"
!define CSIDL_DIRECTORY "0x0010"
!define CSIDL_DRIVES "0x0011"
!define CSIDL_NETWORK "0x0012"
!define CSIDL_NETHOOD "0x0013"
!define CSIDL_FONTS "0x0014"
!define CSIDL_TEMPLATES "0x0015"
!define CSIDL_COMMON_STARTMENU "0x016"
!define CSIDL_COMMON_PROGRAMS "0x0017"
!define CSIDL_COMMON_STARTUP "0x0018"
!define CSIDL_COMMON_DESKTOPDIRECTORY "0x0019"
!define CSIDL_APPDATA "0x001A"
!define CSIDL_PRINTHOOD "0x001B"
!define CSIDL_LOCAL_APPDATA "0x001C"
!define CSIDL_ALTSTARTUP "0x001D"
!define CSIDL_COMMON_ALTSTARTUP "0x001E"
!define CSIDL_COMMON_FAVORITES "0x001F"
!define CSIDL_INTERNET_CACHE "0x0020"
!define CSIDL_COOKIES "0x0021"
!define CSIDL_HISTORY "0x0022"
!define CSIDL_COMMON_APPDATA "0x0023"
!define CSIDL_WINDOWS "0x0024"
!define CSIDL_SYSTEM "0x0025"
!define CSIDL_PROGRAM_FILES "0x0026"
!define CSIDL_MYPICTURES "0x0027"
!define CSIDL_PROFILE "0x0028"
!define CSIDL_SYSTEMX86 "0x0029"
!define CSIDL_PROGRAM_FILESX86 "0x002A"
!define CSIDL_PROGRAM_FILES_COMMON "0x002B"
!define CSIDL_PROGRAM_FILES_COMMONX86 "0x002C"
!define CSIDL_COMMON_TEMPLATES "0x002D"
!define CSIDL_COMMON_DOCUMENTS "0x002E"
!define CSIDL_COMMON_ADMINTOOLS "0x002F"
!define CSIDL_ADMINTOOLS "0x0030"
!define CSIDL_CONNECTIONS "0x0031"
!define CSIDL_COMMON_MUSIC "0x0035"
!define CSIDL_COMMON_PICTURES "0x0036"
!define CSIDL_COMMON_VIDEO "0x0037"
!define CSIDL_RESOURCES "0x0038"
!define CSIDL_RESOURCES_LOCALIZED "0x0039"
!define CSIDL_COMMON_OEM_LINKS "0x003A"
!define CSIDL_CDBURN_AREA "0x003B"
!define CSIDL_COMPUTERSNEARME "0x003D"

; combine with CSIDL_ value to force folder creation in SHGetFolderPath()
!define CSIDL_FLAG_CREATE "0x8000"
; combine with CSIDL_ value to return an unverified folder path
!define CSIDL_FLAG_DONT_VERIFY "0x4000"
; combine with CSIDL_ value to insure non-alias versions of the pidl
!define CSIDL_FLAG_NO_ALIAS "0x1000"
; combine with CSIDL_ value to indicate per-user init (eg. upgrade)
!define CSIDL_FLAG_PER_USER_INIT "0x0800"
; mask for all possible flag values
!define CSIDL_FLAG_MASK "0xFF00"

; dwFlags values for use with SHGetFolderPath
; current value for user, verify it exists
!define SHGFP_TYPE_CURRENT "0x0000"
; default value
!define SHGFP_TYPE_DEFAULT "0x0001"