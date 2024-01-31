package com.do_an.final_project_app;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.RecyclerView;

import java.util.ArrayList;

public class CustomAdapter extends RecyclerView.Adapter<CustomAdapter.MyViewHolder> {

    Context context;
    ArrayList time, people, male, female, g1, g2, g3, g4;

    CustomAdapter(Context context, ArrayList time, ArrayList people, ArrayList male, ArrayList female, ArrayList g1, ArrayList g2, ArrayList g3, ArrayList g4){
        this.context = context;
        this.time = time;
        this.people = people;
        this.male = male;
        this.female = female;
        this.g1 = g1;
        this.g2 = g2;
        this.g3 = g3;
        this.g4 = g4;
    }

    @NonNull
    @Override
    public MyViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
        LayoutInflater inflater = LayoutInflater.from(context);
        View view = inflater.inflate(R.layout.my_row, parent, false);
        return new MyViewHolder(view);
    }

    @Override
    public void onBindViewHolder(@NonNull MyViewHolder holder, int position) {
        holder.time_db.setText(String.valueOf(time.get(position)));
        holder.people_db.setText(String.valueOf(people.get(position)));
        holder.male_db.setText(String.valueOf(male.get(position)));
        holder.female_db.setText(String.valueOf(female.get(position)));
        holder.g1_db.setText(String.valueOf(g1.get(position)));
        holder.g2_db.setText(String.valueOf(g2.get(position)));
        holder.g3_db.setText(String.valueOf(g3.get(position)));
        holder.g4_db.setText(String.valueOf(g4.get(position)));
    }

    @Override
    public int getItemCount() {
        return time.size();
    }

    public class MyViewHolder extends RecyclerView.ViewHolder {

        TextView time_db, people_db, male_db, female_db, g1_db, g2_db, g3_db, g4_db;

        public MyViewHolder(@NonNull View itemView) {
            super(itemView);
            time_db = itemView.findViewById(R.id.time_db);
            people_db = itemView.findViewById(R.id.people_db);
            male_db = itemView.findViewById(R.id.male_db);
            female_db = itemView.findViewById(R.id.female_db);
            g1_db = itemView.findViewById(R.id.g1_db);
            g2_db = itemView.findViewById(R.id.g2_db);
            g3_db = itemView.findViewById(R.id.g3_db);
            g4_db = itemView.findViewById(R.id.g4_db);
        }
    }
}
