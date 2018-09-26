////For Networking Project3
////Reciever program
////Dalton Fox, Ian Woodley, Thomas Gilman

#define BPS 9600
#define TIME 1000.0f / BPS
#define CLOCK digitalRead(SPI_SCK)
#define GETVAL digitalRead(SPI_MISO)
#define SET_MOSI(x) digitalWrite(SPI_MOSI,x)
#define SET_MISO(x) digitalWrite(SPI_MISO,x)
#define SET_CSN(x) digitalWrite(SPI_CSN,x)
#define SET_SCK(x) digitalWrite(SPI_SCK,x)
#define SET_CE(x) digitalWrite(SPI_CE,x)
#define READ_SPI_CSN digitalRead(SPI_CSN)

short SPI_SCK = 13, SPI_MISO = 12, SPI_MOSI = 11, SPI_CSN = 8, SPI_CE = 7;
unsigned int data=0;

void setup() { //sets the pin in and out including the 
  Serial.begin(9600);
  pinMode(SPI_CE, OUTPUT); //Chip enable (for the radio signal chip)
  pinMode(SPI_CSN, OUTPUT); //Chip Select Not /SS
  pinMode(SPI_SCK, OUTPUT); //clock
  pinMode(SPI_MOSI, OUTPUT); //data in
  pinMode(SPI_MISO, INPUT); //data out
  SET_MOSI(HIGH); SET_MISO(HIGH); SET_SCK(LOW);
  SET_CSN(HIGH); SET_CE(HIGH);
}

void ourDelay(float t) //Does Delay in milliseconds first, then delays it in microseconds of remaining time given
{
  int mi = (int)t;
  if (mi) delay(mi);
  t -= mi;
  float mc = t * 1000;
  if (mc) delayMicroseconds(mc);
}

unsigned char SOFT_SPI(unsigned char send_byte) //get data from register
{
  unsigned char recv_data=0;
  for(int i=0;i<8;i++)
  {
    SET_MOSI(send_byte >> (7-i)&1); //send Data
    ourDelay(TIME); SET_SCK(HIGH); //clock delays
    
    recv_data |= (GETVAL << (7-i)); //recieve Data
    ourDelay(TIME); SET_SCK(LOW); //clock delays
  }
  return recv_data;
}

long long SOFT_SPI_LONG(unsigned char send_byte) {
  long long recv_byte = 0;
  unsigned char in_value;
  for (int i=0; i < 5; i++) {
    in_value = SOFT_SPI(send_byte);
    recv_byte <<= 8;
    recv_byte |= in_value;
  }
  return recv_byte;
}

//Needs to read in 10 bits 1 bit for starting the sequence, 8bits for data, and 1 bit to reset to IDLE
void loop() 
{
  if(Serial.available())
  {
    if((Serial.read() == 'R') || (Serial.read() == 'r')) //user wants to read from device 
    {
      Serial.println("\nRegister\tDataHex\t\tDataBinary");
      for(auto chip_reg=0x00;chip_reg<0x18;chip_reg++) //24 address's
      {
        SET_CSN(LOW);
        if(chip_reg > 0x10&& chip_reg< 0x17)
        {
          data = SOFT_SPI_LONG(chip_reg);
        }
        else
        {
          SOFT_SPI(chip_reg);
          data = SOFT_SPI(chip_reg); //get the values from the registers returns (unsigned char) and disable chip
        }
        SET_CSN(HIGH);
        Serial.print(chip_reg,HEX); Serial.print("\t\t"); // prints the output of DATA before looping back again into IDLE
        Serial.print(data,HEX); Serial.print("\t\t");
        Serial.println(data,BIN);
      }
    }
  }
}
