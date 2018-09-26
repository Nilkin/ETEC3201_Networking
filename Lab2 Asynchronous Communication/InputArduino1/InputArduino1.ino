////For Networking Project2
////Reciever program
////Asynchronous Communication
////Dalton Fox, Ian Woodley, Thomas Gilman
#define BPS 9600
#define TIME 1000.0f / BPS

int ledPin = 13;
int inPin = 2;
int val = 0;
float t = 1000.0f / BPS;
unsigned char data = 0;
//
void rateDelay(float t)
{
  // Special raterateDelay for various bit rates
  int bps = t;
  int seconds = bps;
  if (seconds) 
  { 
    delay(seconds); 
  }

  bps -= seconds;

  float microseconds = bps * 1000;
  if (microseconds) 
  { 
    delayMicroseconds(microseconds); 
  }
}
void setup() { //sets the pin in and out including the 
  Serial.begin(9600);
  pinMode(inPin, INPUT);
  pinMode(ledPin, OUTPUT);
}
//Needs to read in 10 bits 1 bit for starting the sequence, 8bits for data, and 1 bit to reset to IDLE
void loop() {
  val = digitalRead(inPin);
  
  while(val==HIGH)//The Arduino is not recieving anything, its in IDLE loop till recieving.
  {
    val = digitalRead(inPin);
    digitalWrite(ledPin,HIGH);
  }
  
  digitalWrite(ledPin,LOW);
  Serial.println("YOU SET ME LOW");//lets the Arduino is out of IDLE and started recieving
  rateDelay(t+(t/2));
  
  for(int i=0;i<=7;i++) //read in 8bits
  {
    val = digitalRead(inPin);
    data |= (val << (7-i)); //shifts left i bits and Bitwise OR's data with Val 
    digitalWrite(ledPin,val);
    rateDelay(t);
  }
  val = digitalRead(inPin);
  
  if(val==LOW)//VAL should not be low a reset bit needs to reset it to HIGH for IDLE
  {
    val = digitalRead(inPin);
    digitalWrite(ledPin,LOW);
    Serial.print("FRAME ERROR!!");
    return 0; //exit
  }
  
  Serial.println(data);// prints the output of DATA before looping back again into IDLE
  Serial.println("\nIn IDLE");
}
