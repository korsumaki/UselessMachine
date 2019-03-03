/*
 * Useless machine - Markku Korsumäki
 * 
 * Idea from: http://www.lamja.com/?p=451
 * 
 * Connections:
 *   pin 2  - in:  On/off switch
 *   pin 9  - out: Finger servo control
 *   pin 10 - out: Cover servo control
 *   pin 11 - out: Servo power switch (MOSFET) <- NOT USED CURRENTLY
 * 
 */

/* 
 * TODO
 * + implement switch to start the machine
 * + two servos connected (when available)
 * + write more sequences
 * + mechanics: build the box (when size is known)
 * + switch to use nano -> can fit in the box
 * + update regulator to schematics
 * + change Arduino NANO to schematics
 * + readme file
 * + remove startup blink from code
 *    - test without blink
 * - battery use
 * - power save
 *   +- mosfet to switch power off -> tried but not yet good
 *   - sleep mode
 *     - using interrupt to wake up
 *     - setting unused pins to HIGH?
 * - measure power consumption
 *   - servos powered on, but not moving
 *   - servos powered off
 */

//#include <Wire.h>

#include <Servo.h>

Servo fingerServo;
Servo coverServo;

const int onOffSwitchPin = 2;
const int fingerServoPin = 9;
const int coverServoPin = 10;
//const int servoPowerSwitchPin = 11;


// Positions for servos
const int FINGER_OUT_TOUCH = 60;
const int FINGER_OUT = 62;
const int FINGER_NEAR_OUT = 75;
const int FINGER_MID = 100;
const int FINGER_IN = 132;

const int COVER_CLOSED = 58;
const int COVER_SLIGHTLY = 63;
const int COVER_MID = 73;
const int COVER_OPEN_MIN = 82;
const int COVER_OPEN = 86;
const int COVER_OPEN_FULL = 90;


// For faster speeds, use delayMicroseconds when needed
const unsigned long SPEED_NOW = 0;
const unsigned long SPEED_FAST = 5;
const unsigned long SPEED_MEDIUM = 10;
const unsigned long SPEED_SLOW = 20;
const unsigned long SPEED_EXTRA_SLOW = 50;



void assert(/*String str, */ int currentPid) {
  pinMode(LED_BUILTIN, OUTPUT);
  while (1) {
    for (int i=0; i<= currentPid; ++i) {
      digitalWrite(LED_BUILTIN, HIGH); // Led13 on
      delay(250);
      digitalWrite(LED_BUILTIN, LOW); // Led13 off
      delay(250);
    }
  delay(1500);
  }
}

void startupBlink() {
  pinMode(LED_BUILTIN, OUTPUT);
  for (int i=0; i<5; ++i) {
    digitalWrite(LED_BUILTIN, HIGH); // Led13 on
    delay(250);
    digitalWrite(LED_BUILTIN, LOW); // Led13 off
    delay(250);
  }
}


// function that executes whenever data is received from master
// this function is registered as an event, see setup()
/*
void receiveEvent(int howMany) {
  // 1 byte: servo 1 or 2
  // 1 byte: position, high
  // 1 byte: position, low
  int servo = Wire.read();
  int pos_high = Wire.read();
  int pos_low = Wire.read();

  int pos = (pos_high<<8) + pos_low;
  if (servo == 1) {
    coverServo.write(pos);
    //move(coverServo, pos, SPEED_MEDIUM);
  }
  else if (servo == 2) {
    fingerServo.write(pos);
    //move(fingerServo, pos, SPEED_MEDIUM);
  }
}
*/

void setup() {
  //startupBlink();
  
  pinMode(onOffSwitchPin, INPUT);
  //pinMode(servoPowerSwitchPin, OUTPUT);

  fingerServo.attach(fingerServoPin);
  coverServo.attach(coverServoPin);

  fingerServo.write(FINGER_IN);
  coverServo.write(COVER_CLOSED);
  //delay(100);

  // UselessMachine is I2C slave
  //Wire.begin(8);                // join i2c bus with address #8
  //Wire.onReceive(receiveEvent); // register event
}

const int STATE_MACHINE_OFF = 0;
const int STATE_MACHINE_ON = 1;

int currentState = STATE_MACHINE_OFF;
int seq = 0;

void sequence(int num) {
  const int NUM_OF_FUNCS = 11;
  num = random(NUM_OF_FUNCS);
  switch (num) {
    case 0: seq_0(); break;
    case 1: seq_1(); break;
    case 2: seq_2(); break;
    case 3: seq_3(); break;
    case 4: seq_4(); break;
    case 5: seq_5(); break;
    case 6: seq_6(); break;
    case 7: seq_7(); break;
    case 8: seq_8(); break;
    case 9: seq_9(); break;
    case 10: seq_10();
      seq = -1;
      break;
    default:
      // TODO parempi tapa palata listan alkuun... Random?
      seq = -1;
      break;
  }
  seq++;
}


void sequence_switch_off_lately() {
  const int NUM_OF_FUNCS = 2;
  int num = random(NUM_OF_FUNCS);
  switch (num) {
    case 0: seq_switch_off_lately_0(); break;
    case 1: seq_switch_off_lately_1(); break;
    case 2: seq_switch_off_lately_2(); break;
    default:
      // fallback
      seq_switch_off_lately_0();
      break;
  }
}


void loop() {
  int onOffState = LOW;
  switch(currentState) {
    case STATE_MACHINE_OFF:
      // read on off switch state
      onOffState = digitalRead(onOffSwitchPin);
      if (onOffState == HIGH) {
        currentState = STATE_MACHINE_ON;
      }
      break;
    case STATE_MACHINE_ON:
      delay(500);
      sequence(seq);
      currentState = STATE_MACHINE_OFF;
      delay(500);
      break;
    //default:
    //  break;
      //assert("Task 1: unknown state", pid);
  }
}

/*
void ServoPowerOn() {
  delay(300);
  digitalWrite(servoPowerSwitchPin, HIGH);
  delay(300);
}
void ServoPowerOff() {
  delay(300);
  digitalWrite(servoPowerSwitchPin, LOW);
  delay(300);
}
*/


//void move(Servo &s, int start, int position, int delayUs) {
void move(Servo &s, int position, int delayMs) {
  if (delayMs == SPEED_NOW) {
    s.write(position);
    return;
  }
  int start = s.read();
  if (start < position) {
    for (int i=start; i<position; ++i) {
      //s.writeMicroseconds(i);
      s.write(i);
      delay(delayMs);
    }
  }
  else {
    for (int i=start; i>position; --i) {
      s.write(i);
      delay(delayMs);
    }
  }
}

void seq_switch_off_early_0() 
{
  delay(1000);
  move(fingerServo, FINGER_IN, SPEED_MEDIUM);
  delay(300);
  move(coverServo, COVER_CLOSED, SPEED_EXTRA_SLOW);
}


// Check whether on-off -switch is put off during sequence.
// Does fallback sequence in that case.
// Returns true if switch was off, false is switch is not moved.
bool is_switch_off_when_cover_opened() 
{
  int onOffState = LOW;
  // read on off switch state
  onOffState = digitalRead(onOffSwitchPin);
  if (onOffState == LOW) {
    seq_switch_off_early_0();
    return true;
  }
  return false;
}

/*
 * 0 hitaasti kytkimelle, nopea naputus ja nopeasti kiinni
 * 1 täysillä nappi pohjaan ja heti kiinni
 * 2 vain heti takaisin kiinni
 * 
 */
void seq_switch_off_lately_0() 
{
  delay(700);
  move(fingerServo, FINGER_OUT, SPEED_EXTRA_SLOW);
  delay(300);
  
  for (int i=0; i<5; ++i) {
    move(fingerServo, FINGER_NEAR_OUT, SPEED_NOW);
    delay(50);
    move(fingerServo, FINGER_OUT_TOUCH, SPEED_NOW);
    delay(50);
  }
  delay(300);
  move(fingerServo, FINGER_IN, SPEED_NOW);
  delay(300);
  move(coverServo, COVER_CLOSED, SPEED_NOW);
}


void seq_switch_off_lately_1() 
{
  move(fingerServo, FINGER_OUT, SPEED_NOW);
  delay(200);
  move(fingerServo, FINGER_IN, SPEED_NOW);
  delay(200);
  move(coverServo, COVER_CLOSED, SPEED_NOW);
}

void seq_switch_off_lately_2() 
{
  move(fingerServo, FINGER_IN, SPEED_SLOW);
  delay(200);
  move(coverServo, COVER_CLOSED, SPEED_NOW);
}

// Check whether on-off -switch is put off during sequence.
// Does fallback sequence in that case.
// Returns true if switch was off, false is switch is not moved.
bool is_switch_off_when_finger_out() 
{
  int onOffState = LOW;
  // read on off switch state
  onOffState = digitalRead(onOffSwitchPin);
  if (onOffState == LOW) {
    sequence_switch_off_lately();
    return true;
  }
  return false;
}

/* 
 *  Sequences
 * 0 normaali (MEDIUM)
 * 1 nopea FAST (kiireinen)
 * 2 hidas SLOW
 * 3 extra slow
 * 4 minimi luukun avaus (ujo)
 * 5 luukun pamautus kiinni (äkäinen)
 * 6 kurkistus (puolet luukkua), luukun väristys, viive, avaus
 * 7 hidas alku/nopea loppu
 * 8 nopea alku/hidas loppu
 * 9 pitkä viive ennen alkua, aukeaminen vaiheittain, odottelua välillä
 * 10 kansi lyö näpeille
 * 
 * intervalli tyyliin
 * ? nopea luukun avaus ja puolet sormen liikkestä, loput hitaasti
 * 
 */


void seq_0() {
  move(coverServo, COVER_OPEN, SPEED_MEDIUM);
  delay(300);
  if (is_switch_off_when_cover_opened()) return;
  
  move(fingerServo, FINGER_NEAR_OUT, SPEED_MEDIUM);
  if (is_switch_off_when_finger_out()) return;
  
  move(fingerServo, FINGER_OUT, SPEED_NOW);
  delay(300);
  move(fingerServo, FINGER_IN, SPEED_MEDIUM);
  delay(300);
  move(coverServo, COVER_CLOSED, SPEED_MEDIUM);
}

void seq_1() {
  move(coverServo, COVER_OPEN, SPEED_FAST);
  delay(100);
  if (is_switch_off_when_cover_opened()) return;
  
  move(fingerServo, FINGER_NEAR_OUT, SPEED_FAST);
  move(fingerServo, FINGER_OUT, SPEED_NOW);
  delay(100);
  move(fingerServo, FINGER_IN, SPEED_FAST);
  delay(100);
  move(coverServo, COVER_CLOSED, SPEED_FAST);
}

void seq_2() {
  move(coverServo, COVER_OPEN, SPEED_SLOW);
  delay(300);
  if (is_switch_off_when_cover_opened()) return;
  
  move(fingerServo, FINGER_NEAR_OUT, SPEED_SLOW);
  if (is_switch_off_when_finger_out()) return;
  move(fingerServo, FINGER_OUT, SPEED_NOW);
  delay(300);
  move(fingerServo, FINGER_IN, SPEED_SLOW);
  delay(300);
  move(coverServo, COVER_CLOSED, SPEED_SLOW);
}

void seq_3() {
  move(coverServo, COVER_OPEN, SPEED_EXTRA_SLOW);
  delay(500);
  if (is_switch_off_when_cover_opened()) return;
  
  move(fingerServo, FINGER_NEAR_OUT, SPEED_EXTRA_SLOW);
  if (is_switch_off_when_finger_out()) return;
  move(fingerServo, FINGER_OUT, SPEED_NOW);
  delay(500);
  move(fingerServo, FINGER_IN, SPEED_EXTRA_SLOW);
  delay(500);
  move(coverServo, COVER_CLOSED, SPEED_EXTRA_SLOW);
}

void seq_4() {
  move(coverServo, COVER_OPEN_MIN, SPEED_SLOW);
  delay(300);
  if (is_switch_off_when_cover_opened()) return;
  
  move(fingerServo, FINGER_NEAR_OUT, SPEED_SLOW);
  move(fingerServo, FINGER_OUT, SPEED_NOW);
  delay(300);
  move(fingerServo, FINGER_IN, SPEED_SLOW);
  delay(300);
  move(coverServo, COVER_CLOSED, SPEED_SLOW);
}

void seq_5() {
  move(coverServo, COVER_OPEN, SPEED_FAST);
  delay(100);
  if (is_switch_off_when_cover_opened()) return;
  
  move(fingerServo, FINGER_NEAR_OUT, SPEED_FAST);
  if (is_switch_off_when_finger_out()) return;
  move(fingerServo, FINGER_OUT, SPEED_NOW);
  delay(100);
  move(fingerServo, FINGER_IN, SPEED_NOW);
  delay(200);
  move(coverServo, COVER_CLOSED, SPEED_NOW);
}

void seq_6() {
  move(coverServo, COVER_MID, SPEED_EXTRA_SLOW);
  delay(700);
  if (is_switch_off_when_cover_opened()) return;
  
  move(coverServo, COVER_CLOSED, SPEED_NOW);
  delay(2000);

  for (int i=0; i<15; ++i) {
    move(coverServo, COVER_SLIGHTLY, SPEED_NOW);
    delay(40);
    move(coverServo, COVER_CLOSED, SPEED_NOW);
    delay(40);
  }
  delay(1000);
  if (is_switch_off_when_cover_opened()) return;

  move(coverServo, COVER_OPEN_MIN, SPEED_FAST);
  delay(300);
  if (is_switch_off_when_cover_opened()) return;

  move(fingerServo, FINGER_NEAR_OUT, SPEED_FAST);
  if (is_switch_off_when_finger_out()) return;
  move(fingerServo, FINGER_OUT, SPEED_NOW);
  delay(100);
  move(fingerServo, FINGER_IN, SPEED_NOW);
  delay(500);
  
  move(coverServo, COVER_OPEN_FULL, SPEED_EXTRA_SLOW);
  delay(100);
  move(coverServo, COVER_CLOSED, SPEED_NOW);
}

void seq_7() {
  move(coverServo, COVER_OPEN, SPEED_SLOW);
  delay(500);
  if (is_switch_off_when_cover_opened()) return;
  
  move(fingerServo, FINGER_NEAR_OUT, SPEED_SLOW);
  if (is_switch_off_when_finger_out()) return;
  move(fingerServo, FINGER_OUT, SPEED_NOW);
  delay(100);
  move(fingerServo, FINGER_IN, SPEED_FAST);
  delay(100);
  move(coverServo, COVER_CLOSED, SPEED_FAST);
}

void seq_8() {
  move(coverServo, COVER_OPEN, SPEED_FAST);
  delay(100);
  if (is_switch_off_when_cover_opened()) return;
  
  move(fingerServo, FINGER_NEAR_OUT, SPEED_FAST);
  if (is_switch_off_when_finger_out()) return;
  move(fingerServo, FINGER_OUT, SPEED_NOW);
  delay(300);
  move(fingerServo, FINGER_IN, SPEED_SLOW);
  delay(500);
  move(coverServo, COVER_CLOSED, SPEED_SLOW);
}

void seq_9() {
  delay(1000);
  move(coverServo, COVER_SLIGHTLY, SPEED_EXTRA_SLOW);
  delay(500);
  if (is_switch_off_when_cover_opened()) return;
  move(coverServo, COVER_MID, SPEED_EXTRA_SLOW);
  delay(500);
  if (is_switch_off_when_cover_opened()) return;
  move(coverServo, COVER_OPEN_MIN, SPEED_EXTRA_SLOW);
  delay(500);
  if (is_switch_off_when_cover_opened()) return;
  move(coverServo, COVER_OPEN, SPEED_EXTRA_SLOW);
  delay(500);
  if (is_switch_off_when_cover_opened()) return;
  move(coverServo, COVER_OPEN_FULL, SPEED_EXTRA_SLOW);
  delay(500);
  if (is_switch_off_when_cover_opened()) return;
  
  move(fingerServo, FINGER_MID, SPEED_EXTRA_SLOW);
  delay(500);
  move(fingerServo, FINGER_NEAR_OUT, SPEED_EXTRA_SLOW);
  delay(500);
  if (is_switch_off_when_finger_out()) return;
  move(fingerServo, FINGER_OUT, SPEED_NOW);

  
  delay(500);
  move(fingerServo, FINGER_IN, SPEED_EXTRA_SLOW);
  delay(500);
  move(coverServo, COVER_CLOSED, SPEED_EXTRA_SLOW);
}

void seq_10() {
  move(coverServo, COVER_OPEN, SPEED_MEDIUM);
  delay(100);
  if (is_switch_off_when_cover_opened()) return;

  move(fingerServo, FINGER_NEAR_OUT, SPEED_SLOW);
  for (int i=0; i<5; ++i) {
    move(coverServo, COVER_MID, SPEED_NOW);
    delay(40);
    move(coverServo, COVER_OPEN_MIN, SPEED_NOW);
    delay(40);
  }

  move(fingerServo, FINGER_MID, SPEED_NOW);
  delay(100);
  move(coverServo, COVER_OPEN, SPEED_FAST);
  delay(500);

  move(fingerServo, FINGER_NEAR_OUT, SPEED_SLOW);
  if (is_switch_off_when_finger_out()) return;
  move(fingerServo, FINGER_OUT, SPEED_NOW);
  delay(100);
  move(fingerServo, FINGER_IN, SPEED_FAST);
  delay(300);
  move(coverServo, COVER_CLOSED, SPEED_MEDIUM);
}


