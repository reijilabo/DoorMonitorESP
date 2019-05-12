// WiFi Manager and OTA support
#include "EasyWiFi.h"

EasyWiFi easyWifi;

//========== User code ==========
#include <HttpClient.h>
#include <Ambient.h>
#include "DataToMaker.h"
#include <WEMOS_SHT3X.h>
#include "user_config.h"

const unsigned int channelId = AMBIENT_CHANNEL;
const char* writeKey = AMBIENT_WRITEKEY;

#define NUM_SENS 3
// sensorId1 "Temperature";
// sensorId2 "Humidity";
// sensorId3 "Door";

WiFiClient client;
Ambient ambient;

SHT3X sht30(0x45);

const char* webhookKey = MAKER_WEBHOOKKEY;

// declare new maker event
DataToMaker webhook(webhookKey);

const char* event_dooropen = MAKER_EVENT_dooropen;
const char* event_doorclose = MAKER_EVENT_doorclose;

int doorstate[2];
int doorhist = 0;
int doormax = 0;
#define DOOR_OPEN 1
#define DOOR_CLOSE 0

#define DOOR_OPEN_LAG 2
#define DOOR_CLOSE_LAG 3

#define adc2door(a) ((a) > 100 ? DOOR_OPEN : DOOR_CLOSE)

unsigned long lastupload = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("\n\nBooting");

  easyWifi.connect();
  easyWifi.ota_begin();

  //=========== User setup =============
  Serial.println("\n** Starting multiple datastream upload to Ambient... **");

  ambient.begin(channelId, writeKey, &client);

  doorstate[1] = doorstate[0] = adc2door(analogRead(A0));
  doorhist = 0;
  doormax = 0;

  // Start up the library
}

void loop()
{
  easyWifi.loop();

  float sensorValue[NUM_SENS];

  int sht30ok = 0;
  if(sht30.get()==0){
    sensorValue[0] = sht30.cTemp;
    sensorValue[1] = sht30.humidity;
    sht30ok = 1;
  }
  else{
    easyWifi.led(1);
    sensorValue[0] = 0.0;
    sensorValue[1] = 0.0;
  }
  int doorsens = analogRead(A0);
  if(doorsens > doormax) doormax = doorsens;
  sensorValue[2] = doormax;
  
  for(int i = 0; i < NUM_SENS; i++){
    ambient.set(i + 1, sensorValue[i]);
    webhook.setValue(i + 1, sensorValue[i]);
    Serial.print("Read sensor value ");
    Serial.println(sensorValue[i]);
  }

  char *event = NULL;
  int changed = 0;
  doorstate[1] = adc2door(doorsens);
  if(doorstate[1] == doorstate[0]){
    doorhist = 0;
  }
  else{
    doorhist++;
    int th = (doorstate[1] == DOOR_OPEN ? DOOR_OPEN_LAG : DOOR_CLOSE_LAG);
    if(doorhist >= th){
      changed = 1;
      doorstate[0] = doorstate[1];
      doorhist = 0;
      if(doorstate[0] == DOOR_OPEN){
        Serial.println("Door OPEN");
        event = (char *)event_dooropen;
      }
      else{
        Serial.println("Door CLOSE");
        event = (char *)event_doorclose;
      }
    }
  }

#if 1
  if(changed){
    Serial.println("Uploading to Webhook");
    bool ret = webhook.connect();
    if(ret){
      easyWifi.led(1);
      webhook.post(event);
      easyWifi.led(0);
    }
    Serial.print("Webhook send returned ");
    Serial.println(ret);
  }
#endif

  if(sht30ok && (millis() - lastupload >= (unsigned long)(30 * 1000))){
    Serial.println("Uploading to Ambient");
    easyWifi.led(1);
#if 1
    bool ret = ambient.send();
#else
    bool ret = 1;
#endif
    easyWifi.led(0);
    if(ret){
      lastupload = millis();
      doormax = 0;
    }
    Serial.print("Ambient send returned ");
    Serial.println(ret);
  }

  delay(1000);
}

/*
 * EOF
 */

