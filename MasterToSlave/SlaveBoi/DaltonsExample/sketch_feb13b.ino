#define CE 7
#define CSN 8
#define SCK 13
#define MOSI 11
#define MISO 12
#define BPS 9600
#define T 1000.0f / BPS


void OGDelay (float t)
{
  int mi = (int) t;
  if (mi) delay (mi);
  t -= mi;
  float mc = t * 1000;
  if (mc) delayMicroseconds(mc);
}

unsigned char SOFT_SPI(unsigned char send_byte)
{
  unsigned char recv_byte = 0;
  int i;
  for (i = 0; i < 8; i++)
  {

    digitalWrite(MOSI,(send_byte >> (7 - i) ) & 1);
    OGDelay(T);
    digitalWrite(SCK, HIGH);
    recv_byte |= (digitalRead(MISO) << (7 - i) );
    OGDelay(T);
    digitalWrite(SCK, LOW);
  }

  return recv_byte;
}

void setup()
{
  Serial.begin(9600);
  pinMode(CE, OUTPUT);
  pinMode(CSN, OUTPUT);
  pinMode(SCK, OUTPUT);
  pinMode(MOSI, OUTPUT);
  pinMode(MISO, INPUT);
  digitalWrite(CE, HIGH);
  digitalWrite(CSN, HIGH);
  digitalWrite(SCK, LOW);
  digitalWrite(MOSI, HIGH);
}

char cmd;
unsigned int value;

void loop()
{
  char input = Serial.read();
  if (input == 'r' || input == 'R')
  {
    int reg;
    for (reg = 0x00; reg < 0x18; reg++)
    {
      Serial.print(reg, HEX);
      Serial.print(" | ");
      digitalWrite(CSN, LOW);

      value = SOFT_SPI(reg);

      Serial.print(value, HEX);
      digitalWrite(CSN, HIGH);
      Serial.print(" | ");
      Serial.println(value, BIN);
    }
  }
}
