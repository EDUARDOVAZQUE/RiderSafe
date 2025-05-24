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
![Logo de Heltec](Hardware/wifi-lora-32-v2-1.png)


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
---

### 6. Regulador de Voltaje Buck LM2596 ×1

Módulo convertidor **step-down** (reductor de voltaje) basado en el chip LM2596 de Texas Instruments, utilizado para alimentar los módulos electrónicos desde fuentes como la batería de una motocicleta.

- **Uso en el proyecto:**
  - Reduce los 12–14V del sistema eléctrico de la moto a 5V estables
  - Alimenta de forma segura la LilyGO SIM7600G-H y otros componentes sensibles
- **Voltaje de entrada recomendado:** 7V a 24V DC
- **Voltaje de salida típico:** Ajustable, comúnmente calibrado a 5V
- **Corriente de salida:** Hasta 2A
- **Frecuencia de conmutación:** 150 kHz

🔗 [Ficha técnica oficial – LM2596 (Texas Instruments)](https://www.ti.com/lit/ds/symlink/lm2596.pdf)

> ⚠️ **Precaución**: Algunos módulos LM2596 económicos requieren calibración manual. Gira el potenciómetro con cuidado y confirma la salida de 5.00V exactos antes de conectar tu placa.


