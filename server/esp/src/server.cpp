#include "Arduino.h"
#include "WiFi.h"
#include "Controller.h"
#include "Drivetrain.h"
#include "LinearActuator.h"

/* GPIO Pins */
#define SERIAL_SIZE_RX  1024

#define Rx 16
#define Tx 17

#define BR_PWM 32
#define FR_PWM 33
#define FL_PWM 27
#define BL_PWM 26

/* Define all subsystems */
Motor fl(FL_PWM);
Motor fr(FR_PWM);
Motor bl(BL_PWM);
Motor br(BR_PWM);
Drivetrain drivetrain(&fr, &fl, &br, &bl);

/* variable for testing */
double val = 0;


/* Main Loop Function */
void Decode()
{
  /* Eventually change to if a GPIO is recieving a HIGH signal */
  while (Serial2.available()) {

    char s = '\0';
    char e = '\0';
        //  1       2       3      4
    short frontL, frontR, backL, backR;
    char isZero;

    // Read all values over UArt
    //we can only read 8 bits at a time, so we have to split the shorts up
    while (s != 's') { s = Serial2.read(); }
    frontL = Serial2.read();
    frontL |= (short) Serial2.read() << 8;
    frontR = Serial2.read();
    frontR |= (short) Serial2.read() << 8;
    backL = Serial2.read();
    backL |= (short) Serial2.read() << 8;
    backR = Serial2.read();
    backR |= (short) Serial2.read() << 8;
    isZero = Serial2.read();
    while (e != 'e') { e = Serial2.read(); }

    // Print all values
    Serial.print("frontL = ");
    Serial.print(frontL, DEC);
    Serial.print(" frontR = ");
    Serial.print(frontR, DEC);
    Serial.print(" backL = ");
    Serial.print(backL, DEC);
    Serial.print(" backR = ");
    Serial.print(backR, DEC);
    Serial.print(" AllZero = ");
    Serial.println(isZero, DEC);

    // disregard all values if special 'haschanged' value isnt 255.
    // This is to mitigate random values that occur
    if(isZero & 0xff) {
      drivetrain.drive(1500, 1500, 1500, 1500);

      return;
    }

    // Ignore completely random values
    if(frontL < 1000 || frontL > 2000) { frontL = 1500;}
    if(frontR < 1000 || frontR > 2000) { frontR = 1500;}
    if(backL < 1000 || backL > 2000) { backL = 1500;}
    if(backR < 1000 || backR > 2000) { backR = 1500;}
    
    drivetrain.drive(frontL, frontR, backL, backR);
  }
}

void setup()
{
  /* Set serial output to 115200 baud */
  Serial.begin(115200);
  //Serial2.setRxBufferSize(SERIAL_SIZE_RX);
  Serial2.begin(115200, SERIAL_8N1, Rx, Tx);
  while (!Serial) ;
  while (!Serial2) ; // wait on ports

  // Set everything to 0
  drivetrain.drive(1500, 1500, 1500, 1500);

} 

void loop() 
{
  Decode();
  delay(100);
}
