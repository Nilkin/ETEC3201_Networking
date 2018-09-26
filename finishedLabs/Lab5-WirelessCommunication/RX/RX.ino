// ETEC 3201.01
// Lab 6, Program 2 'Receiver'
// Keith Webb

#include <SPI.h>

const auto settings = SPISettings(1000000, MSBFIRST, SPI_MODE0);
bool rxing = false;

uint8_t read_register(uint8_t reg)
{
  SPI.beginTransaction(settings);
  digitalWrite(10, LOW);
  SPI.transfer(reg);
  uint8_t v = SPI.transfer(0xFF);
  digitalWrite(10, HIGH);
  SPI.endTransaction();
  return v;
}

void write_register(uint8_t reg, uint8_t val)
{
  SPI.beginTransaction(settings);
  digitalWrite(10, LOW);
  SPI.transfer(reg | 0x20);
  SPI.transfer(val);
  digitalWrite(10, HIGH);
  SPI.endTransaction();
}

void set_address(uint8_t addr[5])
{
  SPI.beginTransaction(settings);
  digitalWrite(10, LOW);

  SPI.transfer(0xA | 0x20);
  SPI.transfer(addr[0]);
  SPI.transfer(addr[1]);
  SPI.transfer(addr[2]);
  SPI.transfer(addr[3]);
  SPI.transfer(addr[4]);

  digitalWrite(10, HIGH);
  SPI.endTransaction();
}

void read_rx_buffer()
{
  char buffer[33];
  buffer[0] = 0;
  
  SPI.beginTransaction(settings);
  digitalWrite(10, LOW);
  SPI.transfer(0x61);

  for(unsigned i = 0; i < 32; ++i)
  {
    buffer[i] = SPI.transfer(0xFF);;
  }
  buffer[32] = 0;
  
  digitalWrite(10, HIGH);
  SPI.endTransaction();

  if(buffer[0] != '\0')
    Serial.println(buffer);
}

void setup()
{
  Serial.begin(9600);

  pinMode(7, OUTPUT);
  pinMode(10, OUTPUT);
  digitalWrite(7, HIGH);
  digitalWrite(10, HIGH);

  SPI.begin();

  Serial.println("RX Firmware Online");

  bool anybody_mode = true;

  delay(5);
  write_register(0, 3); // set status to be Powered on and in RX mode  
  delay(5);
  if(!anybody_mode)
  {
    write_register(5, 3); // set RF channel
    delay(5);
  }
  write_register(1, 0);
  delay(5);
  write_register(0x11, 32);
  delay(5);
  
  if(!anybody_mode)
  {
    uint8_t address[5] = {0x01, 0x23, 0x45, 0x67, 0x89};  
    set_address(address); // set address
  }
}

void loop() 
{ 
  if(Serial.available())
  {
    uint8_t input = Serial.read();
    if(input == 'g')
    {
      Serial.println("Listener Go!");
      rxing = true;      
    }
    else if(input == 's')
    {
      Serial.println("Listener Stop!");
      rxing = false;     
    }
  }

  if(rxing)
  {
    read_rx_buffer();
    //delay(2000);    
  }
}

