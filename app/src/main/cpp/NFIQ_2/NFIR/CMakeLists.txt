cmake_minimum_required(VERSION 3.3)
project(NFIR)

# ##############################################################################
set(OpenCV_DIR "" CACHE PATH "Specifies path to OpenCV installation")
if(OpenCV_DIR)
  find_package(OpenCV REQUIRED NO_CMAKE_PATH NO_CMAKE_ENVIRONMENT_PATH HINTS ${OpenCV_DIR})
else()
  find_package(OpenCV REQUIRED)
endif()

# ##############################################################################
set(LIBRARY_SOURCES
    "src/lib/filter_mask.cpp"
    "src/lib/filter_mask_gaussian.cpp"
    "src/lib/filter_mask_ideal.cpp"
    "src/lib/nfir_lib.cpp"
    "src/lib/resample.cpp"
    "src/lib/resample_down.cpp"
    "src/lib/resample_up.cpp")

# ##############################################################################
set(STATIC_LIB "nfir-static-lib")

add_library(${STATIC_LIB} STATIC ${LIBRARY_SOURCES})

option(FORCE_WIN_STATIC_RUNTIME "Force NFIR to link to the static run-time library when linking with MSVC" OFF)

if(MSVC)
  if (FORCE_WIN_STATIC_RUNTIME)
    foreach(flag_var
        CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
        CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
      if(${flag_var} MATCHES "/MD")
        string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
      elseif(${flag_var} MATCHES "/MDd")
        string(REGEX REPLACE "/MDd" "/MTd" ${flag_var} "${${flag_var}}")
      endif()
    endforeach()
  endif()

  target_compile_definitions(${STATIC_LIB} PRIVATE _WIN32_64)
  target_compile_options(${STATIC_LIB} PRIVATE "/nologo" "/EHsc")
endif()

set_target_properties(
  ${STATIC_LIB}
  PROPERTIES PUBLIC_HEADER "include/nfir_lib.h"
             OUTPUT_NAME "nfir"
             CXX_STANDARD 11
             CXX_STANDARD_REQUIRED YES)

target_include_directories(
  ${STATIC_LIB}
  PRIVATE "thirdparty/szx/glob" "include"
  PUBLIC "${OpenCV_INCLUDE_DIRS}")

target_link_libraries(${STATIC_LIB} PUBLIC ${OpenCV_LIBS})

include(GNUInstallDirs)

install(
  TARGETS ${STATIC_LIB}
  DESTINATION ${CMAKE_INSTALL_LIBDIR}
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
