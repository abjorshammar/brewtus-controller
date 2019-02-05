

float checkTemp(){
  sensors.requestTemperatures();
  float tempRead = sensors.getTempC(tempProbe);
  if (tempRead == -127.00) {
    currentTemp = 0.00;
  } 
  else {
    currentTemp = tempRead;
  }
  return currentTemp;
}

bool runPid(){
  
  // PID
  oldInput = input;
  input = checkTemp();
  if (input == 0.00){
    digitalWrite(relayPin,LOW);
    relayStatus = false;
    return relayStatus;
  }
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
  if (output > now - windowStartTime){
    digitalWrite(relayPin,HIGH);
    relayStatus = true;
  }
  else {
    digitalWrite(relayPin,LOW);
    relayStatus = false;
  }
  return relayStatus;
}
