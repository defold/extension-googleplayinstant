package com.agulev.instantapp;

import android.app.Activity;
import com.google.android.gms.instantapps.InstantApps;
import android.content.Intent;

public class GiaJNI {
    private Activity activity;

    public GiaJNI(Activity activity) {
        this.activity = activity;
    }

    public boolean isInstantApp() {
        return InstantApps.getPackageManagerCompat(this.activity).isInstantApp();
    }

    public void showInstallPrompt() {
        Intent postInstallIntent = new Intent(Intent.ACTION_VIEW, this.activity.getIntent().getData());
        postInstallIntent.addCategory(Intent.CATEGORY_BROWSABLE);
        InstantApps.showInstallPrompt(this.activity, postInstallIntent, 0, "DefoldActivity");
    }
}
