// This microcontroller will control the transmitter part of the system

#include <VirtualWire.h> //required to communicate with the receiving end 

char phone_data = 'w'; //a char is passed from the phone to the 

#define led_pin 13 // LED helps indicate when a message is sent
#define transmit_pin 12 

void setup(){
  Serial.begin(9600); //initialize at 9600 baudrate
  pinMode(led_pin, OUTPUT); //initialize as OUTPUT
  
  // Initialise the IO and ISR
  vw_setup(3000); //set the bps for this library
  vw_set_tx_pin(transmit_pin); //initialize the pin 
}

byte count = 1;

void loop(){
  phone_data = Serial.read();
  
  if (phone_data == 'l') {
    // Get first letter to show the transission 
    // This device sends a message with a start of '111'
    // The second device sends a message with a start of '000'
    digitalWrite(led_pin, HIGH); // Flash light to show transmitting
    digitalWrite(transmit_pin, HIGH);
//    char msg[7] = {'0', '1'};
    int msg = 33;
    // msg[6] = count;
    digitalWrite(led_pin, HIGH); // Flash a light to show transmitting
    vw_send((uint8_t *)msg, 4);
    
    if(vx_tx_active()) {
      Serial.print("Request for lock");
    }
    
    vw_wait_tx(); // Wait until the whole message is gone
    digitalWrite(led_pin, LOW);
    phone_data = 'w'; // Wrong set of pin variable
    count = count + 1;
    
  } else if (phone_data == 'u') {
      digitalWrite(led_pin, HIGH);
      digitalWrite(transmit_pin, HIGH);
      char msg[7] = {'1'};
      msg[6] = count;
      digitalWrite(led_pin, HIGH); 
      vw_send((uint8_t *)msg, 7);
      
      if (vx_tx_active()) {
        Serial.print("Request for unlock");
      }
      
      vw_wait_tx();
      digitalWrite(led_pin, LOW);
      phone_data = 'w';
      count = count + 1;
  } else {
    constantlySend(); //this is being sent to show that this phone is correct phone for the receiving end
  }
}

void constantlySend () {
   char msg[1] = {'2'};
    msg[6] = count;
    digitalWrite(led_pin, HIGH); // Flash a light to show transmitting
    vw_send((uint8_t *)msg, 7);
    
    if(vx_tx_active()) {
    }   
    vw_wait_tx(); // Wait until the whole message is gone
    digitalWrite(led_pin, LOW);
    phone_data = 'w'; // Wrong set of pin variable
    count = count + 1;
    delay(2000);
}

