#include "EasyWiFi.h"


EasyWiFi::EasyWiFi(int config_sw, int status_led, int timeout)
{
	m_config_sw = config_sw;
	m_status_led = status_led;
  wifiManager.setTimeout(timeout);
}

EasyWiFi::~EasyWiFi(void)
{
	
}

bool EasyWiFi::connect(bool config_mode)
{

  pinMode(m_status_led, OUTPUT);
  pinMode(m_config_sw, INPUT_PULLUP);
  
  if(!config_mode){
    digitalWrite(m_status_led, 0);
    for(int i = 0; i < 20; i++){
      delay(100);
      digitalWrite(m_status_led, i&1 ? 0 : 1);
      if(digitalRead(m_config_sw) == LOW){
        config_mode = true;
        break;
      }
    }
  }
  digitalWrite(m_status_led, 1);
  
  if(config_mode){
    Serial.println("Enter configuration");
    String ssid = "ESP" + String(ESP.getChipId());
    wifiManager.startConfigPortal(ssid.c_str(), NULL);
  }

  wifiManager.autoConnect();
  Serial.println("WiFi connected");

  // print MAC address
  byte mac[6];
  WiFi.macAddress(mac);
  char buf[20];
  Serial.print("MAC address: ");
  sprintf(buf,"%02X:%02X:%02X:%02X:%02X:%02X",mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
  Serial.println(buf);
  
  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI): ");
  Serial.print(rssi);
  Serial.println(" dBm");

  digitalWrite(m_status_led, 0);

  return true;
}

bool EasyWiFi::ota_begin(char *ota_passwd)
{
  if(ota_passwd) ArduinoOTA.setPassword((const char *)ota_passwd);

  ArduinoOTA.onStart([]() {
   Serial.println("Start OTA");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd OTA");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  Serial.println("OTA ready");

  return true;
}

void EasyWiFi::loop()
{
  if (WiFi.status() == WL_CONNECTED){
    ArduinoOTA.handle();
    //blink led
  }
  else{
    wifiManager.autoConnect();
  }
}

void EasyWiFi::led(int on)
{
    digitalWrite(m_status_led, on ? 0 : 1);
}
