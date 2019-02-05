// Sketch to use an Wemos D1 mini as tempcontroller
// in an Expobar Brewtus II espresso machine

//--> Import libraries <--
#include <FS.h>
#include <Wire.h>
#include <Arduino.h>
#include <U8g2lib.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <PID_v1.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>

// Variables

// Pins
int relayPin = D3;
int tempPin = D4;

// PID
float desiredTemp = 93;
float offset = 7;
double setpoint = desiredTemp + offset;
double input = 0;
double oldInput = 0;
double output;

// Timing
long previousMillis = 0;
long tempInterval = 1000;
long serialPrintTime = 5000;
long currentTime = 0;


int windowSize = 5000;
unsigned long windowStartTime;

// Defaults
int displayMode = 0;
float currentTemp = 0;
float oldTemp = 0;
boolean relayStatus = true;
boolean oldRelayStatus = false;
boolean refreshDisplay = true;

#define DEVICE_NAME "BrewtusII"

// Setup generic
const char led = LED_BUILTIN;

// Setup display
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

// Setup server
DNSServer dnsServer;
ESP8266WebServer server ( 80 );

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(tempPin);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);
DeviceAddress tempProbe = { 0x28, 0x2D, 0x48, 0x4E, 0x05, 0x00, 0x00, 0xF5 };

// Setup PID
PID myPID(&input, &output, &setpoint,800,0,0, DIRECT);

// Setup
void setup(){

  // Start serial debug
  Serial.begin(115200);

  // Start display
  u8g2.begin();
  //u8g2.setDrawColor(0);
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_profont22_tf);
  const char* msg = "Brewtus II";
  u8g2.drawStr(getCenter(msg),25,msg);
  //u8g2.drawHLine(getCenter(msg),32,u8g2.getStrWidth(msg));
  u8g2.drawHLine(0,30,128);
  u8g2.setFont(u8g2_font_profont15_tf);
  const char* msg2 = "Starting...";
  u8g2.drawStr(getCenter(msg2),55,msg2);
  u8g2.sendBuffer();

  // Setup WiFi
  WiFiManager wifiManager;
  wifiManager.autoConnect("Brewtus");

  //allows serving of files from SPIFFS
  SPIFFS.begin();

  // Setup multicast DNS
  //if (MDNS.begin("brewtus")) {
  //  Serial.println("MDNS responder started");
  //}

  // Respond to root
  server.on("/", handleRoot);

  // Respond to api
  server.on("/api", handleApi);

  // Respond to not found
  server.onNotFound(handleNotFound);

  // Start webserver
  server.begin();
  Serial.println("HTTP server started");

  // Update display
  String ipTemp = WiFi.localIP().toString();
  const char *ip = ipTemp.c_str();
  updateDisplay(currentTemp, ip, false);

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
}


// Run loop
void loop(void){

  dnsServer.processNextRequest();
  server.handleClient();

  // Update display
  String ipTemp = WiFi.localIP().toString();
  const char *ip = ipTemp.c_str();
  relayStatus = runPid();
  updateDisplay(currentTemp, ip, relayStatus);
}
