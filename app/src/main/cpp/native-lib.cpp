#include <jni.h>
#include <hello_world.hpp>
//#include <nfiq2.hpp>

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_myapplication_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {


    std::string hello = "Hello world" +sayHi();
    return env->NewStringUTF(hello.c_str());
}