/**
 * portmacro.h - fast digitalWrite for Arduino Uno / Nano / Pro mini
 * Copyright (c) 2018 fusuian <fusuian@gmail.com>
 *
 * portmacro.h is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * portmacro.h is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with portmacro.h.  If not, see <http://www.gnu.org/licenses/>.
 */

#define PORT(n) ( (n <=  7)? PORTD : \
                ( (n <= 13)? PORTB : \
                ( (n <= 19)? PORTC : PORTB)))

#define REG(n)  ( (n <=  7)? (n) : \
                ( (n <= 13)? (n - 8) : \
                ( (n <= 19)? (n - 14) : (13 - 8) )))

#define portOn(p)  ( PORT(p) |=  _BV(REG(p)) )
#define portOff(p) ( PORT(p) &= ~_BV(REG(p)) )

#define isPin(p)  ( (PORT(p) & ~_BV(REG(p))) != 0 )

