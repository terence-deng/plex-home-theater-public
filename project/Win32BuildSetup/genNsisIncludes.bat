@ECHO OFF
rem Plex for Windows install script
rem Copyright (C) 2005-2008 Team XBMC
rem http://xbmc.org

rem Script by chadoe
rem This script generates nullsoft installer include files for plex's languages, skins, scripts and plugins

rem languages
IF EXIST languages.nsi del languages.nsi > NUL
SETLOCAL ENABLEDELAYEDEXPANSION
SET Counter=1
FOR /F "tokens=*" %%S IN ('dir /B /AD BUILD_WIN32\Plex\language') DO (
  rem English is already included as default language
  IF "%%S" NEQ "English" (
    ECHO Section "%%S" SecLanguage!Counter! >> languages.nsi
    ECHO SectionIn 1 #section is in installtype Full >> languages.nsi
    ECHO SetOutPath "$INSTDIR\language\%%S" >> languages.nsi
    ECHO File /r "${plex_root}\Plex\language\%%S\*.*" >> languages.nsi
    ECHO SectionEnd >> languages.nsi
    SET /A Counter = !Counter! + 1
  )
)
ENDLOCAL

rem skins
rem IF EXIST skins.nsi del skins.nsi > NUL
rem SETLOCAL ENABLEDELAYEDEXPANSION
rem SET Counter=1
rem FOR /F "tokens=*" %%S IN ('dir /B /AD BUILD_WIN32\Plex\Skin') DO (
rem  rem Confluence and PM3.HD is already included as default skin
rem  echo %%S
rem  IF "%%S" NEQ "Confluence" IF "%%S" NEQ "PM3.HD" (
rem    ECHO Section "%%S" SecSkin!Counter! >> skins.nsi
rem    ECHO SectionIn 1 #section is in installtype Full >> skins.nsi
rem    ECHO SetOutPath "$INSTDIR\skin\%%S" >> skins.nsi
rem    ECHO File /r "${plex_root}\Plex\Skin\%%S\*.*" >> skins.nsi
rem    ECHO SectionEnd >> skins.nsi
rem    SET /A Counter = !Counter! + 1
rem  )
rem )
rem ENDLOCAL

rem scripts
IF EXIST scripts.nsi del scripts.nsi > NUL
IF EXIST BUILD_WIN32\Plex\scripts (
  SETLOCAL ENABLEDELAYEDEXPANSION
  SET Counter=1
  ECHO SectionGroup "Scripts" SecScripts >> scripts.nsi
  FOR /F "tokens=*" %%S IN ('dir /B /AD BUILD_WIN32\Plex\scripts') DO (
    ECHO Section "%%S" SecScript!Counter! >> scripts.nsi
    ECHO SectionIn 1 #section is in installtype Full >> scripts.nsi
    ECHO ${If} $PageProfileState == "0" >> scripts.nsi
    ECHO SetOverwrite off >> scripts.nsi
    ECHO CreateDirectory "$APPDATA\Plex\scripts\%%S" >> scripts.nsi
    ECHO SetOutPath "$APPDATA\Plex\scripts\%%S" >> scripts.nsi
    ECHO File /r "${plex_root}\Plex\scripts\%%S\*.*" >> scripts.nsi
    ECHO SetOverwrite on >> scripts.nsi  
    ECHO ${Else} >> scripts.nsi
    ECHO SetOutPath "$INSTDIR\scripts\%%S" >> scripts.nsi
    ECHO File /r "${plex_root}\Plex\scripts\%%S\*.*" >> scripts.nsi
    ECHO ${EndIf} >> scripts.nsi
    ECHO SectionEnd >> scripts.nsi
    SET /A Counter = !Counter! + 1
  )
  ECHO SectionGroupEnd >> scripts.nsi
  ENDLOCAL
)

rem plugins
IF EXIST plugins.nsi del plugins.nsi > NUL
IF EXIST BUILD_WIN32\Plex\plugins (
  ECHO SectionGroup "Plugins" SecPlugins >> plugins.nsi
  for %%a IN (Music Pictures Programs Video Weather) DO (
    IF EXIST BUILD_WIN32\Plex\plugins\%%a (
      SETLOCAL ENABLEDELAYEDEXPANSION
      SET Counter=1
      ECHO SectionGroup "%%a" SecPlugins%%a >> plugins.nsi
      FOR /F "tokens=*" %%S IN ('dir /B /AD BUILD_WIN32\Plex\plugins\%%a') DO (
        ECHO Section "%%S" SecPlugins%%a!Counter! >> plugins.nsi
        ECHO SectionIn 1 #section is in installtype Full >> plugins.nsi
        ECHO ${If} $PageProfileState == "0" >> plugins.nsi
        ECHO SetOverwrite off >> plugins.nsi
        ECHO CreateDirectory "$APPDATA\Plex\plugins\%%a\%%S" >> plugins.nsi
        ECHO SetOutPath "$APPDATA\Plex\plugins\%%a\%%S" >> plugins.nsi
        ECHO File /r "${plex_root}\Plex\plugins\%%a\%%S\*.*" >> plugins.nsi
        ECHO SetOverwrite on >> plugins.nsi
        ECHO ${Else} >> plugins.nsi
        ECHO SetOutPath "$INSTDIR\plugins\%%a\%%S" >> plugins.nsi
        ECHO File /r "${plex_root}\Plex\plugins\%%a\%%S\*.*" >> plugins.nsi
        ECHO ${EndIf} >> plugins.nsi
        ECHO SectionEnd >> plugins.nsi
        SET /A Counter = !Counter! + 1
      )
      ECHO SectionGroupEnd >> plugins.nsi
      ENDLOCAL
    )
  )
  ECHO SectionGroupEnd >> plugins.nsi
)