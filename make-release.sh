#!/bin/sh
rm -rf build/Release/Plex.app
/Developer/usr/bin/xcodebuild -project Plex.xcodeproj
./prep-release.sh