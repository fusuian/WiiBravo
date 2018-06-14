#include <PS2X_lib.h>  //for v1.6
#include "WMExtension.h"

/******************************************************************
 * set pins connected to PS2 controller:
 *   - 1e column: original 
 *   - 2e colmun: Stef?
 * replace pin numbers by the ones you use
 ******************************************************************/
#define PS2_DAT       6 // 13    
#define PS2_CMD       7 // 11
#define PS2_SEL       8 // 10
#define PS2_CLK       9 //  12

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

void setup(){
 
  Serial.begin(38400);
  while (Serial == false)
    ;
  
  delay(300);  //added delay to give wireless ps2 module some time to startup, before configuring it

  error = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, pressures, rumble);  
  if (error == 0){
    Serial.print("Found Controller, configured successful ");
  } else if (error == 1) {
    Serial.println("No controller found, check wiring, see readme.txt to enable debug. visit www.billporter.info for troubleshooting tips");
  } else if (error == 2) {
    Serial.println("Controller found but not accepting commands. see readme.txt to enable debug. Visit www.billporter.info for troubleshooting tips");
  } else if (error == 3) {
    Serial.println("Controller refusing to enter Pressures mode, may not support it. ");
  }
  Serial.print(F("error = "));
  Serial.println(error);
  
  type = ps2x.readType(); 
  switch(type) {
  case 0:
    Serial.print("Unknown Controller type found ");
    break;
  case 1:
    Serial.print("DualShock Controller found ");
    break;
  case 2:
    Serial.print("GuitarHero Controller found ");
    break;
	case 3:
    Serial.print("Wireless Sony DualShock Controller found ");
    break;
  }

  Serial.print(F("type = "));
  Serial.println(type);
  
  WMExtension::init();
  Serial.println(F("WM init"));
}

const int thresh_high = 80;
const int thresh_middle = 40; 
const int thresh_low = 5;


byte vtriangle, vcircle, vcross, vsquare;

int up, down, left, right;
int start, select, home;
int ab, bb, xb, yb;
int lb, rb, zlb, zrb;
int lx, ly, rx, ry;

void loop() {
  Serial.print(F("type = "));
  Serial.print(type);
  Serial.print(F("; error = "));
  Serial.println(error);
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

//  ab = ps2x.Button(PSB_CIRCLE);
//  bb = ps2x.Button(PSB_CROSS);
//  xb = ps2x.Button(PSB_TRIANGLE);
//  yb = ps2x.Button(PSB_SQUARE);

  lb = ps2x.Button(PSB_L2);
  rb = ps2x.Button(PSB_R2);
  zlb = ps2x.Button(PSB_L1);
  zrb = ps2x.Button(PSB_R1);

  lx = ly = rx = ry = 0;

  vcross = ps2x.Analog(PSAB_CROSS);// | ps2x.Analog(PSAB_TRIANGLE);
  lb = xb = yb = 0;
  if (vcross > thresh_high) {
    lb = 1;        
  } else if (vcross > thresh_middle) {
    xb = 1;
  } else if (vcross > thresh_low) {
    yb = 1;
  }
  
  vcircle = ps2x.Analog(PSAB_CIRCLE); // | ps2x.Analog(PSAB_SQUARE);

  Serial.print("; x:");
  Serial.print(vcross);
  Serial.print("; o:");
  Serial.println(vcircle);


  rb = ab = bb = 0;
  if (vcircle > thresh_high) {
    rb = 1;        
  } else if (vcircle > thresh_middle) {
    ab = 1;
  } else if (vcircle > thresh_low) {
    bb = 1;
  }

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
