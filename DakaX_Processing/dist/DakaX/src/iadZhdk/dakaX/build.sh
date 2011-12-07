#!/bin/bash

echo "----------------------------------------------"
echo "Build DakaX"
javac -classpath /home/max/Documents/localApps/processing-1.5.1/lib/core.jar:/home/max/Documents/localApps/processing-1.5.1/modes/java/libraries/serial/library/serial.jar  *.java  -d ./

echo "----------------------------------------------"
echo "Build jar"
jar cvf DakaX.jar iadZhdk

echo "----------------------------------------------"
echo "Copy lib to folder"
cp DakaX.jar ../../../library