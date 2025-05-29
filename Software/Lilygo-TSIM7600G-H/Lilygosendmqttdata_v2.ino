//lilygo
#define TINY_GSM_MODEM_SIM7600
#define SerialMon Serial
#define SerialAT Serial1
#define TINY_GSM_DEBUG SerialMon
#define TINY_GSM_USE_GPRS true
#define TINY_GSM_USE_WIFI false
#define GSM_PIN ""
#define uS_TO_S_FACTOR 1000000ULL
#define TIME_TO_SLEEP 60
#define UART_BAUD 115200
#define MODEM_DTR 25
#define MODEM_TX 27
#define MODEM_RX 26
#define MODEM_PWRKEY 4
#define MODEM_RI 33
#define MODEM_FLIGHT 25
#define MODEM_STATUS 34
#define SD_MISO 2
#define SD_MOSI 15
#define SD_SCLK 14
#define SD_CS 13
#define LED_PIN 12



#include <esp_now.h>
#include <WiFi.h>
#include <TinyGsmClient.h>
#include <PubSubClient.h>
#include <Ticker.h>
#include <SPI.h>
#include <SD.h>
#include <Arduino.h>
#include <ArduinoJson.h>  // Incluye esta librería si puedes usarla
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
//Sensor MPU6050
Adafruit_MPU6050 mpu;
sensors_event_t a, g, temp;
float accX, accY, accZ;
float gyroX, gyroY, gyroZ;
float temperature;
float alpha = 0.5;  // Factor de suavizado
float filteredAccX, filteredAccY, filteredAccZ;

String releState = "false";
String intL = "0";

bool mqttWasConnected = true;  // Asume que inicialmente estaba conectado

unsigned long lastMsg = 0;
const long interval = 500;  // Intervalo de publicación en ms

// Your GPRS credentials, if any
const char apn[] = "web.usacellgsm.mx";
const char gprsUser[] = "";
const char gprsPass[] = "";

uint32_t lastReconnectAttempt = 0;
int inicializador = 0;
// MQTT details
const char *broker = "test.mosquitto.org";
const char *topicSensor = "RS/state";
const char *topicInit = "status/init";

//gps variables
float gpsLat = 0.0;
float gpsLon = 0.0;


#ifdef DUMP_AT_COMMANDS
#include <StreamDebugger.h>
StreamDebugger debugger(SerialAT, SerialMon);
TinyGsm modem(debugger);
#else
TinyGsm modem(SerialAT);
#endif
TinyGsmClient client(modem);
PubSubClient mqtt(client);

Ticker tick;

// Structure example to receive data
typedef struct struct_message {
  int releState;
  int intL;
} struct_message;

struct_message myData;
bool dataReceived = false;  // Flag para indicar que se ha recibido data

void OnDataRecv(const esp_now_recv_info *recv_info, const uint8_t *incomingData, int len) {
  // Convertir a String
  String jsonStr = "";
  for (int i = 0; i < len; i++) {
    jsonStr += (char)incomingData[i];
  }

  Serial.print("Mensaje recibido: ");
  Serial.println(jsonStr);

  // Parsear el JSON recibido
  StaticJsonDocument<200> doc;
  DeserializationError error = deserializeJson(doc, jsonStr);

  if (error) {
    Serial.print("Error al parsear JSON: ");
    Serial.println(error.c_str());
    return;
  }

  // Extraer los valores
  myData.releState = doc["releState"];
  myData.intL = doc["intL"];
  dataReceived = true;

  Serial.print("releState: ");
  Serial.println(myData.releState);
  Serial.print("intL: ");
  Serial.println(myData.intL);
}



boolean mqttConnect() {
  SerialMon.print("Connecting to ");
  SerialMon.print(broker);

  boolean status = mqtt.connect("RiderSafe");

  if (status == false) {
    SerialMon.println(" fail");
    return false;
  }
  SerialMon.println(" success");
  return mqtt.connected();
}

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  delay(10);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  initMPU();
  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register callback for receiving data
  esp_now_register_recv_cb(OnDataRecv);

  // Initialize modem
  SerialAT.begin(UART_BAUD, SERIAL_8N1, MODEM_RX, MODEM_TX);
  pinMode(MODEM_PWRKEY, OUTPUT);
  digitalWrite(MODEM_PWRKEY, HIGH);
  delay(300);
  digitalWrite(MODEM_PWRKEY, LOW);
  pinMode(MODEM_FLIGHT, OUTPUT);
  digitalWrite(MODEM_FLIGHT, HIGH);

  SPI.begin(SD_SCLK, SD_MISO, SD_MOSI, SD_CS);
  if (!SD.begin(SD_CS)) {
    Serial.println("SDCard MOUNT FAIL");
  } else {
    uint32_t cardSize = SD.cardSize() / (1024 * 1024);
    String str = "SDCard Size: " + String(cardSize) + "MB";
    Serial.println(str);
  }

  Serial.println("\nWait...");
  delay(1000);

  DBG("Initializing modem...");
  if (!modem.init()) {
    DBG("Failed to restart modem, delaying 10s and retrying");
  }

  String ret = String(modem.setNetworkMode(2));
  DBG("setNetworkMode:", ret);

  String name = modem.getModemName();
  DBG("Modem Name:", name);

  String modemInfo = modem.getModemInfo();
  DBG("Modem Info:", modemInfo);

#if TINY_GSM_USE_GPRS
  if (GSM_PIN && modem.getSimStatus() != 3) {
    modem.simUnlock(GSM_PIN);
  }
#endif

  SerialMon.print("Waiting for network...");
  if (!modem.waitForNetwork()) {
    SerialMon.println(" fail");
    delay(10000);
    return;
  }
  SerialMon.println(" success");

  if (modem.isNetworkConnected()) {
    SerialMon.println("Network connected");
  }

#if TINY_GSM_USE_GPRS
  SerialMon.print(F("Connecting to "));
  SerialMon.print(apn);
  if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
    SerialMon.println(" fail");
    delay(10000);
    return;
  }
  SerialMon.println(" success");

  if (modem.isGprsConnected()) {
    SerialMon.println("GPRS connected");
  }
#endif

  // MQTT Broker setup
  mqtt.setServer(broker, 1883);
  mqttConnect();  // Connect to MQTT and publish "Iniciado"
}

void loop() {
  // Make sure we're still registered on the network
  if (!modem.isNetworkConnected()) {
    SerialMon.println("Network disconnected");
    if (!modem.waitForNetwork(180000L, true)) {
      SerialMon.println(" fail");
      delay(10000);
      return;
    }
    if (modem.isNetworkConnected()) {
      SerialMon.println("Network re-connected");
    }

#if TINY_GSM_USE_GPRS
    if (!modem.isGprsConnected()) {
      SerialMon.println("GPRS disconnected!");
      SerialMon.print(F("Connecting to "));
      SerialMon.print(apn);
      if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
        SerialMon.println(" fail");
        delay(10000);
        return;
      }
      if (modem.isGprsConnected()) {
        SerialMon.println("GPRS reconnected");
      }
    }
#endif
  }

  if (!mqtt.connected()) {
    if (mqttWasConnected) {
      SerialMon.println("=== MQTT Desconectado, se reconectará en 10 segundos ===");
      mqttWasConnected = false;  // Ya imprimimos el mensaje
    }

    uint32_t t = millis();
    if (t - lastReconnectAttempt > 10000L) {
      lastReconnectAttempt = t;
      if (mqttConnect()) {
        lastReconnectAttempt = 0;
        mqttWasConnected = true;  // Se reconectó, permitir imprimir si se cae otra vez
      }
    }
    delay(100);
    return;
  }

  // Publish data to MQTT
  if (dataReceived) {
    releState = String(myData.releState);
    intL = String(myData.intL);
    dataReceived = false;  // Reset flag
  } else if (inicializador < 5) {
    mqtt.publish(topicSensor, "sin datos");  // Publish "sin datos" if no data received
    if (inicializador < 6) {
      inicializador = inicializador + 1;
    }
  }
  unsigned long now = millis();
  if (now - lastMsg > interval) {
    lastMsg = now;


    mpu.getEvent(&a, &g, &temp);

    // Aplicar filtro a las lecturas
    accX = aplicarFiltro(a.acceleration.x, filteredAccX);
    accY = aplicarFiltro(a.acceleration.y, filteredAccY);
    accZ = aplicarFiltro(a.acceleration.z, filteredAccZ);

    // Calcular ángulos
    //float roll = calcularInclinacion(accY, accZ);
    float pitch = calcularInclinacion(accX, accZ);
    // Obtener coordenadas antes de publicar
    getGPS(gpsLat, gpsLon);

      String datos = "{\"pitch\":\"" + String(pitch - 85) + 
                      "\", \"intL\":\"" + String(intL) +
                      "\", \"accy\":\"" + String(accY) +
                      "\", \"battH\":\"80" +
                      "\", \"intC\":\"" + String(modem.getSignalQuality()) +
                      "\", \"releState\":\"" + String(releState) +
                      "\", \"lat\":\"" + String(gpsLat, 6) +
                      "\", \"lon\":\"" + String(gpsLon, 6) + "\"}";

    if (mqtt.publish("RS/state", datos.c_str())) {
      Serial.print("Datos publicados: ");
      Serial.println(datos);
    } else {
      Serial.println("Error: No se pudieron publicar los datos.");
    }
  }
  mqtt.loop();
}

void initMPU() {
  if (!mpu.begin()) {
    Serial.println("No se encontró el MPU6050");
    while (1) delay(10);
  }
  Serial.println("MPU6050 listo!");
}


float aplicarFiltro(float nuevoValor, float valorFiltrado) {
  return alpha * nuevoValor + (1 - alpha) * valorFiltrado;
}

float calcularInclinacion(float eje1, float eje2) {
  return atan2(eje1, eje2) * (180.0 / PI);
}

bool getGPS(float &lat, float &lon) {
  if (!modem.enableGPS()) {
    Serial.println("No se pudo habilitar el GPS");
    return false;
  }

  unsigned long start = millis();
  while (millis() - start < 10000) {  // Esperar hasta 10 segundos por una señal
    if (modem.getGPS(&lat, &lon)) {
      Serial.println("GPS Fix obtenido");
      Serial.print("Latitud: ");
      Serial.println(lat, 6);
      Serial.print("Longitud: ");
      Serial.println(lon, 6);
      modem.disableGPS();  // Apagar el GPS para ahorrar energía
      return true;
    }
    delay(1000);
  }

  Serial.println("No se pudo obtener posición GPS");
  modem.disableGPS();  // Apagar el GPS si falla
  return false;
}
