#include "MQTTClient.h"

#define SLEEP_TIME 5000

// To do
// debug lights - Connecting problems, fail to connect to wifi or fail to connecto to mqtt server.
// low power mode

// Change the credentials below, so your ESP8266 connects to your router
// const char *ssid = "SINGTEL-D67G"; // Song han wifi password lmfao kek
// const char *password = "r4b9b3m5md"; // 
// const char *mqttServer = "192.168.1.244";
// const int mqttPort = 1883;

const char *ssid = "SINGTEL-61B8"; // Song han wifi password lmfao kek
const char *password = "0001663416"; // 
const char *mqttServer = "192.168.1.87";
const int mqttPort = 1883;

const char *mqttUser = "user";
const char *mqttPassword = "user";
unsigned long timeStart = 0;
unsigned long timeNow = 0;

WiFiClient espClient;
PubSubClient pubSubclient(mqttServer, 1883, espClient);
MQTTClient mqttClient(espClient,pubSubclient);

void setup()
{
	Serial.begin(9600);
	Wire.begin();
	mqttClient.initWifi(ssid, password);
	mqttClient.initMQTTClient(mqttUser, mqttPassword, mqttServer, mqttPort);
	Serial.println("Init sensor");
	mqttClient.initSensor();
	// Update Time now 
	timeStart = millis();
}

void loop()
{
	// every 2 second collect data
	timeNow = millis();
	if (timeNow - timeStart > SLEEP_TIME) 
	{
		timeStart = millis();
		if (!mqttClient.readData())
		{
			Serial.println("Failed to read datas");
		}
	}

	if (!mqttClient.checkConnection())
	{
		mqttClient.reconnect();
	}
	mqttClient.pubSubClientLoop();
}
