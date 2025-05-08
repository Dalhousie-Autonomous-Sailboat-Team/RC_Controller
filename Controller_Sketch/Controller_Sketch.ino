/**
 * @file Controller_Sketch.ino
 * @brief Controller sketch for Esplora board
 * 
 * This sketch reads inputs from the hardware and sends command data to UART.
 * Joystick X-axis is used for rudder control, linear potentiometer for mast offset,
 * and buttons for backflap control.
 * Switch 1 toggles UART output on/off.
 * 
 * Use #define to select which serial port to use for UART output.  RGB LED will be lit when UART is enabled.
 * 
 * @author Zach Fraser
 * @date 2025-04-30
 */

#include <Esplora.h>

#define SERIAL_PORT Serial  /* Use Serial for UART connected to USB, use Serial1 for UART connected to TX/RX pins */
#define SERIAL_BAUD_RATE 9600  /* Baud rate for serial communication - Radio configured for 9600 */

#define TRANSMISSION_PERIOD 50  /* Transmission period in milliseconds */

/* Adjust Maximum Values At Controller Extremes */
#define MIN_RUDDER_ANGLE -18 // Full left
#define MAX_RUDDER_ANGLE 18 // Full right
#define MIN_MAST_OFFSET -20 // Full left
#define MAX_MAST_OFFSET 20 // Full right

/* Adjust Deadzones around Zero */
#define RUDDER_LOW_DEADZONE -12 // Deadzone towards left
#define RUDDER_HIGH_DEADZONE 10 // Deadzone towards right
#define MAST_LOW_DEADZONE 480 // Deadzone towards left
#define MAST_HIGH_DEADZONE 520 // Deadzone towards right

/* Adjust to set controller values that correspond to maximum values */
#define RUDDER_LEFT_MIN -508 // Joystick left extreme
#define RUDDER_LEFT_MAX -13 // Joystick center from left
#define RUDDER_RIGHT_MIN 11 // Joystick center from right
#define RUDDER_RIGHT_MAX 508 // Joystick right extreme
#define MAST_LEFT_MIN 0 // Linear pot left extreme
#define MAST_LEFT_MAX 479 // Linear pot center from left
#define MAST_RIGHT_MIN 521 // Linear pot center from right
#define MAST_RIGHT_MAX 1000 // Linear pot right extreme

bool uartEnabled = true;         /* Start with UART output enabled */
bool lastSwitch1State = false;   /* To detect edge of the button press */

void setup()
{
  SERIAL_PORT.begin(SERIAL_BAUD_RATE);  /* Start serial communication */
}

void loop()
{
  /* Check for button press on SWITCH_1 to toggle UART */
  bool currentSwitch1State = (Esplora.readButton(SWITCH_1) == PRESSED);

  /* Detect rising edge (button press) */
  if (currentSwitch1State && !lastSwitch1State)
  {
    uartEnabled = !uartEnabled;
  }
  lastSwitch1State = currentSwitch1State;

  /* Indicate UART status using the red LED */
  Esplora.writeRed(uartEnabled ? 255 : 0);  /* Max brightness if enabled */

  /* Read inputs */
  int rawRudder = Esplora.readJoystickX();  /* Joystick X-axis: -512 to +512 */
  int rawMast = Esplora.readSlider();       /* Linear potentiometer: 0 to 1023 */

  int rudder, mast;

  /* Apply deadzone and map joystick to rudder angle (-18 to +18) */
  if (rawRudder < RUDDER_LOW_DEADZONE)
  {
    rudder = map(rawRudder, RUDDER_LEFT_MIN, RUDDER_LEFT_MAX, MIN_RUDDER_ANGLE, -1);
  }
  else if (rawRudder > RUDDER_HIGH_DEADZONE)
  {
    rudder = map(rawRudder, RUDDER_RIGHT_MIN, RUDDER_RIGHT_MAX, 1, MAX_RUDDER_ANGLE);
  }
  else
  {
    rudder = 0;
  }

  /* Apply deadzone and map linear pot to mast offset (-20 to +20) */
  if (rawMast < MAST_LOW_DEADZONE)
  {
    mast = map(rawMast, MAST_LEFT_MIN, MAST_LEFT_MAX, MIN_MAST_OFFSET, -1);
  }
  else if (rawMast > MAST_HIGH_DEADZONE)
  {
    mast = map(rawMast, MAST_RIGHT_MIN, MAST_RIGHT_MAX, 1, MAX_MAST_OFFSET);
  }
  else
  {
    mast = 0;
  }

  /* Determine backflap direction */
  int flap = 0;
  if (Esplora.readButton(SWITCH_2) == PRESSED)
  {
    flap = -1;
  }
  else if (Esplora.readButton(SWITCH_4) == PRESSED)
  {
    flap = 1;
  }

  /* Output to serial only if UART is enabled */
  /* Send JSON formatted data */
  /* Example: {"rudder":0,"mast":0,"flap":0} */
  if (uartEnabled)
  {
    SERIAL_PORT.print("{\"rudder\":");
    SERIAL_PORT.print(rudder);
    SERIAL_PORT.print(",\"mast\":");
    SERIAL_PORT.print(mast);
    SERIAL_PORT.print(",\"flap\":");
    SERIAL_PORT.print(flap);
    SERIAL_PORT.println("}");
  }

  delay(TRANSMISSION_PERIOD);  /* Wait for a short period before next transmission */
}
