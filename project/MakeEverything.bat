@echo OFF
rem ----Usage----
rem MakeEverything [release|debug]
rem
rem ----PURPOSE----
rem - Create a Plex release with a single click
rem -------------------------------------------------------------
rem

setlocal

set TargetConfig=Release
if "%1"=="debug" (set TargetConfig=Debug)

pushd BuildDependencies
call DownloadBuildDeps.bat
popd

call MakeDrop.bat %TargetConfig%

msbuild -t:Rebuild -p:Configuration=%TargetConfig%;Platform=Win32 "VS2010Express\Plex for Windows.sln" 

pushd CodeSigning
call InstallCA.cmd
popd

call MakeSetup.bat %TargetConfig%

endlocal
