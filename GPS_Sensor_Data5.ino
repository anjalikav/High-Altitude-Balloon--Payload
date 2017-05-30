 //all of the Libraries and variables are initialized
#include <Time.h>
#include <HMC5883L.h>

#include <SoftwareSerial.h>
#include <TinyGPS.h>
#include <Wire.h>
#include <math.h>
#include <SPI.h>
#include <SD.h>

#define RXPIN 2
#define TXPIN 3
#define address 0x1E //0011110b, I2C 7bit address of HMC5883
#define PI 3.14159265358979323846
#define GPSBAUD 4800

HMC5883L compass;

TinyGPS gps;

SoftwareSerial SerialGPS = SoftwareSerial(2, 3); 
const int chipSelect = 8;
const int offset = -5;   // Central Daylight Time


void setup()
{
  Serial.begin(9600);
  SerialGPS.begin(4800);
  delay(3000);

  compass = HMC5883L();
  Wire.begin();
  //Put the HMC5883 IC into the correct operating mode
  Wire.beginTransmission(address); //open communication with HMC5883
    Wire.write(0x02); //select mode register
  Wire.write(0x00); //continuous measurement mode
  Wire.endTransmission();
  Serial.println("Setting scale to +/- 0.88Ga");
  int error = compass.SetScale(0.88);
  if(error != 0){
    Serial.println(compass.GetErrorText(error));
    }
    
  Serial.println("Sensor is initialized");

  // Open serial communications and wait for port to open:
  
  Serial.println("Initializing SD card...");
  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(chipSelect, OUTPUT);
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect))
  {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");
}


void loop()
{
 while (SerialGPS.available()) {
    if (gps.encode(SerialGPS.read())) 
    {
      MagnetometerScaled scaled = compass.ReadScaledAxis(); //scaled values from compass.
      File gps_sensorFile = SD.open("datalog.txt", FILE_WRITE);  
      
       // Define the variables that will be used
      float latitude, longitude;
      // Then call this function
      gps.f_get_position(&latitude, &longitude);
      // You can now print variables latitude and longitude
      
    // when TinyGPS reports new data...
       unsigned long age;
       int Year;
       byte Month, Day, Hour, Minute, Second;
       gps.crack_datetime(&Year, &Month, &Day, &Hour, &Minute, &Second, NULL, &age);
       if (age < 500) 
       {
         // set the Time to the latest GPS reading
         setTime(Hour, Minute, Second, Day, Month, Year);
         adjustTime(offset * SECS_PER_HOUR);
       }
      Serial.println("Time is set and is not your problem.");
      
         // Prints all of the information logged in the SD Card to the Serial Monitor to track whether it is working
        Serial.print(hour());
        
        Serial.print(":");
        if(minute() < 10){
          Serial.print('0');}
        Serial.print(minute());
     
        Serial.print(":");
        if(second() < 10){
          Serial.print('0');}
        Serial.print(second());
        
        Serial.print(" ");
        Serial.print(day());
        Serial.print(" ");
        Serial.print(month());
        Serial.print(" ");
        Serial.print(year()); 
        Serial.println(); 
        
        Serial.print("Lat/Long: ");
        Serial.print(latitude, 5);
        Serial.print(", ");
        Serial.println(longitude, 5);
        
        Serial.print("x: ");
        Serial.print(scaled.XAxis);
        Serial.print("  y: ");
        Serial.print(scaled.YAxis);
        Serial.print("  z: ");
        Serial.print(scaled.ZAxis);
        Serial.println();
          
      if (gps_sensorFile) {
        
        Serial.println("SD File is open");
       
        // log time, gps and sensor data
        gps_sensorFile.print(hour());
        
        gps_sensorFile.print(":");
        if(minute() < 10){
          gps_sensorFile.print('0');}
        gps_sensorFile.print(minute());
     
        gps_sensorFile.print(":");
        if(second() < 10){
          gps_sensorFile.print('0');}
        gps_sensorFile.print(second());
        
        gps_sensorFile.print(" ");
        gps_sensorFile.print(day());
        gps_sensorFile.print(" ");
        gps_sensorFile.print(month());
        gps_sensorFile.print(" ");
        gps_sensorFile.print(year()); 
        gps_sensorFile.println(); 
        
        gps_sensorFile.print("Lat/Long: ");
        gps_sensorFile.print(latitude, 5);
        gps_sensorFile.print(", ");
        gps_sensorFile.println(longitude, 5);
        
        gps_sensorFile.print("x: ");
        gps_sensorFile.print(scaled.XAxis);
        gps_sensorFile.print("  y: ");
        gps_sensorFile.print(scaled.YAxis);
        gps_sensorFile.print("  z: ");
        gps_sensorFile.print(scaled.ZAxis);
    
        gps_sensorFile.println();
        gps_sensorFile.close();
      }
      else
      {
        Serial.println("error opening datalog.txt");
      }
     
      // Here you can print statistics on the sentences.
      unsigned long chars;
      unsigned short sentences, failed_checksum;
      gps.stats(&chars, &sentences, &failed_checksum);
      Serial.println();
      delay(3000);
      
    }
  }
}



