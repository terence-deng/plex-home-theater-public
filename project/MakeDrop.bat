@echo OFF
rem ----Usage----
rem MakeDrop [release|debug]
rem
rem ----PURPOSE----
rem - Create a Plex drop directory with a single click
rem -------------------------------------------------------------
rem

setlocal

set TargetPlatform=Win32
set TargetConfig=Release
if "%1"=="debug" (set TargetConfig=Debug)

set PlexRoot=..
set BuildRoot=%PlexRoot%\Build
set DeployDir=%BuildRoot%\Windows\%TargetPlatform%\%TargetConfig%\Deploy

set LibBuildRoot=%PlexRoot%\project\VS2010Express\libs

rem ************************************
echo Creating drop for %TargetPlatform% - %TargetConfig%
rem ************************************

rem ************************************
echo Cleaning up deployment directory...
rem ************************************

if exist "%DeployDir%" rd /s /q "%DeployDir%"
if exist "%DeployDir%" (
  echo FATAL ERROR - Could not delete %DeployDir%!
  goto :End
)
md "%DeployDir%"

rem ************************************
echo Preparing exclusion file...
rem ************************************

echo .svn>exclude.txt
echo CVS>>exclude.txt
echo .so>>exclude.txt
echo Thumbs.db>>exclude.txt
echo Desktop.ini>>exclude.txt
echo dsstdfx.bin>>exclude.txt
echo exclude.txt>>exclude.txt
rem and exclude potential leftovers
echo mediasources.xml>>exclude.txt
echo advancedsettings.xml>>exclude.txt
echo guisettings.xml>>exclude.txt
echo profiles.xml>>exclude.txt
echo sources.xml>>exclude.txt
echo userdata\cache\>>exclude.txt
echo userdata\database\>>exclude.txt
echo userdata\playlists\>>exclude.txt
echo userdata\script_data\>>exclude.txt
echo userdata\thumbnails\>>exclude.txt
rem UserData\visualisations contains currently only xbox visualisationfiles
echo userdata\visualisations\>>exclude.txt
rem other platform stuff
echo lib-osx>>exclude.txt
echo players\mplayer>>exclude.txt
echo FileZilla Server.xml>>exclude.txt
echo asound.conf>>exclude.txt
echo voicemasks.xml>>exclude.txt
echo Lircmap.xml>>exclude.txt
echo addons\skin.confluence>>exclude.txt
echo addons\skin.mediastream\media\>>exclude.txt

rem ************************************
echo Copying static files into deployment directory...
rem ************************************

copy "%PlexRoot%\copying.txt" "%DeployDir%"

rem This file doesn't appear to exist at the moment, so make it optional
if exist "%PlexRoot%\known_issues.txt" copy "%PlexRoot%\known_issues.txt" "%DeployDir%"

copy "%PlexRoot%\LICENSE.GPL" "%DeployDir%"
copy "%PlexRoot%\Plex\Windows\winsparkle\WinSparkle-0.3\WinSparkle.dll" "%DeployDir%"
xcopy "%PlexRoot%\project\Win32BuildSetup\dependencies\*.*" "%DeployDir%" /Q /I /Y /EXCLUDE:exclude.txt
xcopy "%PlexRoot%\userdata" "%DeployDir%\userdata" /E /Q /I /Y /EXCLUDE:exclude.txt
  
xcopy "%PlexRoot%\language" "%DeployDir%\language" /E /Q /I /Y /EXCLUDE:exclude.txt
xcopy "%PlexRoot%\addons" "%DeployDir%\addons" /E /Q /I /Y /EXCLUDE:exclude.txt
xcopy "%PlexRoot%\system" "%DeployDir%\system" /E /Q /I /Y /EXCLUDE:exclude.txt
xcopy "%PlexRoot%\media" "%DeployDir%\media" /E /Q /I /Y /EXCLUDE:exclude.txt
xcopy "%PlexRoot%\sounds" "%DeployDir%\sounds" /E /Q /I /Y /EXCLUDE:exclude.txt

if exist exclude.txt del /f exclude.txt

rem ************************************
echo Building MediaStream skin...
echo Creating MediaStream build folder...
rem ************************************

set MediaStreamSourceDir=%PlexRoot%\addons\skin.mediastream
set MediaStreamDeployDir=%DeployDir%\addons\skin.mediastream

if not exist "%MediaStreamDeployDir%\Media" md "%MediaStreamDeployDir%\Media"

echo Creating XBT texture files...
echo Running command "%PlexRoot%\Tools\TexturePacker\TexturePacker.exe" -input "%MediaStreamSourceDir%\Media" -output "%MediaStreamDeployDir%\Media\Textures.xbt"
start "" /b /wait "%PlexRoot%\Tools\TexturePacker\TexturePacker.exe" -input "%MediaStreamSourceDir%\Media" -output "%MediaStreamDeployDir%\Media\Textures.xbt"

if not exist "%MediaStreamDeployDir%\Media\Textures.xbt" (
  echo FATAL ERROR - Could not create Textures.xbt!
  goto :End
)
echo XBT texture files created...

echo ------------------------------------------------------------
echo MakeDrop Succeeded!

:End
endlocal