#ifndef configFileHandler_H
#define configFileHandler_H

class ConfigurationHandler {
  public:
    struct STA
    {
      char ssid[33];                      //WiFi STA mode SSID - 32 chars max are defined by standard
      char password[65];                  //WiFi STA mode Passsword - 64 chars are defined by standard
    };
    STA wifiSTA;
    struct AP
    {
      char ssid[33];                      //WiFi SoftAP mode SSID - 32 chars max are defined by standard
      char password[65];                  //WiFi SoftAP mode Passsword - 64 chars are defined by standard
    };
    AP wifiAP;
    struct stats
    {
      bool mdnsRunning;                   //mDNS successfully started
      bool wifiIsConnected;               //WiFi is connected to Infrastructure Network
      bool softApRunning;                 //Device functions as an AP
      bool authenticated;                 //webserver authentication
      bool mqttIsConnected;
      long auth_timestamp;                //Timestamp when the authentication happened. Being used to logout automatically after 5 minutes.
    };
    stats statuses;

    char hostString[17];                //for the mDNS identification, name of the Hardware Node. Populated in the start_mDNS() function
    char mqttServer[64];                   //MQTT server address
    char mqttUsername[64];
    char mqttPassword[64];
    char mqttSubTopic[16][16];             //MQTT topics to subscribe
    int mqttSubTopicCount;              //number of MQTT subscribe topics
    char deviceUUID[37];                //#DeviceUUID - identifies the device at the server application
    char fingerprint[64];
    char serContent[400];
    ConfigurationHandler(void);         //Constructor - Assigning values to variables
    void getContentSerd(char* text);             //Get configurations JSON string
    void initConfiguration(void);       //Load configuration from file and init file if originally not found
    void updateConfigJSON(void);        //Load the content of the ConfigurationHandler object into the Config JSON object
    //void saveConfig(void);              //Save config to file
    void serialCmdCheckInLoop(void);    //Check if serial port has received something and process
    void generateUUIDNumArr(byte *numarray); //generate UUID number array
    String generateUUIDStr(byte *numarray);

  private:
    int fileSize;                       //Size of config file
    char* defaultJson;                  //Default JSON string if file was not found
    byte uuidNumber[16];                //UUID byte array if file was not found
    String uuidStr;                     //UUID string for the device
    char configFileContent[400];
    char serialData[100];               //Raw data received from the serial port

    bool parseJsonPacket(void);         //Function that parses the received JSON data
};

#endif
