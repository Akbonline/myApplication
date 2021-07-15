# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.10.2)

# Declares and names the project.

project("myapplication")

include_directories("./hello_world/")
include_directories("./NFIQ2/")
add_subdirectory("./hello_world" "/Users/akshatbajpai/Desktop/Work/MyApplication/app/src/main/cpp/")
add_subdirectory("./NFIQ2" "/Users/akshatbajpai/Desktop/Work/MyApplication/app/src/main/cpp/")	# We provide the absolute path

# -----------------------Updated Changes--------------------------

#include_directories(
#		include/algorithm				<
#		include/deps/msvc/sys
#		include/features				<
#		include/opencv					<
#		include/opencv2/imgproc			<
#		include/opencv2/core			<
#		include/opencv2/ml				<
#		include/opencv2					<
#		include/prediction				<
#		include/wsq						<
#		include)						<

include_directories(
		"./NFIQ2/NFIQ2/NFIQ2Algorithm/"
		"./NFIQ2/opencv/"
		"./NFIQ2/NFIQ2/NFIQ2Algorithm/src/features/"
		"./NFIQ2/opencv/modules/ml/"
		"./NFIQ2/opencv/modules/features2d/"
		"./NFIQ2/opencv/modules/imageproc/"
		"./NFIQ2/opencv/modules/core/"
		"./NFIQ2/NFIQ2/NFIQ2Algorithm/include/features/"
		"./NFIQ2/libbiomeval/nbis/lib/wsq/"
		"./NFIQ2/NFIQ2/NFIQ2Algorithm/src/prediction/"
		"./NFIQ2/NFIQ2/NFIQ2Algorithm/include/prediction"
)

#file(GLOB_RECURSE SOURCES
#		"src/features/*.cpp"
#		"src/opencv/core/*.cpp"
#		"src/opencv/core/*.hpp"
#		"src/opencv/imgproc/*.cpp"
#		"src/opencv/imgproc/*.h"
#		"src/opencv/ml/*.cpp"
#		"src/opencv/ml/*.hpp"
#		"src/prediction/*.cpp"
#		"src/wsq/*.cpp"
#		"src/*.cpp"
#		"src/*.c")
file(GLOB_RECURSE SOURCES
		"./NFIQ2/NFIQ2/NFIQ2Algorithm/src/features/*.cpp"
		"./NFIQ2/opencv/modules/core/*.cpp"
		"./NFIQ2/opencv/modules/core/*.hpp"
		"./NFIQ2/opencv/modules/imageproc/*.cpp"
		"./NFIQ2/opencv/modules/imageproc/*.h"
		"./NFIQ2/opencv/modules/ml/*.cpp"
		"./NFIQ2/opencv/modules/ml/*.hpp"
		"./NFIQ2/NFIQ2/NFIQ2Algorithm/src/prediction/*.cpp"
		"./NFIQ2/NFIQ2/NFIQ2Algorithm/include/prediction/*.cpp"
		"./NFIQ2/libbiomeval/nbis/lib/wsq/*.cpp"
		"./*.cpp"
		"./*.c"
		)
#---------------------------------^^^^----------------------------

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

add_library( # Sets the name of the library.
             native-lib

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             native-lib.cpp
		${SOURCES})

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log)

find_library( # Sets the name of the path variable.
		android-lib

		# Specifies the name of the NDK library that
		# you want CMake to locate.
		"./NFIQ2/opencv/modules/java/android_sdk/")
find_package(ZLIB)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
                       native-lib

                       # Links the target library to the log library
                       # included in the NDK.
                       ${log-lib}
			hello_world
		${NFIQ2_SUPERBUILD}
		${ZLIB_LIBRARIES}
		${android-lib}
		-ljnigraphics
			)	# Try NFIQ2_SUPERBUILD if not!
