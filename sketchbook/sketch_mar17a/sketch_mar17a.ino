int ENA=2; //Connect on Arduino, Pin 2
int IN1=3; //Connect on Arduino, Pin 3
int IN2=4; //Connect on Arduino, Pin 4
int ENB=5; //Connect on Arduino, Pin 5
int IN3=6; //Connect on Arduino, Pin 6
int IN4=7; //Connect on Arduino, Pin 7
 
void setup() {
 pinMode(ENA,OUTPUT);
 pinMode(ENB,OUTPUT);
 pinMode(IN1,OUTPUT);
 pinMode(IN2,OUTPUT);
 pinMode(IN3,OUTPUT);
 pinMode(IN4,OUTPUT);
 digitalWrite(ENA,HIGH);// Activer moteur A
 digitalWrite(ENB,HIGH);// Activer moteur B
}
void loop(){
 
 digitalWrite(IN1,LOW);
 digitalWrite(IN2,HIGH);
// digitalWrite(IN3,HIGH);
// digitalWrite(IN4,LOW);
 delay(1000);
 digitalWrite(IN1,LOW);
 digitalWrite(IN2,HIGH);
// digitalWrite(IN3,LOW);
// digitalWrite(IN4,HIGH);
 delay(1000);
 digitalWrite(IN1,HIGH);
 digitalWrite(IN2,LOW);
// digitalWrite(IN3,LOW);
// digitalWrite(IN4,HIGH);
 delay(1000);
 digitalWrite(IN1,HIGH);
 digitalWrite(IN2,LOW);
// digitalWrite(IN3,HIGH);
// digitalWrite(IN4,LOW);
 delay(1000);
}
