#include <VirtualWire.h> 

// Pins definition
const int led_pin = 13;
const int receive_pin = 12;
int pinSpeaker = 10; 

void setup(){
   Serial.begin(9600); // Debugging only
   // Initialise the IO and ISR
   vw_set_rx_pin(receive_pin);
   vw_setup(4000); // Transmission rate
   // Start the receiver PLL
   vw_rx_start();
   // Set LED pin and Buzzer
   pinMode(led_pin, OUTPUT);
   pinMode(pinSpeaker, OUTPUT);
}

 

void loop(){
   uint8_t buf[VW_MAX_MESSAGE_LEN];
   uint8_t buflen = VW_MAX_MESSAGE_LEN;

   // Check if a message was received
    if (vw_get_message(buf, &buflen)) {
      if(buf[0]=='1'){
        Serial.println("Motion detected!");
        digitalWrite(led_pin,1);
        playTone(300, 160);
        delay(150);
      }  

     if(buf[0]=='0'){
       Serial.println("Motion ended!");
       digitalWrite(led_pin,0);
       playTone(0, 0);
       delay(300); 
     }
   }
}



// duration in mSecs, frequency in hertz

void playTone(long duration, int freq) {
    duration *= 1000;
    int period = (1.0 / freq) * 1000000;
    long elapsed_time = 0;
    
    while (elapsed_time < duration) {
      digitalWrite(pinSpeaker,HIGH);
      delayMicroseconds(period / 2);
      digitalWrite(pinSpeaker, LOW);
      delayMicroseconds(period / 2);
    
      elapsed_time += (period);
   }
}


//
// #define rfReceivePin A0  //RF Receiver pin = Analog pin 0
// #define ledPin 13        //Onboard LED = digital pin 13
//
// unsigned int data = 0;   // variable used to store received data
// const unsigned int upperThreshold = 70;  //upper threshold value
// const unsigned int lowerThreshold = 50;  //lower threshold value
//
// void setup(){
//   pinMode(ledPin, OUTPUT);
//   Serial.begin(9600);
// }
//
// void loop(){
//   data=analogRead(rfReceivePin);    //listen for data on Analog pin 0
//   
//    if(data>upperThreshold){
//     digitalWrite(ledPin, LOW);   //If a LOW signal is received, turn LED OFF
//     Serial.println(data);
//   }
//   
//   if(data<lowerThreshold){
//     digitalWrite(ledPin, HIGH);   //If a HIGH signal is received, turn LED ON
//     Serial.println(data);
//   }
// }
