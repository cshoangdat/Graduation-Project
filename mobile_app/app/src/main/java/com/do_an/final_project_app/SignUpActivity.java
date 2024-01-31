package com.do_an.final_project_app;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.content.ContextCompat;

import android.content.Intent;
import android.os.Bundle;
import android.text.SpannableString;
import android.text.Spanned;
import android.text.style.ForegroundColorSpan;
import android.text.style.UnderlineSpan;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

import com.google.android.material.button.MaterialButton;

public class SignUpActivity extends AppCompatActivity {
    AmplifyCognito amplifyCognito;

    EditText etUsername;
    EditText etPassword;
    EditText etEmail;
    MaterialButton btnSignUp;
    TextView signIn;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_sign_up);
        amplifyCognito = new AmplifyCognito(getApplicationContext());
        etUsername = findViewById(R.id.etUsername);
        etPassword = findViewById(R.id.etPassword);
        etEmail = findViewById(R.id.etEmail);
        btnSignUp = findViewById(R.id.buttonSignUp);
        signIn = findViewById(R.id.signIn);
        btnSignUp.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                String username = etUsername.getText().toString();
                String password = etPassword.getText().toString();
                String email = etEmail.getText().toString();
                amplifyCognito.signUp(email, username, password);
            }
        });
        String text = "Already have an account? Log In";
        SpannableString ss = new SpannableString(text);
        UnderlineSpan underlineSpan = new UnderlineSpan();
        ss.setSpan(underlineSpan, text.indexOf("Log In"), text.length(), Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
        ForegroundColorSpan colorSpan = new ForegroundColorSpan(ContextCompat.getColor(this, R.color.primary));
        ss.setSpan(colorSpan, text.indexOf("Log In"), text.length(), Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
        signIn.setText(ss);
        signIn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                amplifyCognito.loadLogin();
            }
        });
    }
}