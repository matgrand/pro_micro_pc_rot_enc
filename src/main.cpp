#include <Arduino.h>
#include <Encoder.h>
#include <HID-Project.h>

#define R_PWM 5 //RED PWM pin
#define G_PWM 3 //GREEN PWM pin
#define B_PWM 6 //BLUE PWM pin

#define BUTTON1 9 //button on encoder
#define BUTTON2 8 //button on encoder
#define BUTTON3 21 //switch
#define BUTTON4 20 //switch
#define BUTTON5 18 //switch
#define BUTTON6 19 //switch

//interrupts pins: 0 1 2 3 7
#define DT1 1 // encoder1 data pin
#define CLK1 0 // encoder1 clock pin
#define DT2 2 // encoder2 data pin
#define CLK2 7 // encoder2 clock pin

const byte pwm_max_duty = 255;

// create a vector of colors of the colors in rgb values
const byte colors[][3] = {
  {255, 0, 0}, // red
  {255, 127, 0}, // orange
  {255, 255, 0}, // yellow
  {0, 255, 0}, // green
  {0, 0, 255}, // blue
  {75, 0, 130}, // indigo
  {148, 0, 211} // violet
};

#define BUTTON_HIST 10 //button history

//map rgb value to pwm value
int rgb2pwm(int value, int pwm_max_duty=255) {
  return map(value, 0, 255, 0, pwm_max_duty);
}

// encoder
Encoder enc1(DT1, CLK1);
Encoder enc2(DT2, CLK2);

long enc1_val = 0; //encoder value
long enc2_val = 0; //encoder value
int incr1 = 0; //increment value
int incr2 = 0; //increment value
bool b1 = false; //button value
bool b2 = false; //button value
bool b3 = false; //button value
bool b4 = false; //button value
bool b5 = false; //button value
bool b6 = false; //button value
bool cli1 = false; //button value
bool cli2 = false; //button value
bool rel1 = false; //button value
bool rel2 = false; //button value
bool cli3 = false; //button value
bool cli4 = false; //button value
bool cli5 = false; //button value
bool cli6 = false; //button value
bool rel3 = false; //button value
bool rel4 = false; //button value
bool rel5 = false; //button value
bool rel6 = false; //button value

void(* resetArduino) (void) = 0; //declare reset function at address 0

void setup() {
  // serial
  Serial.begin(115200);
  Serial.println("Hello World");

  // pin mode
  pinMode(R_PWM, OUTPUT);
  pinMode(G_PWM, OUTPUT);
  pinMode(B_PWM, OUTPUT);

  pinMode(DT1, INPUT_PULLUP);
  pinMode(CLK1, INPUT_PULLUP);
  pinMode(DT2, INPUT_PULLUP);
  pinMode(CLK2, INPUT_PULLUP);

  pinMode(BUTTON1, INPUT_PULLUP); //button on encoder
  pinMode(BUTTON2, INPUT_PULLUP); //button on encoder

  pinMode(BUTTON3, INPUT_PULLUP); // switch
  pinMode(BUTTON4, INPUT_PULLUP); // switch
  pinMode(BUTTON5, INPUT_PULLUP); // switch
  pinMode(BUTTON6, INPUT_PULLUP); // switch

  //initiaize keyboard
  Keyboard.begin(); // initialize keyboard
  Consumer.begin(); // initialize consumer keyboard

  // pwm init
  analogWrite(R_PWM, 255); //255 is off
  analogWrite(G_PWM, 0);
  analogWrite(B_PWM, 0);

  // encoder init
  enc1.write(0);
  enc2.write(0);

  delay(1000);

  Serial.println("Setup done");
}

long prev_e1 = 0;
long prev_e2 = 0;
void update_encoders() {
  // read encoder values, but update 
  // it only when it hits the 4th step
  // to avoid jitter
  long r1 = enc1.read();
  long r2 = enc2.read();
  if (r1 % 4 == 0) enc1_val = r1 / 4; 
  if (r2 % 4 == 0) enc2_val = r2 / 4;
  incr1 = enc1_val - prev_e1;
  incr2 = enc2_val - prev_e2;
  prev_e1 = enc1_val;
  prev_e2 = enc2_val;
}

byte cc1 = 0; //cycle cli 1
byte cr1 = 0; //cycle rel 1
byte cc2 = 0; //cycle cli 2
byte cr2 = 0; //cycle rel 2
byte cc3 = 0; //cycle cli 3
byte cr3 = 0; //cycle rel 3
byte cc4 = 0; //cycle cli 4
byte cr4 = 0; //cycle rel 4
byte cc5 = 0; //cycle cli 5
byte cr5 = 0; //cycle rel 5
byte cc6 = 0; //cycle cli 6
byte cr6 = 0; //cycle rel 6

void update_buttons() {
  byte r1 = !digitalRead(BUTTON1);
  byte r2 = !digitalRead(BUTTON2);
  byte r3 = !digitalRead(BUTTON3);
  byte r4 = !digitalRead(BUTTON4);
  byte r5 = !digitalRead(BUTTON5);
  byte r6 = !digitalRead(BUTTON6);

  if (r1 && !b1)                {cc1 = min(cc1+1, 255); cr1=0;}
  else if (!r1 && b1)           {cr1 = min(cr1+1, 255); cc1=0;}
  if (!b1 && cc1 > BUTTON_HIST) {cli1=true; cc1=0; b1=true;}
  else                          {cli1=false;}
  if ( b1 && cr1 > BUTTON_HIST) {rel1=true; cr1=0; b1=false;}
  else                          {rel1=false;}

  if (r2 && !b2)                {cc2 = min(cc2+1, 255); cr2=0;}
  else if (!r2 && b2)           {cr2 = min(cr2+1, 255); cc2=0;}
  if (!b2 && cc2 > BUTTON_HIST) {cli2=true; cc2=0; b2=true;}
  else                          {cli2=false;}
  if ( b2 && cr2 > BUTTON_HIST) {rel2=true; cr2=0; b2=false;}
  else                          {rel2=false;}

  if (r3 && !b3)                {cc3 = min(cc3+1, 255); cr3=0;}
  else if (!r3 && b3)           {cr3 = min(cr3+1, 255); cc3=0;}
  if (!b3 && cc3 > BUTTON_HIST) {cli3=true; cc3=0; b3=true;}
  else                          {cli3=false;}
  if (b3 && cr3 > BUTTON_HIST)  {rel3=true; cr3=0; b3=false;}
  else                          {rel3=false;}

  if (r4 && !b4)                {cc4 = min(cc4+1, 255); cr4=0;}
  else if (!r4 && b4)           {cr4 = min(cr4+1, 255); cc4=0;}
  if (!b4 && cc4 > BUTTON_HIST) {cli4=true; cc4=0; b4=true;}
  else                          {cli4=false;}
  if (b4 && cr4 > BUTTON_HIST)  {rel4=true; cr4=0; b4=false;}
  else                          {rel4=false;}

  if (r5 && !b5)                {cc5 = min(cc5+1, 255); cr5=0;}
  else if (!r5 && b5)           {cr5 = min(cr5+1, 255); cc5=0;}
  if (!b5 && cc5 > BUTTON_HIST) {cli5=true; cc5=0; b5=true;}
  else                          {cli5=false;}
  if (b5 && cr5 > BUTTON_HIST)  {rel5=true; cr5=0; b5=false;}
  else                          {rel5=false;}

  if (r6 && !b6)                {cc6 = min(cc6+1, 255); cr6=0;}
  else if (!r6 && b6)           {cr6 = min(cr6+1, 255); cc6=0;}
  if (!b6 && cc6 > BUTTON_HIST) {cli6=true; cc6=0; b6=true;}
  else                          {cli6=false;}
  if (b6 && cr6 > BUTTON_HIST)  {rel6=true; cr6=0; b6=false;}
  else                          {rel6=false;}
}

void set_color(int idx, byte max_val=pwm_max_duty) {
  analogWrite(R_PWM, 255-rgb2pwm(colors[idx][0], max_val));
  analogWrite(G_PWM, 255-rgb2pwm(colors[idx][1], max_val));
  analogWrite(B_PWM, 255-rgb2pwm(colors[idx][2], max_val));
}

void loop() {
  // Update buttons, encoders and leds state
  update_buttons();
  update_encoders();
  set_color(enc1_val % 7);

  // ACTUAL LOGIC
  //enc1
  if (b1) { // windows mode
    if (incr1 != 0){
        Keyboard.press(KEY_LEFT_ALT); 
        // if (incr1 < 0) Keyboard.press(KEY_LEFT_SHIFT); //messes up the keyboard layout
        for (int i=0; i < abs(incr1); i++) Keyboard.press(KEY_ESC);
        Keyboard.releaseAll();
      }
  }
  else { //tabs mode
    if (incr1 != 0){
        Keyboard.press(KEY_LEFT_CTRL);
        for (int i=0; i<abs(incr1); i++) 
            if (incr1 > 0) Keyboard.press(KEY_PAGE_DOWN);
            else Keyboard.press(KEY_PAGE_UP);  
        Keyboard.releaseAll();
      }
  }

  //enc2
  // if (cli2) Consumer.write(MEDIA_PLAY_PAUSE); 
  //control volume
  for (int i=0; i < incr2*incr2; i++) //square the increment to make it faster
    if (incr2 > 0) Consumer.write(MEDIA_VOLUME_UP);
    else if (incr2 < 0) Consumer.write(MEDIA_VOLUME_DOWN);

  //buttons
  if (cli3) Consumer.write(MEDIA_PLAY_PAUSE);
  if (cli4) Consumer.write(MEDIA_VOL_MUTE);
  if (cli5) Consumer.write(MEDIA_PREVIOUS);
  if (cli6) Consumer.write(MEDIA_NEXT);
  
  delay(2);
}