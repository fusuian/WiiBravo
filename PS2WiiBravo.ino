#include <PS2X_lib.h>  //for v1.6
#include "WMExtension.h"

const int ps_ok_pin  = 6;
const int wii_ok_pin = 7;

/******************************************************************
 * set pins connected to PS2 controller:
 *   - 1e column: original
 *   - 2e colmun: Stef?
 * replace pin numbers by the ones you use
 ******************************************************************/
#define PS2_DAT       2
#define PS2_CMD       3
#define PS2_SEL       4
#define PS2_CLK       5

/******************************************************************
 * select modes of PS2 controller:
 *   - pressures = analog reading of push-butttons
 *   - rumble    = motor rumbling
 * uncomment 1 of the lines for each mode selection
 ******************************************************************/
#define pressures   true
//#define pressures   false
//#define rumble      true
#define rumble      false

PS2X ps2x; // create PS2 Controller Class

//right now, the library does NOT support hot pluggable controllers, meaning
//you must always either restart your Arduino after you connect the controller,
//or call config_gamepad(pins) again after connecting the controller.

int error = 0;
byte type = 0;
byte vibrate = 0;

const int thresh_middle =  800;
const int thresh_high   = 1800;


int vtriangle, vcircle, vcross, vsquare;
int ptriangle, pcircle, pcross, psquare;
bool ftriangle = false, fcircle = false, fcross = false, fsquare = false;

int up, down, left, right;
int start, select, home;
int ab, bb, xb, yb;
int lb, rb, zlb, zrb;
int lx, ly, rx, ry;

void setup(){

  Serial.begin(57600);
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
      Serial.println("Found Controller, configured successful ");
      digitalWrite(ps_ok_pin, HIGH);
    } else if (error == 1) {
      Serial.println("No controller found, check wiring, see readme.txt to enable debug. visit www.billporter.info for troubleshooting tips");
    } else if (error == 2) {
      Serial.println("Controller found but not accepting commands. see readme.txt to enable debug. Visit www.billporter.info for troubleshooting tips");
    } else if (error == 3) {
      Serial.println("Controller refusing to enter Pressures mode, may not support it. ");
    }
    digitalWrite(ps_ok_pin, HIGH);
    delay(100);
  }
  Serial.print(F("error = "));
  Serial.println(error);

  type = ps2x.readType();
  switch(type) {
  case 0:
    Serial.println("Unknown Controller type found ");
    break;
  case 1:
    Serial.println("DualShock Controller found ");
    break;
  case 2:
    Serial.println("GuitarHero Controller found ");
    break;
	case 3:
    Serial.println("Wireless Sony DualShock Controller found ");
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

  xb = ps2x.Button(PSB_TRIANGLE);
  ab = ps2x.Button(PSB_CIRCLE);
  
  lb = ps2x.Button(PSB_L1);
  rb = ps2x.Button(PSB_R1);
  zlb = ps2x.Button(PSB_L2);
  zrb = ps2x.Button(PSB_R2);

  lx = ly = rx = ry = 0;

  lb = yb = 0;
  vsquare = ps2x.Analog(PSAB_SQUARE);
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

  rb = ab = bb = 0;
  vcross = ps2x.Analog(PSAB_CROSS);
  if (fcross == false && vcross > 0 && pcross > 0) {
    vcross *= vcross;
    Serial.print("x: ");
    Serial.print(vcross);
    if (vcross > thresh_high) {
//      Serial.println(": strong!");
      rb = 1;
    } else if (vcross > thresh_middle) {
//      Serial.println(": middle.");
      ab = 1;
    } else {
//      Serial.println(": weak..");
      bb = 1;
    }
    fcross = true;
  } else if (vcross == 0) {
    fcross = false;
  }
  pcross = vcross;

  lx = ps2x.Analog(PSS_LX);
  ly = ps2x.Analog(PSS_LY);
  rx = ps2x.Analog(PSS_RX);
  ry = ps2x.Analog(PSS_RY);

  WMExtension::set_button_data(left, right, up, down,
    ab, bb, xb, yb,
    lb, rb,
    select, start, home,
    lx, ly, rx, ry,
    zlb, zrb, lb, rb);

  delay(14);
}
