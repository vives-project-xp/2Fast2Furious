#include <ArduinoBLE.h>
#include <Servo.h>

Servo myservo;
int pos = 50;       // startpositie servo
int servoState = 0; // 0 = stop, 1 = omhoog, -1 = omlaag
unsigned long lastServoMove = 0;

#define SPEED_PIN 4             // De enige overgebleven pin voor PWM (snelheidsregeling)
const int SNELHEID_MAX = 170;   // Maximale snelheid voor vooruit/achteruit/links/rechts
const int SNELHEID_DRAAI = 75; // Lagere snelheid voor draaiCW/draaiCCW

#define LazerPin 7

// Motor 1
#define M1_IN1 2
#define M1_IN2 3

// Motor 2
#define M2_IN1 5
#define M2_IN2 6

// Motor 3
#define M3_IN1 10
#define M3_IN2 11

// Motor 4
#define M4_IN1 13
#define M4_IN2 8

// BLE
BLEDevice peripheral;
BLECharacteristic commandChar;
bool connected = false;
String prevCommand = "";
bool tankmode = false;

void setMotorSpeed(int speed_value)
{
  analogWrite(SPEED_PIN, speed_value);
}

void motorVooruit(int in1, int in2)
{
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
}

void motorAchteruit(int in1, int in2)
{
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
}

void motorStop(int in1, int in2)
{
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
}

void vooruit()
{
  setMotorSpeed(SNELHEID_MAX);
  motorVooruit(M1_IN1, M1_IN2);
  motorVooruit(M2_IN1, M2_IN2);
  motorVooruit(M3_IN1, M3_IN2);
  motorVooruit(M4_IN1, M4_IN2);
}
void achteruit()
{
  setMotorSpeed(SNELHEID_MAX);
  motorAchteruit(M1_IN1, M1_IN2);
  motorAchteruit(M2_IN1, M2_IN2);
  motorAchteruit(M3_IN1, M3_IN2);
  motorAchteruit(M4_IN1, M4_IN2);
}
void links()
{
  setMotorSpeed(SNELHEID_MAX);
  motorAchteruit(M1_IN1, M1_IN2);
  motorVooruit(M2_IN1, M2_IN2);
  motorAchteruit(M3_IN1, M3_IN2);
  motorVooruit(M4_IN1, M4_IN2);
}
void rechts()
{
  setMotorSpeed(SNELHEID_MAX);
  motorVooruit(M1_IN1, M1_IN2);
  motorAchteruit(M2_IN1, M2_IN2);
  motorVooruit(M3_IN1, M3_IN2);
  motorAchteruit(M4_IN1, M4_IN2);
}

void draaiCW()
{
  setMotorSpeed(SNELHEID_DRAAI);
  motorAchteruit(M1_IN1, M1_IN2);
  motorVooruit(M2_IN1, M2_IN2);
  motorVooruit(M3_IN1, M3_IN2);
  motorAchteruit(M4_IN1, M4_IN2);
}

void frontleft(){
    setMotorSpeed(SNELHEID_MAX);
  motorVooruit(M2_IN1, M2_IN2);
  motorVooruit(M4_IN1, M4_IN2);
  }

void frontright(){
    setMotorSpeed(SNELHEID_MAX);
    motorVooruit(M1_IN1, M1_IN2);
    motorVooruit(M3_IN1, M3_IN2);
  
  }

void backleft(){
    setMotorSpeed(SNELHEID_MAX);
      motorAchteruit(M1_IN1, M1_IN2);
      motorAchteruit(M3_IN1, M3_IN2);
  }

void backright(){
    setMotorSpeed(SNELHEID_MAX);
    motorAchteruit(M2_IN1, M2_IN2);
    motorAchteruit(M4_IN1, M4_IN2);
  }

void ChangeState()
{
  tankmode = !tankmode;
}

void draaiCCW()
{
  setMotorSpeed(SNELHEID_DRAAI);
  motorVooruit(M1_IN1, M1_IN2);
  motorAchteruit(M2_IN1, M2_IN2);
  motorAchteruit(M3_IN1, M3_IN2);
  motorVooruit(M4_IN1, M4_IN2);
}

void stopAlles()
{
  setMotorSpeed(0);

  motorStop(M1_IN1, M1_IN2);
  motorStop(M2_IN1, M2_IN2);
  motorStop(M3_IN1, M3_IN2);
  motorStop(M4_IN1, M4_IN2);
}

void setup()
{
  Serial.begin(9600);

  myservo.attach(9);
  myservo.write(pos);

  pinMode(SPEED_PIN, OUTPUT);
  setMotorSpeed(0);

  // Motorpinnen
  pinMode(M1_IN1, OUTPUT);
  pinMode(M1_IN2, OUTPUT);
  pinMode(M2_IN1, OUTPUT);
  pinMode(M2_IN2, OUTPUT);
  pinMode(M3_IN1, OUTPUT);
  pinMode(M3_IN2, OUTPUT);
  pinMode(M4_IN1, OUTPUT);
  pinMode(M4_IN2, OUTPUT);

  pinMode(LazerPin, OUTPUT);

  // BLE setup
  if (!BLE.begin())
  {
    Serial.println("BLE start failed!");
    while (1)
      ;
  }

  Serial.println("Central start, zoekt naar NanoController...");
  BLE.scan();
}

void loop(){
  
  if (!connected)
    {
      stopAlles();
      BLEDevice found = BLE.available();
      Serial.println("Searching nano");

      if (found && found.localName() == "NanoClassifier")
      {
        Serial.println("Peripheral gevonden, verbinden...");
        BLE.stopScan();

        if (found.connect())
        {
          Serial.println("Verbonden!");
          peripheral = found;
          connected = true;

          if (peripheral.discoverAttributes())
          {
            commandChar = peripheral.characteristic("abcdef01-1234-5678-1234-56789abcdef0");
            if (commandChar)
            {
              commandChar.subscribe();
              Serial.println("Subscribed op command characteristic!");
            }
          }
        }
        else
        {
          Serial.println("Verbinding mislukt, opnieuw scannen...");
          BLE.scan();
        }
      }
    }

    if (connected && commandChar)
    {
      BLE.poll();

      if (commandChar.valueUpdated())
      {
        uint8_t buffer[20];
        int length = commandChar.readValue(buffer, sizeof(buffer));

        String cmd = "";
        if (length > 0)
        {
          buffer[length] = '\0';
          cmd = String((char *)buffer);
          Serial.print("Ontvangen commando: ");
          Serial.println(cmd);
        }

        if (cmd != prevCommand)
        {
          Serial.println(tankmode);
          prevCommand = cmd;

          if (cmd == "F" && tankmode == false)
          {
            vooruit();
            Serial.println("Vooruit");
          }
          else if (cmd == "C")
          {
            ChangeState();
            Serial.println("Change Mode");
          }
          else if (cmd == "B" && tankmode == false)
          {
            achteruit();
            Serial.println("Achteruit");
          }
          else if (cmd == "L" && tankmode == false)
          {
            links();
            Serial.println("Links");
          }
          else if (cmd == "R" && tankmode == false)
          {
            rechts();
            Serial.println("Rechts");
          }
          else if (cmd == "FL" && tankmode == false)
          {
            frontleft();
            Serial.println("Rechts");
          }
          else if (cmd == "FR" && tankmode == false)
          {
            frontright();
            Serial.println("Rechts");
          }
          else if (cmd == "BL" && tankmode == false)
          {
            backleft();
            Serial.println("Rechts");
          }
          else if (cmd == "BR" && tankmode == false)
          {
            backright();
            Serial.println("Rechts");
          }
          else if (cmd == "S")
          {
            Serial.println("Alles stoppen (ook servo)");
            stopAlles();
            servoState = 0;
          }
          else if (cmd == "F" && tankmode == true)
          {
            Serial.println("Servo omhoog");
            servoState = 1;
          }

          else if (cmd == "B" && tankmode == true)
          {
            Serial.println("Servo omlaag");
            servoState = -1;
          }
          else if (cmd == "L" && tankmode == true)
          {
            draaiCCW();
            Serial.println("Link in Tank Mode");
            }
          else if (cmd == "R" && tankmode == true)
          {
            draaiCW();
            Serial.println("Rechts in Tank Mode");
            }

          else if (cmd == "P")
          {
            Serial.println("Schieten");
            digitalWrite(LazerPin , HIGH);
            delay(3000);
            digitalWrite(LazerPin , LOW);
          }
          else
            stopAlles();
        }
      }

      if (!peripheral.connected())
      {
        Serial.println("Verbinding verbroken, opnieuw scannen...");
        connected = false;
        BLE.scan();
      }
    }
        //Serial.print("Servo Positie");
        //Serial.println(pos);
    if (servoState != 0)
    {
      unsigned long now = millis();

      if (now - lastServoMove >= 20)
      {
        lastServoMove = now;

        if (servoState == 1)
        {
          if (pos < 75)
          {
            pos++;
            myservo.write(pos);
            if (pos >= 75) servoState = 0;
          }
          else
          {
            servoState = 0;
          }
        }

        else if (servoState == -1)
        {
          if (pos > 25)
          {
            pos--;
            myservo.write(pos);
            if (pos <= 25) servoState = 0;
          }
          else
          {
            servoState = 0;
          }
        }
      }
    }
  // test voor alle modes na elkaar
  /*
  delay(5000);
  vooruit();
  Serial.println("Vooruit");
  delay(2000);

  stopAlles();
  delay(1000);

  achteruit();
  Serial.println("Achteruit");
  delay(2000);

  stopAlles();
  delay(1000);

  links();
  Serial.println("Links");
  delay(2000);

  stopAlles();
  delay(1000);

  rechts();
  Serial.println("Rechts");
  delay(2000);

  stopAlles();
  delay(1000);

  draaiCCW();
  Serial.println("Links in Tank Mode");
  delay(2000);

  stopAlles();
  delay(1000);

  draaiCW();
  Serial.println("Rechts in Tank Mode");
  delay(2000);

  stopAlles();*/
}
