#define BPS
#define TIME 1000.0f/BPS
#define CE 7
#define CSN 8
#define SCK 13
#define MOSI 11
#define MISO 12

void setup()
{
 //local serial port setup
 Serial.begin(9600);

 pinMode(SCK,OUTPUT); //Slave Clock
 pinMode(MOSI,OUTPUT); //Master out Slave in
 pinMode(CSN,OUTPUT); //chips select not (idles HIGH)
 pinMode(CE,OUTPUT); //chip enable
 pinMode(MISO,INPUT); //Master in Slave out
 digitalWrite(SCK,LOW); 
 digitalWrite(MOSI,HIGH); digitalWrite(MISO,LOW);
 digitalWrite(CE,HIGH); digitalWrite(CSN,HIGH);
 config_rx();
}

void ODelay (float t)
{
  int mi = (int) t;
  if (mi) delay (mi);
  t -= mi;
  float mc = t * 1000;
  if (mc) delayMicroseconds(mc);
}

void config_tx(void) //configure transmitter and print back values set
{
  Serial.println("Configuring Tx");
  write_register(0x00, 2);
  Serial.print("Register:0x00 = ");Serial.println(read_register(0x00));
  write_register(0x05, 3);
  Serial.print("Register:0x05 = ");Serial.println(read_register(0x05));
  write_register(0x01, 0);
  Serial.print("Register:0x01 = ");Serial.println(read_register(0x01));
  write_register(0x11, 32);
  Serial.print("Register:0x11 = ");Serial.println(read_register(0x11));
  
  write_tx_address("poddy");
  write_rx_address("poddy");
}

void config_rx(void) //configure reciever and print back values set
{
 Serial.println("Configuring Rx");
  write_register(0x00, 3);
  Serial.print("Register:0x00 = ");Serial.println(read_register(0x00));
  write_register(0x05, 3);
  Serial.print("Register:0x05 = ");Serial.println(read_register(0x05));
  write_register(0x01, 0);
  Serial.print("Register:0x01 = ");Serial.println(read_register(0x01));
  write_register(0x11, 32);
  Serial.print("Register:0x11 = ");Serial.println(read_register(0x11));
  
  write_tx_address("poddy");
  write_rx_address("poddy");
}

void flush_tx(void)
{
 digitalWrite(CSN,LOW);
 soft_spi(0xE1);
 digitalWrite(CSN,HIGH);
}

void flush_rx(void)
{
 digitalWrite(CSN,LOW);
 soft_spi(0xE2);
 digitalWrite(CSN,HIGH);
}

unsigned char read_register(unsigned char reg)
{
 digitalWrite(CSN,LOW);
 soft_spi(reg);
 unsigned char retval = soft_spi(reg);
 digitalWrite(CSN,HIGH);
 return retval;
}

void write_tx_address(const char * ch_array)
{
  digitalWrite(CSN,LOW);
  soft_spi(0x20 | 0x10); // Write starting at 0x10;
  short i = 0;
  while(i++ < 5) // Sending to a 5 Byte register
  {
    soft_spi(ch_array[i]);
  }
  digitalWrite(CSN,HIGH);
}

void write_rx_address(const char * ch_array)
{
 digitalWrite(CSN,LOW);
 soft_spi(0x20|0x0A); // Write Starting at 0x0A;
 short i = 0;
 while(i++ < 5) // Sending to a 5 Byte register
 {
  soft_spi(ch_array[i]);
 }
 digitalWrite(CSN,HIGH);
}

void write_payload(unsigned char * payload)
{
 digitalWrite(CSN,LOW);
 soft_spi(0xA0);
 for (short i = 0; i < 32; i++) // Write the 32 payload bytes over SPI
 {
  soft_spi(payload[i]);
 }
 digitalWrite(CSN,HIGH);
}

void read_payload(unsigned char * payload)
{
 digitalWrite(CSN,LOW)
 soft_spi(0x61);
 for (short i = 0; i < 32; i++)
 {
   payload[i] = soft_spi(i);    
 }
 digitalWrite(CSN,HIGH);
}

void write_register(unsigned char reg, unsigned char value)
{
 digitalWrite(CSN,LOW);
 soft_spi(0x20 |= reg); // Send values starting at register reg
 soft_spi(value);
 digitalWrite(CSN,HIGH);
}

unsigned char soft_spi(unsigned char ch)
{
  unsigned char recv_byte = 0;
  
  for (short i = 0; i < 8; i++) // Write bits over SPI
  {
    digitalWrite(MOSI, (ch>>(7-i)&1));
    
    ODelay(TIME/2); digitalWrite(SCK, HIGH); //clock
    recv_byte |= (digitalRead(MISO) << (7 - i) );
    ODelay(TIME/2); digitalWrite(SCK, LOW); //clock
  }
  return recv_byte;
}
