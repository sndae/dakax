rem "build SimpleOpenNI 32bit"
mkdir build32
cd build32
cmake -G "Visual Studio 9 2008" ^
-DNITE_LIBRARY="C:\Program Files (x86)\PrimeSense\NITE\Lib" ^
-DEIGEN3D_INCLUDE=C:\Users\Public\Documents\development\libs\graphics\3d\eigen\eigen-eigen-c40708b9088d ^
-DBOOST_ROOT=C:\Users\Public\Documents\development\libs\os\boost\boost_1_46_1 ^
..
cd ..