cd "$(dirname $0)"

mkdir -p ../Projects/MacOS
cd ../Projects/MacOS

cmake ../../Source -G "Xcode" 									\
	-DDC_BUILD_TESTS=OFF 										\
	-DDC_WITH_RELIGHT=OFF 										\
	-DDC_QT_SUPPORT=Auto 										\
	-DDC_COMPOSER_ENABLED=OFF 									\
	-DJSONCPP_LIBRARY=../../Externals/lib/MacOS/libjsoncpp.a	\
	-DJSONCPP_INCLUDE_DIR=../../Externals/include/jsoncpp		\
	-DDC_USE_PCH=OFF