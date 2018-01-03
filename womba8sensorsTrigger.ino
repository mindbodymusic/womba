/*
   8 sensor analog input for womba, June 2017

   Auto trigger mode

   3 modes, activated by sending 'r', 's', and 't'

   1.) 'r': Switch to poll mode and poll sensors once.
        no more data is sent until 'r' is received again
   2.) 's': Switch to stream mode and continuousl poll all sensors
   3.) 't': Trigger mode. keeps running average of past sensor values, and emits a trigger event
       if sensor value exceeds past value.

*/

//pin locations (assume continuous)
#define DIG_BEGIN   2
#define DIG_END    13
#define ANA_BEGIN   1
#define ANA_END     8

#define MIN_TRIGGER_THRES 10 //set this to lower to increase sensitivity
#define CALIBRATION_SAMPLES 200

#define NUM_ANALOG ANA_END-ANA_BEGIN+1
#define PAST_VALUES 100

enum OP_MODE
{
  WOMBA_POLL,
  WOMBA_STREAM,
  WOMBA_TRIGGER,
  WOMBA_CALIBRATE //unused for now as calibration is implcitly done when switching to trigger mode
};

int x = 0;                              // a place to hold pin values
int ledpin = 13;
OP_MODE currentMode;

//Data for trigger stuff

int pastData[NUM_ANALOG][PAST_VALUES];  //previous data in a long list
int dataPtr = 0;                        //current read/write position in list
int currVal[NUM_ANALOG];                //most recent value
int currAvg[NUM_ANALOG];                //current running average
int trigThres[NUM_ANALOG];              //thresholds for triggering
int calpointsRemain;

void setup() {
  Serial.begin(115200);               // 115200 is the default Arduino Bluetooth speed
  digitalWrite(13, HIGH);             ///startup blink
  delay(600);
  digitalWrite(13, LOW);
  pinMode(13, INPUT);
  Serial.print("Number of analog sensors = " );
  Serial.println(NUM_ANALOG);
  resetTriggerThres();
}



void loop()
{
  delay(10); //for testing

  if (Serial.available() > 0) {        // Check serial buffer for characters
    char readChar = Serial.read();
    //switch to poll mode, and poll
    if (readChar == 'r') {
      currentMode = WOMBA_POLL;
      readAndSendAnalog();
      Serial.println();
      //readAndSendDigital();
    }
    else if (readChar == 's') {
      currentMode = WOMBA_STREAM;

    }
    else if (readChar == 't') {
      resetTriggerThres();
      currentMode = WOMBA_TRIGGER;
      dataPtr = 0;
      calpointsRemain = CALIBRATION_SAMPLES;
      //fill our past data with values
      // so we have valid average
      for (int i = 0; i < PAST_VALUES; i++) {
        triggerCollectSamples();
      }
    }
  }

  //other operation modes
  if (currentMode == WOMBA_STREAM) {
    readAndSendAnalog();
    Serial.println();
    //readAndSendDigital();
  }
  else if (currentMode == WOMBA_TRIGGER) {
    triggerCollectSamples();
    //printTriggerData();
    checkAndSendTriggers();
  }
}

void readAndSendDigital() {

  for (int pin = DIG_BEGIN; pin <= DIG_END; pin++) { // Read and send digital pins 2-13
    x = digitalRead(pin);
    sendValue (x);
  }
}

void readAndSendAnalog() {
  for (int pin = ANA_BEGIN; pin <= ANA_END; pin++) {
    x = analogRead(pin);
    sendValue (x);
  }
}

void sendValue (int x) {             // function to send the pin value followed by a "space".
  Serial.print(x);
  Serial.write(32);
}

void printTriggerData() {
  Serial.print("CURR: ");
  for (int i = 0; i < NUM_ANALOG; i++) {
    sendValue(currVal[i]);
  }
  Serial.println();
  Serial.print("AVG : ");
  for (int i = 0; i < NUM_ANALOG; i++) {
    sendValue(currAvg[i]);
  }
  Serial.println();
}

void triggerCollectSamples() {
  for (int pin = ANA_BEGIN; pin <= ANA_END; pin++) {
    //store current value into list
    x = analogRead(pin);

    //get 0 indexed data slot
    int data_slot = pin - ANA_BEGIN;
    pastData[data_slot][dataPtr] = x;
    currVal[data_slot] = x;
    //calculate running average of past values
    int total = 0;
    for (int i = 0; i < PAST_VALUES; i++)
    {
      total += pastData[data_slot][i];
    }
    currAvg[data_slot] = total / PAST_VALUES;

  }
  dataPtr++;
  dataPtr = dataPtr % PAST_VALUES;
  //Serial.print("dataPtr = ");
  //Serial.println(dataPtr);

}

void checkAndSendTriggers() {

  if (calpointsRemain > 0) { //calibration mode
    for (int i = 0; i < NUM_ANALOG; i++) {
      int diff = abs(currAvg[i] - currVal[i]);
      if (diff > trigThres[i]) {
        trigThres[i] = diff;
      }
    }
    calpointsRemain--;
    if (calpointsRemain == 0) {
      Serial.println("Calibration Complete! Values:");
      for (int i = 0; i < NUM_ANALOG; i++) {
        Serial.print(trigThres[i]);
        Serial.print(" ");
      }
      Serial.println();
      delay(1000);
    }
  }
  else { //normal mode
    for (int i = 0; i < NUM_ANALOG; i++) {
      if (abs(currAvg[i] - currVal[i]) > trigThres[i]) {
        Serial.println(i);
      }
    }
    Serial.println(".");
  }
}

void resetTriggerThres() {
  for (int i = 0; i < NUM_ANALOG; i++) { //default threshold value for tiggers
    trigThres[i] = MIN_TRIGGER_THRES;
  }
}


