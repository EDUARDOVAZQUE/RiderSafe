#include <heltec.h>

#define BAND 868E6  // Frecuencia de LoRa
#define boton 2     // Pin del botón

bool releState = true;  // Estado inicial del relé (Activo)
bool btnState = HIGH;   // Estado del botón (INPUT_PULLUP)
bool lastBtnState = HIGH;
unsigned long lastDebounceTime = 0;
const int debounceDelay = 50;

String mensajeRecibido = "";
int rssi = 0; // Variable para almacenar la intensidad de la señal

void setup() {
    Serial.begin(115200);
    pinMode(boton, INPUT_PULLUP);
    Heltec.begin(true /*Display*/, true /*LoRa*/, true /*Serial*/);
    LoRa.begin(BAND, true);
    LoRa.receive(); // Iniciar en modo recepción
    Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
    Heltec.display->setFont(ArialMT_Plain_10);
    delay(500);
    actualizarDisplay();
}

void actualizarDisplay() {
    Heltec.display->clear();
    Heltec.display->drawString(0, 0, "Estado:");
    Heltec.display->drawString(0, 10, releState ? "Activo" : "Desactivado");
    Heltec.display->drawString(0, 20, "RSSI: " + String(rssi) + " dBm"); // Mostrar RSSI
    Heltec.display->display();
}

void apagarmoto() {
    String mensaje = "change";
    Serial.println("Enviando: " + mensaje);
    
    LoRa.beginPacket();
    LoRa.print(mensaje);
    LoRa.endPacket();
    LoRa.receive(); // Volver a modo recepción

    // Esperar respuesta
    unsigned long timeout = millis() + 3000;
    mensajeRecibido = "";
    while (millis() < timeout && mensajeRecibido.isEmpty()) {
        recibirestado();
    }

    if (!mensajeRecibido.isEmpty()) {
        releState = (mensajeRecibido == "1");
        actualizarDisplay();
    } else {
        Serial.println("Sin respuesta");
    }
}

void recibirestado() {
    int packetSize = LoRa.parsePacket();
    if (packetSize) {
        mensajeRecibido = "";
        while (LoRa.available()) {
            mensajeRecibido += (char)LoRa.read();
        }
        rssi = LoRa.packetRssi(); // Obtener la intensidad de la señal (RSSI)
        Serial.println("Recibido: " + mensajeRecibido);
        Serial.println("RSSI: " + String(rssi) + " dBm");
    }
}

void loop() {
    bool reading = digitalRead(boton);
    
    // Debounce
    if (reading != lastBtnState) {
        lastDebounceTime = millis();
    }

    if ((millis() - lastDebounceTime) > debounceDelay) {
        // Si el estado del botón cambió
        if (reading != btnState) {
            btnState = reading;

            // Si el botón está presionado (LOW)
            if (btnState == LOW) {
                apagarmoto();
            }
        }
    }

    lastBtnState = reading;
}