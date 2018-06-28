#ifndef __BRAVO_BUTTON_
#define __BRAVO_BUTTON_

class BravoButton
{
public:
    BravoButton(int button, int thresh_middle, int thresh_high) : fsquare(false)
    {
        this->button = button;
        this->thresh_middle = thresh_middle;
        this->thresh_high = thresh_high;
    }


    void update(int vsquare, int &yb, int &xb, int &lb)
    {
      if (fsquare == false && vsquare > 0 && psquare > 0) {
        vsquare *= vsquare;
        Serial.print("#: ");
        Serial.print(vsquare);
        if (vsquare > thresh_high) {
    //      Serial.println(": strong!");
          lb = 1;
        } else if (vsquare > thresh_middle) {
    //      Serial.println(": middle.");
          xb = 1;
        } else {
    //      Serial.println(": weak..");
          yb = 1;
        }
        fsquare = true;
      } else if (vsquare == 0) {
        fsquare = false;
      }
      psquare = vsquare;
    }

private:
    int button;
    int thresh_middle;
    int thresh_high;

    int vsquare, psquare;
    bool fsquare;
};

#endif