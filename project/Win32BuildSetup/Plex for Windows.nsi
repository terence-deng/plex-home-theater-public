;Plex for Windows install script
;Copyright (C) 2005-2008 Team XBMC
;http://xbmc.org

;Script by chadoe

;--------------------------------
;Include Modern UI

  !include "MUI2.nsh"
  !include "nsDialogs.nsh"
  !include "LogicLib.nsh"
;--------------------------------
;General

  ;Name and file
  Name "Plex Media Center"
  OutFile "Plex-Media-Center-v${plex_revision}-en-US.exe"

  XPStyle on
  
  ;Default installation folder
  InstallDir "$PROGRAMFILES\Plex\Plex Media Center"

  ;Get installation folder from registry if available
  InstallDirRegKey HKCU "Software\Plex\Plex Media Center" ""

  ;Request application privileges for Windows Vista
  RequestExecutionLevel admin

;--------------------------------
;Variables

  Var StartMenuFolder
  Var PageProfileState
  Var RunArgs
  Var DirectXSetupError
  Var VSRedistSetupError
  
;--------------------------------
;Interface Settings

  !define MUI_HEADERIMAGE
  !define MUI_ICON "..\..\xbmc\win32\Plex.ico"
  ;!define MUI_HEADERIMAGE_BITMAP "plex-banner.bmp"
  ;!define MUI_HEADERIMAGE_RIGHT
  ;!define MUI_WELCOMEFINISHPAGE_BITMAP "plex-left.bmp"
  !define MUI_COMPONENTSPAGE_SMALLDESC
  ;!define MUI_FINISHPAGE_SHOWREADME "$INSTDIR\webapps\ROOT\RELEASE-NOTES.txt"
  !define MUI_FINISHPAGE_LINK "Please visit http://plexapp.com for more information."
  !define MUI_FINISHPAGE_LINK_LOCATION "http://plexapp.com"
  !define MUI_FINISHPAGE_RUN "$INSTDIR\Plex.exe"
  ;!define MUI_FINISHPAGE_RUN_PARAMETERS $RunArgs
  !define MUI_FINISHPAGE_RUN_NOTCHECKED
  !define MUI_ABORTWARNING  
;--------------------------------
;Pages

  !insertmacro MUI_PAGE_WELCOME
  !insertmacro MUI_PAGE_LICENSE "..\..\LICENSE.GPL"
  !insertmacro MUI_PAGE_COMPONENTS
  !insertmacro MUI_PAGE_DIRECTORY
  
  ;Start Menu Folder Page Configuration
  !define MUI_STARTMENUPAGE_REGISTRY_ROOT "HKCU" 
  !define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\Plex" 
  !define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "Start Menu Folder"
  !insertmacro MUI_PAGE_STARTMENU Application $StartMenuFolder  

  !insertmacro MUI_PAGE_INSTFILES
  !insertmacro MUI_PAGE_FINISH

  !insertmacro MUI_UNPAGE_WELCOME
  !insertmacro MUI_UNPAGE_CONFIRM
  UninstPage custom un.UnPageProfile un.UnPageProfileLeave
  !insertmacro MUI_UNPAGE_INSTFILES
  !insertmacro MUI_UNPAGE_FINISH

;--------------------------------
;Languages

  !insertmacro MUI_LANGUAGE "English"

;--------------------------------
;Installer Sections

InstType "Full"
InstType "Minimal" 

Section "Plex" SecPlex
  SetShellVarContext current
  SectionIn RO
  SectionIn 1 2 #section is in installtype Full and Minimal
  ;ADD YOUR OWN FILES HERE...
  SetOutPath "$INSTDIR"
  File "${plex_root}\Plex\Plex.exe"
  File "${plex_root}\Plex\copying.txt"
  File "${plex_root}\Plex\LICENSE.GPL"
  File "${plex_root}\Plex\*.dll"
  SetOutPath "$INSTDIR\media"
  File /r /x *.so "${plex_root}\Plex\media\*.*"
  SetOutPath "$INSTDIR\sounds"
  File /r /x *.so "${plex_root}\Plex\sounds\*.*"
  SetOutPath "$INSTDIR\system"
  File /r /x *.so /x mplayer "${plex_root}\Plex\system\*.*"
  
  ; delete  msvc?90.dll's in INSTDIR, we use the vcredist installer later
  Delete "$INSTDIR\msvcr90.dll"
  Delete "$INSTDIR\msvcp90.dll"
  
  ;Turn off overwrite to prevent files in plex\userdata\ from being overwritten
  SetOverwrite off
  
  SetOutPath "$INSTDIR\userdata"
  File /r /x *.so  "${plex_root}\Plex\userdata\*.*"
  
  ;Turn on overwrite for rest of install
  SetOverwrite on
  
  SetOutPath "$INSTDIR\addons"
  File /r "${plex_root}\Plex\addons\*.*"
  ;SetOutPath "$INSTDIR\web"
  ;File /r "${plex_root}\Plex\web\*.*"

  ;Store installation folder
  WriteRegStr HKCU "Software\Plex" "" $INSTDIR

  ;Create uninstaller
  WriteUninstaller "$INSTDIR\Uninstall.exe"
  
  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
  ;Create shortcuts
  SetOutPath "$INSTDIR"
  
  ; delete old windowed link
  Delete "$SMPROGRAMS\$StartMenuFolder\Plex (Windowed).lnk"
  
  CreateDirectory "$SMPROGRAMS\$StartMenuFolder"
  CreateShortCut "$SMPROGRAMS\$StartMenuFolder\Plex.lnk" "$INSTDIR\Plex.exe" \
    "" "$INSTDIR\Plex.exe" 0 SW_SHOWNORMAL \
    "" "Start Plex."
  CreateShortCut "$SMPROGRAMS\$StartMenuFolder\Uninstall Plex.lnk" "$INSTDIR\Uninstall.exe" \
    "" "$INSTDIR\Uninstall.exe" 0 SW_SHOWNORMAL \
    "" "Uninstall Plex."
  
  WriteINIStr "$SMPROGRAMS\$StartMenuFolder\Visit Plex Online.url" "InternetShortcut" "URL" "http://plexapp.com"
  !insertmacro MUI_STARTMENU_WRITE_END  
  
  ;add entry to add/remove programs
  WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\Plex" \
                 "DisplayName" "Plex"
  WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\Plex" \
                 "UninstallString" "$INSTDIR\uninstall.exe"
  WriteRegDWORD HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\Plex" \
                 "NoModify" 1
  WriteRegDWORD HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\Plex" \
                 "NoRepair" 1
  WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\Plex" \
                 "InstallLocation" "$INSTDIR"
  WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\Plex" \
                 "DisplayIcon" "$INSTDIR\Plex.exe,0"
  WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\Plex" \
                 "Publisher" "Plex, Inc"
  WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\Plex" \
                 "HelpLink" "http://forums.plexapp.com"
  WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\Plex" \
                 "URLInfoAbout" "http://plexapp.com"
SectionEnd

SectionGroup "Language" SecLanguages
Section "English" SecLanguageEnglish
  SectionIn 1 2 #section is in installtype Full and Minimal
  SectionIn RO
  SetOutPath "$INSTDIR\language\English"
  File /r "${plex_root}\Plex\language\English\*.*"
SectionEnd
;languages.nsi is generated by genNsisIncludes.bat
!include /nonfatal "languages.nsi"
SectionGroupEnd

SectionGroup "Skins" SecSkins
Section "MediaStream" SecSkinMediaStream
  SectionIn 1 2 #section is in installtype Full and Minimal
  SectionIn RO
  SetOutPath "$INSTDIR\addons\skin.mediastream\"
  File /r "${plex_root}\Plex\addons\skin.mediastream\*.*"
SectionEnd
;skins.nsi is generated by genNsisIncludes.bat
!include /nonfatal "skins.nsi"
SectionGroupEnd

;SectionGroup "Scripts" SecScripts
;scripts.nsi is generated by genNsisIncludes.bat
!include /nonfatal "scripts.nsi"
;SectionGroupEnd

;SectionGroup "Plugins" SecPlugins
;plugins.nsi is generated by genNsisIncludes.bat
!include /nonfatal "plugins.nsi"
;SectionGroupEnd

;--------------------------------
;Descriptions

  ;Language strings
  LangString DESC_SecPlex ${LANG_ENGLISH} "Plex"

  ;Assign language strings to sections
  !insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${SecPlex} $(DESC_SecPlex)
  !insertmacro MUI_FUNCTION_DESCRIPTION_END

;--------------------------------
;Uninstaller Section

Var UnPageProfileDialog
Var UnPageProfileCheckbox
Var UnPageProfileCheckbox_State
Var UnPageProfileEditBox

Function un.UnPageProfile
    !insertmacro MUI_HEADER_TEXT "Uninstall Plex" "Remove Plex's profile folder from your computer."
  nsDialogs::Create /NOUNLOAD 1018
  Pop $UnPageProfileDialog

  ${If} $UnPageProfileDialog == error
    Abort
  ${EndIf}

  ${NSD_CreateLabel} 0 0 100% 12u "Do you want to delete the profile folder?"
  Pop $0

  ${NSD_CreateText} 0 13u 100% 12u "$APPDATA\Plex\"
  Pop $UnPageProfileEditBox
    SendMessage $UnPageProfileEditBox ${EM_SETREADONLY} 1 0

  ${NSD_CreateLabel} 0 46u 100% 24u "Leave unchecked to keep the profile folder for later use or check to delete the profile folder."
  Pop $0

  ${NSD_CreateCheckbox} 0 71u 100% 8u "Yes, also delete the profile folder."
  Pop $UnPageProfileCheckbox
  

  nsDialogs::Show
FunctionEnd

Function un.UnPageProfileLeave
${NSD_GetState} $UnPageProfileCheckbox $UnPageProfileCheckbox_State
FunctionEnd

Section "Uninstall"

  SetShellVarContext current

  ;ADD YOUR OWN FILES HERE...
  Delete "$INSTDIR\Plex.exe"
  Delete "$INSTDIR\copying.txt"
  Delete "$INSTDIR\known_issues.txt"
  Delete "$INSTDIR\LICENSE.GPL"
  Delete "$INSTDIR\glew32.dll"
  Delete "$INSTDIR\SDL.dll"
  Delete "$INSTDIR\zlib1.dll"
  Delete "$INSTDIR\plex.log"
  Delete "$INSTDIR\plex.old.log"
  Delete "$INSTDIR\msvcp71.dll"
  Delete "$INSTDIR\msvcr71.dll"
  RMDir /r "$INSTDIR\language"
  RMDir /r "$INSTDIR\media"
  RMDir /r "$INSTDIR\plugins"
  RMDir /r "$INSTDIR\scripts"
  RMDir /r "$INSTDIR\skin"
  RMDir /r "$INSTDIR\sounds"
  RMDir /r "$INSTDIR\system"
  RMDir /r "$INSTDIR\visualisations"
  RMDir /r "$INSTDIR\addons"
  RMDir /r "$INSTDIR\web"
  RMDir /r "$INSTDIR\cache"

  Delete "$INSTDIR\Uninstall.exe"
  
;Uninstall User Data if option is checked, otherwise skip
  ${If} $UnPageProfileCheckbox_State == ${BST_CHECKED}
    RMDir /r "$INSTDIR\userdata"  
    RMDir "$INSTDIR"
    RMDir /r "$APPDATA\Plex\"
  ${Else}
;Even if userdata is kept in %appdata%\plex\userdata, the $INSTDIR\userdata should be cleaned up on uninstall if not used
;If guisettings.xml exists in the Plex\userdata directory, do not delete Plex\userdata directory
;If that file does not exists, then delete that folder and $INSTDIR
    IfFileExists $INSTDIR\userdata\guisettings.xml +3
      RMDir /r "$INSTDIR\userdata"  
      RMDir "$INSTDIR"
  ${EndIf}

  
  !insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuFolder
  Delete "$SMPROGRAMS\$StartMenuFolder\Plex.lnk"
  Delete "$SMPROGRAMS\$StartMenuFolder\Plex (Portable).lnk"
  Delete "$SMPROGRAMS\$StartMenuFolder\Plex (Windowed).lnk"
  Delete "$SMPROGRAMS\$StartMenuFolder\Uninstall Plex.lnk"
  Delete "$SMPROGRAMS\$StartMenuFolder\Visit Plex Online.url"
  RMDir "$SMPROGRAMS\$StartMenuFolder"  
  DeleteRegKey HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\Plex"

  DeleteRegKey /ifempty HKCU "Software\Plex"

SectionEnd

;--------------------------------
;vs redist installer Section

Section "Microsoft Visual C++ 2010 Redistributable Package (x86)" SEC_VCREDIST

  SectionIn 1 2
  
  SetOutPath "$TEMP"
  File "${plex_root}\Plex\vcredist_x86.exe"
  DetailPrint "Running VS Redist Setup..."
  ExecWait '"$TEMP\vcredist_x86.exe" /q' $VSRedistSetupError
  DetailPrint "Finished VS Redist Setup"
 
  Delete "$TEMP\vcredist_x86.exe"
 
  SetOutPath "$INSTDIR"
SectionEnd

;--------------------------------
;DirectX webinstaller Section

!if "${plex_target}" == "dx"
Section "DirectX Install" SEC_DIRECTX
 
  SectionIn 1 2 RO

  DetailPrint "Running DirectX Setup..."

  SetOutPath "$TEMP\dxsetup"
  File "${plex_root}\..\dependencies\dxsetup\dsetup32.dll"
  File "${plex_root}\..\dependencies\dxsetup\DSETUP.dll"
  File "${plex_root}\..\dependencies\dxsetup\dxdllreg_x86.cab"
  File "${plex_root}\..\dependencies\dxsetup\DXSETUP.exe"
  File "${plex_root}\..\dependencies\dxsetup\dxupdate.cab"
  File "${plex_root}\..\dependencies\dxsetup\Jun2010_D3DCompiler_43_x86.cab"
  File "${plex_root}\..\dependencies\dxsetup\Jun2010_d3dx9_43_x86.cab"
  ExecWait '"$TEMP\dxsetup\dxsetup.exe" /silent' $DirectXSetupError
  RMDir /r "$TEMP\dxsetup"
  SetOutPath "$INSTDIR"

  DetailPrint "Finished DirectX Setup"
  
SectionEnd

Section "-Check DirectX installation" SEC_DIRECTXCHECK

  IfFileExists $SYSDIR\D3DX9_43.dll +2 0
    MessageBox MB_OK|MB_ICONSTOP|MB_TOPMOST|MB_SETFOREGROUND "DirectX9 wasn't installed properly.$\nPlease download the DirectX End-User Runtime from Microsoft and install it again."

SectionEnd
!endif