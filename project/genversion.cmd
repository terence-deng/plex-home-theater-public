@echo off

setlocal

set PlexRoot=..

for /F "tokens=1 delims=\ " %%a in ('call git rev-parse --short HEAD') do (
  set GitRevision=%%a
)

for /F "tokens=1,2 delims=\=;\ " %%a in ('findstr APPLICATION_VERSION "%PlexRoot%\Plex.xcodeproj\project.pbxproj"') do (
  set AppVersion=%%b
)

for /F "tokens=1,2,3,4 delims=." %%a in ("%AppVersion%") do (
  set AppMajorVersion=%%a
  set AppMinorVersion=%%b
  set AppBuildNumber=%%c
  set AppRevisionNumber=%%d
)

rem Major.Minor.Build.Revision

echo #pragma once
echo.
echo #define CURRENT_GIT_REVISION "%GitRevision%"
echo #define APPLICATION_VERSION "%AppVersion%"
echo #define FULL_APPLICATION_VERSION "%AppVersion%-%GitRevision%"
echo.
echo #define APPLICATION_MAJOR_VERSION_INT %AppMajorVersion%
echo #define APPLICATION_MINOR_VERSION_INT %AppMinorVersion%
echo #define APPLICATION_BUILD_NUMBER_INT %AppBuildNumber%
echo #define APPLICATION_REVISION_NUMBER_INT %AppRevisionNumber%

endlocal
