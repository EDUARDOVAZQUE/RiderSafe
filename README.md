# 🏍️ RIDERSAFE 🔒

Breve descripción del proyecto de IoT, incluyendo su propósito, alcance y las tecnologías utilizadas.

## 📁 Estructura de Carpetas

### 🛠 Tecnologías Utilizadas

- Lenguaje: C++ (Arduino), Python (Raspberry Pi), Bash (PC)
- Frameworks: PlatformIO, Flask, MQTT Mosquitto
- Base de Datos: MySQL, InfluxDB
- Herramientas: Grafana, Zabbix, Node-RED

### ✨ Funcionalidades

- Comunicación LoRa y SIM800 para transmisión de datos
- Recolección de datos ambientales
- Almacenamiento de datos en la nube/local
- Visualización en dashboards (Grafana)
- Monitorización con alertas (Zabbix)

### 📦 Estructura del Código

Dispositivos utilizados:

- Heltec LoRa 32
- Raspberry Pi
- LilyGO SIMxxx
- PC

Descripción por módulos:

- `/main.ino`: captura sensores y envía por LoRa
- `/mqtt.py`: recibe datos en Raspberry y reenvía por MQTT
- `/dashboard_setup/`: configuración de paneles en Grafana
- `/zabbix_agentd.conf`: configuración para monitoreo

---

## 🖥️ Instrucciones de Instalación

### 🔌 En Heltec LoRa 32

1. Instalar PlatformIO
2. Subir `main.ino`
3. Conectar sensores según el esquema

### 🧠 En Raspberry Pi

1. Instalar Mosquitto y Python3
2. Ejecutar `mqtt.py`
3. Configurar red Wi-Fi

---

## 🔩 Hardware

### 📋 Componentes Utilizados

- Heltec LoRa 32
- Raspberry Pi 4
- Sensor DHT11
- Sensor GPS
- Módulo SIM800L
- Actuadores (relevadores, LED)

### 📊 Esquemáticos

> ![Esquema del circuito](Hardware/esquematico_heltec.png)

### ⚙️ Configuración

Conecta los sensores de la siguiente forma:
- DHT11: GPIO 21
- SIM800: UART
- GPS: UART2

---

## 🌐 Interconexión de Red

### 🧷 Cableada

- Raspberry conectada a PC por USB
- Módulo SIM800 conectado vía serial

### 📶 Inalámbrica

- LoRa: comunicación entre nodos
- Wi-Fi: conexión del Raspberry a la nube
- Bluetooth/NFC: para lectura desde celular

> ![Topología física](Diagramas/topologia_fisica.png)

---

## ✅ Funcionabilidad

### 📸 Escenarios de Uso

> Agrega fotos o gifs aquí  
> ![Uso del sistema](Documentacion/uso.gif)

- Usuario revisa datos en Grafana
- Admin recibe alertas por Zabbix

### 📘 Casos de Uso

- Usuario final: observa variables en tiempo real
- Administrador: configura nodos y alertas
- Proveedor: da mantenimiento a hardware remoto

---

## 🚧 Limitaciones y Mejoras Futuras

**Limitaciones actuales:**

- No soporta reconexión automática al LoRa
- El monitoreo no es en tiempo real en zonas sin red

**Mejoras propuestas:**

- Añadir almacenamiento local cuando no haya conexión
- Rediseñar interfaz web
- Mejorar duración de batería

---

## 🤝 Contribuciones

¿Quieres aportar al proyecto?  
1. Haz un fork
2. Crea tu rama: `git checkout -b nueva-funcionalidad`
3. Haz un commit: `git commit -am 'Agrega nueva funcionalidad'`
4. Abre un pull request

---

## 📜 Licencia

Este proyecto está licenciado bajo la MIT License. Consulta el archivo [LICENSE](LICENSE) para más detalles.
![Versión](https://img.shields.io/badge/version-1.0-blue)
![Licencia](https://img.shields.io/badge/licencia-MIT-green)
![LoRa](https://img.shields.io/badge/comunicación-LoRa-yellow)
