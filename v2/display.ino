int getCenter(const char* msg){
  const int stringW = u8g2.getStrWidth(msg);
  return (128 - stringW) / 2;
}

int getRight(const char* msg){
  const int stringW = u8g2.getStrWidth(msg);
  return (128 - stringW);
}

int getLeft(const char* msg){
  const int stringW = u8g2.getStrWidth(msg);
  return (stringW);
}

void updateDisplay(float temp, const char* ip, bool relayStatus){ 
  u8g2.clearBuffer();

  // Temp
  int start = 0;
  int tempInt = temp;
  char tempBuf[8];
  dtostrf(temp,5,1,tempBuf);

  // Convert set point
  char setBuf[3];
  itoa(int(setpoint), setBuf, 10);
  const char* setStr = setBuf;
  
  // Set font, height and startpoints
  u8g2.setFont(u8g2_font_logisoso34_tf);
  const int height = 34;

  // Debug
  Serial.print("tempInt:");
  Serial.print(tempInt);
  Serial.print(" ,tempBuf:");
  Serial.print(tempBuf);
  Serial.print(" ,output:");
  Serial.print(output);
  Serial.print(", Heat:");
  Serial.println(relayStatus);
  
  // Draw the temp, degrees and C
  u8g2.drawStr(start,height,tempBuf);
  const int leftTempStr = start + getLeft(tempBuf);
  u8g2.drawStr(leftTempStr+2,height+6,"\xb0");
  u8g2.setFont(u8g2_font_logisoso16_tf);
  u8g2.drawStr(leftTempStr+10,height,"C");

  // Print set/target temp
  u8g2.setFont(u8g2_font_profont15_tf);
  u8g2.drawStr(64,50,"Set:");
  u8g2.drawStr(getRight(setStr)-12,50,setStr);
  //u8g2.drawStr(118,50,"\xb0");

  u8g2.drawHLine(0,53,128);

  // Print relay status
  const char* status;

  if (relayStatus == true) {
    status = "HEAT";
  } else if (temp == 0.00) {
    status = "ERROR";
  } else {
    status = " -- ";
  }
  u8g2.setFont(u8g2_font_profont10_tf);
  u8g2.drawStr(0,64,status);
  

  // Print IP
  u8g2.setFont(u8g2_font_siji_t_6x10);
  u8g2.drawGlyph(getRight(ip)-2, 64, 0x00e04a);
  u8g2.setFont(u8g2_font_profont10_tf);
  u8g2.drawStr(getRight(ip),64,ip);
  
  u8g2.sendBuffer();
}
