


#ifndef _PID_H
#define _PID_H

class PID {
    public:
        PID(float k_p, float k_i, float k_d, float dt, float max, float min);
        float getOutput(float reference, float value_sens);

    private:
        float k_p;
        float k_i;
        float k_d;
        float dt;
        float max;
        float min;
        float integral;
        float prev_err=0.0f;
};


#endif