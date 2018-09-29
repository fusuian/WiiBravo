#include <PS2X_lib.h>  //for v1.6
#include "WMExtension.h"
#include "BravoButton.h"

#define ULONG_MAX (4294967295)

bool bravo_mode = false;
bool digital_mode = false;

const int thresh_middle =  100; //800;
const int thresh_high   =  1200; //1800;

BravoButton attack_button(thresh_middle, thresh_high);
BravoButton   jump_button(thresh_middle, thresh_high);
BravoButton attack_button2(thresh_middle, thresh_high);
BravoButton   jump_button2(thresh_middle, thresh_high);

const int ps_ok_pin  = 6;
const int wii_ok_pin = 7;
const int red_pin = 7;

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

int up, down, left, right;
int start, select, home;
int ab, bb, xb, yb;
int lb, rb, zlb, zrb;
int lx, ly, rx, ry;

byte clx, cly, crx, cry;

void stop_by_error()
{
  while (true) {
    digitalWrite(red_pin, HIGH);
    delay(100);
    digitalWrite(red_pin, LOW);
    delay(900);
  }
}


void setup(){

  Serial.begin(57600);
  while (Serial == false)
    ;

  pinMode(ps_ok_pin, OUTPUT);
  pinMode(wii_ok_pin, OUTPUT);
  digitalWrite(ps_ok_pin, LOW);
  digitalWrite(wii_ok_pin, LOW);

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

  type = ps2x.readType();
  Serial.print(F("type = "));
  Serial.println(type);
  switch(type) {
  case 0:
    Serial.println(F("Digital Controller found"));
    digital_mode = true;
    break;
  case 1:
    Serial.println(F("DualShock Controller found"));
    bravo_mode = true;
    break;
  default:
    Serial.println(F("Unknown Controller found"));
    stop_by_error();
    break;
  }

  if (bravo_mode) {
    while (ps2x.enablePressures() == false) {
      delay(16);
    }
  }

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
  static int m = 0;
  int n = micros();
  
  if (n >= m) {
    n -= m;
  } else {
    n += ULONG_MAX - m;
  }
  delayMicroseconds(10000 - n);
  delayMicroseconds(6666);
  m = micros();
}



void loop() {
  if(error == 1) //skip loop if no controller found
    return;

  ps2x.read_gamepad(false, vibrate);
  up    = ps2x.Button(PSB_PAD_UP);
  down  = ps2x.Button(PSB_PAD_DOWN);
  left  = ps2x.Button(PSB_PAD_LEFT);
  right = ps2x.Button(PSB_PAD_RIGHT);

  start  = ps2x.Button(PSB_START);
  select = ps2x.Button(PSB_SELECT);
  home = up && select;

  // buttons:
  //   x      A
  // y   a  #   o
  //   b      x

  zlb = ps2x.Button(PSB_L2);
  zrb = ps2x.Button(PSB_R2);

  if (bravo_mode) {
    int vcircle = ps2x.Analog(PSAB_CIRCLE);
    jump_button.update(vcircle, bb, ab, rb);
    int vsquare = ps2x.Analog(PSAB_SQUARE);
    jump_button2.update(vsquare, bb, ab, rb);
    int vcross = ps2x.Analog(PSAB_CROSS);
    attack_button.update(vcross, yb, xb, lb);
    int vtriangle = ps2x.Analog(PSAB_TRIANGLE);
    attack_button2.update(vtriangle, yb, xb, lb);
  } else {
    xb = ps2x.Button(PSB_TRIANGLE);
    ab = ps2x.Button(PSB_CIRCLE);
    yb = ps2x.Button(PSB_SQUARE);
    bb = ps2x.Button(PSB_CROSS);
    lb = ps2x.Button(PSB_L1);
    rb = ps2x.Button(PSB_R1);
  }

  if (digital_mode) {
    lx = crx;
    ly = cry;
    rx = crx;
    ry = cry;  
  } else {
    // クラコンの左アナログスティックは6bit値(ライブラリ側で変換)、y軸を反転
    lx =  ps2x.Analog(PSS_LX);
    if (abs(lx) < 16) { lx = 0; }
    ly =  255 - ps2x.Analog(PSS_LY);
    if (abs(ly) < 16) { ly = 0; }
    
    // クラコンの右アナログスティックは5bit値(ライブラリ側で変換)、y軸を反転
    rx =  ps2x.Analog(PSS_RX);
    if (abs(rx) < 16) { rx = 0; }
    ry = 255 - ps2x.Analog(PSS_RY);
    if (abs(ry) < 16) { ry = 0; }
  }
    
  WMExtension::set_button_data(left, right, up, down,
    ab, bb, xb, yb,
    lb, rb,
    select, start, home,
    lx, ly, rx, ry,
    zlb, zrb, lb, rb);

  delay16();
}
