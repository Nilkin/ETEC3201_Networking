////For Networking Project1, 
///Reciever program
//Dalton Fox, Brandon Hofman, Ian Woodley, Thomas Gilman
int ledPin = 13;
int inPin = 2;
int val = 0;
unsigned char data = 0;
//
void setup() {
  Serial.begin(9600);
  pinMode(inPin, INPUT);
  pinMode(ledPin, OUTPUT);
}

void loop() {
  val = digitalRead(inPin);
  
  while(val==HIGH)
  {
    val = digitalRead(inPin);
    digitalWrite(ledPin,HIGH);
    //Serial.print("WAITING FOR STARTBIT");
  }
  digitalWrite(ledPin,LOW);
  Serial.print("YOU SET ME LOW");
  delay(150);
  for(int i=0;i<=7;i++)
  {
    val = digitalRead(inPin);
    //data<<=1;
    if(val==HIGH)//set data bit high
    {
      digitalWrite(ledPin,HIGH);
      Serial.print("BIT SET HIGH");
      data |= 1 << (7-i);
    }
    delay(100);
  }
  val = digitalRead(inPin);
  while(val==LOW)
  {
    val = digitalRead(inPin);
    digitalWrite(ledPin,LOW);
    Serial.print("ERROR ON STOPBIT");
  }
  Serial.println(data);
}

