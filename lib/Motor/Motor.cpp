#include "Motor.h"
// #include <exception.h>

// class WrongSizeException :  public std::exception{
//     virtual const char* what() const throw()
//     {
//         return "Wrong size of the array passed!";
//     }


// } e;

//Motor::Motor(){}

Motor::Motor(PinName en1, PinName en2, PinName en3, PinName en_chip, PinName* coilPins):
_enable1(en1), _enable2(en2),  _enable3(en3), _en_chip(en_chip), encoder(I2C_SDA, I2C_SCL),
_uh(coilPins[0]), _vh(coilPins[1]), _wh(coilPins[2]){
    // _enable1 = DigitalOut(en1);
    // _enable2 = DigitalOut(en2);
    // _enable3 = DigitalOut(en3);

    // _en_chip = DigitalOut(en_chip);

    // encoder = DigitalEncoderAS5601(I2C_SDA,I2C_SCL);

    // if(sizeof(coilPins) / sizeof(PinName) == 3){    //controlla se coilPins ha 3 valori
    //     _uh = PwmOut(coilPins[0]);
    //     _vh = PwmOut(coilPins[1]);
    //     _wh = PwmOut(coilPins[2]);

    // }else{
    //     //throw e;
    // }

    
}

void Motor::setPeriodU(float period){
    _uh.period(period);
}

void Motor::setPeriodV(float period){
    _vh.period(period);
}

void Motor::setPeriodW(float period){
    _wh.period(period);
}

 void Motor::setEnableChip(uint8_t value){
     _en_chip = value;
 }

/**
 * implementa l'algoritmo per il cambio di fase 
 */
 void Motor::setStep(uint8_t step_number, float duty_right, float duty_left){
     switch(step_number){
                case 0:
                    _uh.write(duty_right);
                    _vh.write(0.0f);
                    _wh.write(duty_left);
                    _enable1 = 1;
                    _enable2 = 0;
                    _enable3 = 1;
                    break;

                case 1:
                    _uh.write(0.0f);
                    _vh.write(duty_right);
                    _wh.write(duty_left);
                    _enable1 = 0;
                    _enable2 = 1;
                    _enable3 = 1;
                    break;

                case 2:
                    _uh.write(duty_left);
                    _vh.write(duty_right);
                    _wh.write(0.0f);
                    _enable1 = 1;
                    _enable2 = 1;
                    _enable3 = 0;
                    break;

                case 3:
                    _uh.write(duty_left);
                    _vh.write(0.0f);
                    _wh.write(duty_right);
                    _enable1 = 1;
                    _enable2 = 0;
                    _enable3 = 1;
                    break;

                case 4:
                    _uh.write(0.0f);
                    _vh.write(duty_left);
                    _wh.write(duty_right);
                    _enable1 = 0;
                    _enable2 = 1;
                    _enable3 = 1;
                    break;

                case 5:
                    _uh.write(duty_right);
                    _vh.write(duty_left);
                    _wh.write(0.0f);
                    _enable1 = 1;
                    _enable2 = 1;
                    _enable3 = 0;
                    break;
            }
 }

