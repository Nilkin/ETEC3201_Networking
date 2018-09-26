// ETEC 3201.01
// Lab 6, Program 1 'Transmitter'
// Keith Webb

#include <SPI.h>

const auto settings = SPISettings(1000000, MSBFIRST, SPI_MODE0);

bool txing = false;
unsigned tx_count = 0;

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

  SPI.transfer(0x10 | 0x20);
  SPI.transfer(addr[0]);
  SPI.transfer(addr[1]);
  SPI.transfer(addr[2]);
  SPI.transfer(addr[3]);
  SPI.transfer(addr[4]);

  digitalWrite(10, HIGH);
  SPI.endTransaction();
}

void write_payload(String s)
{
  SPI.beginTransaction(settings);
  digitalWrite(10, LOW);

  SPI.transfer(0xA0);
  for(int i = 0; i < 32; ++i)
  {
    if(i < s.length())
    {
      SPI.transfer(s[i]);
    }
    else
    {
        SPI.transfer(0);
    }
  }

  digitalWrite(10, HIGH);
  SPI.endTransaction();
  
  /*
  delay(1);
  SPI.beginTransaction(settings);
  digitalWrite(10, LOW);
  SPI.transfer(0xE1);
  digitalWrite(10, HIGH);
  SPI.endTransaction();
  write_register(7, 32);*/
}

void setup()
{
  Serial.begin(9600);

  pinMode(7, OUTPUT);
  pinMode(10, OUTPUT);
  digitalWrite(7, HIGH);
  digitalWrite(10, HIGH);

  SPI.begin();

  Serial.println("TX Firmware Online");

  bool anybody_mode = true;

  delay(5);
  write_register(0, 2); // set status to be Powered on and in TX mode  
  delay(5);
  if(!anybody_mode)
  {
    write_register(5, 3); // set RF channel
    delay(5);
  }
  
  // Disable all ShockBurst Features, 0 into register 1
  write_register(1, 0);
  delay(5);
  
  if(!anybody_mode)
  {
    uint8_t address[5] = {0x01, 0x23, 0x45, 0x67, 0x89};
    set_address(address); // set address
  }
}

void send_packet()
{
  String contents = "Msg:" + String(tx_count);
  
  write_payload(contents);
  
  Serial.println(contents);
}

void loop() 
{ 
  if(Serial.available())
  {
    uint8_t input = Serial.read();
    if(input == 'g')
    {
      Serial.println("Go!");
      txing = true;
      tx_count = 0;
    }
    else if(input == 's')
    {
      Serial.println("Stop!");
      txing = false;
      tx_count = 0;
    }
  }

  if(txing)
  {
      send_packet();          
      tx_count++;      
      delay(100);
  }
}

