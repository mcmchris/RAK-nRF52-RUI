
uint8_t ledPin = 36;
uint8_t inputPin = 13;

void setup()
{
    Serial.begin(115200);
    delay(5000);
    Serial.println("RAKwireless Arduino Digital Example");
    Serial.println("------------------------------------------------------");
    
    pinMode(ledPin, OUTPUT);
    pinMode(inputPin, INPUT_PULLUP);
}

void loop()
{
    int val = digitalRead(inputPin);
    if (val == LOW)
        digitalWrite(ledPin, HIGH);
    else
        digitalWrite(ledPin, LOW);
}
