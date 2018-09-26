#define BPS 9600
#define TIME 1000.0f / BPS
#define CE 7
#define CSN 8
#define SCK 13
#define MOSI 11
#define MISO 12

void ODelay (float t)
{
  int mi = (int) t;
  if (mi) delay (mi);
  t -= mi;
  float mc = t * 1000;
  if (mc) delayMicroseconds(mc);
}


void config_tx(void)
{
  write_register(0x00, 2);
  write_register(0x05, 3);
  write_register(0x01, 0);
  write_register(0x11, 32);
  write_tx_address("poddy");
  write_rx_address("poddy");
}

void config_rx(void)
{
  write_register(0x00, 2);
  write_register(0x05, 3);
  write_register(0x01, 0);
  write_register(0x11, 32);
  write_tx_address("poddy");
  write_rx_address("poddy");
}

void flush_tx(void)
{
  digitalWrite(CSN, LOW);

  // Flush tx command
  soft_spi(0xE1);

  digitalWrite(CSN, HIGH);
}

void flush_rx(void)
{
  digitalWrite(CSN, LOW);

  // Flush rx command
  soft_spi(0xE2);

  digitalWrite(CSN, HIGH);
}

unsigned char read_register(unsigned char reg)
{
  digitalWrite(CSN, LOW);

  // Read value of register
  soft_spi(reg);
  unsigned char retval = soft_spi(reg);

  digitalWrite(CSN, HIGH);
  return retval;
}

void write_tx_address(const char * ch_array)
{
  digitalWrite(CSN, LOW);

  // Write starting at 0x10;
  soft_spi(0x20|0x10);
  // Sending to a 5 Byte register
  for (int i = 0; i < 5; i++)
  {
    soft_spi(ch_array[i]);
  }

  digitalWrite(CSN, HIGH);
}

void write_rx_address(const char * ch_array)
{
  digitalWrite(CSN, LOW);
  // Write Starting at 0x0A;
  soft_spi(0x20|0x0A);
  // Sending to a 5 Byte register
  for (int i = 0; i < 5; i++)
  {
    soft_spi(ch_array[i]);
  }

  digitalWrite(CSN, HIGH);
}

void write_payload(unsigned char * payload)
{
  digitalWrite(CSN, LOW);

  // Write the 32 payload bytes over SPI
  soft_spi(0x0A);
  for (int i = 0; i < 32; i++)
  {
    soft_spi(payload[i]);
  }

  digitalWrite(CSN, HIGH);
}

void read_payload(unsigned char * payload)
{
  digitalWrite(CSN, LOW);

  soft_spi(0x61);
  for (int i = 0; i < 32; i++)
  {
    payload[i] = soft_spi(0);
  }

  digitalWrite(CSN, HIGH);
}

void write_register(unsigned char reg, unsigned char value)
{
  digitalWrite(CSN, LOW);

  // Send values starting at register reg
  soft_spi(0x20|reg);
  soft_spi(value);

  digitalWrite(CSN, HIGH);
}

unsigned char soft_spi(unsigned char ch)
{
  unsigned char recv_byte = 0;

  // Write bits over SPI
  for (int i = 0; i < 8; i++)
  {
    digitalWrite(MOSI,(ch>>(7-i))&1); //writes MOSI HIGH or LOW based on the bit shifted to
    ODelay(TIME);
    digitalWrite(SCK, HIGH);
    recv_byte |= (digitalRead(MISO) << (7 - i) );
    ODelay(TIME);
    digitalWrite(SCK, LOW);
  }
  return recv_byte;
}
