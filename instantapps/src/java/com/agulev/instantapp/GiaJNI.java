package com.agulev.instantapp;

import android.app.Activity;
import com.google.android.gms.instantapps.InstantApps;

public class GiaJNI {
    private Activity activity;

    public GiaJNI(Activity activity) {
        this.activity = activity;
    }

    public boolean isInstantApp() {
        return InstantApps.getPackageManagerCompat(this.activity).isInstantApp;
    }
}
