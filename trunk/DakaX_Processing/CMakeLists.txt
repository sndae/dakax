# -----------------------------------------------------------------------------
# DakaX 
# -----------------------------------------------------------------------------
# Processing Library for DakaX
# prog:  Max Rheiner / Interaction Design / zhdk / http://iad.zhdk.ch/
# -----------------------------------------------------------------------------
#
# -----------------------------------------------------------------------------
	
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

INCLUDE(CheckIncludeFiles)

PROJECT(DakaX)

IF(NOT MACH_ARCH)
    # default 64bit
    SET(MACH_ARCH "64")
ENDIF()

IF(WIN32)
	SET(TPN_WIN32 "/EHsc")
ENDIF()
		  
# -----------------------------------------------------------------------------
# check swig
FIND_PACKAGE(SWIG REQUIRED)
INCLUDE(${SWIG_USE_FILE})

# -----------------------------------------------------------------------------
# check java/jni
FIND_PACKAGE(Java REQUIRED)
FIND_PACKAGE(JNI REQUIRED)
INCLUDE_DIRECTORIES(${JAVA_INCLUDE_PATH})
INCLUDE_DIRECTORIES(${JAVA_INCLUDE_PATH2})
 
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
 
# -----------------------------------------------------------------------------
# java tools
IF(WIN32)
	SET(JAVA_RUNTIME2  $ENV{JAVA_HOME}/bin/java)
	SET(JAVA_ARCHIVE2  $ENV{JAVA_HOME}/bin/jar)
	SET(JAVA_COMPILE2  $ENV{JAVA_HOME}/bin/javac)
	SET(JAVA_DOC	   $ENV{JAVA_HOME}/bin/javadoc)
ELSE()
	# unix
	SET(JAVA_RUNTIME2  java)
	SET(JAVA_ARCHIVE2  jar)
	SET(JAVA_COMPILE2  javac)
	SET(JAVA_DOC	   javadoc)
ENDIF()

# -----------------------------------------------------------------------------
# boost
FIND_PACKAGE(Boost REQUIRED)
#FIND_PACKAGE(Boost COMPONENTS thread system filesystem REQUIRED)
IF(Boost_INCLUDE_DIRS MATCHES "Boost_INCLUDE_DIR-NOTFOUND")
	MESSAGE("Boost should be set, for example:")
	MESSAGE("> cmake -DBOOST_ROOT=/usr/??? ..")	
ELSE()
	INCLUDE_DIRECTORIES ( ${Boost_INCLUDE_DIRS} )
	LINK_DIRECTORIES ( ${Boost_LIBRARY_DIRS} )
	message(${Boost_INCLUDE_DIRS})
	message(${Boost_LIBRARY_DIRS})
  
	IF(APPLE)
		# osx
		set(Boost_LIBRARIES  "boost_thread.a" "boost_system.a")
	ELSEIF(UNIX)
		# unix/linux,shared libs of boost should be installed
		set(Boost_LIBRARIES  "boost_thread.a" "boost_system.a")
	ELSE()
		# windows will find the right libs by itself
		#set(Boost_LIBRARIES  "libboost_thread" "libboost_system")
	ENDIF()
ENDIF()
# -----------------------------------------------------------------------------
# swig 
SET(CMAKE_SWIG_FLAGS -package iadZhdk.dakaX)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include)


# -----------------------------------------------------------------------------
# processing wrappers
SET(P5_WRAPPER ${CMAKE_CURRENT_SOURCE_DIR}/src/p5_src/)
SET(P5_WRAPPER_FILE ${CMAKE_CURRENT_SOURCE_DIR}/src/p5_src/DakaX.java)

# processing core lib
# shoud use find_program
IF(WIN32)
	SET(P5_JAR "C:/Program Files (x86)/processing-1.2.1/lib/core.jar")	
ELSEIF(APPLE)
	SET(P5_JAR "/Applications/Processing.app/Contents/Resources/Java/core.jar")
ELSE()
	# unix/linux
	# SET(P5_JAR "/?")
	IF(NOT DEFINED P5_JAR)
		MESSAGE("Set P5_JAR to the folder where Processing installed the core.jar file, for example:")
		MESSAGE(FATAL_ERROR "> cmake -DP5_JAR=/usr/???/core.jar ..")	
	ENDIF()
ENDIF()

# -----------------------------------------------------------------------------
# eigen 3d template lib
IF(DEFINED EIGEN3D_INCLUDE)
	INCLUDE_DIRECTORIES(${EIGEN3D_INCLUDE})
ELSE()
	# check if the envvar is set
	SET(EIGEN3D_INCLUDE $ENV{EIGEN3D_INCLUDE})
	IF(DEFINED EIGEN3D_INCLUDE)
		INCLUDE_DIRECTORIES(${EIGEN3D_INCLUDE})
	ELSE()
		MESSAGE("EIGEN3D_INCLUDE should be set, for example:")
		MESSAGE("> cmake -DEIGEN3D_INCLUDE=/usr/??? ..")	
	ENDIF()
ENDIF()	



SET(SWIG_SOURCES 
src/DakaX.i
src/DakaX.cpp
src/AsyncSerial.cpp
)

SET_SOURCE_FILES_PROPERTIES(${SWIG_SOURCES} PROPERTIES CPLUSPLUS ON)

# set the folder where the swig files should land
SET(CMAKE_SWIG_OUTDIR ${CMAKE_CURRENT_BINARY_DIR}/swig_src)
MAKE_DIRECTORY(${CMAKE_CURRENT_BINARY_DIR}/src)


SWIG_ADD_MODULE(DakaX Java ${SWIG_SOURCES})
SWIG_LINK_LIBRARIES(DakaX ${JAVA_LIBRARIES} ${JNI_LIBRARIES} ${Boost_LIBRARIES} ${LIBS})
message("boost libs:" ${Boost_LIBRARIES})

ADD_CUSTOM_COMMAND(TARGET DakaX
				   POST_BUILD
				   COMMAND cmake -E echo "-----------------------------"
				   COMMAND cmake -E echo "Compiling Java files..."
				   #COMMAND cmake -E make_directory ./DakaX
				   COMMAND cmake -E make_directory ${CMAKE_SWIG_OUTDIR}
				   #COMMAND cmake -E copy_directory ${P5_WRAPPER} ${CMAKE_SWIG_OUTDIR}
				   # cmake copy doesn't support wildcards, otherwise it just copies the hidden folder of svn, etc.
				   # doesn't works on windows backslash
				   COMMAND cp "${P5_WRAPPER}*.java" "${CMAKE_SWIG_OUTDIR}"
				   #COMMAND cmake -E copy ${P5_WRAPPER_FILE} ${CMAKE_SWIG_OUTDIR}
				   COMMAND ${JAVA_COMPILE2} -classpath ${P5_JAR} ${CMAKE_SWIG_OUTDIR}/*.java -d ./
				   COMMAND cmake -E echo "-----------------------------"
				   COMMAND cmake -E echo "Creating jar file..."
				   COMMAND ${JAVA_ARCHIVE2} cvf DakaX.jar iadZhdk
				   COMMAND cmake -E echo "-----------------------------"
				   COMMAND cmake -E echo "Creating doc files..."
				   COMMAND cmake -E make_directory ./doc
				   #COMMAND ${JAVA_DOC} -quiet -author -public -nodeprecated -nohelp -d ./doc  -version ${P5_WRAPPER}/*.java -version ${CMAKE_SWIG_OUTDIR}/ContextWrapper.java)
				   COMMAND ${JAVA_DOC} -quiet -author -public -nodeprecated -nohelp -d ./doc  -version ${CMAKE_SWIG_OUTDIR}/*.java)
