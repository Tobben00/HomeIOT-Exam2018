Servo garageServo;
int servoPin = D0;
int servoResetPos = 0;
int servoPressPos = 28;
int garageIsOpen = 0;


void setup() 
{
    garageServo.attach(servoPin);
    garageServo.write(servoResetPos);
    Particle.variable("GaVar",garageIsOpen);
    Particle.function("useGarage", garageCommand);
}

void loop() 
{
    Particle.publish("GarageIsOpen?", String(garageIsOpen));
    // Delay slik at den ikke spammer Publish..
    delay(10000);
}

void servoButtonPress()
{
    garageServo.write(servoResetPos);
    delay(300); 
    garageServo.write(servoPressPos);
    delay(1000);
    garageServo.write(servoResetPos);
}

int garageCommand(String command)
{
    servoButtonPress();
    Particle.publish("useGarage");
    
    if(garageIsOpen == 0)
    {
        garageIsOpen = 1;
    }
   else
    {
        garageIsOpen = 0;
    }
        
    return 1;
}