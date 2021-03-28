#include "include/Functions.h"
#include "include/TMC2300.h"
#include "include/CRC.h"

// Important these are the I/O pin values not the physical pin values
#define LED_STATUS 18 // IOT Kit is on IO18
#define MOTOR_EN 32 // IOT Kit is IO32
#define STANDBY 5 // IOT Kit is IO5
#define MOTOR_STEP 21
#define MOTOR_DIR 22

uint8_t result;

void printByte(int32_t var) {
  for (int32_t test = 0x8000; test; test >>= 1) {
    Serial.write(var  & test ? '1' : '0');
  }
  Serial.println();
}

void tmc2300_current(int currentInt)   
{    
  Serial.print("New MaxCurrent set: ");    
  Serial.println(currentInt);    
  uint32_t value = 1 << TMC2300_IHOLDDELAY_SHIFT    
                 | ((currentInt << TMC2300_IRUN_SHIFT) & TMC2300_IRUN_MASK)    
                 | 8 << TMC2300_IHOLD_SHIFT;    
  printByte(value);
  tmc2300_writeInt(TMC2300_IHOLD_IRUN, value);  

  Serial.print("IFCNT expect 8?");
  result = tmc2300_readInt(TMC2300_IFCNT);
  printByte(result);
}    

void setup() {
// Debug console
  Serial.begin(115200);

  Serial.println("Configure the pins");
  
  pinMode(LED_STATUS, OUTPUT);
  pinMode(MOTOR_EN, OUTPUT);
  pinMode(STANDBY, OUTPUT);
  pinMode(MOTOR_STEP,OUTPUT);    
  pinMode(MOTOR_DIR,OUTPUT);    

// Disable the motor
  digitalWrite(MOTOR_EN, LOW);   
  digitalWrite(MOTOR_STEP,LOW);  
  digitalWrite(MOTOR_DIR,LOW);    

// Reset the chip
  digitalWrite(STANDBY, HIGH);  
  delay(200);

  Serial.println("Enable the driver chip");
  digitalWrite(STANDBY, LOW);   

  Serial.println("Initialisating comm port");

// TMC2300 IC UART connection
  Serial1.begin(115200);
  delay(1000);

  Serial.println("Calculating CRC table");
// Initialize CRC calculation for TMC2300 UART datagrams
  tmc_fillCRC8Table(0x07, true, 0);

  Serial.println("Sending initial values");
  tmc2300_writeInt(TMC2300_CHOPCONF, 0x13008001);  // Re-write the CHOPCONF back to the default
  tmc2300_current(10);

  Serial.println("Initialisation Complete");

  Serial.print("GCONF:");
    result = tmc2300_readInt(TMC2300_GCONF);
    printByte(result);
    
    Serial.print("IOIN:");
    result = tmc2300_readInt(TMC2300_IOIN);
    printByte(result);

    Serial.print("GSTAT:");
    result = tmc2300_readInt(TMC2300_GSTAT);
    printByte(result);

  Serial.println("Enable the motor");
  tmc2300_writeInt(TMC2300_VACTUAL, 25000);
  digitalWrite(MOTOR_EN, HIGH);   

}

void loop() {
    // Toggle the status LED while the motor is active
    digitalWrite(LED_STATUS, HIGH);
    delay(250);
    digitalWrite(LED_STATUS, LOW);
    delay(250);
    digitalWrite(LED_STATUS, HIGH);
    delay(250);
    digitalWrite(LED_STATUS, LOW);

    Serial.println("Forward");
    tmc2300_writeInt(TMC2300_VACTUAL, 25000);
    delay(5000);

    Serial.println("Slow");
    tmc2300_writeInt(TMC2300_VACTUAL, 2500);

delay(1000);

  Serial.print("GCONF:");
    result = tmc2300_readInt(TMC2300_GCONF);
    printByte(result);
    
    delay(4000);

    Serial.println("Reverse");
    tmc2300_writeInt(TMC2300_VACTUAL, -25000);
    delay(5000);
}
