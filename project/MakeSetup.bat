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

set PlexRoot=..\..
set BuildRoot=%PlexRoot%\Build
set DeployDir=%BuildRoot%\Windows\%TargetPlatform%\%TargetConfig%\Deploy

pushd Win32NSISSetup

rem ************************************
echo Compressing Plex.pdb...
rem ************************************
set PLEX_SYMBOLFILE=Plex-Media-Center-v%GIT_REV%-en-US-Symbols.zip
"%PlexRoot%\project\Win32BuildSetup\tools\7z\7za.exe" a  %PLEX_SYMBOLFILE% "%DeployDir%\Plex.pdb"

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

set PLEX_SETUPFILE=Plex-Media-Center-v%GIT_REV%-en-US.exe
if exist %PLEX_SETUPFILE% del /f %PLEX_SETUPFILE%

rem ************************************
echo Creating installer package %PLEX_SETUPFILE%...
rem ************************************

SET NSISExe=%NSISExePath%\makensis.exe
"%NSISExe%" /V1 /X"SetCompressor /FINAL lzma" /Dplex_dependency_root="%PlexRoot%\project\Win32BuildSetup\dependencies" /Dplex_vcredist_root="%PlexRoot%\project\Win32BuildSetup\vs_redistributable\vs2010" /Dplex_deploy_root="%DeployDir%" /Dplex_revision="%GIT_REV%" /Dplex_target="dx" "Plex for Windows.nsi"

popd

move /y "Win32NSISSetup\%PLEX_SYMBOLFILE%" . > NUL
move /y "Win32NSISSetup\%PLEX_SETUPFILE%" . > NUL
if not exist "%PLEX_SETUPFILE%" (
  echo Failed to create %PLEX_SETUPFILE%!
  goto :End
)
echo %PLEX_SETUPFILE% Created successfully :-)

echo ------------------------------------------------------------
echo Done!
echo Setup is located at %CD%\%PLEX_SETUPFILE%
echo ------------------------------------------------------------

:End
endlocal