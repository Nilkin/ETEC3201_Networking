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

void ourDelay(float t)
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
  short int lengthToGet = 7;
  if(send_byte >= 0x0b && send_byte<=0x0e)
    lengthToGet = 39;
  for(short i=0;i<8;i++)
  {
    SET_MOSI(send_byte >> (7-i)&1); //bit shift through the char and send to MOSI must be msb first
    ourDelay(TIME/2);               //Delay for 1 clock period
    SET_SCK(HIGH);                //getVALUE for MISO
    recv_data |= (GETVAL << (7-i)); 
    ourDelay(TIME/2);
    SET_SCK(LOW);
  }
  SET_CSN(HIGH); //disable chip till re enable
  return recv_data;
}

void setup() { //sets the pin in and out including the 
  Serial.begin(9600);
  pinMode(SPI_CE, OUTPUT); //Chip enable (for the radio signal chip)
  pinMode(SPI_CSN, OUTPUT); //Chip Select Not /SS
  pinMode(SPI_SCK, OUTPUT); //clock
  pinMode(SPI_MOSI, OUTPUT); //data in
  pinMode(SPI_MISO, INPUT); //data out
  SET_MOSI(LOW); SET_MISO(HIGH); SET_SCK(HIGH);
  SET_CSN(HIGH); SET_CE(LOW);
}
//Needs to read in 10 bits 1 bit for starting the sequence, 8bits for data, and 1 bit to reset to IDLE
void loop() {
  if(Serial.available()==true)
  { //read char bytes from serail
    serailInData = Serial.read();
    if(serailInData == 'R' || serailInData == 'r')
    {
      SET_MOSI(HIGH); //slave is reading in data now
      for(auto i=0x00;i<0x18;i++) //18 address's
      {
        SET_CSN(LOW);//enable chip
        SOFT_SPI(i);
        data = SOFT_SPI(i); //get the values from the registers returns (unsigned char)
        
        Serial.print(i); Serial.print("\t");
        Serial.print(i,HEX); Serial.print("\t");
        Serial.println(data,BIN);
      }
      SET_MOSI(LOW);//no longer reading in data
    }
    ourDelay(TIME);
    // prints the output of DATA before looping back again into IDLE
  }
}
