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

////For Networking Project1, Reciever program with Dalton Fox, Ian Woodley, Thomas Gilman
short SPI_SCK = 13, SPI_MISO = 12, SPI_MOSI = 11, SPI_CSN = 8, SPI_CE = 7;
unsigned char data=0, serailInData=0;

void setup() { //sets the pin in and out including the 
  Serial.begin(9600);
  pinMode(SPI_CE, OUTPUT); //Chip enable (for the radio signal chip)
  pinMode(SPI_CSN, OUTPUT); //Chip Select Not /SS
  pinMode(SPI_SCK, OUTPUT); //clock
  pinMode(SPI_MOSI, OUTPUT); //data in
  pinMode(SPI_MISO, INPUT); //data out
  SET_MOSI(HIGH); SET_MISO(HIGH); SET_SCK(LOW);
  SET_CSN(HIGH); SET_CE(LOW);
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
  int lengthOfData=1, lengthToGet = 8;
  unsigned char recv_data=0;
  
  if(send_byte >= 0x0b && send_byte<=0x0e)//registers 11 through 14 are 5bytes long
  { 
    lengthToGet = 40, lengthOfData = 5;
  }
  for(int i=0;i<lengthToGet;i++)
  {
    SET_MOSI(send_byte >> ((lengthToGet-1)-i)&1); //send Data
    ourDelay(TIME/2); SET_SCK(HIGH); //clock delays
    recv_data |= (GETVAL << ((lengthToGet-1)-i)); //recieve Data
    ourDelay(TIME/2); SET_SCK(LOW); //clock delays
  }
  SET_CSN(HIGH); //disable chip till re enable
  return recv_data;
}


//Needs to read in 10 bits 1 bit for starting the sequence, 8bits for data, and 1 bit to reset to IDLE
void loop() {
  if(Serial.available())
  {
    if(Serial.read() == 'R' || 'r') //user wants to read from device 
    {
      SET_MOSI(HIGH);
      Serial.println("\nRegister\tDataHex\t\tDataBinary");
      for(auto chip_reg=0x00;chip_reg<0x18;chip_reg++) //24 address's
      {
        SET_CSN(LOW); //enable chip
        data = SOFT_SPI(chip_reg); //get the values from the registers returns (unsigned char)
        
        Serial.print(chip_reg,HEX); Serial.print("\t\t"); // prints the output of DATA before looping back again into IDLE
        Serial.print(data,HEX); Serial.print("\t\t"); Serial.println(data,BIN);
      }
      SET_MOSI(LOW); ourDelay(TIME);
    }
  }
}
