# C35 - a Calculator, Based on the Famous HP-35 ROM.

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
  
  ## KEYBOARD:
    [MENU]             7      8      9
    EEX [CONTRAST+]    4      5      6
    +/- [CONTRAST-]    1      2      3
    CLX                0      .      ENTER
  ## MENU1 (press MENU once):
                       STO    RCL    /
                       X<>Y   ROT    *
                                     -
                       CLR           +
  ## MENU2 (press MENU twice):
                       SQRT   X^Y    EXP
                       1/X    LOG    LN
                       arc           PI
                       sin    cos    tan
  ## CIRCUIT DIAGRAM (Arduino/Genuino Micro):
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
  ## HP-35 DISPLAY AND KEYBOARD:
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
 
