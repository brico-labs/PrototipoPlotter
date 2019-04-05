#include <Arduino.h>
// the regular Adafruit "TouchScreen.h" library only works on AVRs

// different mcufriend shields have Touchscreen on different pins
// and rotation.
// Run the UTouch_calibr_kbv sketch for calibration of your shield

#include <Adafruit_GFX.h> // Core graphics library
//#include <Adafruit_TFTLCD.h> // Hardware-specific library
//Adafruit_TFTLCD tft(A3, A2, A1, A0, A4);
#include <MCUFRIEND_kbv.h>
MCUFRIEND_kbv tft; // hard-wired for UNO shields anyway.
#include <TouchScreen.h>
//#include <SD.h>
#include <SdFat.h>
#include <SPI.h>
#if defined(__SAM3X8E__)
#undef __FlashStringHelper::F(string_literal)
#define F(string_literal) string_literal
#endif

//----------------------------------------|
// TFT Breakout  -- Arduino UNO / Mega2560 / OPEN-SMART UNO Black
// GND              -- GND
// 3V3              -- 3.3V
// CS               -- A3
// RS               -- A2
// WR               -- A1
// RD               -- A0
// RST              -- RESET
// LED              -- GND
// DB0              -- 8
// DB1              -- 9
// DB2              -- 10
// DB3              -- 11
// DB4              -- 4
// DB5              -- 13
// DB6              -- 6
// DB7              -- 7
// most mcufriend shields use these pins and Portrait mode:

typedef struct
{
  double x;
  double y;
} Point;

uint8_t YP = A1; // must be an analog pin, use "An" notation!
uint8_t XM = A2; // must be an analog pin, use "An" notation!
uint8_t YM = 7;  // can be a digital pin
uint8_t XP = 6;  // can be a digital pin
uint8_t SwapXY = 0;

#define grbl Serial

// Valores máximos y mínimos del táctil
uint16_t TS_LEFT = 870;
uint16_t TS_RT = 165;
uint16_t TS_TOP = 970;
uint16_t TS_BOT = 110;
char *name = "Unknown controller";

const int MIN_POINT_INTERVAL = 100;

// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// For the one we're using, its 300 ohms across the X plate
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 250);
TSPoint tp;

File myFile;
int feedrate = 5000;

int CS_PIN = 5; //CS_PIN Tarjeta SD
const uint8_t SOFT_MISO_PIN = 12;
const uint8_t SOFT_MOSI_PIN = A4;
const uint8_t SOFT_SCK_PIN = A5;
//
// Chip select may be constant or RAM variable.
//const uint8_t SD_CHIP_SELECT_PIN = 10;

// SdFat software SPI template
SdFatSoftSpi<SOFT_MISO_PIN, SOFT_MOSI_PIN, SOFT_SCK_PIN> SD;

#define MINPRESSURE 20
#define MAXPRESSURE 1000

#define SWAP(a, b)    \
  {                   \
    uint16_t tmp = a; \
    a = b;            \
    b = tmp;          \
  }

uint16_t BOXSIZE;
uint16_t BOXSIZEY;
uint16_t PENRADIUS = 3;
uint16_t identifier, oldcolor, currentcolor;
uint8_t Orientation = 3; //PORTRAIT

// Assign human-readable names to some common 16-bit color values:
#define BLACK 0x0000
#define BLUE 0x001F
#define RED 0xF800
#define GREEN 0x07E0
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0
#define WHITE 0xFFFF

void show_Serial(void)
{
  /*  Serial.print(F("Found "));
  Serial.print(name);
  Serial.println(F(" LCD driver"));
  Serial.print(F("ID=0x"));
  Serial.println(identifier, HEX);
  Serial.println("Screen is " + String(tft.width()) + "x" + String(tft.height()));
  Serial.println("Calibration is: ");
  Serial.println("LEFT = " + String(TS_LEFT) + " RT  = " + String(TS_RT));
  Serial.println("TOP  = " + String(TS_TOP) + " BOT = " + String(TS_BOT));
  Serial.print("Wiring is: ");
  Serial.println(SwapXY ? "SWAPXY" : "PORTRAIT");
  Serial.println("YP=" + String(YP) + " XM=" + String(XM));
  Serial.println("YM=" + String(YM) + " XP=" + String(XP));
*/
}
/*
void show_tft(void) // FUNCION para mostrar parámetros de la SD. Comentada para ahorrar tiempo más adelante, la dejamos por si hiciese falta comprobar algo
{
  tft.setCursor(0, 0);
  tft.setTextSize(2);
  tft.print(F("Found "));
  tft.print(name);
  tft.println(F(" LCD"));
  tft.setTextSize(1);
  tft.print(F("ID=0x"));
  tft.println(identifier, HEX);
  tft.println("Screen is " + String(tft.width()) + "x" + String(tft.height()));
  tft.println("Calibration is: ");
  tft.println("LEFT = " + String(TS_LEFT) + " RT  = " + String(TS_RT));
  tft.println("TOP  = " + String(TS_TOP) + " BOT = " + String(TS_BOT));
  tft.print("\nWiring is: ");
  if (SwapXY)
  {
    tft.setTextColor(CYAN);
    tft.setTextSize(2);
  }
  tft.println(SwapXY ? "SWAPXY" : "PORTRAIT");
  tft.println("YP=" + String(YP) + " XM=" + String(XM));
  tft.println("YM=" + String(YM) + " XP=" + String(XP));
  tft.setTextSize(2);
  tft.setTextColor(RED);
  tft.setCursor((tft.width() - 48) / 2, (tft.height() * 2) / 4);
  tft.print("EXIT");
  tft.setTextColor(YELLOW, BLACK);
  tft.setCursor(0, (tft.height() * 6) / 8);
  tft.print("Touch screen for loc");
  while (1)
  {
    tp = ts.getPoint();
    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);
    pinMode(XP, OUTPUT);
    pinMode(YM, OUTPUT);
    if (tp.z < MINPRESSURE || tp.z > MAXPRESSURE)
      continue;
    if (tp.x > 450 && tp.x < 570 && tp.y > 450 && tp.y < 570)
      break;
    tft.setCursor(0, (tft.height() * 3) / 4);
    tft.print("tp.x=" + String(tp.x) + " tp.y=" + String(tp.y) + "   ");
  }
}
*/

void setup(void)
{
  uint16_t tmp;
  tft.begin(9600);

  tft.reset();
  identifier = tft.readID();
  if (identifier == 0x6814)
    name = "RM68140";
  switch (Orientation)
  { // adjust for different aspects
  case 0:
    break; //no change,  calibrated for PORTRAIT
  case 1:
    tmp = TS_LEFT, TS_LEFT = TS_BOT, TS_BOT = TS_RT, TS_RT = TS_TOP, TS_TOP = tmp;
    break;
  case 2:
    SWAP(TS_LEFT, TS_RT);
    SWAP(TS_TOP, TS_BOT);
    break;
  case 3:
    tmp = TS_LEFT, TS_LEFT = TS_TOP, TS_TOP = TS_RT, TS_RT = TS_BOT, TS_BOT = tmp;
    break;
  }

  grbl.begin(9600);

  tft.begin(0x6814); //to enable RM68140 driver code
  show_Serial();
  tft.setRotation(Orientation);
  tft.fillScreen(BLACK);
  //  show_tft();        // Retiramos la pantalla para ahorrar el tener que darle a exit

  BOXSIZE = 480 / 4; ////tft.width() / 6;
  BOXSIZEY = 320 / 10;
  tft.fillScreen(BLACK);

  // DIBUJAMOS BOTONES
  tft.fillRect(0, 0, BOXSIZE, BOXSIZEY, BLUE);
  tft.setCursor(BOXSIZE / 2 - 10, BOXSIZEY / 2 - 7);
  tft.setTextSize(2);
  tft.setTextColor(BLACK, BLUE);
  tft.print("H");
  tft.fillRect(BOXSIZE, 0, BOXSIZE, BOXSIZEY, RED);
  tft.setCursor(BOXSIZE + BOXSIZE / 2 - 10, BOXSIZEY / 2 - 7);
  tft.setTextSize(2);
  tft.setTextColor(BLACK, RED);
  tft.print("X");
  tft.fillRect(BOXSIZE * 2, 0, BOXSIZE, BOXSIZEY, YELLOW);
  tft.setCursor(BOXSIZE * 2 + BOXSIZE / 2 - 10, BOXSIZEY / 2 - 7);
  tft.setTextColor(BLACK, YELLOW);
  tft.setTextSize(2);
  tft.print("S");
  tft.fillRect(BOXSIZE * 3, 0, BOXSIZE, BOXSIZEY, GREEN);
  tft.setCursor(BOXSIZE * 3 + BOXSIZE / 2 - 15, BOXSIZEY / 2 - 7);
  tft.setTextColor(BLACK, GREEN);
  tft.setTextSize(2);
  tft.print("->");
  //tft.drawRect(0, 0, BOXSIZE, BOXSIZEY, WHITE);
  currentcolor = WHITE;
  delay(1000);
  SD.begin(CS_PIN);
  if (SD.begin(CS_PIN)) {
    Serial.println(";SD"); // COMENTADO DE MOMENTO YA QUE SI SE INICIALIZA LA SD, LA PANTALLA DEJA DE DIBUJAR.
  }
}

double segmentLength = 20; // mm
char lineBuffer[50];

double paddingLeft = 0;
double paddingRight = 0;
double paddingTop = 0;

double height = 580;
double width = 710;

Point M1 = {-paddingLeft, height + paddingTop};
Point M2 = {width + paddingRight, height + paddingTop};

Point currentPos = {0, 0};
unsigned long currentPosTime = millis();

double module(Point p)
{
  return sqrt(p.x * p.x + p.y * p.y);
}

Point getVector(Point p1, Point p2)
{
  return {p2.x - p1.x, p2.y - p1.y};
}

double distance(Point p1, Point p2)
{
  Point p = getVector(p1, p2);
  return module(p);
}

double screenXSize[2] = {0, 500};
double screenYSize[2] = {350, 0};

double mapX(double x) {
  return map(x, screenXSize[0], screenXSize[1], 0, width);
}

double mapY(double y) {
  //Serial.print("; map y: "); Serial.print(y);Serial.print(" => ");Serial.println(map(y, screenYSize[0], screenYSize[1], 0, height));
  return map(y, screenYSize[0], screenYSize[1], 0, height);
}

void generateTranslateGcode(const char *op, double x, double y, double f)
{
  sprintf(lineBuffer, "%s X", op);
  dtostrf(x, 4, 2, &lineBuffer[strlen(lineBuffer)]);
  strcpy(&lineBuffer[strlen(lineBuffer)], " Y");
  dtostrf(y, 4, 2, &lineBuffer[strlen(lineBuffer)]);
  if (f)
  {
    strcpy(&lineBuffer[strlen(lineBuffer)], " F");
    dtostrf(f, 4, 2, &lineBuffer[strlen(lineBuffer)]);
  }
}

void waitForOk()
{
  // read the receive buffer (if anything to read)
  char c, lastc;
  while (true)
  {
    if (grbl.available())
    {
      c = grbl.read();
      // Serial.print("rec:");
      // Serial.println(c);
      if (lastc == 'o' && c == 'k')
      {
        //Serial.println("GRBL <- OK");
        return;
      }
      lastc = c;
    }
    delay(3);
  }
}

void sendToGrbl(const char *gcode)
{
  // vaciar buffer
  while (grbl.available())
  {
    grbl.read();
  }
  grbl.println(gcode);
  waitForOk();
}

Point toPolar(Point cartesian)
{
  Point value = {
      module(getVector(M1, cartesian)),
      module(getVector(M2, cartesian))};
  /*
    Serial.print("  Cartesian: ");
    Serial.print(cartesian.x);
    Serial.print(",");
    Serial.println(cartesian.y);
    Serial.print("  Polar: ");
    Serial.print(value.x);
    Serial.print(",");
    Serial.println(value.y);
  */
  return value;
}


void processLine(char *line)
{
  double X;
  double Y;
  double F = 100;
  char *lineCopy = strdup(line);
  char *op = strtok(lineCopy, " ");
  if ((strcmp(op, "G1") == 0) || (strcmp(op, "G0") == 0))
  {
    char *param;
    while (param = strtok(NULL, " "))
    {
      //        Serial.print("Param: ");
      //        Serial.print(param);
      //        Serial.println(".");
      if (param[0] == 'X')
      {
        //sscanf(&param[1], "%lf", &X);
        X = atof(&param[1]);
      }
      else if (param[0] == 'Y')
      {
        //sscanf(&param[1], "%lf", &Y);
        Y = atof(&param[1]);
      }
      else if (param[0] == 'F')
      {
        //sscanf(&param[1], "%lf", &F);
        F = atof(&param[1]);
      }
      else
      {
      //  Serial.print("Unknown param ");
      //  Serial.println(param);
      }
    }

    Point sourcePos = { mapX(currentPos.x), mapY(currentPos.y) };
    Point targetPos = { mapX(X), mapY(Y) };
    Point vector = getVector(sourcePos, targetPos);
    double v_mod = module(vector);

    double segments = v_mod / segmentLength;

    double deltaX = vector.x / segments;
    double deltaY = vector.y / segments;
    Point nextPos;
    Point nextPosPolar;
    for (int i = 1; i < segments; i++)
    {
      nextPos.x = sourcePos.x + deltaX * i;
      nextPos.y = sourcePos.y + deltaY * i;
      nextPosPolar = toPolar(nextPos);
      generateTranslateGcode(op, nextPosPolar.x, nextPosPolar.y, F);
      sendToGrbl(lineBuffer);
    }
    nextPosPolar = toPolar(targetPos);
    generateTranslateGcode(op, nextPosPolar.x, nextPosPolar.y, F);
    sendToGrbl(lineBuffer);

    currentPos.x = X;
    currentPos.y = Y;
  }
  else
  {
    sendToGrbl(line);
  }
  free(lineCopy);
}

void guardasd()
{
  myFile = SD.open("test.txt", FILE_WRITE);
  generateTranslateGcode("G1", currentPos.x, currentPos.y, feedrate);
  myFile.println(lineBuffer);
  myFile.close();
}

void pantallaserial()
{
  generateTranslateGcode("G1", currentPos.x, currentPos.y, feedrate);
//  Serial.println(lineBuffer);
}

void calcfeedrate()
{
  /* tenemos posición anterior y posición actual. necesitamos distancia entre los dos puntos.
    a = xpos - xpos_old 
    b = ypos - ypos_old
    c^2 = x^2 + y^2
    c = sqrt(a*a+b*b) 
    tenemos millis anterior y actual
    tiempo =  millis_old - primermillis(); (no se me ocurre como detectar el primertoque ahora mismo)
    feedrate = c/tiempo;
    */
}

void sdaserial()
{ // se sustituirá por "enviargcode", usar para ver que funciona bien "guardasd"
  //Serial.println("Archivo test.txt: ");
  myFile = SD.open("test.txt");
  while (myFile.available())
  {
  //  Serial.write(myFile.read());
  }
  myFile.close();
  //Serial.println("-----------");
}

void borra()
{ // Seica si abres el archivo en modo escritura y lo cierras sin decir nada, se borra el contenido.
  SD.remove("test.txt");
  tft.fillRect(0, BOXSIZEY, tft.width(), tft.height() - BOXSIZEY, BLACK);
}

void setHome()
{
  //Serial.println("Set Home");

  sendToGrbl("G90"); // absolute coordinates
  sendToGrbl("G21");
  currentPos = {screenXSize[0], screenYSize[1]};
  double x = mapX(currentPos.x);
  double y = mapY(currentPos.y);
  generateTranslateGcode("G92", x, y, 0.0); // set zero
  sendToGrbl(lineBuffer);
}

void sdagrbl()
{
  char lineBuffer[50];
  int lineBufferLength = 0;
  // enviar G90, G21 y G92 X0 Y0
  myFile = SD.open("test.txt");
  while (myFile.available())
  {
    char c = myFile.read();
    if (c == '\n' || c == '\r')
    {
      lineBuffer[lineBufferLength] = '\0';
      processLine(lineBuffer);
      lineBufferLength = 0;
    }
    else
    {
      lineBuffer[lineBufferLength++] = c;
    }
  }
  lineBuffer[lineBufferLength] = '\0';
  processLine(lineBuffer);
  myFile.close();
}

void loop()
{
  uint16_t xpos, ypos; //screen coordinates
  tp = ts.getPoint();  //tp.x, tp.y are ADC values

  // if sharing pins, you'll need to fix the directions of the touchscreen pins
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  pinMode(XP, OUTPUT);
  pinMode(YM, OUTPUT);
  //    digitalWrite(XM, HIGH);
  //    digitalWrite(YP, HIGH);
  // we have some minimum pressure we consider 'valid'
  // pressure of 0 means no pressing!

  if (tp.z > MINPRESSURE && tp.z < MAXPRESSURE && (millis() > currentPosTime + MIN_POINT_INTERVAL))
  {

    // Serial.print("X = "); Serial.print(tp.x);
    // Serial.print("\tY = "); Serial.print(tp.y);
    // Serial.print("\tPressure = "); Serial.println(tp.z);
    // is controller wired for Landscape ? or are we oriented in Landscape?
    if (SwapXY != (Orientation & 1))
      SWAP(tp.x, tp.y);
    // scale from 0->1023 to tft.width  i.e. left = 0, rt = width
    // most mcufriend have touch (with icons) that extends below the TFT
    // screens without icons need to reserve a space for "erase"
    // scale the ADC values from ts.getPoint() to screen values e.g. 0-239

    xpos = map(tp.x, TS_LEFT, TS_RT, 0, tft.width());
    ypos = map(tp.y, TS_TOP, TS_BOT, 0, tft.height());
    if (Orientation == 3)
    {
      xpos = tft.width() - xpos;
      ypos = tft.height() - ypos;
    }

    // Si pos < altura botones comprueba pos horizontal para determinar boton en concreto
    if ((ypos < BOXSIZEY) && (ypos >= 0))
    {
      if (xpos < BOXSIZE)
      {
      //   Serial.println("H");
        setHome();
        return;
      }
      else if (xpos < BOXSIZE * 2)
      {
        borra();
       //  Serial.println("X");
        return;
      }
      else if (xpos < BOXSIZE * 3)
      {
        sdaserial();
      //   Serial.println("S");
        return;
      }
      else if (xpos < BOXSIZE * 4)
      {
       // Serial.println("->");
        sdagrbl();
        return;
      }
    }
    // Si pos > altura botones, dibuja línea
    if (ypos > 0 && ((ypos - PENRADIUS) > BOXSIZEY) && ((ypos + PENRADIUS) < (uint16_t)tft.height()))
    {
      //tft.fillCircle(xpos, ypos, PENRADIUS, WHITE);
      tft.drawLine(currentPos.x, currentPos.y, xpos, ypos, WHITE);
      currentPos.x = xpos;
      currentPos.y = ypos;
      currentPosTime = millis();
      //pantallaserial();     // escribe en serialmonitor
      guardasd(); // escribe en SD
    }
  }
}
