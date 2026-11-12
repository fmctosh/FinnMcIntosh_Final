int modePin = 34;
int bypassPin = 33;
int led = 36;
int axesPins[2] = {39, 38};
String axesNames[2] = {"X", "Y"};

bool bypassState = LOW;
bool lastBypassState = LOW;
int lastAxesVals[2] = {0, 0};
int axesVals[2] = {0, 0};
int note = 0;
int lastNote = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(modePin, INPUT);
  pinMode(bypassPin, INPUT);
  pinMode(led, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(digitalRead(modePin) == HIGH){
    fxSeq();
  } else {
    checkNote_KEY_MODE();
  }
  
}

void fxSeq() {
  checkPot_FX_MODE(0);
  checkPot_FX_MODE(1);
}

void checkPot_FX_MODE(int index) {
  axesVals[index] = map(analogRead(axesPins[index]), 0, 1023, 0, 127);
  if(axesVals[index] != lastAxesVals[index]) {
    usbMIDI.sendControlChange(index, axesVals[index], 0);
    Serial.println(axesNames[index] + ": " + axesVals[index]);
  }
  else {
    digitalWrite(led, 0);
  }
  lastAxesVals[index] = axesVals[index];
  delay(5);
}

void checkNote_KEY_MODE(){

  bypassState = digitalRead(bypassPin);
  if(bypassState == HIGH && lastBypassState == LOW) {
    note = map(analogRead(axesPins[0]), 0, 1023, 0, 10) * 12 + map(analogRead(axesPins[1]), 0, 1023, 0, 11);
    usbMIDI.sendNoteOn(note, 127, 1);
    lastNote = note;
  } else if(bypassState == LOW && lastBypassState == HIGH) {
    usbMIDI.sendNoteOff(lastNote, 0, 1);
  }
  lastBypassState = bypassState;
  delay(5);
  
}
