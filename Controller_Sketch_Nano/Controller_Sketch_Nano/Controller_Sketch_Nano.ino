/**
 * @file Controller_Sketch_Nano.ino
 * @brief Controller sketch for Arduino Nano
 * 
 * This sketch reads inputs from the hardware and sends command data to UART.
 * Joystick is used for rudder control, potentiometer for mast offset,
 * and potentiometer for backflap control.
 * Controller inputs are mapped to +/- 20 degrees.
 * 
 * @author Zach Fraser
 * @date 2025-05-07
 */

#define SERIAL_PORT Serial
#define SERIAL_BAUD_RATE 9600
#define TRANSMISSION_PERIOD 50

#define MIN_ANGLE -20 //Extreme left
#define MAX_ANGLE 20 //Extreme right

#define DEADZONE_LOW 480 //Center from left
#define DEADZONE_HIGH 544 //Center from right

/* Map analog input to angle */
int readControl(int raw)
{
  if (raw < DEADZONE_LOW)
  {
    return map(raw, 0, DEADZONE_LOW, MIN_ANGLE, -1);
  }
  else if (raw > DEADZONE_HIGH)
  {
    return map(raw, DEADZONE_HIGH, 1023, 1, MAX_ANGLE);
  }
  else
  {
    return 0;
  }
}

/* Run at startup */
void setup()
{
  SERIAL_PORT.begin(SERIAL_BAUD_RATE);
}

/* Run once per TRANSMISSION_PERIOD - Send control values over UART in JSON format */
void loop()
{
  int rawRudder = analogRead(A1);
  int rawMast = analogRead(A2);
  int rawFlap = analogRead(A3);

  int rudder = readControl(rawRudder);
  int mast = readControl(rawMast);
  int flap = readControl(rawFlap);

  SERIAL_PORT.print("{\"rudder\":");
  SERIAL_PORT.print(rudder);
  SERIAL_PORT.print(",\"mast\":");
  SERIAL_PORT.print(mast);
  SERIAL_PORT.print(",\"flap\":");
  SERIAL_PORT.print(flap);
  SERIAL_PORT.println("}");

  delay(TRANSMISSION_PERIOD);
}
