#!/bin/sh
pushd build/Release/Plex.app/Contents/Resources/Plex
rm -rf addons/metadata.*
rm -rf addons/skin.mediastream/.git
rm -rf addons/skin.confluence
cd addons/skin.mediastream
../../../../../../../../tools/TexturePacker/TexturePacker -input media
rm media/*
mv Textures.xbt media/