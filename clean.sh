#!/bin/sh

delete_dir() {
  if [ -d $1 ]; then
    echo "Deleting dir $1"
    rm -rf $1
  fi;
}

delete_dir_recursive() {
  echo "Deleting dir recursive $1"
  find . -type d -name $1 -exec rm -r {} \;
}


delete_file() {
  echo "Deleting file $1"
#  find . -type f -name $1
  find . -type f -name $1 -delete
}

if [ -f Makefile ]; then
	echo "make clean"
	make clean
fi


delete_file Makefile
delete_file CMakeCache.txt
delete_file cmake_install.cmake
delete_file CTestTestFile.cmake
delete_dir_recursive CMakeFiles
delete_dir_recursive Testing
delete_dir bin
delete_dir lib
delete_dir ext
