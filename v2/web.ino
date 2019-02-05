// Web root
void handleRoot() {
  digitalWrite(led, 1);
  server.send(200, "text/plain", "Brewtus Status");
  digitalWrite(led, 0);
}

// Api
void handleApi() {
  String statusResp;
  statusResp += "{";
  statusResp += "\"settemp\":\"";
  statusResp += desiredTemp;
  statusResp += "\",";
  statusResp += "\"offset\":\"";
  statusResp += offset;
  statusResp += "\",";
  statusResp += "\"currenttemp\":\"";
  statusResp += currentTemp;
  statusResp += "\"";
  statusResp += "}";
  server.send(200, "application/json", statusResp);
}

// Handle 404 file not found
void handleNotFound(){
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}
