#!/bin/sh
# --------------------------------------------------------------------------
# buildscript for linux 64bit
# --------------------------------------------------------------------------
# DakaX
# http://code.google.com/p/dakax
# --------------------------------------------------------------------------
# prog:  Max Rheiner / Interaction Design / zhdk / http://iad.zhdk.ch/
# date:  06/11/2011 (m/d/y)
# ----------------------------------------------------------------------------
# Change those vars to the folders you have on your system:
#	-DEIGEN3D_INCLUDE 	= folder of Eigen3d headers
#	-DBOOST_ROOT 		= folder of Boost root
#	-DBOOST_LIBRARYDIR 	= folder of Boost library folder
#	-DP5_JAR 			= filepath to your core.jar (Processing)
# ----------------------------------------------------------------------------

# optional, but gives a clean build
rm -r build64

# check if build folder exists
if [ ! -d "build64" ]; then
    mkdir build64
fi

cd ./build64

echo "--- generate cmake ---"
# changes this according to your environment
cmake -DEIGEN3D_INCLUDE=/usr/include/eigen3/ \
	  -DBOOST_ROOT=~/Documents/development/libs/boost/boost_1_46_1/ \
	  -DBOOST_LIBRARYDIR=~/Documents/development/libs/boost/boost_1_46_1/stage/lib \
	  -DP5_JAR=~/Documents/localApps/processing-1.5.1/lib/core.jar \
	  ..


echo "--- build ---"
# build with 6 threads, verbose is optional, but otherwise you can't see the compiler directives
# make -j 6 VERBOSE=1
make -j 6

echo "--- copy ---"
# copy the library
cp DakaX.jar ~/sketchbook/libraries/DakaX/library/
cp libDakaX.so ~/sketchbook/libraries/DakaX/library/

cp DakaX.jar ../dist/DakaX/library/
cp libDakaX.so ../dist/DakaX/library/

# copy the doc
#cp -r ./doc/* ../dist/all/SimpleOpenNI/documentation/

