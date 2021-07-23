#include <jni.h>
#include <string>
//#include <hello_world.hpp>

#include <nfiq2.hpp>      // Booyaah!(Nvmnd, not working anymore)
#include<memory>
//#include "test.cpp"
// Have a fun called getNFIQ2_Score with an arg: location and name of the image file
// Pass a byte array to the func -> NFIQ2 score (int b/w 0 and 100)
// How to convert byte array PNG to PGM
// C_SDK and C-Service

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_myapplication_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {

            //    NFIQ2::Algorithm nfiq2;
            std::string hello = "Jello";    // tried having a Char ** instead; didn't work either
//            char **argv = {"-h","SFinGe_Test01.pgm"};
//            run_examples(2);
            return env->NewStringUTF(hello.c_str());
            // returning the String in UTF-8 format

}
