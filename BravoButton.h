/**
 * WiiBravo - Sony DualShock2 Controller Adapter for Wii (especially designed for Bravoman)
 * Copyright (c) 2018 fusuian <fusuian@gmail.com>
 *
 * This file is part of WiiBravo.
 *
 * WiiBravo is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * WiiBravo is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with WiiBravo.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __BRAVO_BUTTON__
#define __BRAVO_BUTTON__

class BravoButton
{
public:
    BravoButton(int low_max, int middle_max) : fsquare(false)
    {
        this->low_max = low_max;
        this->middle_max = middle_max;
    }


    void update(byte vsquare, byte &yb, byte &xb, byte &lb)
    {
      if (fsquare == false && vsquare > 0 && psquare > 0) {
        uint16_t value = vsquare * psquare;
//        Serial.print(psquare);
//        Serial.print(" * ");
//        Serial.print(vsquare);
//        Serial.print(" = ");
//        Serial.print(value);
//        Serial.print(": ");
        if (value > middle_max) {
//          Serial.println(": strong!");
          lb = 1;
        } else if (value > low_max) {
//          Serial.println(": middle.");
          xb = 1;
        } else {
//          Serial.println(": weak..");
          yb = 1;
        }
        fsquare = true;
      } else if (vsquare == 0) {
        fsquare = false;
      }
      psquare = vsquare;
    }

private:
    int low_max;
    int middle_max;

    byte psquare;
    bool fsquare;
};

#endif
