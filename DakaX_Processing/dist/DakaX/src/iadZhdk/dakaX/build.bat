@ECHO OFF
ECHO ----------------------------------------------
ECHO Build DakaX

"C:\Program Files\Java\jdk1.6.0_23\bin\javac.exe" -classpath ^
C:\Users\Public\Documents\Apps\processing-1.5.1\lib\core.jar;^
C:\Users\Public\Documents\Apps\processing-1.5.1\modes\java\libraries\serial\library\serial.jar  ^
*.java  -d ./"

ECHO ----------------------------------------------
ECHO Build jar
"C:\Program Files\Java\jdk1.6.0_23\bin\jar.exe"  cvf DakaX.jar iadZhdk

ECHO ----------------------------------------------
ECHO Copy lib to folder
cp DakaX.jar ../../../library