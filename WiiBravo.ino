/**
 * WiiBravo - Sony DualShock2 Controller Adapter for Wii (especially designed for Bravoman)
 * Copyright (c) 2018 ASAHI,Michiharu <fusuian@gmail.com>
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


#include <PS2X_lib.h>  // v1.6
#include "WMExtension.h"
#include "BravoButton.h"
#include "portmacro.h"

#define ULONG_MAX (4294967295)

// ベラボーマンモードでの、弱中強のしきい値
const byte      low_max =  128;
const uint16_t  middle_max   =  1000;

// DualShock2のアナログスティックのしきい値
const byte threshold = 64;

// 動作モードフラグ

// DualShock2モード
// true: DualShock2が接続されている
// false: PS1初代コントローラまたはアーケードコントローラが接続されている
bool ds2_mode = false;

// ベラボーモード
// select+Rでオン、select+Lでオフ
// ds2_mode が真:
//      true: DualShock2のボタンがそれぞれ、弱中強３段階の攻撃/ジャンプボタンになる。
//      false: クラシックコントローラ配列
// ds2_mode が偽:
//      true: PS1コントローラのボタンをVCのメガドライブ配列(上段)
//            ファミコン配列(下段)に割り当てる
//      false: クラシックコントローラ配列
bool bravo_mode = false;


BravoButton attack_button(low_max, middle_max);
BravoButton jump_button(low_max, middle_max);
BravoButton attack_button2(low_max, middle_max);
BravoButton jump_button2(low_max, middle_max);

// インジケータLEDピン
const byte weak_pin = 6;
const byte middle_pin = 7;
const byte strong_pin = 8;
const byte red_pin = 6;



// PlayStation コントローラ読み取りピン
#define PS2_DAT       2
#define PS2_CMD       3
#define PS2_SEL       4
#define PS2_CLK       5

#define pressures   true
#define rumble      false

PS2X ps2x;

int error = 0;
byte type = 0;
byte vibrate = 0;

// Wiiリモコンへ送る各ボタンのフラグ
byte up, down, left, right;
byte start, select, home;
byte ab, bb, xb, yb;   // A/B/X/Yボタン
byte lb, rb, zlb, zrb; // L/R/ZL/ZRボタン
byte lx, ly, rx, ry;   // アナログスティックL/R
byte lt, rt;           // アナログトリガーL/R

byte clx, cly, crx, cry;  // アナログスティックの中央値

// エラーが発生したら赤LEDをチカチカさせる
void stop_by_error()
{
  while (true) {
    digitalWrite(red_pin, HIGH);
    delay(100);
    digitalWrite(red_pin, LOW);
    delay(900);
  }
}


void setup()
{
  Serial.begin(57600);
  while (Serial == false)
    ;

  pinMode(weak_pin, OUTPUT);
  pinMode(middle_pin, OUTPUT);
  pinMode(strong_pin, OUTPUT);
  portOff(weak_pin);
  portOff(middle_pin);
  portOff(strong_pin);

  // PSコントローラ初期化
  error = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, pressures, rumble);
  Serial.print(F("error = "));
  Serial.println(error);
  if (error == 0){
    Serial.println(F("Found Controller, configured successful"));
  } else if (error == 1) {
    Serial.println(F("No controller found"));
    stop_by_error();
  } else if (error == 2) {
    Serial.println(F("Controller found but not accepting commands"));
  } else if (error == 3) {
    Serial.println(F("Controller refusing to enter Pressures mode"));
  }

  // DualShock2であればベラボーマンモード、PS1コントローラであればデジタルモード
  type = ps2x.readType();
  Serial.print(F("type = "));
  Serial.println(type);
  switch(type) {
  case 0:
    Serial.println(F("Digital Controller found"));
    break;
  case 1:
    Serial.println(F("DualShock Controller found"));
    ds2_mode = true;
    break;
  default:
    Serial.println(F("Unknown Controller found"));
    stop_by_error();
    break;
  }

  if (ds2_mode) {
    while (ps2x.enablePressures() == false) {
      delay(16);
    }
  }
  blinkStart(pinx(), 60);

  // Wiiリモコン初期化
  WMExtension::init();
  Serial.println(F("WM init"));
  clx = WMExtension::get_calibration_byte(2);
  cly = WMExtension::get_calibration_byte(5);
  crx = WMExtension::get_calibration_byte(8);
  cry = WMExtension::get_calibration_byte(11);
}

// 1/60秒単位のウェイトをより正確に
void delay16()
{
  static unsigned long m = 0;
  unsigned long n = micros();

  if (n >= m) {
    n -= m;
  } else {
    n += ULONG_MAX - m;
  }
  delayMicroseconds(10000 - n);
  delayMicroseconds(6666);
  m = micros();
}

byte blink_count;
byte blink_pin;
byte blink_mode;
const byte blink_frames = 5;


void bravo_buttons()
{
  lt = rt = 0;
  yb = xb = lb = 0;
  bb = ab = rb = 0;
  byte vsquare = ps2x.Analog(PSAB_SQUARE);
  jump_button2.update(vsquare, bb, ab, rb);
  byte vcircle = ps2x.Analog(PSAB_CIRCLE);
  jump_button.update(vcircle, bb, ab, rb);
  byte vcross = ps2x.Analog(PSAB_CROSS);
  attack_button.update(vcross, yb, xb, lb);
  byte vtriangle = ps2x.Analog(PSAB_TRIANGLE);
  attack_button2.update(vtriangle, yb, xb, lb);
}


void digital_keys()
{
  lx = left? 0 : (right? 255 : clx);
  ly = down? 0: (up? 255: cly);
  rx = left? 0 : (right? 255 : crx);
  ry = down? 0: (up? 255: cry);
}


void analog_sticks()
{
  // クラコンのアナログスティックのビット幅の変換はライブラリに任せる
  lx =  ps2x.Analog(PSS_LX);
  if (abs(lx) < threshold) { lx = 0; }
  ly =  255 - ps2x.Analog(PSS_LY); // y軸を反転
  if (abs(ly) < threshold) { ly = 0; }

  rx =  ps2x.Analog(PSS_RX);
  if (abs(rx) < threshold) { rx = 0; }
  ry = 255 - ps2x.Analog(PSS_RY); // y軸を反転
  if (abs(ry) < threshold) { ry = 0; }
}


void classic_buttons()
{
  xb = ps2x.Button(PSB_TRIANGLE);
  ab = ps2x.Button(PSB_CIRCLE);
  yb = ps2x.Button(PSB_SQUARE);
  bb = ps2x.Button(PSB_CROSS);

  lb = ps2x.Button(PSB_L1);
  rb = ps2x.Button(PSB_R1);

  // L1/R1のアナログ値をL/Rトリガー値としてセット。
  // (VCAのアサルト以外に、トリガー値使ってるゲームあるの？)
  lt =  ps2x.Analog(PSAB_L1);
  rt =  ps2x.Analog(PSAB_R1);
}


void ac_buttons()
{
  ab = ps2x.Button(PSB_CIRCLE);
  bb = ps2x.Button(PSB_CROSS);
  xb = ps2x.Button(PSB_TRIANGLE);
  yb = ps2x.Button(PSB_SQUARE);
  lb = ps2x.Button(PSB_L1);
  rb = ps2x.Button(PSB_R1);

  lt = lb*255;
  rt = rb*255;
}


void fc_buttons()
{
  lt = rt = 0;
  yb = xb = lb = 0;
  bb = ab = rb = 0;

  // 上段: メガドラ用   y b a : # A L1
  yb |= ps2x.Button(PSB_SQUARE);
  bb |= ps2x.Button(PSB_TRIANGLE);
  ab |= ps2x.Button(PSB_L1);

  // 下段: ファミコン用 x y a : x o R1
  xb |= ps2x.Button(PSB_CROSS);
  yb |= ps2x.Button(PSB_CIRCLE);
  ab |= ps2x.Button(PSB_R1);
}


void blinkStart(byte pin, int count)
{
  blink_pin = pin;
  blink_count = count;
  blink_mode = HIGH;
}


void  blinkUpdate()
{
  if (blink_count > 0) {
      if (--blink_count % blink_frames == 0) {
        blink_mode = !blink_mode;
        if (blink_mode) {
          portOn(blink_pin);
        } else {
          portOff(blink_pin);
        }
      }
  }
  if (blink_count <= 0) {
    portOff(blink_pin);
  }
}


byte pinx()
{
  if (ds2_mode) {
    if (bravo_mode) {
      return strong_pin;
    } else {
      return weak_pin;
    }
  } else {
    if (bravo_mode) {
      return middle_pin;
    } else {
      return weak_pin;
    }
  }
}


void loop()
{
  blinkUpdate();

  ps2x.read_gamepad(false, vibrate);

  up    = ps2x.Button(PSB_PAD_UP);
  down  = ps2x.Button(PSB_PAD_DOWN);
  left  = ps2x.Button(PSB_PAD_LEFT);
  right = ps2x.Button(PSB_PAD_RIGHT);

  start  = ps2x.Button(PSB_START);
  select = ps2x.Button(PSB_SELECT);
  home = up && select;

  zlb = ps2x.Button(PSB_L2);
  zrb = ps2x.Button(PSB_R2);

  if (ds2_mode) {
    analog_sticks();
    if (bravo_mode) {
      bravo_buttons();
    } else {
      classic_buttons();
    }
  } else {
    digital_keys();
    if (bravo_mode) {
      fc_buttons();
    } else {
      ac_buttons();
    }
  }

  // モードが変わっても切り替えに使うボタンはL1/R1
  if (select & ps2x.Button(PSB_L1)) {
    bravo_mode = true;
    blinkStart(pinx(), 60);
  }
  if (select & ps2x.Button(PSB_R1)) {
    bravo_mode = false;
    blinkStart(pinx(), 60);
  }

  if (yb | bb) {
    portOn(weak_pin);
  } else {
    portOff(weak_pin);
  }
  if (xb | ab) {
    portOn(middle_pin);
  } else {
    portOff(middle_pin);
  }
  if (lb | rb) {
    portOn(strong_pin);
  } else {
    portOff(strong_pin);
  }

  WMExtension::set_button_data(left, right, up, down,
    ab, bb, xb, yb,
    lb, rb,
    select, start, home,
    lx, ly, rx, ry,
    zlb, zrb, lt, rt);

  delay16();
}
