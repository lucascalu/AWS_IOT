#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>  // Biblioteca para criação de JSON

// Configurações Wi-Fi
const char* ssid = "<SSID>";
const char* password = "<SENHA>";

// Configurações MQTT
const char* mqtt_server = "<IP>";
const char* mqtt_user = "teste"; // Se necessário
const char* mqtt_password = ""; // Se necessário
const char* mqtt_topic = "teste";

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  
  // Espera até conectar ao Wi-Fi
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Conectado ao Wi-Fi!");

  client.setServer(mqtt_server, 1883); // Porta padrão do MQTT
  
  while (!client.connected()) {
    if (client.connect("ESP8266", mqtt_user, mqtt_password)) {
      Serial.println("Conectado ao Broker MQTT");
    } else {
      delay(5000);
      Serial.print(".");
    }
  }
}

void loop() {
  if (!client.connected()) {
    // Reconnecta caso a conexão com o broker MQTT seja perdida
    while (!client.connected()) {
      if (client.connect("ESP8266", mqtt_user, mqtt_password)) {
        Serial.println("Reconectado ao Broker MQTT");
      } else {
        delay(5000);
        Serial.print(".");
      }
    }
  }

  // Lê o valor analógico do pino A0 (analogRead(0) pode ser usado diretamente)
  int valorAnalogico = analogRead(A0);  // Lê o valor de 0 a 1023

  // Cria um objeto JSON
  StaticJsonDocument<200> doc;
  doc["resultado"] = valorAnalogico;

  // Converte o objeto JSON para uma string
  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer);

  // Publica o JSON no tópico MQTT
  client.publish(mqtt_topic, jsonBuffer);
  Serial.println(jsonBuffer);  // Imprime o JSON no monitor serial

  // Aguarda 10 segundos antes de fazer a próxima leitura
  delay(30000);
}
