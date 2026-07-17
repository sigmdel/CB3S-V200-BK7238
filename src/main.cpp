/*
 *  See blink_leds.ino for license and attribution.
 */

#include <Arduino.h>

#define MySerial Serial1

//-----------------------------------/
//
// User defined paramaters


#define TOGGLE_TIME 30000 // time in milliseconds that a GPIO pin is toggled ON and OFF

#define LOW_TIME   100    // time in milliseconds the GPIO pin is set LOW when toggling it

#define HIGH_TIME  200    // time in milliseconds the GPIO pin is set HIGH when toggling it

#define TEST_DELAY  5000  // time in milliseconds between tests

//----------------------------------/

/// ---- board pad labels and io pin assignement ---- ////

// Non GPIO pads

#define NC  -1
#define VCC -2
#define GND -3
#define CEN -4
#define RST -5
#define RX  -6
#define TX  -7


const int padCount = 22;  
const int pads[padCount] = {1, 2, 3, 4, 5, 6, 7, 8,         // left edge
                            17, 18, 19, 20, 21, 22,          // bottom edge
                            9, 10, 11, 12, 13, 14, 15, 16};  // right edge

const int gpios[padCount] = {RST, 20, CEN, 22, 6, 8, 9, VCC,         // left edge
                             17, 16, 15, 14, 7, NC,             // bottom edge
                             GND, 23, 0, 1, 24, 26, 10, 11};         // right edge

const int untestedCount = 2;
const int untested[untestedCount] = {PIN_P21, PIN_P28}; // one of these is definitely not used

void printPin(int index) {
  switch (gpios[index]) {
    case TX:
      MySerial.print("GPIO 11 (TX1)");
      break;
    case RX:
      MySerial.print("GPIO 10 (RX1)");
      break;
    case RST:
      MySerial.print("RST");
      break;  
    case CEN:
      MySerial.print("CEN");
      break;
    case GND:
      MySerial.print("GND");
      break;
    case VCC:
      MySerial.print("VCC");
      break;
    case NC:
      MySerial.print("nothing");
      break;
    default:
      MySerial.printf("GPIO %2d", gpios[index]);  
  }
}  

void showAll(void) {
  MySerial.println("Orient the module so that the PCB antenna is visible and at the top.");
  for (int p=0; p < padCount; p++) {
    if (pads[p] == 1)
      MySerial.println("Starting at top of left edge of the module, going down:");
    else if (pads[p] == 17)
      MySerial.println("Starting at left of the bottom edge of the module, going across:");
    else if (pads[p] ==9)
      MySerial.println("Starting at the bottom of the right edge of the module, going up:");
    MySerial.printf("  Pad %2d is connected to ", pads[p]);
    printPin(p);
    MySerial.println();
  }
  MySerial.println("");
}

void toggleGPIO(int pin) {
  if ((pin == PIN_TX1) || (pin == PIN_RX1)) {
    MySerial.flush();
    delay(10);
    MySerial.end();
    delay(50);
  }
  pinMode(pin, OUTPUT);
  unsigned long toggleTimer = millis();
  while (millis() - toggleTimer < TOGGLE_TIME) {
    digitalWrite(pin, LOW);
    delay(LOW_TIME);
    digitalWrite(pin, HIGH);
    delay(HIGH_TIME);
  }
  digitalWrite(pin, LOW);
  pinMode(pin, INPUT); 
  if ((pin == PIN_TX1) || (pin == PIN_RX1)) {
    MySerial.begin(115200);
    delay(3000);
    while (MySerial.available() > 0) 
      int ignore = MySerial.read();
  }  
}

void testPad(int pad) {
  if (pads[pad] == 1)
     MySerial.println("Starting at top of left edge of the module, going down:");
  else if (pads[pad] == 17)
    MySerial.println("Starting at left of the bottom edge of the module, going across:");
  else if (pads[pad] ==9)
    MySerial.println("Starting at the bottom of the right edge of the module, going up:");

  MySerial.printf("\n  Pad %d is connected to ", pads[pad]);
  printPin(pad);
  if (gpios[pad] < 0) 
    MySerial.println("; it will not be tested");
  else {
    MySerial.println(" and will be tested by setting the GPIO mode to OUTPUT and toggling its value between HIGH and LOW");
    toggleGPIO(gpios[pad]);
    MySerial.println("  Stopping test");
  }  
  delay(TEST_DELAY);
}


void setup() {
  MySerial.begin(115200); 
  delay(5000);

  Serial2.end(); // stop TX2 - loggin ?

  MySerial.println("\n\nProject: gpios");

  // sanity check
  if (LT.getChipFamily() != F_BK7238) {
    MySerial.println("This firmware runs on BK7228 microcontrollers only");
    MySerial.println("Will reboot in 25 seconds");
    delay(25000);
    LT.restart();
  }  
  
  MySerial.println("Purpose: Verify gpio pin connections of the CB3S-V200 board pads");
  MySerial.print(  "  Board: ");
  MySerial.println(LT.getBoard());
  MySerial.println("\n");
  
  showAll();
  delay(5000);
  MySerial.println("Test starting");  
}

int padindex = 0;

void loop() {
  testPad(padindex);
  padindex++;
  if (padindex >= padCount) {
    MySerial.println("\nTest completed, restarting.\n\n");
    padindex = 0;
  }
};
