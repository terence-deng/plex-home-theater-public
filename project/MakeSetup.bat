@echo OFF
rem ----Usage----
rem MakeSetup [release|debug]
rem
rem ----PURPOSE----
rem - Create a Plex setup package with a single click
rem -------------------------------------------------------------
rem

setlocal

set TargetPlatform=Win32
set TargetConfig=Release
if "%1"=="debug" (set TargetConfig=Debug)

call extract_git_rev.bat

rem ************************************
echo Creating setup package for %TargetPlatform%-%TargetConfig%, revision %GIT_REV%
rem ************************************

pushd Win32NSISSetup

set PlexRoot=..\..
set BuildRoot=%PlexRoot%\Build
set DeployDir=%BuildRoot%\Windows\%TargetPlatform%\%TargetConfig%\Deploy

set FixedTime=%time%
set FixedDate=%date%

set Year=%FixedDate:~-4%
set Month=%FixedDate:~4,2%
set Day=%FixedDate:~7,2%

set Hour=%FixedTime:~0,2%
set Minute=%FixedTime:~3,2%
set Second=%FixedTime:~6,2%

rem Fix first character of Hour being a space in the morning
if "%Hour:~0,1%" == " " (
  set Hour=0%Hour:~1,2%
)

set TimeStamp=%Year%-%Month%-%Day%--%Hour%-%Minute%-%Second%
set SetupDir=%BuildRoot%\Windows\%TargetPlatform%\%TargetConfig%\Setup\%TimeStamp%

rem ************************************
echo Signing drop binaries...
rem ************************************
call "%PlexRoot%\project\CodeSigning\Sign.cmd" "%DeployDir%\Plex.exe"
call "%PlexRoot%\project\CodeSigning\Sign.cmd" "%DeployDir%\glew32.dll"
call "%PlexRoot%\project\CodeSigning\Sign.cmd" "%DeployDir%\sdl.dll"
call "%PlexRoot%\project\CodeSigning\Sign.cmd" "%DeployDir%\WinSparkle.dll"
call "%PlexRoot%\project\CodeSigning\Sign.cmd" "%DeployDir%\zlib1.dll"

for %%f in ("%DeployDir%\system\*.dll") do (
  call "%PlexRoot%\project\CodeSigning\Sign.cmd" "%%f"
)

for %%f in ("%DeployDir%\system\cdrip\*.dll") do (
  call "%PlexRoot%\project\CodeSigning\Sign.cmd" "%%f"
)

for %%f in ("%DeployDir%\system\players\dvdplayer\*.dll") do (
  call "%PlexRoot%\project\CodeSigning\Sign.cmd" "%%f"
)

for %%f in ("%DeployDir%\system\players\paplayer\*.dll") do (
  call "%PlexRoot%\project\CodeSigning\Sign.cmd" "%%f"
)

for %%f in ("%DeployDir%\python\*.dll") do (
  call "%PlexRoot%\project\CodeSigning\Sign.cmd" "%%f"
)

for %%f in ("%DeployDir%\python\DLLs\*.pyd") do (
  call "%PlexRoot%\project\CodeSigning\Sign.cmd" "%%f"
)

for %%f in ("%DeployDir%\system\webserver\*.dll") do (
  call "%PlexRoot%\project\CodeSigning\Sign.cmd" "%%f"
)

rem ************************************
echo Generating installer includes...
rem ************************************
call genNsisIncludes.bat "%DeployDir%"

rem ************************************
echo Finding location of NSIS executable...
rem ************************************

REG QUERY HKLM\Software /f NSIS > nul
if not "%errorlevel%" == "0" goto :SkipRegistry

rem get path to makensis.exe from registry, first try tab delim
FOR /F "tokens=2* delims=	" %%A IN ('REG QUERY "HKLM\Software\NSIS" /ve') DO SET NSISExePath=%%B
IF NOT EXIST "%NSISExePath%" (
  rem try with space delim instead of tab
  FOR /F "tokens=2* delims= " %%A IN ('REG QUERY "HKLM\Software\NSIS" /ve') DO SET NSISExePath=%%B
)
IF NOT EXIST "%NSISExePath%" (
  rem fails on localized windows (Default) becomes (Par Défaut)
  FOR /F "tokens=3* delims=	" %%A IN ('REG QUERY "HKLM\Software\NSIS" /ve') DO SET NSISExePath=%%B
)
IF NOT EXIST "%NSISExePath%" (
  FOR /F "tokens=3* delims= " %%A IN ('REG QUERY "HKLM\Software\NSIS" /ve') DO SET NSISExePath=%%B
) 

:SkipRegistry

REG QUERY HKLM\Software\Wow6432Node /f NSIS > nul
if not "%errorlevel%" == "0" goto :SkipWow64Registry

rem proper Wow64 registry checks
IF NOT EXIST "%NSISExePath%" (
  FOR /F "tokens=2* delims=	" %%A IN ('REG QUERY "HKLM\Software\Wow6432Node\NSIS" /ve') DO SET NSISExePath=%%B
)
IF NOT EXIST "%NSISExePath%" (
  rem try with space delim instead of tab
  FOR /F "tokens=2* delims= " %%A IN ('REG QUERY "HKLM\Software\Wow6432Node\NSIS" /ve') DO SET NSISExePath=%%B
)
IF NOT EXIST "%NSISExePath%" (
  rem on win 7 x64, the previous fails
  FOR /F "tokens=3* delims=	" %%A IN ('REG QUERY "HKLM\Software\Wow6432Node\NSIS" /ve') DO SET NSISExePath=%%B
)
IF NOT EXIST "%NSISExePath%" (
  rem try with space delim instead of tab
  FOR /F "tokens=3* delims= " %%A IN ('REG QUERY "HKLM\Software\Wow6432Node\NSIS" /ve') DO SET NSISExePath=%%B
)

:SkipWow64Registry

IF NOT EXIST "%NSISExePath%\makensis.exe" (
  echo Failed to find location of MakeNSIS.exe! Best match was "%NSISExePath%"
  goto :End
)
echo Found MakeNSIS.exe at "%NSISExePath%"

set PLEX_SETUP_PACKAGE=Plex-Media-Center-v%GIT_REV%-en-US.exe
rem ************************************
echo Creating installer package %PLEX_SETUP_PACKAGE%...
rem ************************************

if exist "%PLEX_SETUP_PACKAGE%" del /f "%PLEX_SETUP_PACKAGE%"
set NSISExe=%NSISExePath%\makensis.exe
"%NSISExe%" /V1 /X"SetCompressor /FINAL lzma" /Dplex_dependency_root="%PlexRoot%\project\Win32BuildSetup\dependencies" /Dplex_vcredist_root="%PlexRoot%\project\Win32BuildSetup\vs_redistributable\vs2010" /Dplex_deploy_root="%DeployDir%" /Dplex_revision="%GIT_REV%" /Dplex_target="dx" "Plex for Windows.nsi"

if not exist "%PLEX_SETUP_PACKAGE%" (
  echo FATAL ERROR - Failed to create %PLEX_SETUP_PACKAGE%!
  goto :End
)
echo %PLEX_SETUP_PACKAGE% created successfully

rem ************************************
echo Signing setup package...
rem ************************************
call "%PlexRoot%\project\CodeSigning\Sign.cmd" "%PLEX_SETUP_PACKAGE%"

rem ************************************
echo Creating symbols package...
rem ************************************
set PLEX_SYMBOLS_PACKAGE=Plex-Media-Center-v%GIT_REV%-en-US-Symbols.zip
if exist "%PLEX_SYMBOLS_PACKAGE%" del /f "%PLEX_SYMBOLS_PACKAGE%"

if exist symbols-list.txt del /f symbols-list.txt
for /R "%DeployDir%" %%P in (*.pdb) do (
  echo %%P >> symbols-list.txt
)
"%PlexRoot%\project\Win32BuildSetup\tools\7z\7za.exe" a "%PLEX_SYMBOLS_PACKAGE%" @symbols-list.txt

if not exist "%PLEX_SYMBOLS_PACKAGE%" (
  echo FATAL ERROR - Failed to create symbols package!
  goto :End
)
del /f symbols-list.txt

rem ************************************
echo Moving packages to final location...
rem ************************************

if exist "%SetupDir%" rd /s /q "%SetupDir%"
md "%SetupDir%"

move /y "%PLEX_SYMBOLS_PACKAGE%" "%SetupDir%"
move /y "%PLEX_SETUP_PACKAGE%" "%SetupDir%"

pushd "%SetupDir%"
set AbsSetupDir=%CD%
popd

echo ------------------------------------------------------------
echo Setup packages created successfully:
echo %AbsSetupDir%
echo   %PLEX_SETUP_PACKAGE%
echo   %PLEX_SYMBOLS_PACKAGE%
echo ------------------------------------------------------------

popd

:End
endlocal