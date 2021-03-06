ConfigurationHandler::ConfigurationHandler() {
#ifdef DEBUG
  Serial.println("ConfigurationHandler object has been created");
#endif
  statuses.mdnsRunning = false;
  statuses.wifiIsConnected = false;
  statuses.softApRunning = false;
  statuses.authenticated = false;
  statuses.mqttIsConnected = false;
  sprintf(hostString, "KEECO_%06X", (uint16_t)ESP.getEfuseMac());
  strcpy(wifiSTA.ssid, "DefaultSSID");
  strcpy(wifiSTA.password, "12345678");
  strcpy(fingerprint, "00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00");
  strcpy(mqttServer, "mlhass.local");
  strcpy(mqttUsername, "NA");
  strcpy(mqttPassword, "NA");

  strcpy(wifiAP.password, "12345678");
}

void ConfigurationHandler::generateUUIDNumArr(byte *numarray){
  for (int i=0; i<16; i++) {
    numarray[i] = random(255);
  }
}

String ConfigurationHandler::generateUUIDStr(byte *numarray) {
  String result;
  for (int i=0; i<16; i++){
    result = result + String(numarray[i], HEX);
    if ((i == 3 )|| (i == 5) || (i == 7)) {
      result = result + "-";
    }
  }
  return result;
}

void ConfigurationHandler::getContentSerd(char* text) {
  char tempJsonString[400];
  File configFile = SPIFFS.open("/config.txt", "r");
  fileSize = configFile.size();
  if (fileSize > 320) {
    fileSize = 319;
  }
  configFile.readBytes(text, fileSize);
  configFile.close();
  //return tempJsonString;
}
void ConfigurationHandler::initConfiguration() {
  StaticJsonDocument<500> configDoc;
  DeserializationError configError;
  char tempJsonString[400];

  if ( ! SPIFFS.exists("/config.txt")) {
    File cfgFile = SPIFFS.open("/config.txt", "w");
    Serial.println("Config file doesn't exists, creating initial JSON file with new UUID");
    configDoc.clear();
    configDoc["ssid"] = wifiSTA.ssid;
    configDoc["password"] = wifiSTA.password;
    configDoc["tls_fingerprint"] = fingerprint;
    configDoc["mqttserver"] = mqttServer;
    configDoc["mqttusername"] = mqttUsername;
    configDoc["mqttpassword"] =  mqttPassword;
    generateUUIDNumArr(uuidNumber);
    uuidStr = generateUUIDStr(uuidNumber);
    uuidStr.toCharArray(deviceUUID, 37);
    configDoc["uuid"] = deviceUUID;
    serializeJson(configDoc, tempJsonString);
    cfgFile.print(tempJsonString);
    cfgFile.close();
  }

  File configFile = SPIFFS.open("/config.txt", "r");
  fileSize = configFile.size();
  if (fileSize > 320) {
    fileSize = 319;
  }
  configFile.readBytes(tempJsonString, fileSize);
  configFile.close();
  configDoc.clear();
  configError = deserializeJson(configDoc, tempJsonString);
  if (configError) {
    Serial.print(F("deserializeJson() failed in InitConfiguration function: "));
    Serial.println(configError.c_str());
    return;
  }
  const char* ssid = configDoc["ssid"].as<char*>();
  strcpy(wifiSTA.ssid, ssid);
  const char* password = configDoc["password"].as<char*>();
  strcpy(wifiSTA.password, password);
  const char* mqttsrvtemp = configDoc["mqttserver"].as<char*>();
  strcpy(mqttServer, mqttsrvtemp);
  const char* mqttuser = configDoc["mqttusername"].as<char*>();
  strcpy(mqttUsername, mqttuser);
  const char* mqttpasstemp = configDoc["mqttpassword"].as<char*>();
  strcpy(mqttPassword, mqttpasstemp);
  const char* temp_fingerprint = configDoc["tls_fingerprint"].as<char*>();
  strcpy(fingerprint, temp_fingerprint);
  const char* devUUID = configDoc["uuid"].as<char*>();
  strcpy(deviceUUID, devUUID);

#ifdef DEBUG
  Serial.println("Config file has been successfully opened");
  Serial.println("Config file content:");
  getContentSerd(serContent);
  Serial.println(serContent);
#endif
}

void ConfigurationHandler::updateConfigJSON() {
  StaticJsonDocument<500> configDoc;
  DeserializationError configError;
  char tempJsonString[400];

  configDoc.clear();
  configDoc["ssid"] = wifiSTA.ssid;
  configDoc["password"] = wifiSTA.password;
  configDoc["tls_fingerprint"] = fingerprint;
  configDoc["uuid"] = deviceUUID;
  configDoc["mqttserver"] = mqttServer;
  configDoc["mqttusername"] = mqttUsername;
  configDoc["mqttpassword"] =  mqttPassword;
  serializeJson(configDoc, tempJsonString);
#ifdef DEBUG
  Serial.println("Configuration JSON Object has been updated");
#endif
  File cfgFile = SPIFFS.open("/config.txt", "w");
  Serial.println("Saving config data");
  serializeJson(configDoc, tempJsonString);
  cfgFile.print(tempJsonString);
  cfgFile.close();
#ifdef DEBUG
  Serial.println("Configuration JSON File has been updated updated");
  Serial.println(tempJsonString);
#endif
}

void ConfigurationHandler::serialCmdCheckInLoop() {
  if (Serial.available()) {
    Serial.readBytesUntil('\r', serialData, 80);
    parseJsonPacket();
  }
}

bool ConfigurationHandler::parseJsonPacket() {
  StaticJsonDocument<200> configUartDoc;
  DeserializationError error;
  error = deserializeJson(configUartDoc, serialData);
#ifdef DEBUG
  Serial.println("Received Command: ");
  Serial.println(serialData);
#endif
  if (error) {
    Serial.print(F("deserializeJson() failed in Serial Command Parser: "));
    Serial.println(error.c_str());
    return false;
  }
  const char* command = configUartDoc["command"].as<char*>();
  if (strcmp(command, "wifi") == 0) {
    strcpy(wifiSTA.ssid, configUartDoc["ssid"]);
    strcpy(wifiSTA.password, configUartDoc["password"]);
  }
  if (strcmp(command, "mqtt") == 0) {
    strcpy(mqttServer, configUartDoc["server"]);
  }
  if (strcmp(command, "fingerprint") == 0) {
    strcpy(fingerprint, configUartDoc["fingerprint"]);
  }
  if (strcmp(command, "save") == 0) {
    updateConfigJSON();
    //saveConfig();
  }
  if (strcmp(command, "reset") == 0) {
    Serial.println("Resetting device...");
    ESP.restart();
  }
  if (strcmp(command, "read") == 0) {
    getContentSerd(serContent);
  Serial.println(serContent);
  }
  if (strcmp(command, "help") == 0) {
    Serial.println("Available commands:");
    Serial.println("{Set new SSID and Password:");
    Serial.println("{\"command\":\"wifi\", \"ssid\":\"___\", \"password\":\"___\"}");
    Serial.println("Set new MQTT Server:");
    Serial.println("{\"command\":\"mqtt\", \"server\":\"___\"}");
    Serial.println("Set new TLS fingerprint:");
    Serial.println("{\"command\":\"fingerprint\", \"fingerprint\":\"___\"}");
    Serial.println("Save current settings into LittleFS:");
    Serial.println("{\"command\":\"save\"}");
    Serial.println("Reset device:");
    Serial.println("{\"command\":\"reset\"}");
    Serial.println("Read settings JSON string:");
    Serial.println("{\"command\":\"read\"}");
  }
  return true;
}
