#!/bin/bash

i=0
end=0
start=0
if [ $# -lt 1 ]
then
echo "create [num]"
exit
fi
echo $1
num=$1
if [[ $num -lt 1000 ]]
then
 num=1000
fi
while [ ! $end -gt 160000 ]
do
#	echo "./test_run_shanghai_index "$start" >> log_"$start".txt" > "ishanghai"$i".sh" 
	echo "./tele_run_shanghai_index -i "$start > "ishanghai"$i".sh" 
chmod u+x "ishanghai"$i".sh"
start=`expr $start + $num`
let i++
end=`expr $start + $num`
done
