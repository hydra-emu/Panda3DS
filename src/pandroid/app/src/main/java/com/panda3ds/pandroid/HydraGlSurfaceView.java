package com.panda3ds.pandroid;

import android.app.Activity;
import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.DisplayMetrics;

import com.panda3ds.pandroid.HydraGlRenderer;

public class HydraGlSurfaceView extends GLSurfaceView {
    final HydraGlRenderer renderer;

    public HydraGlSurfaceView(Context context) {
        super(context);
        setEGLContextClientVersion(3);
        renderer = new HydraGlRenderer();
        setRenderer(renderer);
    }

    static {
        System.loadLibrary("pandroid");
    }
}