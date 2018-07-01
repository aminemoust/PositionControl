#include "Motor.h"
#include <exception>

class WrongSizeException :  public std::exception{
    virtual const char* what() const throw()
    {
        return "Wrong size of the array passed!";
    }


} e;


Motor::Motor(PinName en1, PinName en2, PinName en3, PinName en_chip, PinName* coilPins){
    this->enable1 = new DigitalOut(en1);
    this->enable2 = new DigitalOut(en2);
    this->enable3 = new DigitalOut(en3);

    this-> en_chip = new DigitalOut(en_chip);

    if(sizeof(coilPins) / sizeof(PinName) == 3){    //controlla se coilPins ha 3 valori
        this->uh = new PwmOut(coilPins[0]);
        this->vh = new PwmOut(coilPins[1]);
        this->wh = new PwmOut(coilPins[2]);

    }else{
        throw e;
    }

    
}