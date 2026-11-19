#include <Trill.h>

Trill trillSensor;
const boolean verbose = true;

int xPos = 0;
int yPos = 0;

int lastXPos = 0;
int lastYPos = 0;

int scalePin = 28;
int modePin = 34;
int bypassPin = 33;
int touchLed = 8;
int bypassLed = 10;

bool bypassState = LOW;
bool lastBypassState = LOW;
bool bypass = LOW;
bool isTouched = false;
bool lastIsTouched = false;
int note = 0;
int lastNote = 0;
int scales[2][7] = {
    {0, 2, 4, 5, 7, 9, 11},
    {0, 2, 3, 5, 7, 8, 10}
  };

void setup() {
  // put your setup code here, to run once:
  pinMode(modePin, INPUT);
  pinMode(bypassPin, INPUT);
  pinMode(bypassLed, OUTPUT);
  pinMode(touchLed, OUTPUT);
  pinMode(scalePin, INPUT);
  Serial.begin(9600);
  int ret = trillSensor.setup(Trill::TRILL_SQUARE);
  if (ret != 0) {
    Serial.println("failed to initialise trillSensor");
    Serial.print("Error code: ");
    Serial.println(ret);
  } else {
    if (verbose) {
      Serial.println("Success initialising trillSensor");
    }
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(50);
  trillSensor.read();
  checkTouch();
  
  sequence();
}

void sequence() {
  if(!digitalRead(modePin)){
    checkBypass();
    squareSeq_FX_MODE();
  } else{
    checkScale();
    posUpdate_KEYBOARD();
  }
}

void squareSeq_FX_MODE() {
  if (!isTouched && !bypass) {
    return;
  } else if (!isTouched && bypass) {
    posUpdate_BYPASS();
    return;
  }
  posUpdate_DEFAULT();
}

void posUpdate_DEFAULT(){
  xPos = map(trillSensor.touchHorizontalLocation(0), 0, 1792, 0, 127);
  yPos = map(trillSensor.touchLocation(0), 0, 1792, 0, 127);
  if (xPos != lastXPos) {
    usbMIDI.sendControlChange(0, xPos, 0);
    Serial.print("X: ");
    Serial.println(+xPos);
  }
  if (yPos != lastYPos) {
    usbMIDI.sendControlChange(1, yPos, 0);
    Serial.print("Y: ");
    Serial.println(+yPos);
  }
  lastXPos = xPos;
  lastYPos = yPos;
  delay(5);
}

void posUpdate_KEYBOARD(){
  isTouched = trillSensor.getNumTouches() > 0 || trillSensor.getNumHorizontalTouches() > 0;
  xPos = map(trillSensor.touchHorizontalLocation(0), 0, 1792, 0, 6);
  yPos = (map(trillSensor.touchLocation(0), 0, 1792, 0, 3) * 10) + 50 + map(analogRead(A13), 0, 1023, 0, 12);
  note = scales[checkScale()][xPos] + yPos;
  if ((isTouched && !lastIsTouched)) {
    usbMIDI.sendNoteOn(note, 127, 1);
    lastNote = note;
    Serial.println("TOUCHING");
  } else if (!isTouched && lastIsTouched) {
    usbMIDI.sendNoteOff(lastNote, 0, 1);
    Serial.println("NOT TOUCHING");
  } else if (isTouched && (note != lastNote)){
    usbMIDI.sendNoteOff(lastNote, 0, 1);
    usbMIDI.sendNoteOn(note, 127, 1);
    lastNote = note;
  }
  lastIsTouched = isTouched;
  delay(5);
}

void posUpdate_BYPASS() {
  xPos = map(analogRead(39), 0, 1023, 0, 127);
  yPos = map(analogRead(38), 0, 1023, 0, 127);
  if (xPos != lastXPos) {
    usbMIDI.sendControlChange(0, xPos, 0);
    Serial.print("X: ");
    Serial.println(+xPos);
  }
  if (yPos != lastYPos) {
    usbMIDI.sendControlChange(1, yPos, 0);
    Serial.print("Y: ");
    Serial.println(+yPos);
  }
  lastXPos = xPos;
  lastYPos = yPos;
  delay(5);
}

void checkBypass() {
  bypassState = digitalRead(bypassPin);
  if (bypassState != lastBypassState) {
    bypass = !bypass;
    if(bypass) {
      analogWrite(bypassLed, 50);
    } else {
      analogWrite(bypassLed, 0);
    }
  }
  lastBypassState = bypassState;
  delay(5);
}

void checkTouch() {
  isTouched = trillSensor.getNumTouches() > 0 || trillSensor.getNumHorizontalTouches() > 0;
  if (isTouched && !lastIsTouched) {
    digitalWrite(touchLed, 25);
  } else if (!isTouched && !lastIsTouched) {
    digitalWrite(touchLed, 0);
  }
  delay(5);
}

int checkScale() {
  if (digitalRead(scalePin)) {
    return 0;
  } else {
    return 1;
  }
}