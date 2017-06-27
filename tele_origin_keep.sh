#!/bin/bash
while [ True ]
do
pid=`pidof tele_run_shanghai_index`
if [ -z $pid ]
then
 echo "start tele"
nohup ./tele_run_shanghai_index > log0.txt 2>&1 -k 192.168.1.107:9092 -i 0 -p 48 &
fi
sleep 30
done
