#include "HackPublisher.h"
#include "Wifi.h"
#include "AM232X.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP32Servo.h>
#include <Adafruit_NeoPixel.h>
#include <string>
#include <iostream>

Servo probe;
AM232X AM2320;

/*------------Pinout Declaration--------------*/
const int agas = A2;
const int trigPin = 5;
const int echoPin = 18;
const int button1 = 12;
const int button2 = 32;
const int servo = 13;
const int rled = 14;
const int buzzer = 15;

/*------------WIFI INFO--------------*/
const char* ssid = "ASUS-F8";
const char* password = "K33pi7$@f3%";

/*------------OLED INFO-------------*/
#define SCREEN_WIDTH 128 // OLED display width,  in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

/*------------Publisher Setup--------------*/
HackPublisher publisher("Engineering League");  // publisher instance for team "Engineering League"

/*------------Misc Variables------------*/ 
float temp = 0;  // variable that holds the temperature
float hazgas = 0;
float humid = 0; 
float duration, distance;
int oldState, newState;
int oldState2;
int newState2;
int mode1 = 0; 
int mode2 = 0;
int pos = 0;

/*------------Ring LED Declarations---------*/
#define NUMPIXELS 12
#define DELAYVAL 500
Adafruit_NeoPixel pixels(NUMPIXELS, rled, NEO_GRB + NEO_KHZ800);


void setup() {
/*--------------------------------Pin Setup-----------------------------------------*/
  pinMode(agas, INPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(button1, INPUT_PULLUP);
  probe.attach(servo);
  pinMode(button2, INPUT_PULLUP);
  newState = digitalRead(button1);
  pinMode(buzzer, OUTPUT);


/*--------------------------------Initialize serial communication-----------------------------------------*/
  Serial.begin(115200);
  while (!Serial) continue;

/*--------------------------------Connect to WiFi and display local IP Addy--------------------------------*/

    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

/*--------------------------------Initialize publisher----------------------------------------------------*/
    publisher.begin();

/*--------------------------------Temp/Humidity Sensor Setup----------------------------------------------*/
    Serial.println("---------------------------------------------------");
    if (! AM2320.begin() )
    {
    Serial.println("Sensor not found");
    while (1);
    }
    AM2320.wakeUp();
    delay(50);

    Serial.println("Type,\tStatus,\tHumidity (%),\tTemperature (C)");
    
 /*--------------------------------OLED SETUP------------------------------------------*/
 // initialize OLED display with address 0x3C for 128x64
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(100);
  display.clearDisplay();

/*-----------------------------OLED Info---------------------------------*/
  display.setTextSize(1);
  display.setTextColor(WHITE);

/*----------------------------Ring LED Setup------------------------------*/
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif

  pixels.begin();
  pixels.show();
  newState2 = digitalRead(button2);
 }

void loop() {
// TEMP HUMID SENSOR----------------------------------------
  //READ DATA
    Serial.print("AM2320, \t");
    int status = AM2320.read();
    switch (status)
    {
    case AM232X_OK:
      Serial.print("OK,\t");
      break;
    default:
      Serial.print(status);
      Serial.print("\t");
      break;
    }
    // DISPLAY DATA, sensor only returns one decimal.
    Serial.print(AM2320.getHumidity(), 1);
    Serial.print(",\t");
    Serial.println(AM2320.getTemperature(), 1);
    temp = AM2320.getTemperature();
    humid = AM2320.getHumidity();
    delay(50);

//HAZ GAS DETECTOR
    float hazgas = analogRead(agas);
    Serial.println("---------------------------------------------------");
    Serial.println(hazgas);
    delay(50);

//ULTRASONIC 
  // Clears the trigPin
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10); 
    digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
    duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
    distance = 34300.0 * (duration / 1000000.0) / 2.0; 
  // Prints the distance on the Serial Monitor
    Serial.println("---------------------------------------------------");
    Serial.print("distance: ");
    Serial.print(distance);
    Serial.println(" cm");
    delay(50);

//Servo
oldState = newState;
  newState = digitalRead(button1);
  
 if((newState == LOW) && (oldState == HIGH)) {
  delay(5);
   newState = digitalRead(button1);
    
   if(newState == LOW){
    Serial.println("button1 has been pressed");
    if(++mode1 > 1) mode1 = 0;
     switch(mode1){
      case 0:
        {for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    probe.write(pos);    // tell servo to go to position in variable 'pos'
    delay(15);             // waits 15ms for the servo to reach the position
          }
          break;
        }
        case 1:
        {
  for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
    probe.write(pos);    // tell servo to go to position in variable 'pos'
    delay(15);             // waits 15ms for the servo to reach the position
  }
        }
     }
   }
 }

//Ring LED
 pixels.clear();
  oldState2 = newState2;
  newState2 = digitalRead(button2);
  
 if((newState2 == LOW) && (oldState2 == HIGH)) {
    // Short delay to debounce button.
    delay(5);
    // Check if button is still low after debounce.
    newState2 = digitalRead(button2);
    Serial.println("button2 has been pressed");
    if(newState2 == LOW) {      // Yes, still low
      if(++mode2 > 1) mode2 = 0; // Advance to next mode, wrap around after #8
      switch(mode2) {           // Start the new animation...
        case 0:
          colorWipe(pixels.Color(  0,   0,   0), 10);    // Black/off
          break;
        case 1:
          colorWipe(pixels.Color(255,   255,   255), 10);    // white
          break;
 }
    }
 }
//OLED Display and buzzer
  display.setCursor(0, 0);
  // Display static text
  display.clearDisplay();
  display.print("Temp:");
  display.print(temp);
  display.print((char)248);
  display.println("C");
  display.print("Humidity:");
  display.print(humid);
  display.println((char)37);
  display.print("Distance:");
  display.print(distance);
  display.println("cm");
  display.print("Haz Gas:");
  display.println(hazgas);
  if(hazgas<=1000)
  {display.println();
  display.println("Air Quality OK");
  }
  else if(hazgas>1000 && hazgas <=3000)
  {display.println();
  display.println("Air Quality SUS");
  tone(buzzer, 1000);
  delay(20);
  noTone(buzzer);
  tone(buzzer, 1000);
  delay(20);
  noTone(buzzer);
  }
   else if(hazgas>3000)
  {display.println();
  display.println("Air Quality ALERT");
  tone(buzzer, 1000);
  delay(500);
  noTone(buzzer);
  tone(buzzer, 1000);
  delay(20);
  noTone(buzzer);
  }   

  if(temp>32 || humid<45 )
  {display.println();
  display.println("ALERT: FIRE SUSPECTED");
    tone(buzzer, 1000);
  delay(500);
  noTone(buzzer);
  tone(buzzer, 1000);
  delay(20);
  noTone(buzzer);
  }
  else if(temp<=32&&temp>28||humid>=45&&humid<=55)
  {display.println();
  display.println("Fire Conditions");
    tone(buzzer, 1000);
  delay(20);
  noTone(buzzer);
  tone(buzzer, 1000);
  delay(20);
  noTone(buzzer);
  }
   else if(temp<=28||humid>55)
  {display.println();
  display.println("Ideal Conditions");
  }    
  display.display(); 
  
//PUBLISHER CODE--------------------------------------------------
    publisher.store("ultrasonic", distance); // store value for ultrasonic sensor
    publisher.store("temp", temp);        // store value for temp
    publisher.store("humidity", humid);        // store value for humidity
    publisher.store("gasdetect", hazgas);   // store value for haz gas detect

    publisher.send();                     // send stored data to website

    delay(100);
    temp++;


}

void colorWipe(uint32_t color, int wait) {
  for(int i=0; i<pixels.numPixels(); i++) { // For each pixel in strip...
    pixels.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    pixels.show();                          //  Update strip to match
    delay(wait);                           //  Pause for a moment
  }
}
