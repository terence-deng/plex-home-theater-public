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

copy "%PlexRoot%\copying.txt" "%DeployDir%" > NUL
copy "%PlexRoot%\known_issues.txt" "%DeployDir%" > NUL
copy "%PlexRoot%\LICENSE.GPL" "%DeployDir%" > NUL
copy "%PlexRoot%\Plex\Windows\winsparkle\WinSparkle-0.3\WinSparkle.dll" "%DeployDir%" > NUL
xcopy "%PlexRoot%\project\Win32BuildSetup\dependencies\*.*" "%DeployDir%" /Q /I /Y /EXCLUDE:exclude.txt  > NUL
rem xcopy "%PlexRoot%\project\Win32BuildSetup\vs_redistributable\vs2010\vcredist_x86.exe" "%DeployDir%" /Q /I /Y /EXCLUDE:exclude.txt  > NUL
xcopy "%PlexRoot%\userdata" "%DeployDir%\userdata" /E /Q /I /Y /EXCLUDE:exclude.txt > NUL
  
xcopy "%PlexRoot%\language" "%DeployDir%\language" /E /Q /I /Y /EXCLUDE:exclude.txt  > NUL
xcopy "%PlexRoot%\addons" "%DeployDir%\addons" /E /Q /I /Y /EXCLUDE:exclude.txt > NUL
xcopy "%PlexRoot%\system" "%DeployDir%\system" /E /Q /I /Y /EXCLUDE:exclude.txt  > NUL
xcopy "%PlexRoot%\media" "%DeployDir%\media" /E /Q /I /Y /EXCLUDE:exclude.txt  > NUL
xcopy "%PlexRoot%\sounds" "%DeployDir%\sounds" /E /Q /I /Y /EXCLUDE:exclude.txt > NUL

if exist exclude.txt del exclude.txt

rem ************************************
echo Building MediaStream Skin...
rem ************************************

REM SET build_path=%CD%
REM cd "%PlexRoot%\addons\skin.mediastream"
REM call build.bat > NUL
REM cd %build_path%

echo Creating MediaStream build folder...
set MediaStreamSourceDir=%PlexRoot%\addons\skin.mediastream
set MediaStreamDeployDir=%DeployDir%\addons\skin.mediastream

if not exist "%MediaStreamDeployDir%\Media" md "%MediaStreamDeployDir%\Media"

echo Creating XBT texture files...
echo Running command "%PlexRoot%\Tools\TexturePacker\TexturePacker.exe" -input "%MediaStreamSourceDir%\Media" -output "%MediaStreamDeployDir%\Media\Textures.xbt"
start "" /b /wait "%PlexRoot%\Tools\TexturePacker\TexturePacker.exe" -input "%MediaStreamSourceDir%\Media" -output "%MediaStreamDeployDir%\Media\Textures.xbt"
echo XBT texture files created...

echo ------------------------------------------------------------
echo MakeDrop Succeeded!

:End
endlocal