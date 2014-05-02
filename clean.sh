#!/bin/sh
if [ -f Makefile ]; then
	echo "make clean"
	make clean
    echo ""
fi


echo "Deleting Makefiles"
find . -type f -name Makefile
find . -type f -name Makefile -delete

echo ""
echo "Deleting Testing dirs"
find . -type d -name Testing -exec rm -r {} \;

echo ""
echo "Deleting CMakeFiles dirs"
#find . -type d -name CMakeFiles
find . -type d -name CMakeFiles -exec rm -r {} \;

echo ""
echo "Deleting CMakeCache.txt"
find . -type f -name CMakeCache.txt
find . -type f -name CMakeCache.txt -delete

echo ""
echo "Deleting misc cmake files"
find . -type f -name cmake_install.cmake
find . -type f -name cmake_install.cmake -delete
find . -type f -name CTestTestfile.cmake
find . -type f -name CTestTestfile.cmake -delete

if [ -d bin ]; then 
	echo ""
	echo "Deleting bin dir"
	rm -rf bin
fi	

if [ -d lib ]; then 
	echo ""
	echo "Deleting lib dir"
	rm -rf lib
fi	
