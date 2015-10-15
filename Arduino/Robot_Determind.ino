/*-------------------------------------------------------------------------------\
|  FILE       : Robot_Determind |  Robot Determind                               |
|  DEVELOPER  : DiO             |                                                |
|  STAND      : See History     |                                                |
|  VERSION    : 1.00            |                                                |
|--------------------------------------------------------------------------------|
|                                                        | Copyright             |
|                                                        |   PGEE Lomonosov.     |
|                     Robot_Dermind.cs                   |   N. Petrov #31,      |
|                                                        |   5100 G.Oryahovitsa, |
|                                                        |   Bulgaria            |
|--------------------------------------------------------------------------------|
|                              H I S T O R Y                                     |
|                                                                                |
| 150412 DiO  Creation of the file                                               |
| 150413 DiO  Edit and make doc. Update modifiers.                               |
\-------------------------------------------------------------------------------*/

#include <String.h>
#include <stdlib.h>

//////////////////////////////////////////////////////////////////////////
// Motor pin index.
//////////////////////////////////////////////////////////////////////////
#define LEFT_MOTOR_1  4
#define LEFT_MOTOR_2  5
#define RIGHT_MOTOR_1 6
#define RIGHT_MOTOR_2 7
#define MOTORS_ENABLE 9

// Green cable.
#define SENSOR_RIGHT A0
// Brown cable.
#define SENSOR_CENTER A1
// Yellow cable.
#define SENSOR_LEFT A2

//////////////////////////////////////////////////////////////////////////
// LED indication pin number.
//////////////////////////////////////////////////////////////////////////
#define LED_PIN 13

//
String IncommingCommnad = "";

//
boolean Echo = true;

//
char printl[256];

//////////////////////////////////////////////////////////////////////////
// Setup function.
//////////////////////////////////////////////////////////////////////////
void setup()
{
  // Initialize the digital pin as an output.
  pinMode(LED_PIN, OUTPUT);
  pinMode(LEFT_MOTOR_1, OUTPUT);
  pinMode(LEFT_MOTOR_2, OUTPUT);
  pinMode(RIGHT_MOTOR_1, OUTPUT);
  pinMode(RIGHT_MOTOR_2, OUTPUT);
  
  // Setup Serial library at 9600 bps.
  Serial.begin(9600);           
  Serial.println("Tamagochi - Robot test!");
  
}

//////////////////////////////////////////////////////////////////////////  
// Loop function.
//////////////////////////////////////////////////////////////////////////
void loop()
{
  ReadCommand();
  //MotorTest();
}

//////////////////////////////////////////////////////////////////////////  
// Read incomming data from the serial buffer.
//////////////////////////////////////////////////////////////////////////  
void ReadCommand()
{
  // Fill the command data buffer with command.
  while(Serial.available())
  {
    // Add new char.
    IncommingCommnad += (char)Serial.read();
    // Wait a while for a a new char.
    delay(5);
  }
  
  // If command if not empty parse it.
  if(IncommingCommnad != "")
  {
    boolean isValid = ValidateCommand(IncommingCommnad);
    if(isValid)
    {
      ParseCommand(IncommingCommnad);
    }
    // Print command for feedback.
    if(Echo == true)
    {
      Serial.print("Cmd: ");
      Serial.println(IncommingCommnad);
    }
  }
  
  // Clear the command data buffer.
  IncommingCommnad = ""; 
}

//////////////////////////////////////////////////////////////////////////  
// Validate incomming command from the serial buffer.
//////////////////////////////////////////////////////////////////////////  
boolean ValidateCommand(String command)
{
  //
  // ?R+255\n
  // ||||||||
  // ||||||\\- Termin
  // ||\\\\--- Degree value [-180 : +180].
  // ||\------ Direction index .
  // |\------- Motion type - in this case rotation.
  // \-------- Start symbol.
  //
  // ?M+255\n
  // ||||||||
  // ||||||\\- Termin
  // ||\\\\--- Degree value [-999 : +999].
  // ||\------ Direction index .
  // |\------- Motion type - in this case move.
  // \-------- Start symbol.
  //
  // Package lenght 1 + 1 + 1 + 3 + 1 = 7 bytes;
  //
  
  
  boolean state = false;
  // Number value.
  int numValue = 0;
  
  // Convert commands from string to numbers.
  numValue = atoi(command.substring(3, 6).c_str());
  
  if(command[0] == '?' && command[6] == '\n')
  {
    if(command[2] == '-' || command[2] == '+')
    {
      if(command[1] == 'M')
      {
        if(numValue <= 255 && numValue >= -255)
        {
          // If is valid.
          state = true;
        }
      }
      if(command[1] == 'R')
      {
        if(numValue <= 255 && numValue >= -255)
        {
          // If is valid.
          state = true;
        } 
      }
    }
  }
  
  if(command == "?SENSORS\n")
  {
    // If is valid.
    state = true;
  }
  
  // If is not valid.
  return state;
}

//////////////////////////////////////////////////////////////////////////
// Parse the serial command.
//////////////////////////////////////////////////////////////////////////
void ParseCommand(String command)
{
  // Number value.
  int numValue = 0;
  
  // Convert commands from string to numbers.
  numValue = atoi(command.substring(3, 6).c_str());
  
  if(command[1] == 'M')
  {
    if(command[2] == '-')
    {
      // Set direction.
      digitalWrite(LEFT_MOTOR_1, LOW);
      digitalWrite(LEFT_MOTOR_2, HIGH);
      digitalWrite(RIGHT_MOTOR_1, LOW);
      digitalWrite(RIGHT_MOTOR_2, HIGH);
     // Speed [000 - 255].
      analogWrite(MOTORS_ENABLE, numValue);
    }
    else if(command[2] == '+')
    {
      // Set direction.
      digitalWrite(LEFT_MOTOR_1, HIGH);
      digitalWrite(LEFT_MOTOR_2, LOW);
      digitalWrite(RIGHT_MOTOR_1, HIGH);
      digitalWrite(RIGHT_MOTOR_2, LOW);
     // Speed [000 - 255].
      analogWrite(MOTORS_ENABLE, numValue);
    }
    // Shut down the bridge.
    if(numValue == 0)
    {
      // Set direction.
      digitalWrite(LEFT_MOTOR_1, LOW);
      digitalWrite(LEFT_MOTOR_2, LOW);
      digitalWrite(RIGHT_MOTOR_1, LOW);
      digitalWrite(RIGHT_MOTOR_2, LOW);
     // Speed [000 - 255].
      analogWrite(MOTORS_ENABLE, numValue);
    }
  }
  else if(command[1] == 'R')
  {
    if(command[2] == '-')
    {
      // Set direction.
      digitalWrite(LEFT_MOTOR_1, LOW);
      digitalWrite(LEFT_MOTOR_2, HIGH);
      digitalWrite(RIGHT_MOTOR_1, HIGH);
      digitalWrite(RIGHT_MOTOR_2, LOW);
     // Speed [000 - 255].
      analogWrite(MOTORS_ENABLE, numValue);
    }
    else if(command[2] == '+')
    {
      // Set direction.
      digitalWrite(LEFT_MOTOR_1, HIGH);
      digitalWrite(LEFT_MOTOR_2, LOW);
      digitalWrite(RIGHT_MOTOR_1, LOW);
      digitalWrite(RIGHT_MOTOR_2, HIGH);
     // Speed [000 - 255].
      analogWrite(MOTORS_ENABLE, numValue);
    }
    // Shut down the bridge.
    if(numValue == 0)
    {
      // Set direction.
      digitalWrite(LEFT_MOTOR_1, LOW);
      digitalWrite(LEFT_MOTOR_2, LOW);
      digitalWrite(RIGHT_MOTOR_1, LOW);
      digitalWrite(RIGHT_MOTOR_2, LOW);
     // Speed [000 - 255].
      analogWrite(MOTORS_ENABLE, numValue);
    }
  }
  else if(command == "?SENSORS\n")
  {
    // read the analog in value:
    int rightSensor = analogRead(SENSOR_RIGHT);
    int centralSensor = analogRead(SENSOR_CENTER);
    int leftSensor = analogRead(SENSOR_LEFT);
    
    sprintf(printl, "L:%d;C:%d;R:%d", leftSensor, centralSensor, rightSensor);
    
    Serial.println(printl);
  }
}

//////////////////////////////////////////////////////////////////////////
// Scale the value from the sensor.
//////////////////////////////////////////////////////////////////////////
float scaleSensor(int value)
{
  return 0.0;
}
