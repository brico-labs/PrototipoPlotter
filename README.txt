link de la documentación va a una web china si es que funciona. Muiy lenta y no carga
Solución:  https://github.com/HobbyComponents/CH340-Drivers/tree/master/CH341SER
Instalar drivers desde la carpeta de 64bit
Driver install failure
https://www.youtube.com/watch?v=JmQbNyUqtCs
windows dice que ya está el mejor.
funciona correctamente seleccionando la placa como un UNO.


--------------- TARJETA SD ----------------------

https://learn.adafruit.com/adafruit-micro-sd-breakout-board-card-tutorial/arduino-library
https://www.luisllamas.es/tarjeta-micro-sd-arduino/

#include <SD.h>
 
File myFile;
 
void setup()
{
  Serial.begin(9600);

  // comprobación de que la SD funciona.
if (!SD.begin(10)) {
    Serial.println("initialization failed!");
    return;
  }
 
    // ESCRITURA: Abrir archivo, escribir y cerrar 
    myFile = SD.open("test.txt", FILE_WRITE);
    myFile.println("testing lite");
    myFile.close();     // close the file:

 Serial.println("escritura lista");

    // LECTURA: Abrir archivo, leer byte por byte y cerrar.
    myFile = SD.open("test.txt");
    // read from the file until there's nothing else in it:
    while (myFile.available()) {      // Comprobar si quedan bytes por leer
     Serial.write(myFile.read());     // IMPORTANTE!!!!!!   Read() only returns a byte at a time. It does not read a full line or a number!
    }
    myFile.close();   // close the file:
  
}
 
void loop()
{
  // nothing happens after setup
}


UN BUEN RATO DE INVESTIGACIÓN INTENTANDO COPIAR LA TRADUCCIÓN DE CARTESIANO A POLAR NO DA RESULTADOS.
VOY A INTENTAR CENTRARME EN ALMACENAR VALORES CARTESIANOS Y MANDARLOS A LOS MOTORES.
CAMBIAR LOS VALORES QUE SE ALIMENTAN A LOS MOTORES, DEBERÍA SER SENCILLO.