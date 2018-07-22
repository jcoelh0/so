#!/bin/sh

chmod +x /home/patricia/so1718-t02g3/sofs17/bin

./createDisk mnt 1000
./mksofs mnt
./sofsmount -d -l 0,1000 mnt test
