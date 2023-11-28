package com.panda3ds.pandroid;

public class AlberDriver {

    AlberDriver() {
        super();
    }

    public static native void Initialize();
    public static native void RunFrame(int fbo);
    public static native boolean HasRomLoaded();
    public static native void LoadRom(String path);
    public static native void Finalize();

    public static native void KeyDown(int code);
    public static native void KeyUp(int code);
    public static native void SetCirclepadAxis(int x, int y);
    public static native void TouchScreenUp();
    public static native void TouchScreenDown(int x, int y);

    static {
        System.loadLibrary("Alber");
    }
}