#include <Adafruit_Fingerprint.h>  //Libreria para usar el sensor de huella
#include <Preferences.h>           //Libreria para usar la memoria del esp

#define RX_HUELLA 17  //Pin RX del sensor
#define TX_HUELLA 16  //Pin TX del sensor

const int botonEnrolar = 18;  //Boton para activar la funcion de enrolar
const int botonBuscar = 19;   //Boton para activar la funcion de buscar

const int ledEnrolar = 22;  //Led para saber si la funcion de enrolar esta activa
const int ledBuscar = 23;   //Led para saber si la funcion de buscar esta activa

HardwareSerial Serial_Huella(1);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&Serial_Huella);

Preferences prefs;
String readStringFromSerial() {
  String input = "";
  while (true) {
    if (Serial.available()) {
      char c = Serial.read();
      if (c == '\n' || c == '\r') {
        if (input.length() > 0) {
          break;
        }
      } else {
        input += c;
      }
    }
  }
  return input;
}

byte fingerprintID = 0;  //ID para almacenar las huellas guardadas
String Nombre_Alumno;    //String para almacenar los nombres asociados a las huellas

void EnrolarPersonal(void);
void buscarModoLectura();
int buscarFingerprintID();
void Mostrar_Alumno(byte ID, String Nombre);

bool modoBusquedaActivo = false; //estado para activar la funcion de busqueda
bool estadoAnteriorBoton = HIGH; //estado para saber el estado del boton

void setup() {

  Serial.begin(115200);
  Serial_Huella.begin(57600, SERIAL_8N1, RX_HUELLA, TX_HUELLA);

  prefs.begin("nombre", false);
  delay(2000);

  //Iniciar verificacion para saber si hay conexion con el sensor
  Serial.println("Llamando a verifyPassword()...");
  bool estado = finger.verifyPassword();
  Serial.print("verifyPassword() devolvió: ");
  Serial.println(estado);

  if (estado) {
    Serial.println("Sensor disponible\n");
  } else {
    Serial.println("Sensor NO disponible!\nRevise las conexiones y reinicie el Arduino...");
    while (1) {
      delay(2000);
    }
  }
  //Configurar botones
  pinMode(botonEnrolar, INPUT_PULLUP);
  pinMode(botonBuscar, INPUT_PULLUP);

  //Configurar leds
  pinMode(ledEnrolar, OUTPUT);
  pinMode(ledBuscar, OUTPUT);



  //Conectar con PLX DAQ
 
  Serial.println("LABEL, FECHA, HORA, ID, NOMBRE");  //Columnas en el excel
  Serial.println("RESETTIMER");
}

//Se usaran dos botones para usar las funcionalidades
//Mientras que la funcion este activa se activara un led
void loop() {
   bool estadoActualBoton = digitalRead(botonBuscar);

  // Detectar cambio de estado del botón (presión)
  if (estadoAnteriorBoton == HIGH && estadoActualBoton == LOW) {
    modoBusquedaActivo = !modoBusquedaActivo;

    if (modoBusquedaActivo) {
      Serial.println("Modo búsqueda activado");
      digitalWrite(ledBuscar, HIGH);
    } else {
      Serial.println("Modo búsqueda desactivado");
      digitalWrite(ledBuscar, LOW);
    }

    delay(200);  
  }

  estadoAnteriorBoton = estadoActualBoton;

  // Ejecutar búsqueda solo si el modo está activo
  if (modoBusquedaActivo) {
    buscarModoLectura();  
  }
  //Ejecutar si se presiona el boton de enrolar
  if (digitalRead(botonEnrolar) == LOW) {
    Serial.println("Botón Enrolar presionado");
    EnrolarPersonal(); delay(500);
    }
}

uint8_t readnumber(void) {
  uint8_t num = 0;
  while (num == 0) {
    while (!Serial.available())
      ;
    num = Serial.parseInt();
  }
  return num;
}


//Funcion para guardar una huella en el sensor
void EnrolarPersonal(void) {
  digitalWrite(ledEnrolar, HIGH);
  Serial.println("Preparado para enrolar una huella");
  Serial.println("Ingrese el Numero del ID de la huella a guardar (1 -127): ");
  fingerprintID = readnumber();  //lee el numero para guardar el ID
  if (fingerprintID == 0) {
    digitalWrite(ledEnrolar, HIGH);
    return;
  }
  Serial.print("Enrolando ID #");
  Serial.println(fingerprintID);
  //Si se guardo correctamente ingresa un nombre por consola para asignarlo al ID
  if (getFingerprintEnroll() == true) {
    Serial.println("Ingrese el nombre del alumno:");
    Nombre_Alumno = readStringFromSerial();                         // leer el nombre
    prefs.putString(String(fingerprintID).c_str(), Nombre_Alumno);  // Guardarlo en la memoria del esp
    String mensajeGuardado = "Nombre guardado correctamente: ";
    mensajeGuardado += Nombre_Alumno;
    String mensajeId = " ,con el ID: ";
    mensajeGuardado += mensajeId;
    mensajeGuardado += fingerprintID;
    Serial.println(mensajeGuardado);

    digitalWrite(ledEnrolar, LOW);
    Serial.println("Regresando al menú principal...\n");
    delay(1500);
  } else {
    Serial.println("No se pudo enrolar la huella. Intente de nuevo.");
    delay(1500);
  }
}

uint8_t getFingerprintEnroll() {

  int p = -1;
  Serial.print("Esperando una huella valida para enrolar ");
  Serial.println(fingerprintID);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    if (p == FINGERPRINT_NOFINGER) delay(100);
  }

  p = finger.image2Tz(1);
  if (p != FINGERPRINT_OK) return p;

  Serial.println("Remueva el dedo");
  delay(2000);
  while (finger.getImage() != FINGERPRINT_NOFINGER)
    ;

  Serial.println("Coloque el mismo dedo de nuevo");
  while ((p = finger.getImage()) != FINGERPRINT_OK)
    ;

  p = finger.image2Tz(2);
  if (p != FINGERPRINT_OK) return p;

  p = finger.createModel();
  if (p != FINGERPRINT_OK) return p;

  p = finger.storeModel(fingerprintID);
  if (p == FINGERPRINT_OK) {
    Serial.println("EXITO - Huella Guardada!");
    return true;
  }
  return false;
}


//Funcion para buscar huella
void buscarModoLectura() {
    fingerprintID = buscarFingerprintID();
    if (fingerprintID != -1) {
      Nombre_Alumno = prefs.getString(String(fingerprintID).c_str(), "");
      if (Nombre_Alumno != "") {
        Mostrar_Alumno(fingerprintID, Nombre_Alumno);
      }
    }
}

int buscarFingerprintID() {
  digitalWrite(ledBuscar, HIGH);  // Enciende LED al iniciar búsqueda

  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK) {
    digitalWrite(ledBuscar, LOW);
    return -1;
  }

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) {
    digitalWrite(ledBuscar, LOW);
    return -1;
  }

  p = finger.fingerFastSearch();

  if (p == FINGERPRINT_OK) {
    Serial.print("ID Identificado = #");
    Serial.print(finger.fingerID);
    Serial.print(" con precisión ");
    Serial.println(finger.confidence);
    digitalWrite(ledBuscar, LOW);  // Apaga LED al terminar
    return finger.fingerID;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("No se encontró coincidencia en la base de datos.");
  }


  digitalWrite(ledBuscar, LOW);  // Apaga LED al terminar
  return -1;                     // Retorna -1 en caso de error o no coincidencia
}



void Mostrar_Alumno(byte ID, String Nombre) {
  Serial.print("DATA, DATE, TIME,");
  Serial.print(ID);
  Serial.print(",");
  Serial.println(Nombre);
  Serial.print("\nBienvenido a clase, ");
  Serial.println(Nombre + "\n");
}
