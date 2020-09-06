package com.example.killercowapp3;

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
    protected void onDestroy()
    {
        org.fmod.FMOD.close();
    }
}

static
{
    System.loadLibrary("fmod");
}