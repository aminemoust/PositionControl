#include "PID.h"



PID::PID(float k_p, float k_i, float k_d, float dt):
k_p(k_p), k_i(k_i), k_d(k_d), dt(dt){}


/**
 * Metodo che implementa l'algoritmo per il controllore PID
 * @param reference input di riferimento
 * @param value_sens valore del sensore
 * 
 * @return Output del PID con la somma delle costanti moltiplicate per l'errore.
 * 
 */
float PID::getOutput(float reference, float value_sens){
    //Calcola l'errore in output al controllore pid
    float err = reference - value_sens;

    float pout = err * k_p;

    this->integral += err * dt;

    float iout = this->integral * k_i;

    float deriv = (err - this->prev_err)/dt;

    this->prev_err = err;

    float dout = deriv * k_d;

    return pout + iout + dout;  //Proportional + Integral + Derivative parts
}