#include <VirtualWire.h>

int incoming_byte = 0; //Value from phone

#define led_pin 13
#define transmit_pin 12
#define receive_pin 2
#define transmit_en_pin 3

//const int led_pin = 13;
//const int transmit_pin = 12;
//const int receive_pin = 2;
//const int transmit_en_pin = 3;

void setup(){
  // Initialise the IO and ISR
  vw_set_tx_pin(transmit_pin);
  vw_set_rx_pin(receive_pin);
  vw_set_ptt_pin(transmit_en_pin);
  vw_set_ptt_inverted(true); // Required for DR3100
  vw_setup(2000);	 // Bits per sec
//  pinMode(transmit_pin, OUTPUT);
//  pinMode(led_pin, OUTPUT);
  Serial.begin(9600);
}

byte count = 1;

void loop(){
  
  if (incoming_byte == 1) {
    digitalWrite(led_pin, HIGH); // Flash light to show transmitting
    digitalWrite(transmit_pin, HIGH);
    char msg[7] = {'1','0','0','1','0','1','0'  };
    msg[6] = count;
    digitalWrite(led_pin, HIGH); // Flash a light to show transmitting
    vw_send((uint8_t *)msg, 7);
    
    if(vx_tx_active()) {
      Serial.print("THIS BOY SENT STUFF");
      Serial.println(msg);
    }
    
    vw_wait_tx(); // Wait until the whole message is gone
    digitalWrite(led_pin, LOW);
    delay(1000);
    count = count + 1;
  }
}

