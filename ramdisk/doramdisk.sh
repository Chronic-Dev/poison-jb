#!/bin/bash
echo "Sending iBEC" &&
/usr/local/bin/irecovery -f iBEC.n72ap.RELEASE.dfu &&
/usr/local/bin/irecovery -c "go" &&
sleep 3 &&
echo "Sending DeviceTree" &&
/usr/local/bin/irecovery -f DeviceTree.n72ap.img3 &&
/usr/local/bin/irecovery -c "devicetree" &&
sleep 3 &&
echo "Sending Ramdisk" &&
/usr/local/bin/irecovery -f ramdisk.dmg &&
/usr/local/bin/irecovery -c "ramdisk" &&
sleep 3 &&
echo "Unplug and Replug" &&
read &&
echo "Sending kernelcache" &&
/usr/local/bin/irecovery -f kernelcache.release.n72 &&
echo "Kernelcache sent.. run bootx now!" &&
/usr/local/bin/irecovery -s
