#include <VirtualWire.h>

int incoming_byte = 0; //Value from phone

const int led_pin = 13;
const int transmit_pin = 12;
const int receive_pin = 2;
const int transmit_en_pin = 3;

void setup(){
  // Initialise the IO and ISR
  vw_set_tx_pin(transmit_pin);
  vw_set_rx_pin(receive_pin);
  vw_set_ptt_pin(transmit_en_pin);
  vw_set_ptt_inverted(true); // Required for DR3100
  vw_setup(2000);	 // Bits per sec
  Serial.begin(9600);
}

byte count = 1;

void loop(){
  
  if (incoming_byte == 1) {
    char msg[7] = {'h','e','l','l','o',' ','#'  };
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
  //      vw_send((uint8_t *)msg, strlen(msg));
}

