#include <Servo.h>

#define DOWN -1
#define UP 1


class HwSwitch {
  public:
    String Name;
    int SwitchPort;
    int _pressedState;
    int _lastCheckMillis;
    int _lastPinState;
    HwSwitch(String, int, int, int);
    bool IsPressed();
    bool SwitchStateChanged();
    int GetPinState();
};

class HwServo {
  public:
    HwServo();
    HwServo(int, int, HwSwitch &);
    int ServoPort;
    int ZeroPoint;
    HwSwitch & LimitSwitch;
    void RotateUp();
    void RotateDown();
    void Stop();
    Servo _servo;
};

HwSwitch::HwSwitch(String switchName, int switchPort, int inputType, int pressedState)
{ 
    Name = switchName;
    SwitchPort = switchPort;
    _pressedState = pressedState;
    _lastCheckMillis = 0;

    pinMode(switchPort, inputType);
    _lastPinState = digitalRead(SwitchPort);
}

bool HwSwitch::IsPressed()
{
    int currentPinState = GetPinState();
    return currentPinState == _pressedState;
}

bool HwSwitch::SwitchStateChanged()
{
    int currentPinState = GetPinState();
    if (_lastPinState != currentPinState)
    {
        Serial.println("---");
        Serial.println("1. Now: " + String(currentPinState) + " - Prev: " + String(_lastPinState));

        _lastPinState = currentPinState;
        Serial.println("2. Now: " + String(currentPinState) + " - Prev: " + String(_lastPinState));

        return true;
    }

    return false;
}

int HwSwitch::GetPinState()
{
    unsigned long ms = millis();
    if ((ms - _lastCheckMillis) < 50)
    {
        return _lastPinState;
    }

    _lastCheckMillis = ms;
    return digitalRead(SwitchPort);
}

HwServo::HwServo(int servoPort, int zeroPoint, HwSwitch &limitSwitch)
{
    _servo.attach(servoPort);
    _servo.write(zeroPoint);

    ServoPort = servoPort;
    ZeroPoint = zeroPoint;

    LimitSwitch = limitSwitch;
}

void HwServo::RotateUp()
{
    _servo.write(ZeroPoint + UP);
}

void HwServo::RotateDown()
{
    if (!LimitSwitch.IsPressed())
    {
        _servo.write(ZeroPoint + DOWN);
    }
}

void HwServo::Stop()
{
    _servo.write(ZeroPoint);
}

HwServo *HwServos[2];
HwSwitch *s1;
HwSwitch *s2; 
HwServo *sv1; 
HwServo *sv2;

void SendSwitchStateUpdate(HwSwitch & hws){
  Serial.println("something happened");
}

void setup() {
  // put your setup code here, to run once:
  s1 = new HwSwitch("S1", 14, INPUT_PULLUP, HIGH);
  s2 = new HwSwitch("S2", 8, INPUT_PULLUP, HIGH);
  sv1 = new HwServo(9, 94, *s1);
  sv2 = new HwServo(5, 90, *s2);
  HwServos[0] = sv1;
  HwServos[1] = sv2;
}

void loop() {
  // put your main code here, to run repeatedly:
  for(int i = 0; i < 2; i++)
    {

        if (HwServos[i]->LimitSwitch.SwitchStateChanged())
        {
            SendSwitchStateUpdate(HwServos[i]->LimitSwitch);

            if (HwServos[i]->LimitSwitch.IsPressed())
            {
                HwServos[i]->Stop();
            }
        }
    }
}
