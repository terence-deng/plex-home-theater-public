#!/bin/sh

CONFIG=Release
VERSION=`python ./print-version.py`

rm -rf /tmp/Plex
mkdir -p /tmp/Plex
sh prep-release.sh

cp -R "build/${CONFIG}/Plex.app" "/tmp/Plex"
hdiutil create Plex-Laika-$VERSION-OSX.dmg -srcfolder /tmp/Plex -ov

# create changelog
TAGNAME=jenkins-snapshot
if git tag -l | grep $TAGNAME
then
	git log --pretty=oneline $TAGNAME... > Changes-$VERSION.txt
else
	git log --pretty=oneline -10 > Changes-$VERSION.txt
fi
git tag -f $TAGNAME

ssh plex-upload@10.0.42.250 mkdir -p /data/plex-nightlies/laika/$VERSION
scp Plex-Laika-$VERSION-OSX.dmg Changes-$VERSION.txt plex-upload@10.0.42.250:/data/plex-nightlies/laika/$VERSION
ssh plex-upload@10.0.42.250 /home/plex-upload/link-latest-laika.sh $VERSION

