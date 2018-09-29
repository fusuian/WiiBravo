#include <PS2X_lib.h>  //for v1.6
#include "WMExtension.h"
#include "BravoButton.h"

#define ULONG_MAX (4294967295)

const int thresh_middle =  800;
const int thresh_high   = 1800;

BravoButton attack_button(PSAB_SQUARE, thresh_middle, thresh_high);
BravoButton   jump_button(PSAB_CROSS,  thresh_middle, thresh_high);

const int ps_ok_pin  = 6;
const int wii_ok_pin = 7;

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

void setup(){

  Serial.begin(38400);
  while (Serial == false)
    ;

  pinMode(ps_ok_pin, OUTPUT);
  pinMode(wii_ok_pin, OUTPUT);
  digitalWrite(ps_ok_pin, LOW);
  digitalWrite(wii_ok_pin, LOW);

  delay(300);
  error = 1;
  while (error != 0) {
    digitalWrite(ps_ok_pin, LOW);
    error = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, pressures, rumble);
    if (error == 0){
      Serial.println(F("Found Controller, configured successful"));
      digitalWrite(ps_ok_pin, HIGH);
    } else if (error == 1) {
      Serial.println(F("No controller found, check wiring, see readme.txt to enable debug. visit www.billporter.info for troubleshooting tips"));
    } else if (error == 2) {
      Serial.println(F("Controller found but not accepting commands. see readme.txt to enable debug. Visit www.billporter.info for troubleshooting tips"));
    } else if (error == 3) {
      Serial.println(F("Controller refusing to enter Pressures mode, may not support it."));
    }
    digitalWrite(ps_ok_pin, HIGH);
    delay(100);
  }
  Serial.print(F("error = "));
  Serial.println(error);

  type = ps2x.readType();
  switch(type) {
  case 0:
    Serial.println(F("Unknown Controller type found"));
    break;
  case 1:
    Serial.println(F("DualShock Controller found"));
    break;
  case 2:
    Serial.println(F("GuitarHero Controller found"));
    break;
	case 3:
    Serial.println(F("Wireless Sony DualShock Controller found"));
    break;
  }

  Serial.print(F("type = "));
  Serial.println(type);

  while (ps2x.enablePressures() == false) {
    digitalWrite(ps_ok_pin, LOW);
    delay(250);
    digitalWrite(ps_ok_pin, HIGH);
    delay(250);
  }

  WMExtension::init();
  Serial.println(F("WM init"));
  digitalWrite(wii_ok_pin, HIGH);
}

int m;

void loop() {
  m = micros();
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

  xb = ps2x.Button(PSB_TRIANGLE);
  ab = ps2x.Button(PSB_CIRCLE);

  lb = ps2x.Button(PSB_L1);
  rb = ps2x.Button(PSB_R1);
  zlb = ps2x.Button(PSB_L2);
  zrb = ps2x.Button(PSB_R2);

  lx = ly = rx = ry = 0;

  yb = 0;
  int vsquare = ps2x.Analog(PSAB_SQUARE);
  attack_button.update(vsquare, yb, xb, lb);

  bb = 0;
  int vcross = ps2x.Analog(PSAB_CROSS);
  jump_button.update(vcross, bb, ab, rb);

  // クラコンの左アナログスティックは6bit値(ライブラリで変換)、y軸を反転
  lx =  ps2x.Analog(PSS_LX);
  ly =  255 - ps2x.Analog(PSS_LY);
  // クラコンの右アナログスティックは5bit値(ライブラリで変換)、y軸を反転
  rx =  ps2x.Analog(PSS_RX);
  ry = 255 - ps2x.Analog(PSS_RY);

  WMExtension::set_button_data(left, right, up, down,
    ab, bb, xb, yb,
    lb, rb,
    select, start, home,
    lx, ly, rx, ry,
    zlb, zrb, lb, rb);

  // 1/60秒単位のループをより正確に
  int n = micros();
  if (n >= m) {
    n -= m;
  } else {
    n += ULONG_MAX - m;
  }
  delayMicroseconds(10000 - n);
  delayMicroseconds(6666);
}
