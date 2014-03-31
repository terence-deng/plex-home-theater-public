#!/bin/sh

# we need to exit with error if something goes wrong
set -ex

UPDATEFILE=$1
INSTALLPATH=/storage/.update
POST_UPDATE_PATH=/storage/.post_update.sh
TTL=5000 # how long to show notifications

post_update()
{
  if [ -f "$POST_UPDATE_PATH" ]; then
    chmod +x $POST_UPDATE_PATH 
    /bin/sh $POST_UPDATE_PATH
  fi
}

notify()
{
  /usr/bin/xbmc-send --port 9778 --action="Notification('$1', '$2', $TTL)"
}

abort()
{
  notify 'Update Aborted!' $1
  exit
}

trap 'abort "Update script bug : ("' SIGHUP SIGINT SIGTERM # Notify if aborted



notify 'Updating...' 'Update has started, system will reboot soon.'

# first we make sure to create the update path:
if [ ! -d $INSTALLPATH ]; then
	mkdir -p $INSTALLPATH
fi

notify 'Updating...' 'Checking archive integrity...'

# Grab KERNEL and SYSTEM path within tarball
CONTENTS=$(tar -tf $UPDATEFILE)
KERNEL=$(echo $CONTENTS | tr " " "\n" | grep KERNEL$)
SYSTEM=$(echo $CONTENTS | tr " " "\n" | grep SYSTEM$)
KERNELMD5=$(echo $CONTENTS | tr " " "\n"  | grep KERNEL.md5)
SYSTEMMD5=$(echo $CONTENTS | tr " " "\n" | grep SYSTEM.md5)
POST_UPDATE=$(echo $CONTENTS | tr " " "\n" | grep post_update.sh)

[ -z "$KERNEL" ] && abort 'Invalid archive - no kernel.'
[ -z "$KERNELMD5" ] && abort 'Invalid archive - no kernel check.'
[ -z "$SYSTEM" ] && abort 'Invalid archive - no system.'
[ -z "$SYSTEMMD5" ] && abort 'Invalid archive - no system check.'

notify 'Updating...' 'Beginning extraction.'

# untar both SYSTEM and KERNEL into installation directory
tar -xf $UPDATEFILE -C $INSTALLPATH  $KERNEL $SYSTEM $KERNELMD5 $SYSTEMMD5

notify 'Updating...' 'Finished extraction, validating checksums.'

if [ -f "$POST_UPDATE" ];then
  notify 'Running post update script'
  cp $POST_UPDATE $POST_UPDATE_PATH
  post_update
  notify 'Post-update complete!'
fi

kernel_check=`/bin/md5sum $KERNEL | awk '{print $1}'`
system_check=`/bin/md5sum $SYSTEM | awk '{print $1}'`

kernelmd5=`cat $KERNELMD5 | awk '{print $1}'`
systemmd5=`cat $SYSTEMMD5 | awk '{print $1}'`

[ "$kernel_check" != "$kernelmd5" ] && abort 'Kernel checksum mismatch'
[ "$system_check" != "$systemmd5" ] && abort 'System checksum mismatch'

notify 'Updating...' 'Checksums valid! Cleaning up...'
# move extracted files to the toplevel
cd $INSTALLPATH
mv $KERNEL $SYSTEM $KERNELMD5 $SYSTEMMD5 .

# remove the directories created by tar
rm -r */
rm $UPDATEFILE
