#!/bin/bash
dirs=`ls`
for dir in $dirs
do
  if [ -d $dir ] && [ ${dir:0:3} = "log" ]
  then
	cd $dir
	files=`ls`
	for file in $files
	do
	  echo $file
	  cat /dev/null > $file
	done
	cd ..
  fi
done
