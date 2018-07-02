#include "Motor.h"
#include <exception>

class WrongSizeException :  public std::exception{
    virtual const char* what() const throw()
    {
        return "Wrong size of the array passed!";
    }


} e;

Motor::Motor(){}

Motor::Motor(PinName en1, PinName en2, PinName en3, PinName en_chip, PinName* coilPins = NULL){
    this->enable1 = DigitalOut(en1);
    this->enable2 = DigitalOut(en2);
    this->enable3 = DigitalOut(en3);

    this-> en_chip = DigitalOut(en_chip);

    if(sizeof(coilPins) / sizeof(PinName) == 3){    //controlla se coilPins ha 3 valori
        this->uh = PwmOut(coilPins[0]);
        this->vh = PwmOut(coilPins[1]);
        this->wh = PwmOut(coilPins[2]);

    }else{
        throw e;
    }

    
}

void Motor::setPeriodU(float period){
    this->uh.period(period);
}

void Motor::setPeriodV(float period){
    this->uh.period(period);
}

void Motor::setPeriodW(float period){
    this->uh.period(period);
}

 void Motor::setEnableChip(uint8_t value){
     this->en_chip = value;
 }

/**
 * implementa l'algoritmo per il cambio di fase 
 */
 void Motor::setStep(uint8_t step_number, float duty_right, float duty_left){
     switch(step_number){
                case 0:
                    this->uh.write(duty_right);
                    this->vh.write(0.0f);
                    this->wh.write(duty_left);
                    this->enable1 = 1;
                    this->enable2 = 0;
                    this->enable3 = 1;
                    break;

                case 1:
                    this->uh.write(0.0f);
                    this->vh.write(duty_right);
                    this->wh.write(duty_left);
                    this->enable1 = 0;
                    this->enable2 = 1;
                    this->enable3 = 1;
                    break;

                case 2:
                    this->uh.write(duty_left);
                    this->vh.write(duty_right);
                    this->wh.write(0.0f);
                    this->enable1 = 1;
                    this->enable2 = 1;
                    this->enable3 = 0;
                    break;

                case 3:
                    this->uh.write(duty_left);
                    this->vh.write(0.0f);
                    this->wh.write(duty_right);
                    this->enable1 = 1;
                    this->enable2 = 0;
                    this->enable3 = 1;
                    break;

                case 4:
                    this->uh.write(0.0f);
                    this->vh.write(duty_left);
                    this->wh.write(duty_right);
                    this->enable1 = 0;
                    this->enable2 = 1;
                    this->enable3 = 1;
                    break;

                case 5:
                    this->uh.write(duty_right);
                    this->vh.write(duty_left);
                    this->wh.write(0.0f);
                    this->enable1 = 1;
                    this->enable2 = 1;
                    this->enable3 = 0;
                    break;
            }
 }