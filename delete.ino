#include <Adafruit_Fingerprint.h>

#define RX_HUELLA 17  // Pin RX del sensor
#define TX_HUELLA 16  // Pin TX del sensor

HardwareSerial Serial_Huella(1); 
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&Serial_Huella);

void setup()
{
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n\nEliminar huella");

  // Configurar la velocidad de comunicación con el sensor
  Serial_Huella.begin(57600, SERIAL_8N1, RX_HUELLA, TX_HUELLA);

  if (finger.verifyPassword()) {
    Serial.println("¡Sensor de huella encontrado!");
  } else {
    Serial.println("No se encontró el sensor de huella :(");
    while (1);  // Detener ejecución
  }
}

uint8_t leerNumero(void) {
  uint8_t num = 0;

  while (num == 0) {
    while (!Serial.available());
    num = Serial.parseInt();
  }
  return num;
}

void loop()
{
  Serial.println("Por favor, ingresa el ID (del 1 al 127) que deseas eliminar...");
  uint8_t id = leerNumero();
  if (id == 0) { // ID 0 no permitido
    return;
  }

  Serial.print("Eliminando huella con ID #");
  Serial.println(id);

  eliminarHuella(id);
}

uint8_t eliminarHuella(uint8_t id) {
  uint8_t p = -1;

  p = finger.deleteModel(id);

  if (p == FINGERPRINT_OK) {
    Serial.println("¡Huella eliminada exitosamente!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Error de comunicación con el sensor");
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("No se pudo eliminar en esa ubicación");
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error al escribir en la memoria del sensor");
  } else {
    Serial.print("Error desconocido: 0x"); Serial.println(p, HEX);
  }

  return p;
}
