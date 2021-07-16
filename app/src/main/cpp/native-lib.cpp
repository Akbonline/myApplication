#include <jni.h>
#include <string>
//#include <hello_world.hpp>

#include "NFIQ_2/NFIQ2/NFIQ2Algorithm/include/nfiq2.hpp"        // Booyaah!

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_myapplication_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {

            //    NFIQ2::Algorithm nfiq2;
            std::string hello = "Jello";
            return env->NewStringUTF(hello.c_str());
}