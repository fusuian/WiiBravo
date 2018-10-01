#ifndef __BRAVO_BUTTON_
#define __BRAVO_BUTTON_

#include "portmacro.h"

const int weak_pin = 7;
const int middle_pin = 6;

class BravoButton
{
public:
    BravoButton(int thresh_middle, int thresh_high) : fsquare(false)
    {
        this->thresh_middle = thresh_middle;
        this->thresh_high = thresh_high;
    }


    void update(byte vsquare, byte &yb, byte &xb, byte &lb)
    {
      if (fsquare == false && vsquare > 0 && psquare > 0) {
        vsquare *= vsquare;
//        Serial.print("#: ");
//        Serial.print(vsquare);
        if (vsquare > thresh_high) {
//          Serial.println(": strong!");
          portOn(weak_pin);
          portOn(middle_pin);
          lb = 1;
        } else if (vsquare > thresh_middle) {
//          Serial.println(": middle.");
          portOff(weak_pin);
          portOn(middle_pin);
          xb = 1;
        } else {
//          Serial.println(": weak..");
          yb = 1;
          portOn(weak_pin);
          portOff(middle_pin);
        }
        fsquare = true;
      } else if (vsquare == 0) {
        fsquare = false;
        portOff(weak_pin);
        portOff(middle_pin);
      }
      psquare = vsquare;
    }

private:
    int thresh_middle;
    int thresh_high;

    int vsquare, psquare;
    bool fsquare;
};

#endif
