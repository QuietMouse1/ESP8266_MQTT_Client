# include "network_funcs.h"
void NetworkFuncs::reconnect(PubSubClient &client)
{
    while (!client.connected())
    {
        Serial.print("Attempting MQTT connection...");
        if (client.connect("ESP8266Client"))
        {
            Serial.println("connected");
            // Subscribe or resubscribe to a topic
            // You can subscribe to more topics (to control more LEDs in this example)
            client.subscribe("test/test1");
        }
        else
        {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
}
