@echo off
set PlexRoot=..

if not exist "%PlexRoot%\system\players\dvdplayer" md "%PlexRoot%\system\players\dvdplayer"

copy /y "%PlexRoot%\xbmc\cores\dvdplayer\Codecs\ffmpeg\libavcodec\avcodec-53.dll" "%PlexRoot%\system\players\dvdplayer"
copy /y "%PlexRoot%\xbmc\cores\dvdplayer\Codecs\ffmpeg\libavformat\avformat-53.dll" "%PlexRoot%\system\players\dvdplayer"
copy /y "%PlexRoot%\xbmc\cores\dvdplayer\Codecs\ffmpeg\libavutil\avutil-51.dll" "%PlexRoot%\system\players\dvdplayer"
copy /y "%PlexRoot%\xbmc\cores\dvdplayer\Codecs\ffmpeg\libpostproc\postproc-51.dll" "%PlexRoot%\system\players\dvdplayer"
copy /y "%PlexRoot%\xbmc\cores\dvdplayer\Codecs\ffmpeg\libswscale\swscale-2.dll" "%PlexRoot%\system\players\dvdplayer"

if not exist "%PlexRoot%\lib\ffmpeg\libavutil" md "%PlexRoot%\lib\ffmpeg\libavutil"
copy /y "%PlexRoot%\xbmc\cores\dvdplayer\Codecs\ffmpeg\libavutil\avconfig.h" "%PlexRoot%\lib\ffmpeg\libavutil"
