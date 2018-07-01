#include "PID.h"



PID::PID(float k_p, float k_i, float k_d, float dt, float max, float min):
k_p(k_p), k_i(k_i), k_d(k_d), dt(dt), max(max), min(min){}


float PID::getOutput(float reference, float value_sens){
    //Calcola l'errore in output al controllore pid
    float err = reference - value_sens;

    float pout = err * k_p;

    integral += err * dt;

    float iout = integral * k_i;

    float deriv = (err - prev_err)/dt;

    float dout = deriv * k_d;

    return pout + iout + dout;
}