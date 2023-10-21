#include <jni.h>
#include <android/log.h>
#include <memory>
#include "emulator.hpp"
#include "renderer_gl/renderer_gl.hpp"
#include <EGL/egl.h>
#include <glad/gl.h>
#include <filesystem>
#include <optional>

RendererGL* renderer;

extern "C"
JNIEXPORT void JNICALL
Java_com_panda3ds_panda3ds_MainActivity_initDriver(JNIEnv *env, jobject thiz) {
    __android_log_print(ANDROID_LOG_INFO, "pandroid", "Initializing pandroid...");
    Emulator emulator2;
    __android_log_print(ANDROID_LOG_INFO, "pandroid", "2Initializing pandroid...");
    return;
    //emulator = std::make_unique<Emulator>();

    //if (emulator->getRendererType() != RendererType::OpenGL) {
//        throw std::runtime_error("Renderer is not OpenGL");
  //  }
//    renderer = static_cast<RendererGL*>(emulator->getRenderer());

  //  if (!gladLoadGLES2Loader(reinterpret_cast<GLADloadproc>(eglGetProcAddress))) {
//        Helpers::panic("OpenGL ES init failed");
  //  }
}