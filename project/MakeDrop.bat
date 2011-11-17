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
set CopyOnly=0

rem Parse command line args
for %%b in (%1, %2, %3, %4, %5) do (
  if "%%b"=="debug" set TargetConfig=Debug
  if "%%b"=="copyonly" set CopyOnly=1
)

set PlexRoot=..
set BuildRoot=%PlexRoot%\Build
set DeployDir=%BuildRoot%\Windows\%TargetPlatform%\%TargetConfig%\Deploy

set MediaStreamSourceDir=%PlexRoot%\addons\skin.mediastream
set MediaStreamDeployDir=%DeployDir%\addons\skin.mediastream

set LibBuildRoot=%PlexRoot%\project\VS2010Express\libs

rem ************************************
echo Creating drop for %TargetPlatform% - %TargetConfig%
rem ************************************

if not "%CopyOnly%" == "1" (
  rem ************************************
  echo Cleaning up deployment directory...
  rem ************************************

  if exist "%DeployDir%" rd /s /q "%DeployDir%"
  if exist "%DeployDir%" (
    echo FATAL ERROR - Could not delete %DeployDir%!
    goto :End
  )
  md "%DeployDir%"
)

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

if not exist "%DeployDir%\system\webserver" md "%DeployDir%\system\webserver"
xcopy "%PlexRoot%\lib\libmicrohttpd_win32\bin\*.dll" "%DeployDir%\system\webserver" /E /Q /I /Y

del /f exclude.txt

if not exist "%DeployDir%\system\python\Lib" md "%DeployDir%\system\python\Lib"
echo .svn>py_exclude.txt
echo test>>py_exclude.txt
echo plat->>py_exclude.txt
xcopy "%PlexRoot%\xbmc\lib\libPython\Python\Lib" "%DeployDir%\system\python\Lib" /E /Q /I /Y /EXCLUDE:py_exclude.txt

del /f py_exclude.txt

if not "%CopyOnly%" == "1" (

  rem ************************************
  echo Building MediaStream skin...
  echo Creating MediaStream build folder...
  rem ************************************

  if not exist "%MediaStreamDeployDir%\Media" md "%MediaStreamDeployDir%\Media"

  echo Creating XBT texture files...
  echo Running command "%PlexRoot%\Tools\TexturePacker\TexturePacker.exe" -input "%MediaStreamSourceDir%\Media" -output "%MediaStreamDeployDir%\Media\Textures.xbt"
  start "" /b /wait "%PlexRoot%\Tools\TexturePacker\TexturePacker.exe" -input "%MediaStreamSourceDir%\Media" -output "%MediaStreamDeployDir%\Media\Textures.xbt"

  if not exist "%MediaStreamDeployDir%\Media\Textures.xbt" (
    echo FATAL ERROR - Could not create Textures.xbt!
    goto :End
  )
  echo XBT texture files created...
)

echo ------------------------------------------------------------
echo MakeDrop Succeeded!

:End
endlocal