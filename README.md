# 🏍️ RIDERSAFE 🏍️
### *Sistema Inteligente de Seguridad y Monitoreo para Motocicletas*

RiderSafe es un sistema de geolocalización y monitoreo en tiempo real diseñado para mejorar la seguridad de las motocicletas en Mixquiahuala de Juárez, Hidalgo, México. Este sistema responde al incremento sostenido del robo de motocicletas en la región, utilizando tecnologías como LoRa, GPS y conectividad 4G para rastrear, controlar y actuar ante eventos sospechosos o situaciones de robo.

---

## 📁 Estructura de Carpetas
/Software → Archivos .ino, scripts, dashboard
/Hardware → Fichas técnicas, modelos
/Documentacion → Manuales, reportes técnicos
/Diagramas → Diagramas de esquemáticos, flujo de datos

---

### 🧠 Tecnologías Utilizadas

- Lenguaje de programación: C++ (Arduino IDE), HTML, CSS, JavaScript  
- Protocolos de comunicación: LoRa, I2C, SPI, MQTT, ESPNOW, AT Commands  
- Sensores y módulos: MPU6050, GPS, SIM7600G-H  
- Servicios en la nube: Mosquitto (servidor MQTT)  
- Interfaces de usuario: Dashboard web con NODE-RED  

---

### ⚙️ Funcionalidades

- Geolocalización precisa vía GPS  
- Comunicación de largo alcance con LoRa  
- Transmisión de datos por 4G a la nube vía MQTT  
- Detección de inclinación y aceleración con MPU6050  
- Activación remota de un cortacorriente  
- Visualización de estado de batería, señal LoRa, estado del relé  
- Modularidad para visualizar datos y controlar el sistema desde una aplicación web o dashboard  

---

## 🖥️ Instrucciones de Instalación

### 🔌 En Heltec LoRa 32 v2 (Llavero Portátil)

1. Cargar código `LlaveroPortatilv3`  
2. Instalar en la PCB impresa 1  

### 🔁 En Heltec LoRa 32 v2 (Nodo Puente)

1. Cargar código `Heltec-gateway`  
2. Instalar en la PCB impresa 2  

### 🌐 En LilyGO (Nodo SIM7600)

1. Cargar código `SendMQTT_data`  
2. Instalar en la PCB impresa 2  

### 🧠 En Raspberry Pi

1. Instalar IoT Stack  
2. Importar JSON con el nombre `RiderSafe.json`  
3. Ejecutar script para Grafana  

---

### ⚙️ Configuración

- Usar el regulador de voltaje a **5V**, establecer ese voltaje antes de la instalación  
- Algunos sensores **MPU6050** no son reconocidos por la biblioteca Adafruit  
  - Cargar código `who_am_i` y reemplazar dirección del dispositivo en la librería  

---

### 📸 Escenarios de Uso

🎥 Mira la demostración en video:  
[![Ver Video](https://img.youtube.com/vi/g3GmbGhdLP4/0.jpg)](https://youtu.be/g3GmbGhdLP4)

---

### 📘 Casos de Uso

- **Usuario Final**: consulta ubicación y estado desde app web, puede activar o desactivar relé remotamente.  
- **Administrador**: monitorea estados, accede a estadísticas y análisis.  
- **Proveedor**: configura módulos, actualiza firmware, da mantenimiento.  

---

## 🚧 Limitaciones y Mejoras Futuras

### ⚠️ Limitaciones actuales

- Limitada tolerancia a pérdida de energía prolongada  
- Falta de protecciones de alto o bajo voltaje  
- El dashboard funciona con limitaciones sin internet  
- Inestabilidad e incompatibilidad de datos en Grafana  

### 🌱 Mejoras propuestas

- Añadir almacenamiento local cuando no haya conexión  
- Rediseñar interfaz web  
- Mejorar duración de batería  
- Agregar protecciones contra alto voltaje  
- Encendido y apagado automático de la motocicleta  
- Implementar batería de emergencia  
- Medidor de batería en ambos dispositivos  
- Relevador lógico de 3.3V especializado  
- Agregar más casos para alertas  
- Uso de PCB impresa  

---

## 🤝 Contribuciones

¿Quieres aportar al proyecto?

1. Haz un fork del repositorio  
2. Crea una nueva rama
3. Realiza cambios y commits descriptivos
4. Reporta errores o mejoras en la sección "Issues"

## 📜 Licencia
---
