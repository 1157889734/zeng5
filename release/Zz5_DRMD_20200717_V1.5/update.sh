#!/bin/bash
if [ -e /root/update.zip ]
then
	cd /root/
	rm -fr /root/config
	rm -fr /root/fonts
	rm -fr /root/media
	rm -fr /root/drmd
	rm -fr /root/main
	rm -fr /root/hiEnvBin
	rm -fr /root/qt-run-env.sh
	rm -fr /root/taskset.sh
	rm -fr /root/del.sh
	rm -fr /root/process.sh		
	rm -fr /root/start.sh
	rm -fr /root/start1.sh
	rm -fr /root/nfs.sh
		
	sleep 2
	unzip -o  update.zip
	echo "update update.zip"
	
	sleep 2
	chmod 777 drmd
	chmod 777 main
	chmod 777 hiEnvBin
	rm -fr /root/update.zip
else
	echo "no update file"
fi

if [ -e /root/update.tar.gz ]
then
	cd /root/
	rm -fr /root/config
	rm -fr /root/fonts
	rm -fr /root/media
	rm -fr /root/drmd
	rm -fr /root/main
	rm -fr /root/hiEnvBin
	rm -fr /root/qt-run-env.sh
	rm -fr /root/taskset.sh
	rm -fr /root/del.sh
	rm -fr /root/process.sh
	rm -fr /root/start.sh
	rm -fr /root/start1.sh
	rm -fr /root/nfs.sh
	sleep 2
	tar -xzvf update.tar.gz
	echo "update update.tar.gz"
	sleep 2
	chmod 777 drmd
	chmod 777 main
	chmod 777 hiEnvBin
	rm -fr /root/update.tar.gz
else
	echo "no update file"
fi

if [ -d /root/tslib/ ]
then
	cd /root/
	cp -fr /root/tslib/ /opt/
	sleep 1
	rm -rf tslib/
else
	echo "no tslib dir"
fi

sh /root/start.sh


