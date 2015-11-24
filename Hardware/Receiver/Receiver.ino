// This microcontroller will control the transmission and recieving
// on the phone side
#include <VirtualWire.h>
#include <AccelStepper.h>
#define HALFSTEP 8

// Pins definition
#define led_pin 13
#define transmit_pin 12
#define receive_pin 2
#define transmit_en_pin 3

//Stepper Motor Pins
#define motorPin1 8 // IN1
#define motorPin2 9 // IN2
#define motorPin3 10 // IN3
#define motorPin4 11 // IN4

AccelStepper stepper (HALFSTEP, motorPin1, motorPin2, motorPin3, motorPin4);

// variables

int turnSteps = 2100; // steps for 90 degree turn
int stepperSpeed = 1000; // stepper speed (steps/second)
int steps1 = 0; // keep track of the step count for motor

boolean turn = false; // keep track if we are turning or not

void setup(){
   Serial.begin(9600); // Debugging purposes....
   Serial.println("setup");
   
   // Motor Setup
   stepper.setMaxSpeed(2000.0);
   stepper.setAcceleration(100.0);
   stepper.setSpeed(200);
   stepper.moveTo(20000);
   
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
        transmitCodeBack();
//        digitalWrite(led_pin, LOW); // Turn off LED light 
        
   }
// --- RECEIVING PART ENDS ---

// --- TRANSMITTING PART STARTS ---


// --- TRANSMITTING PART ENDS ---
}

void lockDoor () {

}

void unlockDoor() {
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
