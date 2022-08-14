
#include "config.h"
#include <Arduino.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <Wire.h>
#include "SparkFun_SCD30_Arduino_Library.h" 
#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"
#include <SensirionI2CScd4x.h> // << SCD41 sensor libary
#include "Zanshin_BME680.h"   ///< The BME680 sensor library

// Sensor Helper Functions
class MQTTClient{

    private: 
        WiFiClient espClient;
        PubSubClient pubSubClient; 
        #ifdef SENSOR_SCD30
        SCD30 airSensor;
        void iniitSCD30();
        bool readSCD30data();
        #endif
        #ifdef SENSOR_BME680
        BME680_Class BME680;
        void initBME680();
        bool readBME680data();
        #endif
        #ifdef SENSOR_SCD41
        SensirionI2CScd4x scd4x; // I2C
        void printUint16Hex(uint16_t value);
        void printSerialNumber(uint16_t serial0, uint16_t serial1, uint16_t serial2);
        void initSCD41();
        bool readSCD41data();
        #endif

    public:
        void initWifi(const char* ssid, const char* password);
        void initMQTTClient(const char* mqttUser, const char* mqttPassword, const char* mqttServer, const int mqttPort);
        void reconnect();
        void pubSubClientLoop();
        void initSensor();
        bool publish(const char * topic, const char * data );
        bool checkConnection();
        bool readData();

        MQTTClient();
        MQTTClient(WiFiClient& _espClient, PubSubClient& _client);
};