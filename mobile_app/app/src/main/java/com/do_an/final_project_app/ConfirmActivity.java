package com.do_an.final_project_app;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.EditText;

import com.google.android.material.button.MaterialButton;

public class ConfirmActivity extends AppCompatActivity {

    AmplifyCognito amplifyCognito;
    EditText etCode;
    MaterialButton btnVerify;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_confirm);

        amplifyCognito = new AmplifyCognito(getApplicationContext());
        Intent intent = getIntent();
        String username = intent.getStringExtra("username");
        etCode = findViewById(R.id.etCode);
        btnVerify = findViewById(R.id.btnVerify);
        btnVerify.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                String code = etCode.getText().toString();
                amplifyCognito.confirm(username, code);
            }
        });
    }
}