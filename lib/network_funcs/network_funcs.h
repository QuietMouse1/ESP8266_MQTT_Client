#include <Arduino.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <Wire.h>

// Helper functions 
namespace NetworkFuncs
{
    void reconnect(PubSubClient &client);
}
