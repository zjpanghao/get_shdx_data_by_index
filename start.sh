#!/bin/bash
export LD_LIBRARY_PATH=/usr/local/lib
i=0
while [ True ]
do
 if [ ! -f "ishanghai"$i".sh" ]
 then
  exit
 fi
 screen ./"ishanghai"$i".sh"
 let i++
done
