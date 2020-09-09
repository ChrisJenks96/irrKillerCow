package com.albongames.killercowapp3;

import android.app.NativeActivity;
import android.os.Bundle;

public class MainActivity extends NativeActivity
{
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        org.fmod.FMOD.init(this);
    }

    @Override
    public void onDestroy()
    {
        super.onDestroy();
        org.fmod.FMOD.close();
    }

    static
    {
        System.loadLibrary("fmod");
        System.loadLibrary("fmodstudio");
    }
}