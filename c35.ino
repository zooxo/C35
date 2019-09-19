/*
  ____________________

  - Welcome to C35 -
  Scientific RPN Calculator with OLED display and ATMEGA microcontroller
  Version 1.0, (c) 2019 by deetee/zooxo
  This software is covered by the 3-clause BSD license.

  In January 1972, Hewlett-Packard introduced the HP-35 the first scientific
  calculator and revolutionized the way to calculate. 2011 Pietro de Luca
  created (based on the work of Peter Monta and Jacques Laporte) a port of
  the reverse engineered HP-35 ROM for AVR microcontroller.

  C35 uses this HP35-ROM-Engine to offer the calculation power of the HP-35
  to a simple hardware (Arduino/Genuino Micro, 128x64 OLED display, 16 keys).

  Because of 16 keys only (which is quite less for a scientific calculator)
  C35 offers a menu system with 12 (a dozen) function keys (0...9, ., ENTER).
  Call the menu system by pressing the menu key once or twice.

  KEYBOARD:
    [MENU]             7      8      9
    EEX [CONTRAST+]    4      5      6
    +/- [CONTRAST-]    1      2      3
    CLX                0      .      ENTER

  MENU1 (press MENU once):
                       STO    RCL    /
                       X<>Y   ROT    *
                                     -
                       CLR           +

  MENU2 (press MENU twice):
                       SQRT   X^Y    EXP
                       1/X    LOG    LN
                       arc           PI
                       sin    cos    tan

  CIRCUIT DIAGRAM (Arduino/Genuino Micro):
                            _________
       |--15/SCLK-----SCK--| DISPLAY |--GND
       |--16/MOSI-----SDA--| SSD1306 |--Vcc
       |--6/A7--------RES--| 128x32  |
       |--4/A6---------DC--|  OLED   |
       |--GND----------CS--|___SPI___|
       |
       |-----------2/SDA
       |             |
       |      GND--MENU ... Extra wired menu key (interrupt)
       |
       |----------------------------18/A0
       |----------------------19/A1   |
       |-----------------3/SCL  |     |
       |----------21/A3   |     |     |
       |            |     |     |     |
       |--20/A2-----|-----7-----8-----9
       |            |     |     |     |
       |--7---------EE----4-----5-----6
       |            |     |     |     |
       |--8/A8-----CHS----1-----2-----3
       |            |     |     |     |
       |--9/A9------C-----0-----.---ENTER


  HP-35 DISPLAY AND KEYBOARD:
     _________________________
    |                         |
    |     -1.234567890-12     |
    |_________________________|
    |                         |
    | X^Y  log  ln   e^X  CLR |
    | SQRT arc  sin  cos  tan |
    | 1/X  X><Y ROT  STO  RCL |
    | E-N-T-E-R CHS  EEX  CLX |
    |   -     7     8     9   |
    |   +     4     5     6   |
    |   *     1     2     3   |
    |   /     0     .     PI  |
    |_________________________|
  ____________________

*/


// ***** I N C L U D E S

#include <EEPROM.h> // For saving data to EEPROM


// ***** F O N T

#define FONTOFFSET ' '
#define FONTWIDTH 4
const byte font [] PROGMEM = { // Edged font
  0x00, 0x00, 0x00, 0x00, // space
  0x00, 0x5f, 0x00, 0x00, // !
  0x07, 0x00, 0x07, 0x00, // "
  0x06, 0x7e, 0x02, 0x7e, // # PI
  0x6d, 0x11, 0x11, 0x6d, // $ x mean value
  0x64, 0x48, 0x12, 0x26, // %
  0x63, 0x75, 0x75, 0x63, // & hourglass
  0x04, 0x04, 0x00, 0x1f, // ' ^-1
  0x00, 0x7F, 0x41, 0x00, // ( edged
  0x00, 0x41, 0x7F, 0x00, // ] edged
  0x24, 0x18, 0x18, 0x24, // *
  0x08, 0x1c, 0x08, 0x00, // +
  0x10, 0x7f, 0x01, 0x01, // , squareroot
  0x08, 0x08, 0x08, 0x00, // -
  0xc0, 0x00, 0x00, 0x00, // .
  //0x40, 0x30, 0x0c, 0x02, // /
  0x60, 0x18, 0x06, 0x00, // /
  0x7f, 0x41, 0x41, 0x7f, // 0
  0x00, 0x02, 0x7f, 0x00, // 1
  0x79, 0x49, 0x49, 0x4f, // 2
  0x41, 0x49, 0x49, 0x7f, // 3
  0x0f, 0x08, 0x08, 0x7f, // 4
  0x4f, 0x49, 0x49, 0x79, // 5
  0x7f, 0x49, 0x49, 0x79, // 6
  0x03, 0x01, 0x01, 0x7f, // 7
  0x7f, 0x49, 0x49, 0x7f, // 8
  0x4f, 0x49, 0x49, 0x7f, // 9
  //0x00, 0x14, 0x14, 0x00, // :
  0x17, 0x14, 0x14, 0x1f, // : raised y
  0x1b, 0x04, 0x04, 0x1b, // ; raised x
  0x08, 0x14, 0x22, 0x41, // <
  0x14, 0x14, 0x14, 0x14, // =
  0x41, 0x22, 0x14, 0x08, // >
  0x03, 0x59, 0x09, 0x0f, // ?
  0x3e, 0x3e, 0x22, 0x3e, // @ contrast
  0x7f, 0x09, 0x09, 0x7f, // A
  0x7f, 0x49, 0x4f, 0x78, // B
  0x7f, 0x41, 0x41, 0x40, // C
  0x41, 0x7F, 0x41, 0x7F, // D
  0x7F, 0x49, 0x49, 0x41, // E
  0x7F, 0x09, 0x09, 0x01, // F
  0x7f, 0x41, 0x49, 0x79, // G
  0x7F, 0x08, 0x08, 0x7F, // H
  0x40, 0x41, 0x7F, 0x41, // I
  0x60, 0x40, 0x40, 0x7f, // J
  0x7F, 0x08, 0x0f, 0x78, // K
  0x7F, 0x40, 0x40, 0x40, // L
  0x7F, 0x07, 0x07, 0x7F, // M
  0x7F, 0x06, 0x0c, 0x7F, // N
  0x7f, 0x41, 0x41, 0x7f, // O
  0x7F, 0x09, 0x09, 0x0f, // P
  0x7f, 0x41, 0x61, 0x7f, // Q
  0x7F, 0x09, 0x79, 0x4f, // R
  0x4f, 0x49, 0x49, 0x78, // S
  0x01, 0x01, 0x7F, 0x01, // T
  0x7F, 0x40, 0x40, 0x7F, // U
  0x1F, 0x70, 0x70, 0x1F, // V
  0x7F, 0x70, 0x70, 0x7F, // W
  0x77, 0x1c, 0x1c, 0x77, // X
  0x4f, 0x48, 0x48, 0x7f, // Y
  0x71, 0x59, 0x4d, 0x47, // Z
  0x10, 0x18, 0x1c, 0x10, // [ up arrow
  0x02, 0x0c, 0x30, 0x40, // backslash
  0x04, 0x0c, 0x1c, 0x04, // ] down arrow
  0x04, 0x02, 0x02, 0x04, // ^
  0x7f, 0x63, 0x6f, 0x6b, // _ invertet phi
  0x7f, 0x57, 0x45, 0x7f, // ` inverted i
  0x70, 0x54, 0x54, 0x7c, // a
  0x7F, 0x44, 0x44, 0x7c, // b
  0x7c, 0x44, 0x44, 0x40, // c
  0x7c, 0x44, 0x44, 0x7F, // d
  0x7c, 0x54, 0x54, 0x1c, // e
  0x08, 0x7f, 0x09, 0x01, // f
  0x3c, 0xa4, 0xa4, 0xfc, // g
  0x7F, 0x04, 0x04, 0x7c, // h
  0x00, 0x7D, 0x00, 0x00, // i
  0x00, 0x80, 0xfa, 0x00, // j
  0x7F, 0x10, 0x1c, 0x70, // k
  0x01, 0x7f, 0x40, 0x40, // l
  0x7C, 0x04, 0x04, 0x7c, // m
  0x7C, 0x04, 0x04, 0x7c, // n
  0x7c, 0x44, 0x44, 0x7c, // o
  0xFC, 0x24, 0x24, 0x3c, // p
  0x3c, 0x24, 0x24, 0xfc, // q
  0x7C, 0x04, 0x04, 0x04, // r
  0x5c, 0x54, 0x54, 0x70, // s
  0x04, 0x7F, 0x44, 0x40, // t
  0x7C, 0x40, 0x40, 0x7C, // u
  0x04, 0x7c, 0x40, 0x04, // v
  0x7C, 0x40, 0x40, 0x7C, // w
  0x6c, 0x10, 0x10, 0x6c, // x
  0x3C, 0xA0, 0xA0, 0xfC, // y
  0x74, 0x54, 0x54, 0x5c, // z
  0x08, 0x1c, 0x3e, 0x7f, // { left arrow
  0x77, 0x5d, 0x49, 0x63, // | sum
  0x7f, 0x3e, 0x1c, 0x08, // } right arrow
  0x22, 0x72, 0x27, 0x22  // ~ leftright arrow
};


// *****  D I S P L A Y

// DEFINES
#define SCREENBYTES 256 // Number of bytes to address SCREEN (SCREENWIDTH x SCREENHEIGHT)/8

// PINS, PORTS
// Display
#define CS_PORT PORTD   // CS port
#define CS_BIT PORTD6   // CS physical bit number
#define PIN_DC 4        // DC pin number
#define DC_PORT PORTD   // DC port
#define DC_BIT PORTD4   // DC physical bit number
#define PIN_RST 6       // RST pin number
#define RST_PORT PORTD  // RST port
#define RST_BIT PORTD7  // RST physical bit number
#define SPI_MISO_PORT PORTB
#define SPI_MISO_BIT PORTB3
#define SPI_MOSI_PORT PORTB
#define SPI_MOSI_BIT PORTB2
#define SPI_SCK_PORT PORTB
#define SPI_SCK_BIT PORTB1
#define SPI_SS_PORT PORTB
#define SPI_SS_BIT PORTB0

// VARIABLES
static byte dbuf[SCREENBYTES]; // Buffer for virtual screen (costs 256 bytes of dynamic memory)
static byte eachframemillis, thisframestart, lastframedurationms; // Framing times
static boolean justrendered; // True if frame was just rendered
static boolean isscreensave = false; // True if screensaver is active
static long powertimestamp = 0; // Needed for timing of power manangement
static byte brightness = 0; // Brightness

// SUBPROGRAMS
void SPItransfer(byte data) { // Write to the SPI bus (MOSI pin)
  SPDR = data;
  asm volatile("nop"); // Tiny delay before wait
  while (!(SPSR & _BV(SPIF))) {} // Wait for byte to be sent
}

static void ScreenCommandMode(void) { // Set screen to command mode
  bitClear(DC_PORT, DC_BIT);
}

static void ScreenDataMode(void) { // Set screen to data mode
  bitSet(DC_PORT, DC_BIT);
}

static void bootpins(void) { // Declare and boot port pins
  DDRB  |= _BV(SPI_MOSI_BIT) | _BV(SPI_SCK_BIT) | _BV(SPI_SS_BIT);
  PORTD |= _BV(CS_BIT); // Port D
  PORTD &= ~(_BV(RST_BIT));
  DDRD  |= _BV(RST_BIT) | _BV(CS_BIT) | _BV(DC_BIT);
}

static void bootSPI(void) { // Initialize the SPI interface for the display
  SPCR = _BV(SPE) | _BV(MSTR); // master, mode 0, MSB first, CPU clock / 2 (8MHz)
  SPSR = _BV(SPI2X);
}

const byte PROGMEM ScreenBootProgram[] = { // SSD1306 boot sequence
  // 0xAE, // Display Off
  0xD5, 0xF0, // Set Display Clock Divisor v = 0xF0 - default is 0x80
  // 0xA8, 0x3F, // Set Multiplex Ratio v = 0x3F
  // 0xD3, 0x00, // Set Display Offset v = 0
  // 0x40, // Set Start Line (0)
  0x8D, 0x14, // Charge Pump Setting v = enable (0x14) - default is disabled
  0xA1, // Set Segment Re-map (A0) | (b0001) - default is (b0000)
  0xC8, // Set COM Output Scan Direction
  // 0xDA, 0x12, // Set COM Pins v
  0x81, 0xCF, // Set Contrast v = 0xCF
  0xD9, 0xF1, // Set Precharge = 0xF1
  // 0xDB, 0x40, // Set VCom Detect
  // 0xA4, // Entire Display ON
  // 0xA6, // Set normal/inverse display
  0xAF, // Display On
  0x20, 0x00, // Set display mode = horizontal addressing mode (0x00)
  // 0x21, 0x00, 0x7f, // Set col address range ... needed for SSD1309 controller
  // 0x22, 0x00, 0x07 // Set page address range ... needed for SSD1309 controller
};

static void bootscreen(void) { // Boot screen - reset the display
  delayshort(5); // Reset pin should be low here - let it stay low a while
  bitSet(RST_PORT, RST_BIT); // Set reset pin high to come out of reset
  delayshort(5); // Wait
  bitClear(CS_PORT, CS_BIT); // Select the display (permanently, since nothing else is using SPI)

  ScreenCommandMode(); // Run customized boot-up command sequence
  for (byte i = 0; i < sizeof(ScreenBootProgram); i++)
    SPItransfer(pgm_read_byte(ScreenBootProgram + i));
  ScreenDataMode();
}

static void screenoff(void) { // Shut down the display
  ScreenCommandMode();
  SPItransfer(0xAE); // Display off
  SPItransfer(0x8D); // Disable charge pump
  SPItransfer(0x10);
  delayshort(100);
  bitClear(RST_PORT, RST_BIT); // Set RST to low (reset state)
}

static void screenon(void) { // Restart the display after a displayOff()
  bootscreen();
  //setscreencontrast(EEPROM.read(EECONTRAST));
}


// ***** K E Y B O A R D

// PINS
#define KEYBOARDCOL1 21
#define KEYBOARDCOL2 3
#define KEYBOARDCOL3 19
#define KEYBOARDCOL4 18
#define KEYBOARDROW1 20
#define KEYBOARDROW2 7
#define KEYBOARDROW3 8
#define KEYBOARDROW4 9
/*#define KEYBOARDCOL1 5
  #define KEYBOARDCOL2 3
  #define KEYBOARDCOL3 2
  #define KEYBOARDCOL4 18
  #define KEYBOARDROW1 10
  #define KEYBOARDROW2 7
  #define KEYBOARDROW3 8
  #define KEYBOARDROW4 9*/

// DEFINES
#define MAGICKEYPIN 2 // Pin of magic key
#define KEY1 '?' // SHIFT    1-?  2-7  3-8  4-9
#define KEY2 '7' //          5-<  6-4  7-5  8-6
#define KEY3 '8' //          9-> 10-1 11-2 12-3
#define KEY4 '9' //         13-= 14-0 15-: 16-;
#define KEY5 '<' // EE
#define KEY6 '4'
#define KEY7 '5'
#define KEY8 '6'
#define KEY9 '>' // CHS
#define KEY10 '1'
#define KEY11 '2'
#define KEY12 '3'
#define KEY13 '=' // CLX
#define KEY14 '0'
#define KEY15 ':' // DOT
#define KEY16 ';' // ENTER

// VARIABLES
static byte key; // Holds entered key
static byte oldkey = NULL; // Holds oldkey - to prevent keyrepeat

// SUBPROGRAMS
static byte getkey() {
  pinMode(MAGICKEYPIN, INPUT_PULLUP);
  if (!digitalRead(MAGICKEYPIN)) return (KEY1); // F-key pressed
  byte pinscol[4] = {KEYBOARDCOL1, KEYBOARDCOL2, KEYBOARDCOL3, KEYBOARDCOL4}; // Pins
  byte pinsrow[4] = {KEYBOARDROW1, KEYBOARDROW2, KEYBOARDROW3, KEYBOARDROW4};
  char k[4][4] = {
    {NULL, KEY2, KEY3, KEY4},
    {KEY5, KEY6, KEY7, KEY8},
    {KEY9, KEY10, KEY11, KEY12},
    {KEY13, KEY14, KEY15, KEY16},
  };
  byte kee = NULL;
  for (byte r = 0; r < 4; r++) pinMode(pinsrow[r], INPUT_PULLUP); // Set rows to Vcc
  for (byte c = 0; c < 4; c++) {
    pinMode(pinscol[c], OUTPUT); // Set cols as OUTPUT and to GND
    digitalWrite(pinscol[c], LOW);
    for (byte r = 0; r < 4; r++) if (!digitalRead(pinsrow[r])) kee = (k[r][c]); //Assign key
    digitalWrite(pinscol[c], HIGH); // Reset cols to Vcc and INPUT
  }
  return (kee);
  //if (kee) return (kee);
  //else if (!digitalRead(MAGICKEYPIN)) return (KEY1);
  //else return (NULL);
}


// ***** S Y S T E M

// DEFINES
#define SCREENWIDTH 64 // Virtual screen width
#define SCREENHEIGHT 32 // Virtual screen height
#define MAXLIN 4 // Maximum of lines
#define FRAMERATE 10 // Maximal number of screen refreshes per second (>3)
#define SIZES 0x01 // Printing size
#define SIZEM 0x02 // Printing size
#define SIZEL 0x04 // Printing size
#define EECONTRAST 0 // EEPROM address of brightness (1 byte)

// MACROS
#define _abs(x) ((x<0)?(-x):(x)) // abs()-substitute macro
//#define _sign(x) (((x) < 0) ? -1 : ((x) > 0))
#define _sign(x) (((x) < 0) ? -1 : 1)

// SUBPROGRAMS
static void dbufclr(void) { // Clear display buffer
  //for (int i = 0; i < SCREENBYTES; i++) dbuf[i] = NULL;
  memset(dbuf, NULL, SCREENBYTES); // Memset needs 6 bytes more - but what is faster?
}

static void display(void) { // Print display buffer (64x32) to real screen (128x64)
  for (byte l = 0; l < MAXLIN; l++) { // Four lines
    for (byte k = 0; k < 2; k++) { // Two nibbles (double height)
      for (byte j = 0; j < SCREENWIDTH; j++) {
        byte tmp = expand4bit((dbuf[j + l * SCREENWIDTH] >> (k * 4)) & 0x0f); // Expand 0000abcd
        for (byte i = 0; i < 2; i++) SPItransfer(tmp); // Double width
      }
    }
  }
}

static void setframerate(byte rate) { // Calculate frameduration
  eachframemillis = 1000 / rate;
}

static void setscreencontrast(byte c) { // Set screen contrast
  ScreenCommandMode();
  SPItransfer(0x81);
  SPItransfer(c);
  ScreenDataMode();
  EEPROM.write(EECONTRAST, c);
}

static void idle(void) { // Idle, while waiting for next frame
  /*SMCR = _BV(SE); // Select idle mode and enable sleeping
    sleep_cpu();
    SMCR = 0; // Disable sleeping
  */
}

static bool nextFrame(void) { // Wait (idle) for next frame
  byte now = (byte) millis(), framedurationms = now - thisframestart;
  if (justrendered) {
    lastframedurationms = framedurationms;
    justrendered = false;
    return false;
  }
  else if (framedurationms < eachframemillis) {
    if (++framedurationms < eachframemillis) idle();
    return false;
  }
  justrendered = true;
  thisframestart = now;
  return true;
}

static byte expand4bit(byte b) { // 0000abcd  Expand 4 bits (lower nibble)
  b = (b | (b << 2)) & 0x33;     // 00ab00cd
  b = (b | (b << 1)) & 0x55;     // 0a0b0c0d
  return (b | (b << 1));         // aabbccdd
}

static void printcat(byte c, boolean bitshift, byte w, byte h, byte x, byte y) {
  // Print char c (one bit shifted down) with width (1, 2, 4), size (1, 2) at position (x, y)
  byte printbitshift = bitshift ? 1 : 0;
  for (byte k = 0; k < h; k++) {
    for (int i = 0; i < FONTWIDTH; i++) {
      byte tmp = pgm_read_byte(&font[FONTWIDTH * (c - FONTOFFSET) + i]) << printbitshift;
      if (h == 2) tmp = expand4bit(tmp >> (4 * k) & 0x0f);
      for (byte j = 0; j < w; j++)
        dbuf[x + (w * i + j) + (y + k) * SCREENWIDTH] = tmp;
    }
  }
}

static void printsat(char * s, boolean bitshift, byte w, byte h, byte x, byte y) { // Print sized string s at (x|y)
  for (byte i = 0; i < strlen(s); i++)
    printcat(s[i], bitshift, w, h, x + i * (FONTWIDTH + 1) * w, y);
}

static void delayshort(byte ms) { // Delay (with timer) in ms with 8 bit duration
  long t = millis();
  while ((byte)(millis() - t) < ms) ;
}

static double _pow10(int8_t e) { // Calculates 10 raised to the power of e
  double f = 1.0;
  if (e > 0) while (e--) f *= 10;
  else while (e++) f /= 10;
  return (f);
}

static void wakeupnow() {} // Dummy wakeup code


// ***** HP-35-ENGINE by Pietro de Luca

const byte rom[] PROGMEM = { // HP35-ROM, 768 bytes
  0, 221, 2, 255, 2, 36, 0, 23, 1, 68, 2, 68, 0, 132, 1, 16, 2, 209, 3, 251, 0, 95, 0, 195, 1, 168,
  3, 103, 2, 238, 3, 226, 0, 46, 0, 144, 3, 234, 3, 234, 3, 234, 0, 107, 2, 105, 0, 168, 2, 168, 0,
  255, 3, 234, 3, 234, 3, 234, 0, 48, 0, 204, 0, 170, 1, 168, 0, 67, 1, 211, 0, 204, 0, 48, 0,  0,
  0, 131, 1, 68, 0, 68, 0, 187, 2, 68, 0, 159, 2, 132, 3, 11, 0, 46, 0, 144, 3, 40, 3, 111, 3, 234,
  3, 234, 3, 234, 0, 75, 2, 103, 3, 168, 1, 113, 3, 119, 3, 203, 2, 206, 0, 196, 1, 219, 1, 40, 0,
  52, 2, 206, 3, 117, 1, 46, 2, 250, 1, 22, 3, 106, 3, 131, 1, 186, 3, 155, 3, 54, 3, 76, 3, 155,
  0, 28, 1, 234, 0,  2, 1, 51, 2, 196, 2, 214, 3, 166, 1, 20, 2, 31, 1, 125, 3, 119, 0, 210, 1,
  114, 0, 218, 3, 138, 1, 119, 0, 206, 0, 52, 1, 142, 3, 12, 1, 42, 1, 138, 1, 186, 1, 163, 0, 170,
  1, 122, 1, 95, 1, 76, 3, 170, 1, 20, 1, 11, 3, 42, 0, 42, 3, 221, 1, 10, 2, 206, 3, 44, 2, 39, 3,
  178, 1, 235, 2, 209, 0, 144, 1, 20, 3, 219, 3, 178, 0, 250, 1, 142, 1, 186, 1, 255, 0, 218, 0,
  170, 3, 76, 1, 22, 1, 106, 2, 126, 1, 59, 2, 118, 2,  3, 0, 202, 3, 221, 2, 214, 1, 158, 3, 44,
  2, 79, 0, 142, 1, 238, 0, 76, 1, 18, 0, 60, 1, 162, 2, 63, 3, 174, 0, 236, 3, 231, 0, 202, 1,
  132, 1, 235, 0, 254, 1, 168, 0, 46, 3, 250, 3, 250, 1, 250, 1, 250, 0, 74, 2, 143, 3, 174, 3,
  166, 1, 166, 2, 159, 3, 174, 2, 38, 0, 74, 2, 251, 2, 142, 3, 234, 0, 14, 2, 251, 2, 163, 2, 246,
  0, 212, 2, 211, 3, 126, 0, 254, 1, 212, 2, 223, 1, 40, 1, 196, 0, 206, 1, 110, 0, 190, 1, 254, 2,
  46, 0, 48, 0, 144, 1, 113, 1, 68, 3, 119, 2, 206, 1, 158, 2, 36, 3, 63, 1, 250, 2,  4, 1, 84, 3,
  55, 1, 234, 3, 27, 0, 40, 0, 20, 3, 31, 0, 36, 0, 28, 3, 44, 3, 67, 2, 40, 2, 20, 3, 51, 1, 14,
  1, 100, 0, 208, 1, 40, 3, 174, 1, 117, 1, 196, 3, 221, 2, 189, 2, 43, 2, 214, 0, 28, 0, 172, 1,
  23, 3, 12, 2, 238, 2, 246, 3, 226, 3, 226, 0, 140, 0, 60, 3, 98, 3, 191, 0,  2, 3, 171, 3, 226,
  3, 46, 0, 48, 1,  4, 2, 212, 0, 115, 1, 191, 0, 254, 2, 164, 3, 15, 1, 148, 3, 243, 0, 28, 2,
  146, 1, 233, 2, 168, 3, 111, 3, 207, 3, 46, 0, 161, 1, 168, 0, 161, 1, 168, 2, 84, 0, 39, 3, 174,
  1, 84, 0, 75, 0, 222, 2, 153, 1, 40, 2, 149, 2, 97, 0, 149, 1, 168, 2, 153, 2, 148, 3, 107, 2,
  238, 3, 226, 1, 38, 3, 166, 1, 106, 2, 146, 1, 186, 0, 103, 2, 210, 1, 234, 0, 119, 2, 206, 2,
  142, 1, 40, 2, 46, 1,  7, 2, 46, 1, 12, 3, 123, 1, 40, 3, 174, 1, 162, 0, 183, 0, 174, 1, 142, 0,
  138, 3, 47, 1, 142, 0, 84, 0, 151, 2, 148, 1, 183, 1, 84, 0, 87, 0, 254, 3, 190, 0, 55, 2, 146,
  3, 126, 0, 235, 1, 254, 3, 50, 1, 210, 3, 46, 1, 46, 3, 82, 0, 239, 1, 168, 2, 206, 3, 178, 3,
  46, 1, 18, 1, 40, 3, 254, 3, 254, 0, 143, 0, 206, 0, 42, 2, 214, 2, 201, 1, 98, 1, 168, 3, 174,
  1, 12, 2, 145, 1, 140, 2, 109, 2, 12, 2, 109, 0, 140, 2, 24, 2, 140, 2, 109, 2, 57, 2, 109, 3,
  49, 1, 14, 2, 109, 0, 142, 3, 45, 3, 49, 2, 174, 2, 153, 2, 84, 1, 179, 0, 254, 2, 97, 0, 100, 0,
  206, 1, 98, 1, 234, 0, 84, 2, 151, 2, 153, 3, 49, 2, 174, 2, 149, 3, 49, 2, 174, 2, 174, 2, 85, 2,
  174, 3, 173, 3, 49, 2, 140, 2, 113, 2, 57, 2, 12, 2, 117, 0, 140, 2, 24, 1, 140, 2, 113, 1, 12, 2,
  113, 2, 113, 3, 46, 2, 78, 3, 76, 1, 88, 3, 239, 1, 140, 2, 24, 1, 152, 1, 88, 0, 152, 1, 24, 2,
  88, 0, 84, 3, 107, 0, 48, 2, 238, 3, 226, 0, 16, 1, 16, 1, 14, 2, 150, 2, 46, 2, 135, 1, 254, 3,
  14, 2, 131, 3, 142, 1, 16, 1, 16, 1, 74, 1, 16, 1, 226, 3, 78, 2, 163, 3, 206, 1, 14, 0, 28, 2,
  82, 0, 44, 2, 167, 0, 183, 1, 226, 3, 22, 2, 203, 3, 150, 1, 22, 0, 28, 0, 44, 2, 207, 0, 183, 0,
  28, 3, 150, 3, 111, 0, 16, 1, 122, 1, 122, 2, 234, 3, 94, 2, 126, 3, 27, 1, 16, 2,  6, 3, 43, 0,
  254, 3, 46, 3, 14, 1, 16, 0, 206, 2, 204, 1, 216, 2, 24, 1, 88, 0, 216, 2, 88, 2, 24, 0, 88, 1,
  152, 0, 216, 1, 88, 3, 12, 0, 48, 0, 16, 3, 138, 3, 123, 1, 98, 1, 254, 0, 44, 2, 239, 3, 170, 2,
  234, 0, 98, 3, 155, 2, 206, 2, 78, 2, 42, 0, 202, 3, 12, 2, 187, 1, 16, 2, 146, 2, 146, 1, 126,
  3, 179, 1, 210, 3, 18, 2, 50, 0, 142, 3, 126, 3, 187, 3, 178, 1, 168, 0, 30, 0,  7, 1, 14, 3, 178,
  1, 40, 2, 146, 1, 126, 2, 62, 0, 16, 3, 62, 3, 254, 2, 86, 1, 18, 0, 75, 1, 168, 2, 153, 1, 142,
  2, 20, 1, 11, 2, 238, 3, 70, 0,  3, 2, 206, 1, 126, 0,  3, 1, 254, 1, 46, 2, 89, 3, 98, 0, 71, 3,
  50, 3, 158, 0,  7, 1, 204, 1, 181, 2, 12, 2, 117, 2, 76, 2, 113, 3, 249, 2, 140, 2, 113, 1, 245,
  2, 204, 2, 113, 3, 125, 2, 113, 2, 229, 2, 113, 3, 217, 3, 174, 1, 78, 0, 26, 0, 191, 1, 78, 3,
  46, 0, 28, 1, 14, 0, 108, 0, 195, 3, 174, 1, 190, 0, 227, 0, 230, 1, 234, 2, 204, 3, 21, 2, 84, 0,
  27, 1, 84, 2, 83, 3, 217, 2, 157, 2, 83, 3, 217, 3, 177, 2, 229, 2, 204, 2, 109, 3, 125, 2, 140,
  2, 109, 1, 245, 2, 76, 2, 109, 3, 249, 2, 12, 2, 109, 2, 109, 2, 109, 1, 140, 2, 242, 3, 76, 2,
  46, 3, 174, 1, 152, 2, 59, 0, 148, 1, 123, 3, 234, 2, 122, 3, 11, 3, 22, 1, 103, 3, 150, 1, 14, 1,
  106, 1, 115, 2, 206, 0, 210, 3, 170, 1, 190, 1, 179, 3, 46, 3, 14, 0, 238, 2, 206, 2, 46, 0, 206,
  1, 102, 0, 148, 1, 219, 1, 24, 1, 230, 1, 231, 1, 152, 0, 108, 1, 215, 2, 78, 2, 78, 0, 148, 2,
  83, 0, 48, 1, 204, 0, 216, 0, 216, 0, 24, 2, 24, 1, 88, 0, 24, 2, 88, 3, 171, 2, 89, 3, 226, 1,
  46, 1, 126, 2, 27, 2, 210, 3, 174, 1, 22, 3, 174, 3, 126, 2, 35, 3, 46, 3, 226, 3, 49, 0, 144, 2,
  210, 3, 126, 2, 87, 2, 254, 3, 142, 0, 48, 0, 144, 2, 206, 2, 46, 2, 131, 3, 142, 1, 126, 2, 127,
  3, 174, 1, 22, 3, 174, 1, 183, 0, 204, 1, 202, 1, 94, 2, 175, 0, 190, 3, 38, 2, 238, 3, 44, 3, 23,
  0, 102, 2, 219, 0, 84, 0,  3, 0, 146, 3, 102, 1, 250, 2, 50, 3, 166, 0, 144, 2, 36, 1, 152, 2, 88,
  0, 216, 0, 88, 1, 24, 1, 216, 0, 88, 3, 155, 3, 230, 1, 147, 3, 142, 1, 98, 3, 19, 2, 206, 0, 60,
  3, 108, 3, 23, 1, 234, 2, 254, 3, 12, 0, 46, 2, 98, 3, 91, 1, 14, 1, 106, 2, 110, 3, 63, 0, 206,
  1, 42, 3, 142, 2, 126, 3, 31, 3, 166, 1, 142, 0, 46, 3, 12, 1, 235, 2, 76, 0, 216, 0, 88, 0, 24,
  0, 88, 1, 216, 2, 88, 2, 24, 0, 24, 1, 88, 1, 88, 0, 216, 3, 119, 3, 174, 1, 46, 1, 134, 2, 186,
  1, 123, 1, 250, 2, 206, 1, 234, 3, 203, 1, 159, 0, 206, 3, 12, 0, 152, 0, 216, 0, 24, 0, 152, 1,
  88, 2,  7, 1, 76, 1, 251
};

// DEFINES
#define SSIZE 12
#define WSIZE 14
#define ZERO 0

// VARIABLES
static byte a[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // A register
static byte b[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // B register
static byte c[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // C register
static byte d[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // D register
static byte e[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // E register
static byte f[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // F register
static byte m[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // M Scratchpad
static byte t[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // Temporary
static byte s[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} ; // Status
static byte p = 0, pc = 0, ret = 0; // Pointer
static byte offset = 0; // ROM offset
static byte first, last; // Register loop boundaries
static byte carry = 0, prevCarry = 0; // Carry bits
static byte fetch_h, fetch_l, op_code = 0; // ROM fetch and operation code
static byte key_code = -1, key_rom = 0; // Key variables
static boolean display_enable = true, update_display = true; // Display control
static boolean iserror = false; // True if error

// SUBROUTINES
byte do_add(byte x, byte y) { // Add 2 bytes
  int res = x + y + carry;
  if (res > 9) {
    res -= 10;
    carry = 1;
  }
  else carry = 0;
  return ((byte) res);
}

byte do_sub(byte x, byte y)  { // Substract 2 bytes
  int res = x - y - carry;
  if (res < 0) {
    res += 10;
    carry = 1;
  }
  else carry = 0;
  return ((byte)res);
}

void process_rom(void) { // Process key with HP35-engine
  if ((pc == 191) & (offset == 0)) { // *** Error handling
    iserror = true;
    //Serial.println("Error");
  }

  prevCarry = carry; // *** Fetch ROM
  carry = 0;
  fetch_h = pgm_read_byte_near(rom + (offset * 256 * 2) + (pc * 2));
  fetch_l = pgm_read_byte_near(rom + (offset * 256 * 2) + (pc * 2) + 1);
  pc++;
  pc %= 256;

  if (key_code < 255) { // *** Process received key
    key_rom = key_code;
    key_code = -1;
    s[0] = 1;
  }

  if (fetch_l == 0x00) ; // NOP  *** Evaluate fetch
  if ((fetch_l & 0x03) == 0x01) { // Jump subroutine
    ret = pc;
    pc = (((fetch_l >> 2) & 0x3f) | ((fetch_h << 6) & 0x0c0));
  }
  if ((fetch_l & 0x7F) == 0x030) pc = ret; // Return from subroutine
  if ((fetch_l & 0x7F) == 0x010) offset = (((fetch_h << 1) & 06) | ((fetch_l >> 7) & 01)); // ROM
  if (fetch_l == 0x0D0) { // Jump to pc + rom if key is available
    pc = key_rom; // Reset pointer
    s[0] = 0;
  }
  if ((fetch_l & 0x03f) == 0x014) // Set carry due to status
    carry = s[((fetch_h & 0x03) << 2) | ((fetch_l & 0x0c0) >> 6)];
  if ((fetch_l & 0x03f) == 0x004) s[((fetch_h & 0x03) << 2) | ((fetch_l & 0x0c0) >> 6)] = 1; // Set s
  if ((fetch_l & 0x03f) == 0x024) s[((fetch_h & 0x03) << 2) | ((fetch_l & 0x0c0) >> 6)] = 0; // Clear s
  if ((fetch_l & 0x03f) == 0x034) for (byte i = 0; i <= 12; i++) s[i] = 0; // Clear stati
  if ((fetch_l & 0x03f) == 0x02C) { // Set carry
    carry = 0;
    if (p == (((fetch_h & 0x03) << 2) | ((fetch_l & 0x0c0) >> 6))) carry = 1;
  }
  if ((fetch_l & 0x03f) == 0x00C) p = (((fetch_h & 0x03) << 2) | ((fetch_l & 0x0c0) >> 6)); // Set p
  if ((fetch_l & 0x03f) == 0x03C) p = ((p + 1) & 0x0f);
  if ((fetch_l & 0x03f) == 0x01C) p = ((p - 1) & 0x0f);
  if ((fetch_l & 0x3F) == 0x18) { // Load constant
    c[p] = ((fetch_l >> 6) | (fetch_h << 2));
    p = ((p - 1) & 0x0f);
  }
  if (((fetch_h & 0x03) == 0x00) && ((fetch_l & 0x0ef) == 0x0A8)) { // c<->m
    for (byte i = 0; i < WSIZE; i++) {
      int tmp = c[i];
      c[i] = m[i];
      m[i] = tmp;
    }
  }
  if (((fetch_h & 0x03) == 0x01) && ((fetch_l & 0x0ef) == 0x028)) { // c to stack
    for (byte i = 0; i < WSIZE; i++) {
      f[i] = e[i];
      e[i] = d[i];
      d[i] = c[i];
    }
  }
  if (((fetch_h & 0x03) == 0x01) && ((fetch_l & 0x0ef) == 0x0A8)) { // Stack to a
    for (byte i = 0; i < WSIZE; i++) {
      a[i] = d[i];
      d[i] = e[i];
      e[i] = f[i];
    }
  }
  if (((fetch_h & 0x03) == 0x02) && ((fetch_l & 0x0ef) == 0x0A8)) { // m to c
    for (byte i = 0; i < WSIZE; i++) c[i] = m[i];
  }
  if (((fetch_h & 0x03) == 0x03) && ((fetch_l & 0x0ef) == 0x028)) { // Rotate down
    for (byte i = 0; i < WSIZE; i++) {
      int tmp = c[i];
      c[i] = d[i];
      d[i] = e[i];
      e[i] = f[i];
      f[i] = tmp;
    }
  }
  if (((fetch_h & 0x03) == 0x03) && ((fetch_l & 0x0ef) == 0x0A8)) // Clear all register
    for (byte i = 0; i < WSIZE; i++) a[i] = b[i] = c[i] = d[i] = e[i] = f[i] = m[i] = 0;
  if (((fetch_h & 0x03) == 0x00) && ((fetch_l & 0x0ef) == 0x028)) { // No display
    display_enable = false;
    update_display =  false;
  }
  if (((fetch_h & 0x03) == 0x02) && ((fetch_l & 0x0ef) == 0x028)) { // Toggle display
    display_enable = ! display_enable;
    if (display_enable == true) update_display = true;
  }
  if ((fetch_l & 0x03) == 0x03) // Conditional branch
    if ( prevCarry != 1) pc = ((fetch_l & 0x0fc) >> 2) | ((fetch_h & 0x03) << 6);
  if ((fetch_l & 0x03) == 0x02) { // A&R calculations due to opcode
    op_code = ((fetch_l >> 5) & 0x07);
    op_code = op_code | ((fetch_h << 3) & 0x018);
    switch ((fetch_l >> 2) & 0x07) { // Get register boundaries first/last
      case 0:
        first = last = p;
        break;
      case 1:
        first = 3;
        last = 12;
        break;
      case 2:
        first = 0;
        last = 2;
        break;
      case 3:
        first = 0;
        last = 13;
        break;
      case 4:
        first = 0;
        last = p;
        break;
      case 5:
        first = 3;
        last = 13;
        break;
      case 6:
        first = last = 2;
        break;
      case 7:
        first = last = 13;
        break;
    }
    carry = 0;
    switch (op_code) { // Process opcode
      case 0x0: // 0+B
        for (byte i = first; i <= last; i++) if (b[i] != 0) carry = 1;
        break;
      case 0x01: // 0->B
        for (byte i = first; i <= last; i++) b[i] = 0;
        break;
      case 0x02: //  A-C
        for (byte i = first; i <= last; i++) t[i] = do_sub(a[i], c[i]);
        break;
      case 0x03: // C-1
        carry = 1;
        for (byte i = first; i <= last; i++) if (c[i] != 0) carry = 0;
        break;
      case 0x04: // B->C
        for (byte i = first; i <= last; i++) c[i] = b[i];
        break;
      case 0x05: // 0-C->C
        for (byte i = first; i <= last; i++) c[i] = do_sub(ZERO, c[i]);
        break;
      case 0x06: // 0->C
        for (byte i = first; i <= last; i++) c[i] = 0;
        break;
      case 0x07: // 0-C-1->C
        carry = 1;
        for (byte i = first; i <= last; i++) c[i] = do_sub(ZERO, c[i]);
        break;
      case 0x08: // Sh A Left
        for (int8_t i = last; i > first; i--) a[i] = a[i - 1];
        a[first] = 0;
        break;
      case 0x09: // A->B
        for (byte i = first; i <= last; i++) b[i] = a[i];
        break;
      case 0x0a: // A-C->C
        for (byte i = first; i <= last; i++) c[i] = do_sub(a[i], c[i]);
        break;
      case 0x0b: // C-1->C
        carry = 1;
        for (byte i = first; i <= last; i++) c[i] = do_sub(c[i], ZERO);
        break;
      case 0x0c: // C->A
        for (byte i = first; i <= last; i++) a[i] = c[i];
        break;
      case 0x0d: // 0-C
        for (byte i = first; i <= last; i++) if (c[i] != 0) carry = 1;
        break;
      case 0x0e: // A+C->C
        for (byte i = first; i <= last; i++) c[i] = do_add(a[i], c[i]);
        break;
      case 0x0f: // C+1->C
        carry = 1;
        for (byte i = first; i <= last; i++) c[i] = do_add(c[i], ZERO);
        break;
      case 0x010: // A-B
        for (byte i = first; i <= last; i++) t[i] = do_sub(a[i], b[i]);
        break;
      case 0x011: // B<->C
        for (byte i = first; i <= last; i++) {
          byte tmp = b[i];
          b[i] = c[i];
          c[i] = tmp;
        }
        break;
      case 0x012: // Sh C Right
        for (byte i = first; i < last; i++) c[i] =  c[i + 1];
        c[last] = 0;
        break;
      case 0x013: // A-1
        carry = 1;
        for (byte i = first; i <= last; i++) if (a[i] != 0) carry = 0;
        break;
      case 0x014: // Sh B Right
        for (byte i = first; i < last; i++) b[i] =  b[i + 1];
        b[last] = 0;
        break;
      case 0x015: // C+C->A
        for (byte i = first; i <= last; i++) c[i] = do_add(c[i], c[i]);
        break;
      case 0x016: // Sh A Right
        for (byte i = first; i < last; i++) a[i] = a[i + 1];
        a[last] = 0;
        break;
      case 0x017: // 0->A
        for (byte i = first; i <= last; i++) a[i] = 0;
        break;
      case 0x018: // A-B->A
        for (byte i = first; i <= last; i++) a[i] = do_sub(a[i], b[i]);
        break;
      case 0x019: // A<->B
        for (byte i = first; i <= last; i++) {
          byte tmp = a[i];
          a[i] = b[i];
          b[i] = tmp;
        }
        break;
      case 0x01a: // A-C->A
        for (byte i = first; i <= last; i++) a[i] = do_sub(a[i], c[i]);
        break;
      case 0x01b: // A-1->A
        carry = 1;
        for (byte i = first; i <= last; i++) a[i] = do_sub(a[i], ZERO);
        break;
      case 0x01c: // A+B->A
        for (byte i = first; i <= last; i++) a[i] = do_add(a[i], b[i]);
        break;
      case 0x01d: // C<->A
        for (byte i = first; i <= last; i++) {
          byte tmp = a[i];
          a[i] = c[i];
          c[i] = tmp;
        }
        break;
      case 0x01e: // A+C->A
        for (byte i = first; i <= last; i++) a[i] = do_add(a[i], c[i]);
        break;
      case 0x01f: // A+1->A
        carry = 1;
        for (byte i = first; i <= last; i++) a[i] = do_add(a[i], ZERO);
        break;
    }
  }

  if (display_enable) { // *** Display
    update_display = true;
  }
  else if (update_display) {
    printscreen();
    update_display = false;
  }
}


// ***** A P P L I C A T I O N

// MACROS
#define _ones(x) ((x)%10)        // Calculates ones unit
#define _tens(x) (((x)/10)%10)   // Calculates tens unit
#define _huns(x) (((x)/100)%10)  // Calculates hundreds unit
#define _tsds(x) (((x)/1000)%10) // Calculates thousands unit

// DEFINES
#define DIGITS 15
#define KEY_DUMMY 0xff // Needed to enter key-loop and printstring
#define MAXMENU 2 // Last (regular) menu
#define MAXSTRBUF 12 // Maximal length of string buffer sbuf[]

// VARIABLES
unsigned char stackx[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
static boolean isfirstrun = true; // Allows first run of loop and
static boolean ismenu = false, ismenutoggle = false; // Menu flags
static byte menuselect = 0; // Selected menu
static char sbuf[MAXSTRBUF]; // Holds string to print

// COMMANDS
const char c0[] PROGMEM = "CLR"; //0           MENU0 (fast)
const char c1[] PROGMEM = "";  //1
const char c2[] PROGMEM = "";  //2
const char c3[] PROGMEM = "-";   //3
const char c4[] PROGMEM = "X~Y"; //4 swap
const char c5[] PROGMEM = "R]";  //5 rot
const char c6[] PROGMEM = "*";   //6
const char c7[] PROGMEM = "STO"; //7
const char c8[] PROGMEM = "RCL"; //8
const char c9[] PROGMEM = "/";   //9
const char c10[] PROGMEM = "";  //d
const char c11[] PROGMEM = "+";   //x

const char c12[] PROGMEM = "sin"; //0           MENU1 (fast)
const char c13[] PROGMEM = "arc"; //1
const char c14[] PROGMEM = ""; //2
const char c15[] PROGMEM = "#"; //3
const char c16[] PROGMEM = "1/x";  //4
const char c17[] PROGMEM = "log"; //5
const char c18[] PROGMEM = "ln"; //6
const char c19[] PROGMEM = ",$"; //7 sqrt
const char c20[] PROGMEM = "x:"; //8
const char c21[] PROGMEM = "e;"; //9
const char c22[] PROGMEM = "cos"; //d
const char c23[] PROGMEM = "tan"; //x


const char* const cmd[] PROGMEM = {
  c0, c1, c2, c3, c4, c5, c6, c7, c8, c9, c10, c11, c12, c13, c14, c15, c16, c17, c18, c19, c20,
  c21, c22, c23
};
#define numberofcommands (sizeof(cmd)/sizeof(const char *))

// FUNCTION POINTER ARRAY
static void (*dispatch[])(void) = {
  &_clr, &_nop, &_nop, //  MENU0 0 1 2
  &_sub, &_swap, &_rot, //       3 4 5
  &_mult, &_sto, &_rcl, //       6 7 8
  &_div, &_nop, &_add, //        9 d x
  &_sin, &_arc, &_nop, //  MENU1 0 1 2  OFFSET: 12
  &_pi, &_inv, &_log, //         3 4 5
  &_ln, &_sqrt, &_pow, //        6 7 8
  &_exp, &_cos, &_tan, //        9 d x
};

// Function pointer array subroutines
static void _nop(void) {} // NOP - no operation
static void _add(void) { // +
  key = '+';
}
static void _arc(void) { // ARC
  key = 'a';
}
static void _clr(void) { // CLR
  key = '#';
}
static void _cos(void) { // COS
  key = 'c';
}
static void _div(void) { // /
  key = '/';
}
static void _exp(void) { // EXP
  key = 'e';
}
static void _inv(void) { // 1/X
  key = 'i';
}
static void _log(void) { // LOG
  key = 'L';
}
static void _ln(void) { // LN
  key = 'l';
}
static void _mult(void) { // *
  key = '*';
}
static void _pi(void) { // PI
  key = 'P';
}
static void _pow(void) { // POWER
  key = 'p';
}
static void _rcl(void) { // RCL
  key = 'm';
}
static void _rot(void) { // ROT
  key = 'r';
}
static void _sin(void) { // SIN
  key = 's';
}
static void _sqrt(void) { // SQRT
  key = 'Q';
}
static void _sto(void) { // STO
  key = 'M';
}
static void _sub(void) { // -
  key = '-';
}
static void _swap(void) { // SWAP
  key = 'x';
}
static void _tan(void) { // TAN
  key = 't';
}

static void printscreen(void) { // Print screen due to state
  dbufclr();
  byte pos = 0;
  if (iserror) {
    printsat("Error", false, SIZEM, SIZEM, 7, 1);
  }
  else if (ismenu) { // Print menu
    char cs[12] = {7, 8, 9, 4, 5, 6, 1, 2, 3, 0, 10, 11};
    byte c = 0, r = 0;
    for (byte i = 0; i < 12; i++) {
      strcpy_P(sbuf, (char*)pgm_read_word(&(cmd[cs[i] + menuselect * 12])));
      printsat(sbuf, false, SIZES, SIZES, c, r);
      c += 23;
      if (c > 46) {
        c = 0; r++;
      }
    }
  }
  else { // Print stack
    for (int8_t i = WSIZE - 1; i >= 0; i--) { // Create X with register a and b
      if (b[i] >= 8) stackx[pos++] = ' ';
      else if (i == 2) {
        if (a[i] >= 8) stackx[pos++] = '-';
        else stackx[pos++] = ' ';
      }
      else if (i == 13) {
        if (a[i] >= 8) stackx[pos++] = '-';
        else stackx[pos++] = ' ';
      }
      else stackx[pos++] = (a[i] + 48);
      if (b[i] == 2) stackx[pos++] = '.';
    }
    pos = 0;
    for (byte i = 0; i < DIGITS; i++) { // Print X
      if ((i != 11) && ((i != 10) || (stackx[11] != '.'))) {
        printcat(stackx[i], false, SIZES, SIZEM, pos, 2);
        if (stackx[i] == '.') pos += 2;
        else if ((stackx[i] == '-') || (i == 12) || ((i == 0) && (stackx[i] = ' '))) pos += 4;
        else pos += 5;
      }
    }
  }
  display();
}


// SETUP, LOOP

void setup() {
  bootpins(); // System boot procedure
  bootSPI();
  bootscreen();
  setframerate(FRAMERATE);

  setscreencontrast(brightness = EEPROM[EECONTRAST]); // Load contrast

  printsat("A tribute to", false, SIZES, SIZES, 1, 0);
  printsat("HP-35", true, SIZEM, SIZEM, 7, 2);
  display();
  delay(2000);
}

void loop() {

  key = getkey();
  if (key == oldkey) key = NULL; // No key repeat
  else oldkey = key;


  if (key == KEY1) { // ### MENU pressed
    if (!ismenu) {
      key = KEY_DUMMY;
      menuselect = 0; // Start always with the first menu
      ismenutoggle = false;
    }
    ismenu = true;
  }

  if (key) { // ### Evaluate pressed key
    iserror = false;
    if (key != KEY_DUMMY) {
      if (ismenu) { // ### Menu
        if (key == KEY1) { // Toggle menu
          if (ismenutoggle) {
            menuselect--;
            ismenutoggle = false;
          }
          else {
            menuselect++;
            ismenutoggle = true;
          }
          key = NULL;
        }
        else if (key == KEY13) { // ESC
          ismenu = false;
          update_display = true;
          key = NULL;
        }
        else if (key == KEY5) { // Brightness+
          if (brightness <= 191) brightness += 64;
          else brightness = 255;
          setscreencontrast(brightness);
          ismenu = false;
          update_display = true;
          key = NULL;
        }
        else if (key == KEY9) { // Brightness-
          if (brightness >= 64) brightness -= 64;
          else brightness = 0;
          setscreencontrast(brightness);
          ismenu = false;
          update_display = true;
          key = NULL;
        }
        else { // Dispatch menu command
          byte command = key - KEY14 + menuselect * 12;
          (*dispatch[command])();
          ismenu = false;
        }
      }
    }
  }

  if (key) { // ### Define appropriate key code
    switch (key) {
      case '0': // 0
        key_code = 36;
        break;
      case '1': // 1
        key_code = 28;
        break;
      case '2': // 2
        key_code = 27;
        break;
      case '3': // 3
        key_code = 26;
        break;
      case '4': // 4
        key_code = 20;
        break;
      case '5': // 5
        key_code = 19;
        break;
      case '6': // 6
        key_code = 18;
        break;
      case '7': // 7
        key_code = 52;
        break;
      case '8': // 8
        key_code = 51;
        break;
      case '9': // 9
        key_code = 50;
        break;
      case ';': // ENTER
        key_code = 62;
        break;
      case 'i': // 1/x
        key_code = 14;
        break;
      case 's': // SIN
        key_code = 43;
        break;
      case 'c': // COS
        key_code = 42;
        break;
      case 't': // TAN
        key_code = 40;
        break;
      case 'P': // PI
        key_code = 34;
        break;
      case '>': // CHS
        key_code = 59;
        break;
      case '<': // EEX
        key_code = 58;
        break;
      case 'x': // X<->Y
        key_code = 12;
        break;
      case 'r': // Rv
        key_code = 11;
        break;
      case 'M': // STO
        key_code = 10;
        break;
      case 'm': // RCL
        key_code = 8;
        break;
      case 'p': // X^Y
        key_code = 6;
        break;
      case 'L': // LOG
        key_code = 4;
        break;
      case 'l': // LN
        key_code = 3;
        break;
      case 'e': // e^x
        key_code = 2;
        break;
      case 'Q': // SQRT
        key_code = 46;
        break;
      case 'a': // ARC
        key_code = 44;
        break;
      case '+': // +
        key_code = 22;
        break;
      case '-': // -
        key_code = 54;
        break;
      case '/': // /
        key_code = 38;
        break;
      case '*': // *
        key_code = 30;
        break;
      case ':': // .
        key_code = 35;
        break;
      case '#': // CLR
        key_code = 0;
        break;
      case '=': // CLX
        key_code = 56;
        break;
      default:
        key_code = -1;
    }
  }

  if (ismenu) update_display = true;
  process_rom(); // Process key with HP35-ROM-Engine
}

