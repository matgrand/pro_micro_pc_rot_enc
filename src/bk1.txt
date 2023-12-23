#include <Arduino.h>
#include <Encoder.h>
#include <HID-Project.h>

#define R_PWM 5
#define G_PWM 3
#define B_PWM 6

#define BUTTON1 9
#define BUTTON2 8
#define BUTTON3 21
#define BUTTON4 20
#define BUTTON5 18
#define BUTTON6 19

//interrupts pins: 0 1 2 3 7
#define DT1 1
#define CLK1 0
#define DT2 2
#define CLK2 7

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

#define BUTTON_HIST 10

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
bool clicked1 = false; //button value
bool clicked2 = false; //button value
bool released1 = false; //button value
bool released2 = false; //button value
bool clicked3 = false; //button value
bool clicked4 = false; //button value
bool clicked5 = false; //button value
bool clicked6 = false; //button value
bool released3 = false; //button value
bool released4 = false; //button value
bool released5 = false; //button value
bool released6 = false; //button value

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

  // pwm
  analogWrite(R_PWM, 255); //255 is off
  analogWrite(G_PWM, 0);
  analogWrite(B_PWM, 0);

  // encoder
  enc1.write(0);
  enc2.write(0);

  delay(3000);

  Serial.println("Setup done");
}

long prev_e1 = 0;
long prev_e2 = 0;
void update_encoders() {
  // read encoder values, but update it only when it hits the 4th step
  long r1 = enc1.read();
  long r2 = enc2.read();

  if (r1 % 4 == 0) enc1_val = r1 / 4;
  if (r2 % 4 == 0) enc2_val = r2 / 4;
  incr1 = enc1_val - prev_e1;
  incr2 = enc2_val - prev_e2;
  prev_e1 = enc1_val;
  prev_e2 = enc2_val;
}

byte cc1 = 0; //cycle clicked 1
byte cr1 = 0; //cycle released 1
byte cc2 = 0; //cycle clicked 2
byte cr2 = 0; //cycle released 2
byte cc3 = 0; //cycle clicked 3
byte cr3 = 0; //cycle released 3
byte cc4 = 0; //cycle clicked 4
byte cr4 = 0; //cycle released 4
byte cc5 = 0; //cycle clicked 5
byte cr5 = 0; //cycle released 5
byte cc6 = 0; //cycle clicked 6
byte cr6 = 0; //cycle released 6
void update_buttons() {
  byte r1 = !digitalRead(BUTTON1);
  byte r2 = !digitalRead(BUTTON2);
  byte r3 = !digitalRead(BUTTON3);
  byte r4 = !digitalRead(BUTTON4);
  byte r5 = !digitalRead(BUTTON5);
  byte r6 = !digitalRead(BUTTON6);

  if (r1 && !b1) {cc1 = min(cc1+1, 255); cr1=0;}
  else if (!r1 && b1) {cr1 = min(cr1+1, 255); cc1=0;}
  if (!b1 && cc1 > BUTTON_HIST) {clicked1=true; cc1=0; b1=true;}
  else clicked1=false;
  if (b1 && cr1 > BUTTON_HIST) {released1 = true; cr1 = 0; b1=false;}
  else released1=false;

  if (r2 && !b2) {cc2 = min(cc2+1, 255); cr2=0;}
  else if (!r2 && b2) {cr2 = min(cr2+1, 255); cc2=0;}
  if (!b2 && cc2 > BUTTON_HIST) {clicked2=true; cc2=0; b2=true;}
  else clicked2=false;
  if (b2 && cr2 > BUTTON_HIST) {released2 = true; cr2 = 0; b2=false;}
  else released2=false;

  if (r3 && !b3) {cc3 = min(cc3+1, 255); cr3=0;}
  else if (!r3 && b3) {cr3 = min(cr3+1, 255); cc3=0;}
  if (!b3 && cc3 > BUTTON_HIST) {clicked3=true; cc3=0; b3=true;}
  else clicked3=false;
  if (b3 && cr3 > BUTTON_HIST) {released3 = true; cr3 = 0; b3=false;}
  else released3=false;

  if (r4 && !b4) {cc4 = min(cc4+1, 255); cr4=0;}
  else if (!r4 && b4) {cr4 = min(cr4+1, 255); cc4=0;}
  if (!b4 && cc4 > BUTTON_HIST) {clicked4=true; cc4=0; b4=true;}
  else clicked4=false;
  if (b4 && cr4 > BUTTON_HIST) {released4 = true; cr4 = 0; b4=false;}
  else released4=false;

  if (r5 && !b5) {cc5 = min(cc5+1, 255); cr5=0;}
  else if (!r5 && b5) {cr5 = min(cr5+1, 255); cc5=0;}
  if (!b5 && cc5 > BUTTON_HIST) {clicked5=true; cc5=0; b5=true;}
  else clicked5=false;
  if (b5 && cr5 > BUTTON_HIST) {released5 = true; cr5 = 0; b5=false;}
  else released5=false;

  if (r6 && !b6) {cc6 = min(cc6+1, 255); cr6=0;}
  else if (!r6 && b6) {cr6 = min(cr6+1, 255); cc6=0;}
  if (!b6 && cc6 > BUTTON_HIST) {clicked6=true; cc6=0; b6=true;}
  else clicked6=false;
  if (b6 && cr6 > BUTTON_HIST) {released6 = true; cr6 = 0; b6=false;}
  else released6=false;
}

void set_color(int idx, byte max_val=pwm_max_duty) {
  int r = rgb2pwm(colors[idx][0], max_val);
  int g = rgb2pwm(colors[idx][1], max_val);
  int b = rgb2pwm(colors[idx][2], max_val);
  analogWrite(R_PWM, 255-r);
  analogWrite(G_PWM, 255-g);
  analogWrite(B_PWM, 255-b);
}

void loop() {
  update_buttons();
  update_encoders();
  set_color(enc1_val % 7);

  // // serial print a recap of the values
  // Serial.print("enc1: ");
  // Serial.print(enc1_val);
  // Serial.print(" enc2: ");
  // Serial.print(enc2_val);
  // Serial.print(" b1: ");
  // Serial.print(b1);
  // Serial.print(" b2: ");
  // Serial.print(b2);
  // Serial.print(" b3: ");
  // Serial.print(b3);
  // Serial.print(" b4: ");
  // Serial.print(b4);
  // Serial.print(" b5: ");
  // Serial.print(b5);
  // Serial.print(" b6: ");
  // Serial.print(b6);
  // Serial.println();

  if (b1) { // windows mode
    if (incr1 != 0){
        Keyboard.press(KEY_LEFT_ALT); 
        // if (incr1 < 0) Keyboard.press(KEY_LEFT_SHIFT); //messes up the keyboard layout
        for (int i=0; i<abs(incr1); i++) Keyboard.press(KEY_ESC);
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
  // if (clicked2) Consumer.write(MEDIA_PLAY_PAUSE); 
  //control volume
  for (int i=0; i < incr2*incr2; i++) //square the increment to make it faster
    if (incr2 > 0) Consumer.write(MEDIA_VOLUME_UP);
    else if (incr2 < 0) Consumer.write(MEDIA_VOLUME_DOWN);

  //buttons
  if (clicked3) Consumer.write(MEDIA_PLAY_PAUSE);
  if (clicked4) Consumer.write(MEDIA_VOL_MUTE);
  if (clicked5) Consumer.write(MEDIA_PREVIOUS);
  if (clicked6) Consumer.write(MEDIA_NEXT);
  
  delay(2);
}