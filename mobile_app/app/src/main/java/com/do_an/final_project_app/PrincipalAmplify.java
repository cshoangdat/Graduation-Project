package com.do_an.final_project_app;

import android.app.Application;
import android.util.Log;

import com.amplifyframework.AmplifyException;
import com.amplifyframework.auth.cognito.AWSCognitoAuthPlugin;
import com.amplifyframework.core.Amplify;

public class PrincipalAmplify extends Application {
    public void onCreate() {
        super.onCreate();
        try {
            Amplify.addPlugin(new AWSCognitoAuthPlugin());
            Log.i("MyCognitoApp", "Initialized Cognito");
        } catch(AmplifyException error) {
            error.printStackTrace();
            Log.e("MyCognitoApp", "Could not initialize Cognito", error);
        }
        try {
            Amplify.configure(getApplicationContext());
            Log.i("MyAmplifyApp", "Initialized Amplify");
        } catch (AmplifyException error) {
            Log.e("MyAmplifyApp", "Could not initialize Amplify", error);
        }
    }
}
