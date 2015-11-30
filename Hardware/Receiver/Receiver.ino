// This microcontroller will control the transmission and recieving
// on the phone side. An infared sensor is used to detect any motion. 
// The detection will ensure that users, along with the receiving code is 
// allowed to unlock the door. 

#include <VirtualWire.h>
#include <AccelStepper.h>
#define HALFSTEP 

// Pins definition
#define led_pin 13 // LED pin to blink whenever device receives something
#define receive_pin 2 //Receiver module pin
#define pirPin = 4 // Pin info for the Infared Sensor

//Stepper Motor Pins
#define motorPin1 8 // IN1
#define motorPin2 9 // IN2
#define motorPin3 10 // IN3
#define motorPin4 11 // IN4

// Infared Sensor Pins
// Datasheet for this PIR sensor says calibration time between 10-60 seconds
int calibrationTime = 30; //give sensor 30 seconds to calibrate
long unsigned int LowIn; // time when sensor outputs low impulse
long unsigned int pause = 5000; // time the sensor has to be low before we assuming motion has stopped
boolean lockLow = true;
boolean takeLowTime; 

void setup(){
   Serial.begin(9600); // Debugging purposes....
   Serial.println("setup");
   digitalWrite(pirPin, LOW);
   
   Serial.print("calibrating sensor ");
     for (int i = 0; i < calibrationTime; i++) {
       Serial.print(".");
       delay(1000);
     }  
     Serial.println("done");
     Serial.println("SENSOR ACTIVE");
     delay(50);
     
   // Initialise the IO and ISR
   vw_set_tx_pin(transmit_pin);
   vw_set_rx_pin(receive_pin);
   vw_set_ptt_pin(transmit_en_pin);
   vw_set_ptt_inverted(true); // Required for DR3100
   vw_setup(2000);	 // Bits per sec
   vw_rx_start();       // Start the receiver PLL running
}

byte count = 1;

void loop(){
  
  if (digitalRead(pirPin) == HIGH) {
    
  }
// --- RECEIVING PART STARTS ---
   uint8_t buf[VW_MAX_MESSAGE_LEN];
   uint8_t buflen = VW_MAX_MESSAGE_LEN;

   // Check if a password message has been received
    if (vw_get_message(buf, &buflen)) {
      int i;
        vw_wait_rx();
        digitalWrite(led_pin, HIGH); // Turn on LED light when message received

	Serial.print("Got: ");
	for (i = 0; i < buflen; i++)
	{
	    Serial.print(buf[i], HEX);
	    Serial.print(' ');
	}
	Serial.println();
//        transmitCodeBack();
        
   }
// --- RECEIVING PART ENDS ---
}


void motionDetected() {
  
}

void 
void lockDoor () {

}

void unlockDoor() {
  Serial.print("UNLOCKING DOOR");
  delay(1000);
  Serial.println("Stepper distance: " + stepper.distanceToGo());
  stepper.moveTo(-stepper.currentPosition());
  if(stepper.distanceToGo() == 0) {
    stepper.moveTo(-stepper.currentPosition());
  }
  stepper.run();
}

void transmitCodeBack() {
  char msg[7] = {'0', '1', '1', '1', '0', '1', '0' };
  msg[6] = count;
  vw_send((uint8_t *)msg, 7);
  
  if (vx_tx_active()) {
    Serial.print("Sent.");
  }
  
  vw_wait_tx();
  count = count + 1;
}
