#include <MeMCore.h>

#define IR_RIGHT A0 //depend on port connected
#define IR_LEFT A1 //depend on port connected

MeDCMotor leftMotor(M1);
MeDCMotor rightMotor(M2);
int setOutput = 999;// will be obtain through experiment
int leftSpeed = 0;
int rightSpeed = 0;

int get_IR_reading(int pin)
{
  return analogRead(pin);
}

void setup() {
  // put your setup code here, to run once:
  pinMode(IR_LEFT, INPUT);
  pinMode(IR_LEFT, OUTPUT);
}

void loop() 
{
  // put your main code here, to run repeatedly:
  int outputLeft = get_IR_reading(IR_LEFT);
  int outputRight = get_IR_reading(IR_RIGHT);
  if (outputLeft < setOutput)
  {
    leftSpeed += (setOutput - outputLeft);
  }
  else if (outputRight < setOutput)
  {
    rightSpeed += (setOutput - outputRight);
  }

  leftMotor.run(leftSpeed);
  rightMotor.run(rightSpeed);

  leftSpeed -= (setOutput - outputLeft);
  rightSpeed -= (setOutput - outputRight);

}
