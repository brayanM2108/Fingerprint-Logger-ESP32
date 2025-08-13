# Sistema de Asistencia con Huella Digital

Este proyecto permite registrar, buscar y autenticar personas utilizando un **ESP32** y un sensor de huellas.
El sistema envía los datos registrados (fecha, hora, ID y nombre) automáticamente a Google Sheets a través de una implementación Web App en Google Apps Script, lo que permite un control de asistencia en tiempo real.


## 🧩 Funcionalidades principales
- **Enrolamiento de huella**:
  - Guardado de huellas en el sensor con un ID único.
  - Asociación de nombre a cada ID usando memoria interna del ESP32.

- **Búsqueda y validación de huella**:
  - Activación de modo búsqueda mediante botón físico.
  - Identificación de huella y recuperación del nombre asociado.

- **Integración con Google Sheets**:
  - Envío automático de ID y nombre al detectar la huella.
  - Registro en tiempo real en la hoja de cálculo.

- **Indicadores LED**:
  - LED de estado para enrolamiento, búsqueda y confirmación de envío.

- **Botones físicos para**:
  - Activar/desactivar búsqueda.
  - Iniciar proceso de enrolamiento.

## 📦 Hardware necesario

- ✅ESP32 (cualquier modelo con WIFI).
- ✅Sensor de huellas digitales compatible con la librería Adafruit_Fingerprint (En este caso use el modelo Fpm10a).

- ✅2 botones pulsadores:
  - Enrolar huella
  - Activar/desactivar modo búsqueda
- ✅3 LEDs :
  - Indicador de enrolamiento
  - Indicador de búsqueda
  - Indicador de confirmación de envío a Google Sheets
- ✅ Cables y protoboard


## 🔌 Conexiones

| Componente       | Pin ESP32 |
|------------------|-----------|
| Sensor RX        | 17        |
| Sensor TX        | 16        |
| Botón Enrolar    | 18        |
| Botón Buscar     | 19        |
| LED Confirmación | 21        |
| LED Enrolar      | 22        |
| LED Buscar       | 23        |

⚠️ **Importante**: El sensor de huella funciona a 3.3 V o 5 V según el modelo. Revisa su datasheet antes de conectarlo.

---

## 📥 Instalación

1. Clona este repositorio:
```bash
git clone https://github.com/brayanM2108/Fingerprint-Logger-ESP32
```
2. Instala las librerías necesarias en el IDE de Arduino:

- Adafruit Fingerprint Sensor Library

- Preferences (incluida en ESP32 Arduino Core)

- WiFi (incluida en ESP32 Arduino Core)

- HTTPClient (incluida en ESP32 Arduino Core)

3. Configura tus credenciales de WiFi:

4. Configura la URL de la Web App de Google Sheets:

## ☁️ Integración con Google Sheets
Para recibir los datos en una hoja de cálculo:

1. Crea un Google Sheets.

2. Abre el Editor de Apps Script (Extensiones > Apps Script).

3. Pega este script para recibir datos vía POST y guardarlos en la hoja.
```javascript
function doPost(e) {
  const sheet = SpreadsheetApp.getActiveSpreadsheet().getActiveSheet();
  const data = JSON.parse(e.postData.contents);

  const timestamp = new Date(); // hora del servidor

  sheet.appendRow([
    timestamp,
    data.id,
    data.nombre,
  ]);
  return HtmlService.createHtmlOutput("OK");
}
```
4. Despliega el script como Aplicación web con acceso "Cualquiera, incluso anónimo".

5. Copia la URL y pégala en GOOGLE_SHEETS_URL en el código.

## ▶️ Flujo de uso

1. **Encender el sistema y conectar el ESP32 a WiFi.**

2. **Enrolar huella**  
   - Presiona el botón de enrolar.  
   - Ingresa un ID (1–127) en el monitor serie.  
   - Escanea dos veces el mismo dedo.  
   - Ingresa el nombre del alumno.

3. **Activar modo búsqueda**  
   - Presiona el botón de búsqueda para activar/desactivar el modo lectura.  
   - Cuando esté activo, el ESP32 buscará coincidencias en el sensor.

4. **Confirmación y envío**  
   - Si se detecta una huella registrada, se recupera el nombre.  
   - Se envía automáticamente a Google Sheets.  
   - El LED de confirmación parpadea si el envío fue exitoso.

## 👥 Creditos
Este proyecto nació como parte de mi trabajo de grado de grado 11 en 2021 para automatizar el registro de asistencia, con el apoyo del profesor Cristian Quimbayo. 
La primera versión fue desarrollada utilizando un Arduino Nano y una herramienta llamada PLX DAQ para enviar los datos a Excel.  
Desde entonces, el sistema ha evolucionado hacia una solución más robusta con ESP32 y Google Sheets, manteniendo el enfoque en la automatización y la accesibilidad.



