#include <android/log.h>
#include <cstdio>
#include <EGL/egl.h>
#include <glad/gl.h>
#include <filesystem>
#include <jni.h>
#include <string>
#include "emulator.hpp"
#include "renderer_gl.hpp"

Emulator* emulator = nullptr;
RendererGL* renderer = nullptr;

std::string jstring2string(JNIEnv* env, jstring jStr)
{
    if (!jStr)
        return "";

    const jclass stringClass = env->GetObjectClass(jStr);
    const jmethodID getBytes = env->GetMethodID(stringClass, "getBytes", "(Ljava/lang/String;)[B");
    const jbyteArray stringJbytes =
        (jbyteArray)env->CallObjectMethod(jStr, getBytes, env->NewStringUTF("UTF-8"));

    size_t length = (size_t)env->GetArrayLength(stringJbytes);
    jbyte* pBytes = env->GetByteArrayElements(stringJbytes, NULL);

    std::string ret = std::string((char*)pBytes, length);
    env->ReleaseByteArrayElements(stringJbytes, pBytes, JNI_ABORT);

    env->DeleteLocalRef(stringJbytes);
    env->DeleteLocalRef(stringClass);
    return ret;
}

extern "C" JNIEXPORT void JNICALL Java_com_panda3ds_pandroid_HydraGlRenderer_loadGame(JNIEnv* env,
                                                                                       jobject thiz,
                                                                                       jstring path)
{
    emulator = new Emulator;
    renderer = static_cast<RendererGL*>(emulator->getRenderer());
    if (!gladLoadGLES2Loader(reinterpret_cast<GLADloadproc>((void*)eglGetProcAddress))) {
		Helpers::panic("OpenGL ES init failed");
	}
    emulator->initGraphicsContext(nullptr);
    emulator->loadROM(jstring2string(env, path));
}

extern "C" JNIEXPORT void JNICALL Java_com_panda3ds_pandroid_HydraGlRenderer_runFrame(JNIEnv* env,
                                                                                       jobject thiz,
                                                                                       jint fbo)
{
    renderer->resetStateManager();
    renderer->setFBO(fbo);
    emulator->runFrame();
}
