pushd build/Release/Plex.app/Contents/Resources/Plex/addons
rm -rf skin.confluence
cd skin.mediastream
../../../../../../../../tools/TexturePacker/TexturePacker -input media
rm media/*
rm -rf media-lite
mv Textures.xbt media
rm -rf .git
popd