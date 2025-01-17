#include <Wire.h>
uint8_t x;

void setup() {
  //Start the I2C Bus as Slave on "address1"
  pinMode(13, OUTPUT);
  Wire.begin(0x58);
  Wire.onReceive(dev_handle);
  Serial.begin(9600);
}

void dev_handle(int bytes){
  while(Wire.available()){
    x = Wire.read(); //Read one character from I2C
    Serial.println(x);
  }
    //digitalWrite(13, HIGH); //13 là số pin đèn led và HIGH LÀ SỐ VOL CAO

}

void loop() {
  //digitalWrite(13, LOW);
  // Serial.println("Hello");
}
