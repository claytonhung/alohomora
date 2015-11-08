#include <VirtualWire.h>

int led_pin = 13;
int transmit_pin = 12;
int val = 0;

 void setup(){
   Serial.begin(38400);
   vw_set_tx_pin(transmit_pin);
   vw_setup(4000); //Transmission rate
   pinMode(led_pin, OUTPUT);   
 }

// void loop(){
//   Serial.println("START");
//   
//   char msg[1] = {'0'};
//   val = digitalRead(pir_pin);
//   if (val == 1){ //Motion detected
//     msg[0] = '1';
//   for(int i=4000; i>5; i=i-(i/3)){
//     analogWrite(10)
//     //digitalWrite(rfTransmitPin, HIGH);     //Transmit a HIGH signal
//     //digitalWrite(ledPin, HIGH);       //Turn the LED on
//     delay(2000);                           //Wait for 1 second
//     
//     digitalWrite(rfTransmitPin,LOW);      //Transmit a LOW signal
//     digitalWrite(ledPin, LOW);            //Turn the LED off
//     delay(i);                            //Variable delay
//   }
// }
 
void loop(){
  Serial.println("START");
  char msg[1] = {'0'};
  // Get sensor value
//  val = digitalRead(pir_pin);
  // Change message if motion is detected
  if (val == 1){
      msg[0] = '1';
      digitalWrite(led_pin, HIGH); // Flash a light to show transmitting
      
      vw_send((uint8_t *)msg, 1);
      vw_wait_tx(); // Wait until the whole message is gone

//      if (pir_state == LOW) {
//        Serial.println("Motion detected!");
//        pir_state = HIGH;
//      }
   }
   else{
     msg[0] = '0';
     digitalWrite(led_pin, LOW);
     
     vw_send((uint8_t *)msg, 1);
     vw_wait_tx(); // Wait until the whole message is gone

//     if (pir_state == HIGH){
//        Serial.println("Motion ended!");
//        pir_state = LOW;
//     }
   }
}



