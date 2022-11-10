# Laboratorio 3 - MQTT

<aside>
💡 Para este laboratorio se utilizaron los componentes del laboratorio 3, ya que el sensor utilizado (DHT11) de temperatura y humedad también serán utilizados en el proyecto final.

</aside>

# 1. Funciones de generación de paquetes

<aside>
📄 Identificar las funciones de generación de paquetes siguiendo el protocolo MQTT implementadas en mosquito. Comentar las líneas de código.

</aside>

### Funciones identificadas:

- `setServer`
- `setCallback`
- `connect`
- `publish`
- `subscribe`

```c
[...]

void setup() {

	[...]
	
	/*
	La función setServer se encarga de establecer el servidor y el puerto 
	del broker de MQTT por el cual se van a mandar y leer los mensajes.
	*/
  client.setServer(mqtt_broker, mqtt_port);
	
	/*
	La función setCallback se encarga de establecer a otra función que se ejecutará
	una vez se reciban mensajes en el broker.
	*/
  client.setCallback(callback);
	
	/*
	La función connected verifica si el cliente se encuentra conectado al broker.
	*/
  while (!client.connected()) {
    String client_id = "esp8266-client-";
    client_id += String(WiFi.macAddress());
    Serial.printf("The client %s connects to mosquitto mqtt broker\n", client_id.c_str());

		/*
		La función connect se encarga de conectar el cliente al broker.
		*/
    if (client.connect(client_id.c_str())) {
      Serial.println("Public emqx mqtt broker connected");
    } else {
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }
	
	/*
	La función subscribe se encarga de suscribirse a un tópico del
	broker. En este caso, sería el tópico "test/topic".
	*/
  client.subscribe(topic);
}

void callback(char * topic, byte * payload, unsigned int length) {
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char) payload[i]);
  }
  Serial.println();
  Serial.println(" - - - - - - - - - - - -");

  //General
  delay(1000); //Wait before accessing Sensor
}

void loop() {
  //Humedity and Temperature message
  String msg = "Humidity = " + String(dht2.readHumidity()) + " & " + "Temperature = " + String(dht2.readTemperature());
  Serial.println(msg);

	/*
	La función publish se encarga de mandar mensajes por un tópico del broker. En
	este caso, manda los datos leídos por el sensor a través del tópico "test/topic",
	para que puedan ser leídos por el callback.
	*/
  client.publish(topic, msg.c_str());
  Serial.println(msg);
  delay(5000);
}
```

---

<aside>
📄 Hacer un diagrama de bloques a nivel de llamadas de función.

</aside>

# 2. Usar el sensor de humedad e implementar un servicio MQTT.

<aside>
📄 Usando el sensor de humedad del kit de Arduino, implementar un servicio MQTT.

</aside>

## Paso 1: Lectura del sensor

<aside>
💡 Uno de los sensores a utilizar será el de humedad y temperatura DHT11.

</aside>

![Figura 1: Se puede observar el Serial.println() de la temperatura y humedad. Esta misma información será enviada al broker mqtt en los pasos a continuación.](Laboratorio%203%20-%20MQTT%205713387dc6e1402ba6e425bd9e8a80eb/Captura_de_Pantalla_2022-11-03_a_la(s)_19.22.39.png)

Figura 1: Se puede observar el Serial.println() de la temperatura y humedad. Esta misma información será enviada al broker mqtt en los pasos a continuación.

![Figura 2: Se puede observar un sensor de temperatura y humedad DHT11 (1) conectado a un Arduino ESP8266 (2). ](Laboratorio%203%20-%20MQTT%205713387dc6e1402ba6e425bd9e8a80eb/Captura_de_Pantalla_2022-11-09_a_la(s)_19.43.18.png)

Figura 2: Se puede observar un sensor de temperatura y humedad DHT11 (1) conectado a un Arduino ESP8266 (2). 

![Figura 3: Se observa un esquema que representa al Arduino ESP8266, que se utilizó para la lectura del sensor. Los pines utilizados son los señalados: GND, VCC y DATA.](Laboratorio%203%20-%20MQTT%205713387dc6e1402ba6e425bd9e8a80eb/NodeMCU_V2_v2.png)

Figura 3: Se observa un esquema que representa al Arduino ESP8266, que se utilizó para la lectura del sensor. Los pines utilizados son los señalados: GND, VCC y DATA.

## Paso 2: MQTT

<aside>
💡 Adicionalmente, los datos será enviados mediante el broker MQTT.

</aside>

### MQTT Configuration

[MQTT_Guide.pdf](Laboratorio%203%20-%20MQTT%205713387dc6e1402ba6e425bd9e8a80eb/MQTT_Guide.pdf)

```jsx
vim /opt/homebrew/Cellar/mosquitto/2.0.15/etc/mosquitto/mosquitto.conf
```

### MQTT Run Broker

```jsx
/opt/homebrew/Cellar/mosquitto/2.0.15/sbin/mosquitto -c /opt/homebrew/Cellar/mosquitto/2.0.15/etc/mosquitto/mosquitto.conf -v
```

![Figura 4: Captura de pantalla de la consola en la que se visualizan los logs del broker de MQTT en ejecución.](Laboratorio%203%20-%20MQTT%205713387dc6e1402ba6e425bd9e8a80eb/Captura_de_Pantalla_2022-11-03_a_la(s)_19.24.25.png)

Figura 4: Captura de pantalla de la consola en la que se visualizan los logs del broker de MQTT en ejecución.

### MQTT Run Subscriber

```jsx
mosquitto_sub -h localhost -p 1883 -t test/topic
```

![Figura 5: Captura de pantalla con la recepción de la humedad y temperatura publicados por el sensor DHT11.](Laboratorio%203%20-%20MQTT%205713387dc6e1402ba6e425bd9e8a80eb/Captura_de_Pantalla_2022-11-03_a_la(s)_19.25.47.png)

Figura 5: Captura de pantalla con la recepción de la humedad y temperatura publicados por el sensor DHT11.

---

## Diagrama de conexión hacia el broker

<aside>
📄 Hacer un diagrama explicando la conexión hacia el broker.

</aside>

Como se puede observar en la figura número 6, el sensor de humedad DHT11 se conectó al microcontrolador Arduino ESP8266, que en este caso corresponde al sensor y suscriptor del sistema. Este fue conectado mediante la red WiFi haciendo uso de la librería PubSubClient. El broker fue ejecutado localmente en un ordenador (*Code Listing 3*). Para realizar la conexión entre el suscriptor y el broker se configuró en el broker el SSID y contraseña de la red wifi utilizada y la dirección IP del ordenador que ejecutaba el broker. Finalmente, se ejecutó un suscriptor en otro ordenador (*Code Listing 4*) para recibir los valores leídos por el sensor de humedad y temperatura. 

![Figura 6: Diagrama de arquitectura representando la conexión entre el suscriptor, el broker y el suscriptor.](Laboratorio%203%20-%20MQTT%205713387dc6e1402ba6e425bd9e8a80eb/Diagrama_sin_titulo-6.drawio.png)

Figura 6: Diagrama de arquitectura representando la conexión entre el suscriptor, el broker y el suscriptor.

## Código comentado explicando el funcionamiento

<aside>
📄 Comentar el código de Arduino indicando el funcionamiento, con una captura de la señal adquirida y comunicada.

</aside>

```c
#include <ESP8266WiFi.h>
#include "DHT.h"
#include <PubSubClient.h>

//Temperature config
float t = 0.0;
float h = 0.0;
DHT dht2(0, DHT11);

//Mosquitto config
const char * mqtt_broker = "192.168.18.131";
const char * topic = "test/topic";
const int mqtt_port = 1883;

//Network config
WiFiClient espClient;
PubSubClient client(espClient);
const char * ssid = "PEZET-2.4G";
const char * pass = "reb21&ma30";

void setup() {
  Serial.begin(9600);

  // Humedity sensor
  dht2.begin();
  delay(500); //Delay to let system boot
  Serial.println("DHT11 Humidity & temperature Sensor\n\n");

  // Wifi
  Serial.println("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println(WiFi.localIP());

	/*
	Paso 1: Seteamos el broker del cliente asignando la IP y el puerto deseados.
	Paso 2: Seteamos el callback que vamos a ejecutar cada vez que llegue un 
	mensaje al broker.
	*/
  //Connecting to a mqtt broker
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);

	/*
	Paso 3: Nos conectamos al broker.
	*/
  while (!client.connected()) {
    String client_id = "esp8266-client-";
    client_id += String(WiFi.macAddress());
    Serial.printf("The client %s connects to mosquitto mqtt broker\n", client_id.c_str());
    if (client.connect(client_id.c_str())) {
      Serial.println("Public emqx mqtt broker connected");
    } else {
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }
	/*
	Paso 4: Nos suscribimos al tópico por el que vamos a intercambiar los datos
	del sensor. En este caso, "test/topic".
	*/
  client.subscribe(topic);
}

/*
Paso 5: Escribimos el callback que deseamos ejecutar cada vez que llega
un mensaje al broker.
*/
void callback(char * topic, byte * payload, unsigned int length) {
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char) payload[i]);
  }
  Serial.println();
  Serial.println(" - - - - - - - - - - - -");

  //General
  delay(1000); //Wait before accessing Sensor
}

/*
Paso 6: Publicamos los mensajes que emite el sensor y los mandamos por el broker
usando la función publish.
*/
void loop() {
  //Humedity and Temperature message
  String msg = "Humidity = " + String(dht2.readHumidity()) + " & " + "Temperature = " + String(dht2.readTemperature());
  Serial.println(msg);
  client.publish(topic, msg.c_str());
  Serial.println(msg);
  delay(5000);
}
```

## Diagrama de flujo de funciones

En la imagen abajo se ve nuestro diagrama de funciones.

→ El bloque grande de la izquierda es la función `setup`, la cual se encarga de establecer la conexión del **broker** y también el tópico al cual se **suscribe** el cliente.

→ El bloque de la derecha representa a la función `loop`, la cual se encarga de ir leyendo los datos del sensor y **publicarlos** en el tópico del broker, usando el método `publish`.

![Figura 7: Digrama con llamadas de función en el código del publisher.](Laboratorio%203%20-%20MQTT%205713387dc6e1402ba6e425bd9e8a80eb/functions.drawio_(1).png)

Figura 7: Digrama con llamadas de función en el código del publisher.

# Anexo

### Código utilizado

```cpp
#include <ESP8266WiFi.h>
#include "DHT.h"
#include <PubSubClient.h>

//Temperature config
float t = 0.0;
float h = 0.0;
DHT dht2(0, DHT11);

//Mosquitto config
const char * mqtt_broker = "192.168.18.131";
const char * topic = "test/topic";
const int mqtt_port = 1883;

//Network config
WiFiClient espClient;
PubSubClient client(espClient);
const char * ssid = "PEZET-2.4G";
const char * pass = "reb21&ma30";

void setup() {
  Serial.begin(9600);

  // Humedity sensor
  dht2.begin();
  delay(500); //Delay to let system boot
  Serial.println("DHT11 Humidity & temperature Sensor\n\n");

  // Wifi
  Serial.println("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println(WiFi.localIP());

  //Connecting to a mqtt broker
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);
  while (!client.connected()) {
    String client_id = "esp8266-client-";
    client_id += String(WiFi.macAddress());
    Serial.printf("The client %s connects to mosquitto mqtt broker\n", client_id.c_str());
    if (client.connect(client_id.c_str())) {
      Serial.println("Public emqx mqtt broker connected");
    } else {
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }
  // publish and subscribe
  // client.publish(topic, "Hello From ESP8266");
  client.subscribe(topic);
}

void callback(char * topic, byte * payload, unsigned int length) {
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char) payload[i]);
  }
  Serial.println();
  Serial.println(" - - - - - - - - - - - -");

  //General
  delay(1000); //Wait before accessing Sensor
}

void loop() {
  //Humedity and Temperature message
  String msg = "Humidity = " + String(dht2.readHumidity()) + " & " + "Temperature = " + String(dht2.readTemperature());
  Serial.println(msg);
  client.publish(topic, msg.c_str());
  Serial.println(msg);
  delay(5000);
}
```

### Configuración de conexión de Arduino ESP8266

![Figura 8: Selección de placa Generic ESP8266 Module.](Laboratorio%203%20-%20MQTT%205713387dc6e1402ba6e425bd9e8a80eb/Captura_de_Pantalla_2022-11-09_a_la(s)_19.16.21.png)

Figura 8: Selección de placa Generic ESP8266 Module.

### Instalación de librerías

![Figura 9: Instalación de la librería PubSubClient by Nick O’Leary.](Laboratorio%203%20-%20MQTT%205713387dc6e1402ba6e425bd9e8a80eb/Captura_de_Pantalla_2022-11-09_a_la(s)_19.17.58.png)

Figura 9: Instalación de la librería PubSubClient by Nick O’Leary.

![Figura 10: Instalación de la librería DHT sensor library by Adafruit.](Laboratorio%203%20-%20MQTT%205713387dc6e1402ba6e425bd9e8a80eb/Captura_de_Pantalla_2022-11-09_a_la(s)_19.23.05.png)

Figura 10: Instalación de la librería DHT sensor library by Adafruit.