//llavero
//librerias
#include <heltec.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Arduino.h>

#define BAND 868E6 //banda de transmicion en Mhz
#define boton 2

bool pantallaEncendida = true;
unsigned long tiempoEncendidoPantalla = 0;
const unsigned long tiempoPantallaActiva = 30000; //30s

float pitch = 120; //pitch de inicio

bool releState = HIGH;
bool btnState = HIGH;

//antirrebote del boton
bool lastBtnState = HIGH;
unsigned long lastDebounceTime = 0;
const int debounceDelay = 50;

//mensaje
String mensajeRecibido = "";
int rssi = 0; //intensidad de la señal LoRa

//variables tiempo
unsigned long previousHello = 0; //polling o envio enfrecuente para saber si aun hay conexion y la intensidad de esta.
const unsigned long helloInterval = 10000;

// Configuración WiFi y MQTT para obtener el pitch
const char* ssid = "LobosIT";
const char* password = "V4M4LVU5H11113k";
const char* mqtt_server = "192.168.137.60"; //raspberry pi

bool dataReceived = false; 

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  pinMode(boton, INPUT_PULLUP);
  Heltec.begin(true, true, true);
  LoRa.begin(BAND, true);
  LoRa.receive();

  Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
  Heltec.display->setFont(ArialMT_Plain_10);
  delay(500);
  actualizarDisplay();

  setup_wifi();
  client.setServer(mqtt_server, 1883); //Algunas maquinas windows ya tienen ocupado el puerto 1883, asi que puede cambiar a 1884 o superior
  client.setCallback(callback);
}

void actualizarDisplay() {
  if (!pantallaEncendida) return;

  Heltec.display->clear();
  Heltec.display->setTextAlignment(TEXT_ALIGN_CENTER);
  Heltec.display->drawString(50, 0, "HOLA SOY");
  Heltec.display->drawString(50, 12, "La Locura :)");

 int centerX = 32, centerY = 60, radius = 18;
float angleRad = radians(pitch);

// Dibujar medio círculo (ejemplo: de 0° a 180°)
for (int angle = 0; angle <= 180; angle += 5) {
  float a = radians(angle);
  int x = centerX + radius * cos(a - PI);  // Corrige orientación
  int y = centerY + radius * sin(a - PI);
  Heltec.display->setPixel(x, y);  // Dibuja punto del semicírculo
}

// Calcular coordenadas del extremo de la aguja
int xEnd = centerX + radius * cos(angleRad - PI);
int yEnd = centerY + radius * sin(angleRad - PI);

// Dibuja línea del ángulo (más gruesa: 3 líneas paralelas)
for (int offset = -1; offset <= 1; offset++) {
  Heltec.display->drawLine(centerX + offset, centerY, xEnd + offset, yEnd);
}


  Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
  Heltec.display->drawString(87, 10, "LoRa");
  //crea las barras para mostrar la intensidad del RSSI
  int bars = map(rssi, -120, -40, 0, 4);
  for (int i = 0; i < 4; i++) {
    int barHeight = (i + 1) * 3;
    int x = 114 + i * 4;
    int y = 35 - barHeight;
    if (i < bars) Heltec.display->fillRect(x, y, 3, barHeight);
    else Heltec.display->drawRect(x, y, 3, barHeight);
  }

  Heltec.display->drawString(84, 20, String(rssi) + " dBm");

//candado
  int cx = 100, cy = 50;
  Heltec.display->drawRect(cx, cy, 12, 10);

  if (releState == LOW) {
    Heltec.display->drawLine(cx + 3, cy, cx + 3, cy - 6);
    Heltec.display->drawLine(cx + 9, cy, cx + 9, cy - 6);
    Heltec.display->drawLine(cx + 3, cy - 6, cx + 9, cy - 6);
  } else {
    Heltec.display->drawLine(cx + 9, cy, cx + 9, cy - 6);
    Heltec.display->drawLine(cx + 3, cy - 6, cx + 9, cy - 6);
  }

  for (int i = 3; i <= 7; i++)
    Heltec.display->setPixel(cx + 6, cy + i);

  Heltec.display->display();
}

void enviarHello() {
 // Serial.println("Enviando: hola"); //emvia lora
  LoRa.beginPacket();
  LoRa.print("hola");
  LoRa.endPacket();
  LoRa.receive();

  unsigned long timeout = millis() + 3000;
  mensajeRecibido = "";
  while (millis() < timeout && mensajeRecibido == "") {
    recibirMensaje();
  }

  if (mensajeRecibido != "") {
    if (mensajeRecibido == "ok") {
      rssi = LoRa.packetRssi();
      //Serial.println("Respuesta 'ok' recibida. RSSI: " + String(rssi) + " dBm");
    } else {
      //Serial.println("Respuesta diferente a 'ok': " + mensajeRecibido);
    }
  } else {
    //Serial.println("Sin respuesta a 'hola'");
  }
  actualizarDisplay();
}

void enviarChange() { //para cambio del relevador
  String mensaje = "change"; //mensaje que se envia a la lora moto
  Serial.println("Enviando: " + mensaje);
  LoRa.beginPacket();
  LoRa.print("mensaje");
  LoRa.endPacket();
  LoRa.receive();

  unsigned long timeout = millis() + 3000;
  mensajeRecibido = "";
  while (millis() < timeout && mensajeRecibido == "") {
    recibirMensaje();
  }

  if (mensajeRecibido != "") {
    if (mensajeRecibido == "1" || mensajeRecibido == "0") {
      releState = (mensajeRecibido == "1");
      Serial.println("Estado actualizado a: " + mensajeRecibido);
    } else {
      Serial.println("Respuesta inesperada: " + mensajeRecibido);
    }
  } else {
    Serial.println("Sin respuesta a 'change'");
  }
  actualizarDisplay();
}

void recibirMensaje() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    mensajeRecibido = "";
    while (LoRa.available()) {
      mensajeRecibido += (char)LoRa.read();
    }
    rssi = LoRa.packetRssi();
    Serial.println("Recibido: " + mensajeRecibido + ", RSSI: " + String(rssi) + " dBm");

    // Nuevo: analizar pitch recibido
    if (mensajeRecibido.indexOf("\"pitch\"") != -1) {
  int start = mensajeRecibido.indexOf(":") + 1;
  int end = mensajeRecibido.indexOf("}");
  if (start > 0 && end > start) {
    String pitchStr = mensajeRecibido.substring(start, end);
    pitch = pitchStr.toFloat();
    Serial.println("Pitch actualizado (JSON): " + String(pitch));
    actualizarDisplay();

  String json = "\"pitch\""
  }
}

  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  bool reading = digitalRead(boton);
  if (reading != lastBtnState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != btnState) {
      btnState = reading;
      if (btnState == LOW) {
        enviarChange();
        encenderPantalla();
      }
    }
  }
  lastBtnState = reading;

  unsigned long currentMillis = millis();
  if (currentMillis - previousHello >= helloInterval) {
    previousHello = currentMillis;
    enviarHello();
  }

  if (pantallaEncendida && millis() - tiempoEncendidoPantalla > tiempoPantallaActiva) {
    pantallaEncendida = false;
    Heltec.display->displayOff();
  }

  delay(10);
}

void encenderPantalla() {
  if (!pantallaEncendida) {
    pantallaEncendida = true;
    Heltec.display->displayOn();
  }
  tiempoEncendidoPantalla = millis();
  actualizarDisplay();
}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* message, unsigned int length) { //funcion para leer topico RS-pitch
  Serial.print("Mensaje MQTT en topic: ");
  Serial.println(topic);

  String messageTemp;
  for (int i = 0; i < length; i++) {
    messageTemp += (char)message[i];
  }

  Serial.print("Contenido del mensaje: ");
  Serial.println(messageTemp);

  if (String(topic) == "RS-pitch") {
    // Parsear JSON del mensaje MQTT
    StaticJsonDocument<128> doc;
    DeserializationError error = deserializeJson(doc, messageTemp);

    if (!error && doc.containsKey("pitch")) {
      pitch = doc["pitch"];
      Serial.print("Pitch MQTT actualizado a: ");
      Serial.println(pitch);
      encenderPantalla(); // para asegurar que se ve lapantalla
      actualizarDisplay();
    } else {
      Serial.println("Error al parsear JSON o 'pitch' no encontrado.");
    }
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("Llavero")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("RS-pitch");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }

}