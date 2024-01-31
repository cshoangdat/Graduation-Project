package com.do_an.final_project_app;

public class Data {
    int devId;
    double temp, humid, press;
    int co2, tvoc;

    public Data(int devId, double temp, double humid, double press, int co2, int tvoc) {
        this.devId = devId;
        this.temp = temp;
        this.humid = humid;
        this.press = press;
        this.co2 = co2;
        this.tvoc = tvoc;
    }

    public int getDevId() {
        return devId;
    }

    public void setDevId(int devId) {
        this.devId = devId;
    }

    public double getTemp() {
        return temp;
    }

    public void setTemp(double temp) {
        this.temp = temp;
    }

    public double getHumid() {
        return humid;
    }

    public void setHumid(double humid) {
        this.humid = humid;
    }

    public double getPress() {
        return press;
    }

    public void setPress(double press) {
        this.press = press;
    }

    public int getCo2() {
        return co2;
    }

    public void setCo2(int co2) {
        this.co2 = co2;
    }

    public int getTvoc() {
        return tvoc;
    }

    public void setTvoc(int tvoc) {
        this.tvoc = tvoc;
    }
}