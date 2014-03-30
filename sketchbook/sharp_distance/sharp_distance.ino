char GP2D12;
char a,b;

void setup()
{
  Serial.begin(9600);
}

void loop()
{
  int val;
 
  GP2D12 = read_gp2d12_range(pin);
  val = a * 10 + b;
  if (val > 10 && val < 80)
  {
    Serial.print(a, DEC);
    Serial.print(b, DEC);
    Serial.println("cm");
  }
  else
  {
    Serial.println("over");
  }
  delay(1000);
}


float read_gp2d12_range(byte pin)
{
  int tmp;
  tmp = analogRead(pin);
  if (tmp < 3)
  {
    return -1;
  }
  return (6787.0 / ((float)tmp - 3.0)) - 4.0;
}
