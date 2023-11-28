package com.panda3ds.pandroid.math;

public class Vector2 {
    public float x,y;
    public Vector2(Vector2 value){
        this(value.x,value.y);
    }

    public Vector2(float x, float y){
        this.x = x;
        this.y = y;
    }

    public float distanceTo(Vector2 vec){
        return distance(x,y,vec.x, vec.y);
    }

    public static float distance(float x, float y, float x2, float y2){
        return (float) Math.sqrt((x - x2) * (x - x2) + (y - y2) * (y - y2));
    }

    public void set(float x, float y) {
        this.x  = x;
        this.y  = y;
    }
}
