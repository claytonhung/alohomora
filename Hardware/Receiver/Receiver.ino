#include <VirtualWire.h>

//#define transmit_pin A0
//#define led_pin 13

// Pins definition
#define led_pin 13
#define transmit_pin 12
#define receive_pin 2
#define transmit_en_pin 3
//const int led_pin = 13;
//const int transmit_pin = 12;
//const int receive_pin = 11;
//const int transmit_en_pin = 3;
//unsigned int data = 0;

void setup(){
   delay(1000);
   Serial.begin(9600);	// Debugging only
   Serial.println("setup");
   pinMode(ledPin, OUTPUT);
    // Initialise the IO and ISR
    vw_set_tx_pin(transmit_pin);
    vw_set_rx_pin(receive_pin);
    vw_set_ptt_pin(transmit_en_pin);
    vw_set_ptt_inverted(true); // Required for DR3100
    vw_setup(2000);	 // Bits per sec
//
    vw_rx_start();       // Start the receiver PLL running
}

void loop(){
   uint8_t buf[VW_MAX_MESSAGE_LEN];
   uint8_t buflen = VW_MAX_MESSAGE_LEN;

   // Check if a message was received
    if (vw_get_message(buf, &buflen)) {
      int i;
        vw_wait_rx();
        digitalWrite(led_pin, HIGH); // Flash a light to show received good message
	// Message with a good checksum received, print it.
	Serial.print("Got: ");
	
	for (i = 0; i < buflen; i++)
	{
	    Serial.print(buf[i], HEX);
	    Serial.print(' ');
	}
	Serial.println();
        digitalWrite(led_pin, LOW);
        vw_rx_stop();
   }

}
