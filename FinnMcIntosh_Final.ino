int bypassPin = 33;
int led = 36;
int axes[2] = {39, 38};
String axesNames[2] = {"X", "Y"};


bool bypassState = LOW;
bool lastBypassState = LOW;
int lastAxesVals[2] = {0, 0};
int axesVals[2] = {0, 0};

void setup() {
  // put your setup code here, to run once:
  pinMode(bypassPin, INPUT);
  pinMode(led, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(led, 100);
  checkPot(0);
  checkPot(1);
}

void checkPot(int index) {
  axesVals[index] = map(analogRead(axes[index]), 0, 1023, 0, 127);
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
