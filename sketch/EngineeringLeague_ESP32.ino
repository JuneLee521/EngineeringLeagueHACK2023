#include "HackPublisher.h"
#include "Wifi.h"
#include "AM232X.h"

AM232X AM2320;
/*------------Pinout Declaration--------------*/

/*------------WIFI INFO--------------*/
const char* ssid = "nambawon";
const char* password = "jlee3250811";

/*------------Publisher Setup--------------*/
HackPublisher publisher("Engineering League");  // publisher instance for team "Engineering League"
int temp = 0;  // variable that holds the temperature
int hazgas = 0;
int humid = 0; 

void setup() {
// Initialize serial communication
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
    delay(2000);

    Serial.println("Type,\tStatus,\tHumidity (%),\tTemperature (C)");
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
    delay(2000);

//PUBLISHER CODE--------------------------------------------------
    publisher.store("ultrasonic", 12.56); // store value for ultrasonic sensor
    publisher.store("temp", temp);        // store value for temp
    publisher.store("humidity", humid);        // store value for humidity
    publisher.store("gasdetect", hazgas);   // store value for haz gas detect

    publisher.send();                     // send stored data to website

    delay(2000);
    temp++;
}
