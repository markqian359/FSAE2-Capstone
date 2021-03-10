// LCD LIBRARY INCLUSION
#include <LiquidCrystal_I2C.h> // includes the LCD display library
LiquidCrystal_I2C myDisplay(0x27,16,2); // sets up the display as a 2x16 array of pixels

// LOAD CELL LIBRARY INCLUSION
#include "HX711.h" // includes the load cell amplifier library
#define DOUT  3 // defines the output pin
#define CLK  2 // defines the input pin
HX711 scale(DOUT, CLK); // defines the scale readings as output & input
float calibration_factor = -7050; // defines the calibration factor (needs changing ocassionally)

// SET-UP OF BUTTONS AND POTENTIOMETERS
int motor=6; // sets the motor in pin 6
int potenciometer=A3; // sets the potentiometer in analog pin 3
int start_button=10; // sets the start button in pin 10
int start_button_state; // initializes a variable for push button state

// SETUP LOOP. INCLUDE CODE TO BE RAN ONCE
void setup(){
  pinMode(10,INPUT); // defines the start button pin as an input
  pinMode(6,OUTPUT); // defines the motor pin as an output
  Serial.begin(9600); // begins serial communication at 9600 baud rate
  scale.set_scale(); // sets load scale to the settings defined previously
  scale.tare(); // zeroes out the scale
  long zero_factor = scale.read_average();
  myDisplay.init(); // initializes the LCD display                     
  myDisplay.backlight(); // turns on the backlight of the display
    
}

// MAIN LOOP. INCLUDE CODE TO BE RAN ITERATIVELY
void loop(){
  scale.set_scale(calibration_factor); // includes calibration factor for scale to measure in correct units
  int angle = analogRead(potenciometer); // reads the potentiometer to determine the angle of rotation

  // SPEED SETTING 1
  if (angle>=0 && angle<512){ // determines course of action if potentiometer is in this region
    start_button_state=digitalRead(start_button); // reads the start button state to determine if its pressed or not
    myDisplay.setCursor(0,0); // sets LCD cursor to pixel [0,0]
    myDisplay.print("Setting 1"); // displays the current setting to let the user know the speed selected
    if (start_button_state==HIGH){ // determines the course of action if the start button is pressed
      int motor_speed=70; // sets the speed of the motor
      analogWrite(motor,motor_speed); // outputs the speed of the motor through the PWM pin
      Serial.println(scale.get_units()); // prints the readings of the scale in the serial monitor
      myDisplay.setCursor(0,0); // sets the display cursor to pixel [0,0]
      myDisplay.print("Running..."); // displays the word "running" to let user know that the motor is running and the load cell is recording data
      delay(100); // introduces a delay for proper display of the word "running"
    }
    else { // determines the course of action if the start button is not pressed
      int motor_speed=0; // sets the motor speed
      analogWrite(motor,motor_speed); // outputs the selected motor speed through the PWM pin
    }
  }
  
  // SPEED SETTING 2
  else if(angle>=512 && angle<=1023){ // determines course of action if potentiometer is in this region
    start_button_state=digitalRead(start_button); // reads the start button state to determine if its pressed or not
    myDisplay.setCursor(0,0); // sets LCD cursor to pixel [0,0]
    myDisplay.print("Setting 2"); // displays the current setting to let the user know the speed selected
    if (start_button_state==HIGH){ // determines the course of action if the start button is pressed
      int motor_speed=255; // sets the speed of the motor
      analogWrite(motor,motor_speed); // outputs the speed of the motor through the PWM pin
      Serial.println(scale.get_units()); // prints the readings of the scale in the serial monitor
      myDisplay.setCursor(0,0); // sets the display cursor to pixel [0,0]
      myDisplay.print("Running..."); // displays the word "running" to let user know that the motor is running and the load cell is recording data
      delay(100); // introduces a delay for proper display of the word "running"
    }
    else { // determines the course of action if the start button is not pressed
      int motor_speed=0; // sets the motor speed
      analogWrite(motor,motor_speed); // outputs the selected motor speed through the PWM pin
    }
  }

}
  
// Displacement Sensor - To Be Changed
int sensorPin = 0; //analog pin 0

void setup(){
  Serial.begin(9600);
}

void loop(){
  int val = analogRead(sensorPin);
  Serial.println(val);

  //just to slow down the output - remove if trying to catch an object passing by
  delay(100);

}

// Thermistor
int ThermistorPin = 0;
int Vo;
float R1 = 10000;
float logR2, R2, T;
float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;

void setup() {
Serial.begin(9600);
}

void loop() {

  Vo = analogRead(ThermistorPin);
  R2 = R1 * (1023.0 / (float)Vo - 1.0);
  logR2 = log(R2);
  T = (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2));
  T = T - 273.15;
  //T = (T * 9.0)/ 5.0 + 32.0; 

  Serial.print("Temperature: "); 
  Serial.print(T);
  Serial.println(" C"); 

  delay(500);
}

// Load Cell Code - To be edited
#include "HX711.h"

#define calibration_factor -7050.0 //This value is obtained using the SparkFun_HX711_Calibration sketch

#define LOADCELL_DOUT_PIN  3
#define LOADCELL_SCK_PIN  2

HX711 scale;

void setup() {
  Serial.begin(9600);
  Serial.println("HX711 scale demo");

  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(calibration_factor); //This value is obtained by using the SparkFun_HX711_Calibration sketch
  scale.tare(); //Assuming there is no weight on the scale at start up, reset the scale to 0

  Serial.println("Readings:");
}

void loop() {
  Serial.print("Reading: ");
  Serial.print(scale.get_units(), 1); //scale.get_units() returns a float
  Serial.print(" lbs"); //You can change this to kg but you'll need to refactor the calibration_factor
  Serial.println();
}
