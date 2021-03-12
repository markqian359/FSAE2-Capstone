//-------------------Include Libraries--------------------------------
#include "HX711.h" // includes the load cell amplifier library
#define DOUT  3 // defines the output pin
#define CLK  2 // defines the input pin
#include <Wire.h>
#include <Adafruit_ADS1015.h>
HX711 scale(DOUT, CLK); // defines the scale readings as output & input
float calibration_factor = -7050; // defines the calibration factor (needs changing ocassionally)
//---------------------------------------------------------------------

// ------------------Temperature Sensor--------------------------------
int ThermistorPin = 0;
int Vo;
float R1 = 10000;
float logR2, R2, T, temperature;
float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;

void setup() {
Serial.begin(9600);
}

void loop() {

  Vo = analogRead(ThermistorPin);
  R2 = R1 * (1023.0 / (float)Vo - 1.0);
  logR2 = log(R2);
  T = (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2));
  temperature = T - 273.15;
  //T = (T * 9.0)/ 5.0 + 32.0; 

  Serial.print("Recorded Damper Dynamometer Temperature: "); 
  Serial.print(temperature);
  Serial.println(" C"); 

  delay(1000);//Take samples every one second
}
//---------------------------------------------------------------------------


//----------------------------Force Sensor-----------------------------------
#include "HX711.h"

#define calibration_factor -10000.0 //This value is obtained using the SparkFun_HX711_Calibration sketch

#define LOADCELL_DOUT_PIN  3
#define LOADCELL_SCK_PIN  2

//Calibrating scale
HX711 scale;

float calibration_factor = -7050; //-7050 worked for my 440lb max scale setup

void setup() {
  Serial.begin(9600);
  Serial.println("HX711 calibration sketch");
  Serial.println("Remove all weight from scale");
  Serial.println("After readings begin, place known weight on scale");
  Serial.println("Press + or a to increase calibration factor");
  Serial.println("Press - or z to decrease calibration factor");
  
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale();
  scale.tare(); //Reset the scale to 0
  
  long zero_factor = scale.read_average(); //Get a baseline reading
  Serial.print("Zero factor: "); //This can be used to remove the need to tare the scale. Useful in permanent scale projects.
  Serial.println(zero_factor);
}

void loop() {

  scale.set_scale(calibration_factor); //Adjust to this calibration factor
  Serial.print("Reading: ");
  Serial.print(scale.get_units(), 1);
  Serial.print(" kg");
  Serial.print("calibration_factor: ");
  Serial.print(calibration_factor);
  Serial.println();

  if(Serial.available())
  {
    char temp = Serial.read();
    if(temp == '+' || temp == 'a')
      calibration_factor += 10;
    else if(temp == '-' || temp == 'z')
      calibration_factor -= 10;
  }
}

//Record force output
void setup() {
  Serial.begin(9600);
  Serial.println("HX711 scale demo");

  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(calibration_factor);
  scale.tare();

  Serial.println("Readings:");
}

void loop() {
  Serial.print("Reading: ");
  Serial.print(scale.get_units(), 1); //scale.get_units() returns a float
  Serial.print(" kg"); 
  Serial.println("Force Output");
  force = scale.get_units()*9.81
  Serial.print(force);
  Serial.println(" N");
}
//--------------------------------------------------------------------------------------------

// ------------------------------Displacement Sensor------------------------------------------
Adafruit_ADS1115 ads;  
 
int cin1; //for reading the instructions from serial
int samplingfreq; //sampling frequency [milliseconds]
float distance = 0; //for the calibrated distance
float adcRead; //for the raw data
 
void setup(void)
{
  Serial.begin(115200);
 
  //some info to know what is on the Arduino
  Serial.println("KPM12-100mm Displacement sensor, Button.");
  Serial.println("Getting differential reading from AIN0 (P) and AIN1 (N)");
  Serial.println("ADC Range: +/- 6.144V (1 bit = 3mV/ADS1015, 0.1875mV/ADS1115)");
 
  // Adjust he ADC input range
  //                                                                ADS1015  ADS1115
  //                                                                -------  -------
  // ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
  // ads.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
  // ads.setGain(GAIN_TWO);        // 2x gain   +/- 2.048V  1 bit = 1mV      0.0625mV
  // ads.setGain(GAIN_FOUR);       // 4x gain   +/- 1.024V  1 bit = 0.5mV    0.03125mV
  // ads.setGain(GAIN_EIGHT);      // 8x gain   +/- 0.512V  1 bit = 0.25mV   0.015625mV
  // ads.setGain(GAIN_SIXTEEN);    // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV
 
 
  //----Set default------
  ads.setSPS(DR_920SPS); //Sampling speed
 
  ads.setGain(GAIN_TWOTHIRDS); // Gain (set to max now)
  ads.begin();
}
 

void loop(void)
{
 
  double startTime = millis(); // Store current time as start time
 
  if (Serial.available() > 0) {
 
    char cin = Serial.read(); // read character
 
    switch (cin) // Our usual SWITCH-CASE method
    {
      // Changing SPS settings
      case 'd':
        delay(50);
        while (!Serial.available());
 
        SPSifchecker(cin1); // Change the SPS (sampling frequency) by sending and integer between 0-7 (Check the function below)
        // Example: the message "d 2" sets the SPS to 2 which is: 490 samples per second
        break;
      // Changing PGA settings
      case 'w':
        delay(50);
        while (!Serial.available());
 
        PGAifchecker(cin1); //we change the PGA (onboard amplifier) by sending an integer between 0-6 (Check the function below)
        //Example: the message "w 2" sets PGA to 2 which is: 2x gain   +/- 2.048V  1 bit = 1mV      0.0625mV
        break;
      // Reading data and sending to PC
      case 's': //s: Start
 
        while (Serial.read() != 'N') { //While we don't interrupt it by sending 'N' to the Arduino
         
          double elapsedTime = millis() - startTime; //start another timer
          adcRead = ads.readADC_Differential_0_1(); //read the ADC 0 and 1 pins in differential mode
          distance = (-0.0009353088486627 * adcRead + 27.36432542127); //calibrated displacement
          
          //Serial printout
          Serial.print(elapsedTime); //elapsed time since this part of the code is running
          Serial.print("\t"); //tab for separation
          Serial.print(adcRead, 0); //raw data, 0 decimals
          Serial.print("\t"); //tab for separation
          Serial.print(distance, 6); //calibrated data (displacement), 6 decimals
          Serial.print("\n"); //linebreak
 
          delay(200); //wait 200 ms
        }
        break;
      //
      //Single Readout to PC
      case 'r': //Read. Just a one shot reading.
 
        adcRead = ads.readADC_Differential_0_1();  //read the ADC 0 and 1 pins in differential mode
        distance = (-0.0009353088486627 * adcRead + 27.36432542127); //calibrated displacement
        //Serial printout
        Serial.print(adcRead, 0); // raw data, 0 decimals
        Serial.print("\t"); //tab for separation
        Serial.print(distance, 6); //calibrated data (displacement), 6 decimals
        Serial.print("\n"); //linebreak
        delay(3000); //wait 3 seconds
        
       //Sampling Frequency
      case 'l': //
        samplingfreq = Serial.parseInt();
 
        while (Serial.read() != 'N') { //While we don't interrupt it by sending 'N' to the Arduino    
         
          double elapsedTime = millis() - startTime; //start another timer
          adcRead = ads.readADC_Differential_0_1(); //read the ADC 0 and 1 pins in differential mode
          distance = (-0.0009353088486627 * adcRead + 27.36432542127); //calibrated displacement
          //Serial printout
          Serial.print(elapsedTime); //elapsed time since this part of the code is running
          Serial.print("\t"); //tab for separation
          Serial.print(adcRead, 0); // raw data, 0 decimals
          Serial.print("\t"); //tab for separation
          Serial.print(distance, 6); //calibrated data (displacement), 6 decimals
          Serial.print("\n"); //linebreak
          delay(samplingfreq); //we have to subtract the code's running time, to get the exact frequency.
        }
      default:
        
        break;
    }
  }
 
 
}
 
void PGAifchecker(int cin1)
{
  //the commented parts are for testing, but I left them in the code
  cin1 = Serial.parseInt();
  //Serial.println(cin1);
  //Serial.println("first cin");
  delay(50);
  //Serial.println("after delay");
 
  if (cin1 < 1 )
  {
    ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default) //1
    //Serial.println("test");
    //Serial.println("PGA-0");
  }
  else if (cin1 < 2)
  {
    ads.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV // 1,5
    //Serial.println("PGA-1");
  }
  else if (cin1 < 3)
  {
    ads.setGain(GAIN_TWO);        // 2x gain   +/- 2.048V  1 bit = 1mV      0.0625mV // 3
    //Serial.println("PGA-2");
  }
  else if (cin1 < 4)
  {
    ads.setGain(GAIN_FOUR);       // 4x gain   +/- 1.024V  1 bit = 0.5mV    0.03125mV // 6
    //Serial.println("PGA-3");
  }
  else if (cin1 < 5)
  {
    ads.setGain(GAIN_EIGHT);      // 8x gain   +/- 0.512V  1 bit = 0.25mV   0.015625mV //12
    //Serial.println("PGA-4");
  }
  else if (cin1 < 6)
  {
    ads.setGain(GAIN_SIXTEEN);    // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV //24
    //Serial.println("PGA-5");
  }
 
  //Serial.println("The value of the PGA now is:");
  //Serial.println(cin1);
 
}
 
void SPSifchecker(int cin1)
{
  //the commented parts are for testing, but I left them in the code
  cin1 = Serial.parseInt();
  //Serial.println(cin1);
  //Serial.println("first cin");
  delay(50);
  //Serial.println("after delay");
 
  // try it with == 1, without ' ' or " "
  if (cin1 < 1)
  {
    ads.setSPS(DR_128SPS);        // 128 samples per second
    //Serial.println("128 SPS");
  }
  else if (cin1 < 2)
  {
    ads.setSPS(DR_250SPS);        // 250 samples per second
  }
  else if (cin1 < 3)
  {
    ads.setSPS(DR_490SPS);        /// 490 samples per second
  }
  else if (cin1 < 4)
  {
    ads.setSPS(DR_920SPS);       // 920 samples per second
  }
  else if (cin1 < 5)
  {
    ads.setSPS(DR_1600SPS);      // 1600 samples per second (default)
  }
  else if (cin1 < 6)
  {
    ads.setSPS(DR_2400SPS);    // 2400 samples per second
    //Serial.println("2400 SPS");
  }
  else if (cin1 < 7)
  {
    ads.setSPS(DR_3300SPS);    // 3300 samples per second
  }
 
  //Serial.println("The value of the PGA now is:");
  //Serial.println(cin1);
}
//----------------------------------------------------------------------------

// ---------------------PLX-DAQ Data Logging----------------------------------
unsigned long int milli_time;    //variable to hold the time
float voltage;         //variable to hold the voltage form A0
void setup() {
  Serial.begin(128000);               //Fastest baudrate
  Serial.println("CLEARDATA");        //This string is defined as a 
                                      // commmand for the Excel VBA 
                                      // to clear all the rows and columns
  Serial.println("Recorded Time,Force(N),Displacement(mm),Temperature(C)");  
                                      //LABEL command creates label for 
                                      // columns in the first row with bold font
}
void loop() {
  milli_time = millis();
  voltage = 5.0 * analogRead(A0) / 1024.0;
  Serial.print("Time");
  Serial.print(milli_time);
  Serial.print(",");
  Serial.print(force);
  Serial.print(,);
  Serial.print(distance);
  Serial.print(,);
  Serial.print(temperature);   
  delay(1000);                    //Take samples every one second
}
//---------------------------------------------------------------------------------
