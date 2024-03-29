#!/bin/sh
# --------------------------------------------------------------------------
# install script for linux
# --------------------------------------------------------------------------
# Processing Wrapper for the OpenNI/Kinect library
# http://code.google.com/p/simple-openni
# --------------------------------------------------------------------------
# prog:  Max Rheiner / Interaction Design / zhdk / http://iad.zhdk.ch/
# date:  06/11/2011 (m/d/y)
# ----------------------------------------------------------------------------
# Change P5_Path to the folder where Processing stores the libraries
# On linux it should be in '~/sketchbook' (Processing 1.5.1)
# ----------------------------------------------------------------------------

# copy the libs/doc/examples to the processing folders
P5_Path=~/sketchbook

# check if libraries folder exists
if [ ! -d $P5_Path/libraries ]; then
    mkdir $P5_Path/libraries
fi

# copie the files
cp -r ./dist/DakaX/  $P5_Path/libraries/

# remove all subversion folders
cd $P5_Path/libraries/DakaX
rm -rf `find . -type d -name .svn`

echo "--- installed DakaX ---"