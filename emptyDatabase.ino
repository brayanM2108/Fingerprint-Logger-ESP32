#include <Adafruit_Fingerprint.h>

// Pines para el sensor en el ESP32
#define RX_HUELLA 17  //Pin RX del sensor
#define TX_HUELLA 16  //Pin TX del sensor

HardwareSerial Serial_Huella(1); 
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&Serial_Huella);

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("\n\nBorrando todas las huellas de la base de datos...");
  Serial.println("Presiona 'Y' y Enter para continuar.");

  // Esperar confirmación
  while (true) {
    if (Serial.available() && (Serial.read() == 'Y')) {
      break;
    }
  }

  // Inicializar puerto del sensor
  Serial_Huella.begin(57600, SERIAL_8N1, RX_HUELLA, TX_HUELLA);

  // Verificar sensor
  if (finger.verifyPassword()) {
    Serial.println("Sensor encontrado!");
  } else {
    Serial.println("No se encontró el sensor de huella :(");
    while (true) {
      delay(1000);
    }
  }

  // Vaciar base de datos
  finger.emptyDatabase();
  Serial.println("Base de datos borrada correctamente.");
}

void loop() {
  // No hay nada que hacer en el loop
}
