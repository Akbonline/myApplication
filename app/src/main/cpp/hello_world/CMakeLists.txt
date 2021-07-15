
cmake_minimum_required(VERSION 3.10.2)

                                # Declares and names the project.

project("hello_world")          # Defining the folder Name

include_directories("./")       # Including all the directories in the path/ Providing the Relative path of the folder


add_library(                    # Sets the name of the library.
        hello_world

                                # Sets the library as a shared library.
        SHARED

                                # Provides a relative path to your source file(s).
        hello_world.cpp )


find_library(                   # Sets the name of the path variable.
        log-lib

                                # Specifies the name of the NDK library that
                                # you want CMake to locate.
        log )


target_link_libraries(          # Specifies the target library.
        hello_world

                                # Links the target library to the log library
                                # included in the NDK.

        ${log-lib} )
