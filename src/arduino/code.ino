// Los comentarios del código se encuentran en el informe.
#include <ESP8266WiFi.h>
#include "DHT.h"
#include <PubSubClient.h>

// Temperature config
float t = 0.0;
float h = 0.0;
DHT dht2(0, DHT11);

// Mosquitto config
const char *mqtt_broker = "192.168.18.131";
const char *topic = "test/topic";
const int mqtt_port = 1883;

// Network config
WiFiClient espClient;
PubSubClient client(espClient);
const char *ssid = "PEZET-2.4G";
const char *pass = "reb21&ma30";

void setup()
{
      Serial.begin(9600);

      // Humedity sensor
      dht2.begin();
      delay(500); // Delay to let system boot
      Serial.println("DHT11 Humidity & temperature Sensor\n\n");

      // Wifi
      Serial.println("Connecting to ");
      Serial.println(ssid);
      WiFi.begin(ssid, pass);
      while (WiFi.status() != WL_CONNECTED)
      {
            delay(500);
            Serial.print(".");
      }
      Serial.println("");
      Serial.println("WiFi connected");
      Serial.println(WiFi.localIP());

      // Connecting to a mqtt broker
      client.setServer(mqtt_broker, mqtt_port);
      client.setCallback(callback);
      while (!client.connected())
      {
            String client_id = "esp8266-client-";
            client_id += String(WiFi.macAddress());
            Serial.printf("The client %s connects to mosquitto mqtt broker\n", client_id.c_str());
            if (client.connect(client_id.c_str()))
            {
                  Serial.println("Public emqx mqtt broker connected");
            }
            else
            {
                  Serial.print("failed with state ");
                  Serial.print(client.state());
                  delay(2000);
            }
      }
      // publish and subscribe
      // client.publish(topic, "Hello From ESP8266");
      client.subscribe(topic);
}

void callback(char *topic, byte *payload, unsigned int length)
{
      Serial.print("Message arrived in topic: ");
      Serial.println(topic);
      Serial.print("Message:");
      for (int i = 0; i < length; i++)
      {
            Serial.print((char)payload[i]);
      }
      Serial.println();
      Serial.println(" - - - - - - - - - - - -");

      // General
      delay(1000); // Wait before accessing Sensor
}

void loop()
{
      // Humedity and Temperature message
      String msg = "Humidity = " + String(dht2.readHumidity()) + " & " + "Temperature = " + String(dht2.readTemperature());
      client.publish(topic, msg.c_str());
      Serial.println(msg);
      delay(5000);
}
