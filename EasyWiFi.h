#ifndef EASYWIFI_H_
#define EASYWIFI_H_

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include <WiFiManager.h>
#include <Ticker.h>


class EasyWiFi {
public:
  EasyWiFi(int config_sw = 0, int status_led = 2, int timeout = 180);
  ~EasyWiFi();

  bool connect(bool force_config_mode = false);
  bool ota_begin(char *ota_passwd = NULL);
  void loop();
  void led(int on);
  
private:
  int m_config_sw;
  int m_status_led;
  WiFiManager wifiManager;

//  char m_ota_passwd[80];
};

#endif  // EASYWIFI_H_
