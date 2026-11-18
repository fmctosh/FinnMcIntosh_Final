#include <Trill.h>

Trill trillSensor;
const boolean verbose = true;

int xPos = 0;
int yPos = 0;

int lastXPos = 0;
int lastYPos = 0;

int modePin = 34;
int bypassPin = 33;
int led = 31;
int axesPins[2] = { 39, 38 };
String axesNames[2] = { "X", "Y" };

bool bypassState = LOW;
bool lastBypassState = LOW;
bool bypass = LOW;
bool isTouched = false;
bool lastIsTouched = false;
int lastAxesVals[2] = { 0, 0 };
int axesVals[2] = { 0, 0 };
int note = 0;
int lastNote = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(modePin, INPUT);
  pinMode(bypassPin, INPUT);
  pinMode(led, OUTPUT);
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
  // if(digitalRead(modePin) == HIGH){
  //   fxSeq();
  // } else {
  //   checkNote_KEY_MODE();
  // }
  checkTouch();
  checkBypass();
  squareSeq_FX_MODE();
}

void fxSeq() {
  checkPot_FX_MODE(0);
  checkPot_FX_MODE(1);
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

void checkPot_FX_MODE(int index) {
  axesVals[index] = map(analogRead(axesPins[index]), 0, 1023, 0, 127);
  if (axesVals[index] != lastAxesVals[index]) {
    usbMIDI.sendControlChange(index, axesVals[index], 0);
    Serial.println(axesNames[index] + ": " + axesVals[index]);
  } else {
    digitalWrite(led, 0);
  }
  lastAxesVals[index] = axesVals[index];
  delay(5);
}

void checkNote_KEY_MODE() {

  bypassState = digitalRead(bypassPin);
  if (bypassState && !lastBypassState) {
    note = map(analogRead(axesPins[0]), 0, 1023, 0, 10) * 12 + map(analogRead(axesPins[1]), 0, 1023, 0, 11);
    usbMIDI.sendNoteOn(note, 127, 1);
    lastNote = note;
  } else if (bypassState == LOW && lastBypassState == HIGH) {
    usbMIDI.sendNoteOff(lastNote, 0, 1);
  }
  lastBypassState = bypassState;
  delay(5);
}

void checkBypass() {
  bypassState = digitalRead(bypassPin);
  if (bypassState != lastBypassState) {
    bypass = !bypass;
  }
  delay(5);
}

void checkTouch() {
  isTouched = trillSensor.getNumTouches() > 0 || trillSensor.getNumHorizontalTouches() > 0;
  if (isTouched && !lastIsTouched) {
    digitalWrite(led, 25);
  } else if (!isTouched && !lastIsTouched) {
    digitalWrite(led, 0);
  }
  lastIsTouched = isTouched;
  delay(5);
}