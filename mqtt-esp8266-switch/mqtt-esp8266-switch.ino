/****************************************************************************************************************************************************
 *  TITLE: MQTT Communication Between The Arduino Nano 33 IoT & WeMos D1 Mini Board
 *
 *  By Frenoy Osburn
 *  YouTube Video: https://youtu.be/lK4v-6xzSEA
 ****************************************************************************************************************************************************/

 /********************************************************************************************************************
 Boards Manager URL: http://arduino.esp8266.com/stable/package_esp8266com_index.json
 ********************************************************************************************************************/
 
 /********************************************************************************************************************
 *  Board Settings:
 *  Board: "WeMos D1 R1 or Mini"
 *  Upload Speed: "921600"
 *  CPU Frequency: "80MHz"
 *  Flash Size: "4MB (FS:@MB OTA:~1019KB)"
 *  Debug Port: "Disabled"
 *  Debug Level: "None"
 *  VTables: "Flash"
 *  IwIP Variant: "v2 Lower Memory"
 *  Exception: "Legacy (new can return nullptr)"
 *  Erase Flash: "Only Sketch"
 *  SSL Support: "All SSL ciphers (most compatible)"
 *  COM Port: Depends *On Your System*
 *********************************************************************************************************************/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "credentials.h"

#define SWITCH_PIN    D7

const char* ssid = networkSSID;
const char* password = networkPASSWORD;
const char* mqttServer = mqttSERVER;
const char* mqttUsername = mqttUSERNAME;
const char* mqttPassword = mqttPASSWORD;

char pubTopic[] = "arduino/ledControl";     //payload[0] will control/set LED

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;
int switchState = 0;

void setup_wifi() 
{
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() 
{
  // Loop until we're reconnected
  while (!client.connected()) 
  {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(), mqttUsername, mqttPassword)) 
    {
      Serial.println("connected");
    } else 
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() 
{
  pinMode(SWITCH_PIN, INPUT);     
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqttServer, 1883);
}

void loop() 
{
  if (!client.connected()) 
  {
    reconnect();
  }
  client.loop();

  if(digitalRead(SWITCH_PIN) == LOW)
  {
    Serial.println("Switch pressed");
    if(switchState == 0)
    {
      switchState = 1;  
    }
    else
    {
      switchState = 0;
    }

    char payLoad[1];
    itoa(switchState, payLoad, 10);
    client.publish(pubTopic, payLoad);

    delay(500);
  }

  long now = millis();
  if (now - lastMsg > 5000) 
  {
    lastMsg = now;
    char payLoad[1];
    itoa(switchState, payLoad, 10);
    client.publish(pubTopic, payLoad);
  }
}
