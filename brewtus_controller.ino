// Sketch to use an Arduino as tempcontroller
// in an Expobar Brewtus II espresso machine

//--> Import libraries <--
#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal.h>
#include <PID_v1.h>


//--> Setup variables <--

// The target temperature
float desiredTemp = 93;

// Pins
int relayPin = 11;
int tempPin = 12;

// Timing
long previousMillis = 0;
long tempInterval = 1000;
long serialPrintTime = 5000;
long currentTime = 0;

// PID
double setpoint = 93;
double input = 0;
double oldInput = 0;
double output;
int windowSize = 5000;
unsigned long windowStartTime;

// Defaults
int displayMode = 0;
float temp = 0;
float oldTemp = 0;
boolean relayStatus = true;
boolean oldRelayStatus = false;
boolean refreshDisplay = true;

// Custom LCD graphics
byte on[8] = {
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
};
byte off[8] = {
  B11111,
  B10001,
  B10001,
  B10001,
  B10001,
  B10001,
  B11111,
};


//--> Configure libs <--

// Setup tempsensor
#define ONE_WIRE_BUS tempPin

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);
DeviceAddress tempProbe = { 0x28, 0x2D, 0x48, 0x4E, 0x05, 0x00, 0x00, 0xF5 };
//DeviceAddress tempProbe = { 0x28, 0xFF, 0x70, 0xFC, 0x10, 0x14, 0x00, 0xDB };

// Initialize LCD display
LiquidCrystal lcd(5, 4, 0, 1, 2, 3);

// Setup PID
#define RelayPin 11
PID myPID(&input, &output, &setpoint,800,0,0, DIRECT);


//--> Arduino Setup <--

void setup(void)
{
  // Setup the LCD
  lcd.begin(16, 2);
  lcd.createChar(1, on);
  lcd.createChar(2, off);
  // Print startup message to LCD
  lcd.clear();
  lcd.print("-= Brewtus II =-");

  // start serial port
  Serial.begin(9600);

  // Setup relay pin
  pinMode(relayPin, OUTPUT);

  // Start up the sensors library
  sensors.begin();
  // set the resolution to 10 bit
  sensors.setResolution(tempProbe, 12);

  // Setup PID
  windowStartTime = millis();
  myPID.SetOutputLimits(0, windowSize);
  myPID.SetMode(AUTOMATIC);

  // Add a delay to show the "boot screen"
  delay(2000);
}


//--> Functions <--

float checkTemp(){
  sensors.requestTemperatures();
  float tempRead = sensors.getTempC(tempProbe);
  if (tempRead == -127.00) {
    return 0.00;
  } 
  return tempRead;
}

boolean controlRelay(float currentTemp, float targetTemp){
  if (currentTemp == 0.00){
    digitalWrite(relayPin, LOW);
    return false;
  }
  else if (currentTemp >= targetTemp - 0.5){
    digitalWrite(relayPin, LOW);
    return false;
  }
  else if (currentTemp <= targetTemp - 0.75){
    digitalWrite(relayPin, HIGH);
    return true;
  }
}

void printDisplay(int mode, double temp, boolean relayStatus){
  if (mode == 0){
    lcd.clear();
    lcd.print("Temp: ");
    if (temp == 0.00) {
      lcd.print("Read Error");
    }
    else {
      lcd.print(temp);
      lcd.print((char)223);
    }
    lcd.setCursor(0,1);
    lcd.print(" Set: ");
    lcd.print(setpoint, 0);
    lcd.print((char)223);
    lcd.print(" Heat:");
    if (relayStatus == true){
      lcd.write(1);
    }
    else {
      lcd.write(2);
    }
  }
  refreshDisplay = false;
}


//--> Arduino Main Loop <--

void loop(void){
//  unsigned long currentMillis = millis();
//
//  // Is it time to check the temp?
//  if(currentMillis - previousMillis > tempInterval) {
//    previousMillis = currentMillis;
//
//    temp = checkTemp();
//    relayStatus = controlRelay(temp, desiredTemp);
//
//    if (temp != oldTemp){
//      refreshDisplay = true;
//      oldTemp = temp;
//    }
//    else if (relayStatus != oldRelayStatus){
//      refreshDisplay = true;
//      oldRelayStatus = relayStatus;
//    }
//  }
//
//  if (refreshDisplay == true){
//    printDisplay(displayMode);
//  }

  // PID
  oldInput = input;
  input = checkTemp();
  myPID.Compute();

  /************************************************
   * turn the output pin on/off based on pid output
   ************************************************/
  unsigned long now = millis();

  if(now - previousMillis > serialPrintTime) {
    previousMillis = now;
    currentTime = currentTime + serialPrintTime / 1000;
    Serial.print(currentTime);
    Serial.print(",");
    Serial.print(input);    
    Serial.print("\n");
  }

  if (now - windowStartTime > windowSize){
    //time to shift the Relay Window
    windowStartTime += windowSize;                   
  }
  oldRelayStatus = relayStatus;
  if (input == 0.00){
    digitalWrite(RelayPin,LOW);
    relayStatus = false;
  }
  else if (output > now - windowStartTime){
    digitalWrite(RelayPin,HIGH);
    relayStatus = true;
  }
  else {
    digitalWrite(RelayPin,LOW);
    relayStatus = false;
  }

  if (relayStatus != oldRelayStatus){
    printDisplay(displayMode, input, relayStatus);
  }
  if (input != oldInput){
    printDisplay(displayMode, input, relayStatus);
  }

}
