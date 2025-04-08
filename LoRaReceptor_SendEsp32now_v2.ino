#include <heltec.h>
#include <esp_now.h>
#include <WiFi.h>

#define BAND 868E6  // Frecuencia de LoRa
#define RELAY_PIN 2 // Pin del relé

bool relayState = false; // Estado inicial del relé
String mensajeRecibido = "";

// Dirección MAC del ESP32 receptor
uint8_t receiverMAC[] = {0xC8, 0x2E, 0x18, 0x6B, 0xFA, 0x10};  

esp_now_peer_info_t peerInfo;

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    Serial.print("Status de envío: ");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Entregado" : "Falló");
}

void setup() {
    Serial.begin(115200);
    Heltec.begin(false, true, true, true, BAND); // Iniciar LoRa sin pantalla

    pinMode(RELAY_PIN, OUTPUT);
    digitalWrite(RELAY_PIN, relayState);

    Serial.println("LoRa Receiver Ready");

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
        Serial.println("Error al agregar peer receptor");
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

        if (mensajeRecibido == "change") {
            relayState = !relayState;
            digitalWrite(RELAY_PIN, relayState);
            Serial.print("Nuevo estado del relé: ");
            Serial.println(relayState);

            String estadoRele = String(relayState);
            LoRa.beginPacket();
            LoRa.print(estadoRele);
            LoRa.endPacket();
            
            esp_now_send(receiverMAC, (uint8_t *)estadoRele.c_str(), estadoRele.length());
        }
    }
}
