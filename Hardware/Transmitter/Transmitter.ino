// This microcontroller will control the transmission and recieving
// on the phone side

// First Transmitter = phone transmitter
// Second Transmitter = Lock transmitter
// First Receiver = Lock Receiver
// Second Receiver = Phone Receiver
#include <VirtualWire.h>

int incoming_byte = 0; //Value from phone

#define led_pin 13
#define transmit_pin 12
#define receive_pin 2
//#define transmit_en_pin 3

void setup(){
  // Initialise the IO and ISR
  vw_set_tx_pin(transmit_pin);
  vw_set_rx_pin(receive_pin);
  // Push to Talk
//  vw_set_ptt_pin(transmit_en_pin);
//  vw_set_ptt_inverted(true); // Required for DR3100
  vw_setup(2000);	 // Bits per sec
//  pinMode(transmit_pin, OUTPUT);
//  pinMode(led_pin, OUTPUT);
  Serial.begin(9600);
}

byte count = 1;

void loop(){
  incoming_byte = Serial.read();


// --- TRANSMITTING PART --- 
  if (incoming_byte == 'y') {
    // Get first letter to show the transission 
    // This device sends a message with a start of '111'
    // The second device sends a message with a start of '000'
    digitalWrite(led_pin, HIGH); // Flash light to show transmitting
    digitalWrite(transmit_pin, HIGH);
    char msg[7] = {'1','1','1','1','0','1','0'  };
    msg[6] = count;
    digitalWrite(led_pin, HIGH); // Flash a light to show transmitting
    vw_send((uint8_t *)msg, 7);
    
    if(vx_tx_active()) {
      Serial.print("THIS BOY SENT STUFF");
    }
    
    vw_wait_tx(); // Wait until the whole message is gone
    digitalWrite(led_pin, LOW);
    incoming_byte = 'w'; // Wrong set of pin variable
    count = count + 1;
  }
// --- TRANSMITTING PART END ---


// --- RECEIVER PART STARTS --- 
  uint8_t buf[VW_MAX_MESSAGE_LEN];
  uint8_t buflen = VW_MAX_MESSAGE_LEN;
  
  if (vw_get_message(buf, &buflen)) {
    int i; 
    vw_wait_rx();
    
    // Store the value to see which transmitter it is
    // 0 is the receiver we want to receive from 
    Serial.print("Got: ");
    int enc_dec = (buf[i], HEX);
    if (enc_dec == 0) {
      // This is where we receive data from our second transmitter
      for(i = 0; i < buflen; i++) {
        Serial.print(buf[i], HEX);
        Serial.print(' ');
      }
    }

    Serial.println();
    digitalWrite(led_pin, LOW);
  }
  // --- RECEIVING PART END ---
  
}

