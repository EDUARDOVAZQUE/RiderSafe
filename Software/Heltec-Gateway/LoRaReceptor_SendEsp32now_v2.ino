//heltec moto
#include <heltec.h>
#include <esp_now.h>
#include <WiFi.h>

#define BAND 868E6  // Frecuencia de LoRa
#define RELAY_PIN 2 // Pin del relé

bool relayState = false; // Estado inicial del relé
String mensajeRecibido = "";

// Dirección MAC (del nodo maestro) para comunicación ESP-NOW
uint8_t receiverMAC[] = {0xD8, 0x13, 0x2A, 0x2E, 0xDD, 0xD0}; //mac esp32

esp_now_peer_info_t peerInfo;

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Status de envío: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Entregado" : "Falló");
}

void setup() {
  Serial.begin(115200);
  // Inicia LoRa (sin pantalla)
  Heltec.begin(false, true, true, true, BAND);
  
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, relayState);

  Serial.println("Placa con relé iniciada");
  LoRa.receive();

  // Inicialización de ESP-NOW
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error al iniciar ESP-NOW");
    return;
  }
  esp_now_register_send_cb(OnDataSent);
  memcpy(peerInfo.peer_addr, receiverMAC, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Error al agregar peer maestro");
    return;
  }
}

void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    mensajeRecibido = "";
    while (LoRa.available()) {
      char c = (char)LoRa.read();
      mensajeRecibido += c;
    }
    Serial.print("Paquete recibido: ");
    Serial.println(mensajeRecibido);
    
    // Si se recibe "hola", se responde con "ok"
    if (mensajeRecibido == "hola") {
      Serial.println("Recibido 'hola'. Respondiendo con 'ok'");
      LoRa.beginPacket();
      LoRa.print("ok");
      LoRa.endPacket();
      
      // Medir RSSI del paquete recibido y, si está en el rango, apagar el relé
      int currentRSSI = LoRa.packetRssi();
      Serial.println("RSSI del paquete: " + String(currentRSSI) + " dBm");
      if (currentRSSI >= -200 && currentRSSI <= -110) {
        relayState = false;
        digitalWrite(RELAY_PIN, relayState);
        Serial.println("RSSI en rango. Relé apagado");

        // Comunicar el cambio vía LoRa (envío del nuevo estado)
        String estadoRele = String(relayState);
        LoRa.beginPacket();
        LoRa.print(estadoRele);
        LoRa.endPacket();

        // Comunicar el cambio vía ESP-NOW (envío de JSON)
        String jsonMessage = "{\"releState\":" + String(estadoRele) +
                             ",\"intL\":" + String(currentRSSI) + "}";
        esp_now_send(receiverMAC, (uint8_t *)jsonMessage.c_str(), jsonMessage.length());
      }
    }
    // Si se recibe "change", se procede a cambiar el estado (toggle)
    else if (mensajeRecibido == "change") {
      relayState = !relayState;
      digitalWrite(RELAY_PIN, relayState);
      Serial.print("Nuevo estado del relé: ");
      Serial.println(relayState);
      
      String estadoRele = String(relayState);
      LoRa.beginPacket();
      LoRa.print(estadoRele); //envio de estado rele a la banda lora enviar
      LoRa.endPacket();
      int currentRSSI = LoRa.packetRssi();
      String jsonMessage = "{\"releState\":" + String(estadoRele) +
                           ",\"intL\":" + String(currentRSSI) + "}";
      esp_now_send(receiverMAC, (uint8_t *)jsonMessage.c_str(), jsonMessage.length());
    }
  }
}