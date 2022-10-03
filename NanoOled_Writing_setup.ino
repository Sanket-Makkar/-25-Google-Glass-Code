#include <SPI.h>
#include <Wire.h>
#include <dht.h>
dht DHT;
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define NUMFLAKES     10 // Number of snowflakes in the animation example

#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16

//WaterLevelPins: A3 -- SensorPin
//                8 -- Power
//                GND -- Ground
#define sensorPower 8
#define sensorPin A3

int textS = 2;
//DHTPins: A0 -- SensorPin
//         9 -- Power
//         GND -- Ground
#define DHT11_PIN A0
#define DHTPower 9


//UltraSonicPins: 10 --Power
//                11 -- Trig
//                12 -- Echo
//                GND -- Ground
#define USPower 10
#define trigPin 11
#define echoPin 12

//Various values
int val = 0;
int level = 0;
//mode switch
int mode1 = 1;
int activate = 0;


//ultrasnoic sensor
// defines variables
long duration; // variable for the duration of sound wave travel
int distance; // variable for the distance measurement

//level adjuster
int levelAdj = 0;

void setup() {
  display.clearDisplay();
  Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { //Ive changed the address //already chill
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }

  display.display();

  // Clear the buffer
  display.clearDisplay();
  pinMode(sensorPower, OUTPUT);
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(echoPin, INPUT);
  pinMode(9, OUTPUT);
  digitalWrite(9, HIGH);
  // Set to LOW so no power flows through the sensor
  digitalWrite(sensorPower, LOW);

  //adjust swipe sensitivity based on readings when initialized
  int templev = readSwipeSensor();
  levelAdj = templev;


  pinMode(DHTPower, OUTPUT);
  pinMode(USPower, OUTPUT);
  digitalWrite(DHTPower, HIGH);
  digitalWrite(USPower, HIGH);
  testdrawroundrect();
  testfillroundrect();
}

int temp1 = 0;
int humid1 = 0;
int distance1 = 0;

void loop() {
  digitalWrite(trigPin, LOW);
  Serial.println(readSwipeSensor());
  //setup DHT measurement
  int chk = DHT.read11(DHT11_PIN);
  //get the reading from the function below and print it
  level = readSwipeSensor();

  if (((levelAdj + 3) > level) && ((levelAdj - 3 < level)) || (levelAdj == level)) {
    mode1 = mode1;
  }
  else {
    if (activate == 0) {
      if (mode1 < 3) {
        if (mode1 == 1){
          testdrawtriangle();
          testfilltriangle();
        }
        if (mode1 == 2){
          display.clearDisplay();
          testdrawline();
        }
        mode1 += 1;
      }
      else {
        mode1 = 1;
        testdrawcircle();
        testfillcircle();
      }
      activate = 1;
    }
    else {
      activate = 0;
    }
  }

  if (mode1 == 1 && DHT.humidity < 100 && DHT.humidity > -1) {
    //testscrolltext();    // Draw scrolling text
    display.clearDisplay();
    display.setTextSize(textS); // Draw 2X-scale text
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.print(("Fog"));
    if (humid1 < 3){
      humid1++;
    }
    else{
      humid1 = 0;
    }
    for (int number = 0; number < humid1; number++){
      display.print(("."));
    }
    display.println((""));
    display.print((DHT.humidity));
    display.println((" %"));
    display.display();      // Show initial text
    temp1 = 0;
    distance1 = 0;
    
  }
  else if (mode1 == 2 && DHT.temperature < 100 && DHT.temperature > -100) {
    //testscrolltext();    // Draw scrolling text
    display.clearDisplay();
    display.setTextSize(textS); // Draw 2X-scale text
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.print(("Thermal"));
    if (temp1 < 3){
      temp1++;
    }
    else{
      temp1 = 0;
    }
    for (int number = 0; number < temp1; number++){
      display.print(("."));
    }
    display.println((""));
    display.print((((DHT.temperature - 3.55) * 1.8) + 32));
    display.println((" *F"));
    display.display();      // Show initial text
    humid1 = 0;
    distance1 = 0;
  }
  else if (mode1 == 3) {
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    distance = duration * 0.034 / 2; 
    //testscrolltext();    // Draw scrolling text
    display.clearDisplay();
    display.setTextSize(textS); // Draw 2X-scale text
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.print(("Stretch"));
    if (distance1 < 3){
      distance1++;
    }
    else{
      distance1 = 0;
    }
    for (int number = 0; number < distance1; number++){
      display.print(("."));
    }
    display.println((""));
    display.print((distance));
    display.println((" cm"));
    display.display();      // Show initial text
    temp1 = 0;
    humid1 = 0;
  }
  delayMethod(1);
}

void testscrolltext(void) {
  display.clearDisplay();

  display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 0);
  display.println(F("scroll"));
  display.display();      // Show initial text
  delay(100);

  // Scroll in various directions, pausing in-between:
  display.startscrollright(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  delay(1000);
  display.startscrollleft(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  delay(1000);
  display.startscrolldiagright(0x00, 0x07);
  delay(2000);
  display.startscrolldiagleft(0x00, 0x07);
  delay(2000);
  display.stopscroll();
  delay(1000);
}

//This is a function used to get the reading
int readSwipeSensor() {
  digitalWrite(sensorPower, HIGH);  // Turn the sensor ON
  delay(10);              // wait 10 milliseconds
  val = analogRead(sensorPin);    // Read the analog value form sensor
  digitalWrite(sensorPower, LOW);   // Turn the sensor OFF
  return val;             // send current reading
}

void delayMethod(int times) {
  for (int i = 0; i < times; i++) {
    level = readSwipeSensor();
    delay(10);
  }
}

void testdrawroundrect(void) {
  display.clearDisplay();

  for(int16_t i=0; i<display.height()/2-2; i+=2) {
    display.drawRoundRect(i, i, display.width()-2*i, display.height()-2*i,
      display.height()/4, SSD1306_WHITE);
    display.display();
    delay(1);
  }

  delay(20);
}

void testfillroundrect(void) {
  display.clearDisplay();

  for(int16_t i=0; i<display.height()/2-2; i+=2) {
    // The INVERSE color is used so round-rects alternate white/black
    display.fillRoundRect(i, i, display.width()-2*i, display.height()-2*i,
      display.height()/4, SSD1306_INVERSE);
    display.display();
    delay(1);
  }

  delay(20);
}

void testdrawtriangle(void) {
  display.clearDisplay();

  for(int16_t i=0; i<max(display.width(),display.height())/2; i+=5) {
    display.drawTriangle(
      display.width()/2  , display.height()/2-i,
      display.width()/2-i, display.height()/2+i,
      display.width()/2+i, display.height()/2+i, SSD1306_WHITE);
    display.display();
    delay(1);
  }

  delay(20);
}

void testfilltriangle(void) {
  display.clearDisplay();

  for(int16_t i=max(display.width(),display.height())/2; i>0; i-=5) {
    // The INVERSE color is used so triangles alternate white/black
    display.fillTriangle(
      display.width()/2  , display.height()/2-i,
      display.width()/2-i, display.height()/2+i,
      display.width()/2+i, display.height()/2+i, SSD1306_INVERSE);
    display.display();
    delay(1);
  }

  delay(20);
}
void testdrawcircle(void) {
  display.clearDisplay();

  for(int16_t i=0; i<max(display.width(),display.height())/2; i+=2) {
    display.drawCircle(display.width()/2, display.height()/2, i, SSD1306_WHITE);
    display.display();
    delay(1);
  }

  delay(20);
}

void testfillcircle(void) {
  display.clearDisplay();

  for(int16_t i=max(display.width(),display.height())/2; i>0; i-=3) {
    // The INVERSE color is used so circles alternate white/black
    display.fillCircle(display.width() / 2, display.height() / 2, i, SSD1306_INVERSE);
    display.display(); // Update screen with each newly-drawn circle
    delay(1);
  }

  delay(20);
}
void testdrawline() {
  int16_t i;
  for(i=0; i<display.height(); i+=4) {
    display.drawLine(display.width()-1, 0, 0, i, SSD1306_WHITE);
    display.display();
    delay(0.01);
  }
  for(i=0; i<display.width(); i+=4) {
    display.drawLine(display.width()-1, 0, i, display.height()-1, SSD1306_WHITE);
    display.display();
    delay(0.01);
  }

  delay(20); // Pause for 2 seconds
}
