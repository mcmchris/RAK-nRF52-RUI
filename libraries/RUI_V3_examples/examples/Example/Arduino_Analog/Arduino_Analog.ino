
void setup()
{
    Serial.begin(115200);
    delay(5000);
    Serial.println("RAKwireless Arduino Analog Example");
    Serial.println("------------------------------------------------------");
}

void loop()
{
    uint16_t adc_value = analogRead(UDRV_ADC_PIN_1);
    Serial.printf("ADC pin1 value! %u\r\n", adc_value);
    delay(1000);
}
