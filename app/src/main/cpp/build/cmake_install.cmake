# Install script for directory: /Users/akshatbajpai/Desktop/Work/myApplication copy/app/src/main/cpp/NFIQ_2

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "0")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "TRUE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xnfiq2_clix" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/nfiq2/bin" TYPE DIRECTORY PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES "/Users/akshatbajpai/Desktop/Work/MyApplication/app/src/main/cpp/build/install_staging/nfiq2/bin/")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xnfiq2_clix" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/nfiq2/share" TYPE DIRECTORY FILES "/Users/akshatbajpai/Desktop/Work/MyApplication/app/src/main/cpp/build/install_staging/nfiq2/share/")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/nfiq2/lib" TYPE DIRECTORY FILES "/Users/akshatbajpai/Desktop/Work/MyApplication/app/src/main/cpp/build/install_staging/nfiq2/lib/")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/nfiq2/include" TYPE DIRECTORY FILES "/Users/akshatbajpai/Desktop/Work/MyApplication/app/src/main/cpp/build/install_staging/nfiq2/include/")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xnfiq2_clix" OR NOT CMAKE_INSTALL_COMPONENT)
  execute_process(COMMAND /Users/akshatbajpai/Library/Android/sdk/cmake/3.10.2.4988404/bin/cmake -E make_directory /Users/akshatbajpai/Desktop/Work/MyApplication/app/src/main/cpp/build/install_staging/nfiq2/../bin)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xnfiq2_clix" OR NOT CMAKE_INSTALL_COMPONENT)
  execute_process(COMMAND ln -s -f "../nfiq2/bin/nfiq2" "nfiq2" WORKING_DIRECTORY "/Users/akshatbajpai/Desktop/Work/MyApplication/app/src/main/cpp/build/install_staging/nfiq2/../bin")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xnfiq2_clix" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE FILE FILES "/Users/akshatbajpai/Desktop/Work/MyApplication/app/src/main/cpp/build/install_staging/nfiq2/../bin/nfiq2")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/Users/akshatbajpai/Desktop/Work/MyApplication/app/src/main/cpp/build/fingerjetfxose/FingerJetFXOSE/libFRFXLL/src/cmake_install.cmake")

endif()

