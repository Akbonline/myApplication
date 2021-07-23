# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.10.2)

# Declares and names the project.

project("myapplication")

#include_directories("./hello_world/")
include_directories("./NFIQ_2/")
include_directories("./NFIQ_2/NFIQ2/NFIQ2Algorithm/")
include_directories("./NFIQ_2/NFIR/")
include_directories("./NFIQ_2/opencv/")
include_directories("./NFIQ_2/libbiomeval/")
include_directories("./NFIQ_2/fingerjetfxose")
include_directories("./NFIQ_2/digestpp")
include_directories("./NFIQ_2/conformance")
#add_subdirectory("./hello_world" "/Users/akshatbajpai/Desktop/Work/MyApplication/app/src/main/cpp/")
#add_subdirectory("./NFIQ_2" "/Users/akshatbajpai/Desktop/Work/MyApplication/app/src/main/cpp/")	# We provide the absolute path

# -----------------------Updated Changes--------------------------

#include_directories(
#		include/algorithm
#		include/deps/msvc/sys
#		include/features
#		include/opencv
#		include/opencv2/imgproc
#		include/opencv2/core
#		include/opencv2/ml
#		include/opencv2
#		include/prediction
#		include/wsq
#		include)

include_directories(		# trying to individually importing all the subdirectories
		"./NFIQ_2/NFIQ2/NFIQ2Algorithm/"
		"./NFIQ_2/opencv/"
		"./NFIQ_2/fingerjetfxose/FingerJetFXOSE/libFJFX/include"
		"./NFIQ_2/NFIQ2/NFIQ2Algorithm/src/features/"
		"./NFIQ_2/opencv/modules/ml/"
		"./NFIQ_2/opencv/modules/features2d/"
		"./NFIQ_2/opencv/modules/imageproc/"
		"./NFIQ_2/opencv/modules/core/"		# TRY: Exact location for the header file
		"./NFIQ_2/NFIQ2/NFIQ2Algorithm/include/features/"
		"./NFI_2/libbiomeval/nbis/lib/wsq/"
		"./NFIQ_2/NFIQ2/NFIQ2Algorithm/src/prediction/"
		"./NFIQ_2/NFIQ2/NFIQ2Algorithm/include/prediction"
		"./NFIQ_2/opencv/modules/core/include/opencv2/core/cuda/utility.hpp"
		"./NFIQ_2/NFIQ2/NFIQ2Algorithm/include/features/FeatureFunctions.h"
		"./NFIQ_2/NFIQ2/NFIQ2Algorithm/include/features/Module.h"
		"./NFIQ_2/NFIQ2/NFIQ2Algorithm/include/features/ImgProcROIFeature.h"
		"./NFIQ_2/NFIQ2/NFIQ2Algorithm/include/features/MuFeature.h"
		"./NFIQ_2/NFIQ2/NFIQ2Algorithm/include/features/FDAFeature.h"
		"./NFIQ_2/NFIQ2/NFIQ2Algorithm/include/features/FJFXMinutiaeQualityFeatures.h"
		"./NFIQ_2/NFIQ2/NFIQ2Algorithm/include/features/FingerJetFXFeature.h"
		# TRY: Find and verify the location and append here
)
#include(NFIQ_2/CMakeLists.txt)		#included the nested CMakeLists(Failed:
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

file(   GLOB_RECURSE SOURCES
		"./NFIQ_2/NFIQ2/NFIQ2Algorithm/src/features/*.cpp"
		"./NFIQ_2/"
		"./NFIQ_2/opencv/modules/core/*.cpp"
		"./NFIQ_2/opencv/modules/core/*.hpp"
		"./NFIQ_2/opencv/modules/imageproc/*.cpp"
		"./NFIQ_2/opencv/modules/imageproc/*.h"
		"./NFIQ_2/opencv/modules/ml/*.cpp"
		"./NFIQ_2/opencv/modules/ml/*.hpp"
		"./NFIQ_2/NFIQ2/NFIQ2Algorithm/src/prediction/*.cpp"
		"./NFIQ_2/NFIQ2/NFIQ2Algorithm/include/prediction/*.cpp"
		"./NFIQ_2/libbiomeval/nbis/lib/wsq/*.cpp"
		"./NFIQ_2/NFIQ2/NFIQ2Algorithm/*.cpp"
		"./NFIQ_2/NFIQ2/NFIQ2Api/*.cpp"
		"./*.cpp"
		"./*.c"
		"./NFIQ_2/opencv/3rdparty/openexr/*cpp"
		"./NFIQ_2/NFIQ2/NFIQ2Algorithm/include/features/FeatureFunctions.h"
		"./NFIQ_2/NFIQ2/NFIQ2Algorithm/include/features/FingerJetFXFeature.h"
		"./NFIQ_2/opencv/3rdparty/openexr/llmlmf"
		"./NFIQ_2/opencv/modules/core/include/opencv2/core/cuda/utility.hpp"
		"./NFIQ_2/NFIQ2/NFIQ2Algorithm/include/features/FeatureFunctions.h"
		"./NFIQ_2/NFIQ2/NFIQ2Algorithm/include/features/Module.h"
		"./NFIQ_2/NFIQ2/NFIQ2Algorithm/include/features/ImgProcROIFeature.h"
		"./NFIQ_2/NFIQ2/NFIQ2Algorithm/include/features/MuFeature.h"
		"./NFIQ_2/NFIQ2/NFIQ2Algorithm/include/features/FDAFeature.h"
		"./NFIQ_2/NFIQ2/NFIQ2Algorithm/include/features/FJFXMinutiaeQualityFeatures.h"
		"./NFIQ_2/NFIQ2/NFIQ2Algorithm/include/features/FingerJetFXFeature.h"
		"./NFIQ_2/NFIQ2/NFIQ2Algorithm/include/features/*.h"
		"./NFIQ_2/NFIQ2/NFIQ2Algorithm/include/features/*.hpp"
		"./NFIQ_2/NFIQ2/NFIQ2Algorithm/include/features/*.cpp"
		"./NFIQ_2/NFIQ2/NFIQ2Algorithm/include/features/*.c"
		)

#----------------------------^^^^----------------------------

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
        ${SOURCES}
		)

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
			android)

find_package(ZLIB)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
                       native-lib
                       # Links the target library to the log library
                       # included in the NDK.
                       ${log-lib}
#			            hello_world
						${NFIQ2_SUPERBUILD}
		                ${ZLIB_LIBRARIES}
		                ${android-lib}
		                -ljnigraphics
			)	# Try NFIQ2_SUPERBUILD if not!
