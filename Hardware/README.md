# 🔩 RiderSafe – Componentes de Hardware

Este documento detalla los componentes de hardware utilizados en el desarrollo del sistema RiderSafe, incluyendo placas de desarrollo, sensores, actuadores y módulos de comunicación.

---

## 🧠 Placas de Desarrollo

### 1. Heltec WiFi LoRa 32 (V2) ×2

Microcontrolador ESP32 con pantalla OLED integrada y módulo LoRa SX1276 para comunicación de largo alcance.

- Uso en el proyecto:
  - Nodo cliente portátil
  - Nodo puente intermedio
- Comunicación: SPI (LoRa), I2C (sensores), UART (comunicación entre nodos)
- Voltaje de operación: 3.3V

🔗 [Ficha técnica – Heltec WiFi LoRa 32 V2](https://heltec.org/project/wifi-lora-32v2/)

---

### 2. LilyGO T-SIM7600G-H ×1

Placa basada en ESP32 con módulo SIM7600G-H incorporado para comunicación por red celular 4G, GPS y GNSS.

- Uso en el proyecto:
  - Nodo central de transmisión 4G
- Protocolos: UART (AT Commands), MQTT
- Funciones: Envío de datos por internet, GPS integrado

🔗 [Ficha técnica – LilyGO T-SIM7600G-H](https://lilygo.cc/products/t-sim7600)

---

## 🔧 Sensores y Módulos

### 3. MPU6050 ×1

Sensor de movimiento que combina acelerómetro y giroscopio de 3 ejes.

- Uso en el proyecto:
  - Detección de inclinación, aceleración y posibles caídas
- Comunicación: I2C
- Rango de voltaje: 3.3V a 5V

🔗 [Ficha técnica – MPU6050 (InvenSense)](https://invensense.tdk.com/wp-content/uploads/2015/02/MPU-6000-Datasheet1.pdf)

---

### 4. Módulo Relevador SRD-05VDC-SL-C ×1

Actuador electromecánico que permite abrir o cerrar circuitos eléctricos mediante una señal digital.

- Uso en el proyecto:
  - Activación/corte del sistema eléctrico de la motocicleta
- Voltaje de activación: 5V
- Tipo: Normalmente abierto (NO) / normalmente cerrado (NC)

🔗 [Ficha técnica – Relevador SRD-05VDC-SL-C](https://naylampmechatronics.com/img/cms/000263/SRD-05VDC-SL-C-Datasheet.pdf)

---

### 5. Convertidor Lógico de 4 Canales (Bidireccional) ×1

Permite la comunicación entre dispositivos con diferentes niveles de voltaje (3.3V ↔ 5V).

- Uso en el proyecto:
  - Interfaz segura entre módulos a diferentes voltajes (por ejemplo, ESP32 y relevador)
- Compatible con I2C y señales digitales

🔗 *Enlace genérico según el módulo utilizado. (Puedes incluir tu proveedor específico si lo tienes)*

---

## ⚙️ Diagrama de Interconexión

> 💡 Recomendación: agrega aquí un esquema de conexión como `hardware/esquematico_general.png` para visualizar cómo se conectan todos los módulos entre sí.

```md
![Diagrama general de conexiones](../diagramas/esquematico_general.png)
