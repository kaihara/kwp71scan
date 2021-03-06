#include <LiquidCrystal_I2C.h>

/*
  note:
  Engine speed
  0x01, 0x3a,"Engine speed", 1,"RPM", 1, "#scaling unsigned 1 x*40 1/min";

  Engine Temperature
  0x08, 0x03,"ADC 3 Water temperature", 1,"Water temperature", 1, "#scaling unsigned 2 -0.000014482*(X**3)+0.006319247*(X**2)-1.35140625*X+144.4095455 Deg./C";

  Air temperature
  0x08, 0x02,"ADC_2 airtemp sensor voltage", 1,"Tair sensor",  1,"#scaling unsigned 2 -2.01389E-05*(x**3)+0.008784722*(x**2)-1.676875*x+156.74375 Deg./C";

  Air quantity
  unknown

  Battery voltage
  0x08, 0x01,"ADC 1 Battery Voltage", 1,"Battery voltage", 1, "#scaling unsigned 2 X/14.68 Volt";

  Vehicle speed
  unknown

  DTC Code List
  0x01  Control unit faulty
  0x02  Idle actuator closing winding
  0x03  Petrol pump relay
  0x04  Idle actuator opening winding
  0x05  Rpm sensor
  0x07  Air flow meter
  0x08  Phase sensor
  0x09  Vehicle speed sensor
  0x0A  Lambda sensor integrator
  0x0C  Throttle potentiometer
  0x0D  Torque reduction
  0x0F  Failure warning lamp / Line to Infocenter
  0x12  Connection with ABS
  0x13  Torque reduction 2
  0x14  Automatic transmission input
  0x15  Conditioner control
  0x1A  Permanent power supply
  0x1C  Lambda sensor
  0x22  Evaporation control valve
  0x25  Battery voltage
  0x2C  Air temperature sensor
  0x2D  Water temperature sensor
  0x2F  EGR
  0x55  Conditioner input
  0x64  Final power drivers 1
  0x65  Final power drivers 2
  0x80  Knock sensor 1
  0x81  Knock sensor 2
  0x8F  Knock measuring circuit
  0xC8  Electronic key
  0xCA  Error memory
  0xE1  Injector 1

*/

/* Setting parameter */
byte NUMBER_INFO_BLOCKS = 2; // Number of information blocks at initialization 155 V6 -> 2 ,155 16V -> 4

/* Setting parameter */

const int K_RX = 0;
const int K_TX = 1;

boolean initialized = false;  // 5baud init status
boolean clear_lcd = false;    // if cause exception then clear LCD
byte bc = 1;                   // block counter
byte data[30] = {};

const byte WAIT = 8;        // wait time.Waiting time settings may need to be fine-tuned for each model.
const int TIME_OUT = 1000;  // loop time out(ms)
const byte EOB = 0x03;      // byte of block end.


/* Parameters for obtaining vehicle information */
/* { Length , Parameters(without block counter and End of Block.) } */
const byte ACK[]            = { 1, 0x09};
const byte ADC_BATTERY[]    = { 2, 0x08, 0x01};             // ( data[4] * 0.0681 + 0.0019 , 1)
const byte ADC_WATER_TEMP[] = { 2, 0x08, 0x03};             // ( (-0.000014482 * pow(data[4], 3) + 0.006319247 * pow(data[4], 2) - 1.35140625 * data[4] + 144.4095455), 1)
const byte BATTERY[]        = { 4, 0x01, 0x01, 0x00, 0x36}; // ( data[3] * 0.0681 + 0.0019 , 1)
const byte ENGINE_SPEED[]   = { 4, 0x01, 0x02, 0x00, 0x3b}; // ( 0.2 * data[3] * data[4], 0)
const byte DTC[]            = { 1, 0x07};

/* LCD Setting */
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  delay(1000);

  lcd.init();
  lcd.backlight();
  lcd.clear();

  pinMode(K_TX, OUTPUT);
  pinMode(K_RX, INPUT);

  Serial.begin(4800);

  //clear rx buffer
  clear_buffer();

  initialized = false;
  clear_lcd = true;

  //LCD print
  lcd.clear();
  lcd.print("Initializing");
}

void loop() {
  clear_buffer();

  //init
  if ( initialized == false ) {
    kw_init();
    if ( initialized == true && clear_lcd == true ) {
      lcd.clear();
    }
  }

  //Get information
  if (initialized == true) {
    //battery v
    lcd.setCursor(0, 0);
    if ( rcv_info(BATTERY) == false ) {
      initialized = false;
      clear_lcd = true;
      lcd.print("ERROR");
      return;
    } else {
      lcd.print("BA ");
      lcd.print( data[3] * 0.0681 + 0.0019 , 1);
    }
    delay(20);

    lcd.setCursor(8, 0);
    if ( rcv_info(ADC_WATER_TEMP) == false ) {
      initialized = false;
      clear_lcd = true;
      lcd.print("ERROR");
      return;
    } else {
      lcd.print("WT ");
      lcd.print( (-0.000014482 * pow(data[4], 3) + 0.006319247 * pow(data[4], 2) - 1.35140625 * data[4] + 144.4095455), 1);
    }
    delay(20);

    lcd.setCursor(0, 1);
    if ( rcv_info(ENGINE_SPEED) == false ) {
      initialized = false;
      clear_lcd = true;
      lcd.print("ERROR");
      return;
    } else {
      lcd.print("rpm ");
      lcd.print( 0.2 * data[3] * data[4] );
    }
    delay(20);

    lcd.setCursor(9, 1);
    lcd.print("DTC: ");
    lcd.print(get_dtc_count());
    delay(20);

    clear_lcd = false;
  }

}

// Recieve ECU additional infomation.
bool rcv_ecu_info() {
  if ( rcv_block(data) && send_block(ACK) ) {
    return true;
  }
  return false;
}

// Recieve specified information of vehicle.
bool rcv_info(byte * para) {
  if ( send_block(para) && rcv_block(data) ) {
    return true;
  }
  return false;
}

byte get_dtc_count() {
  byte c = 0;
  if ( rcv_info(DTC) == false ) {
    c = 255;
  } else {
    if (data[0] == 6) {
      if (data[3] == 0xff && data[4] == 0xff) {
        c = 0;
      } else {
        c = 1;
      }
    } else {
      c = (data[0] - 3) / 5;  // kwp71 DTC data unit size = 5. kwp1281 = 3?
    }
  }
  return c;
}


// Recieve block data.
bool rcv_block(byte * b) {
  byte t = 0; // time out checker.
  while (t != TIME_OUT  && (Serial.available() == 0)) {  //wait data
    delay(1);
    t++;
  }

  // In kw-71, the first byte of block data is the number of data bytes
  b[0] = read_byte(); // data block size
  if (b[0] == -1 ) return false;

  delay(WAIT);
  send_byte( b[0] ^ 0xFF );  //return byte

  for (byte i = 1; i <= b[0]; i++) {
    b[i] = read_byte();

    if (b[i] == -1 ) return false; // abort

    //03 = end of block. Do not reply at the end.
    if ( i != b[0] ) {
      send_byte( b[i] ^ 0xFF );
    }
  }

  // When receiving 03 at the end, block reception is regarded as normal end
  if ( b[(b[0])] == EOB ) {
    bc = b[1];
    return true;
  }
  return false;
}


bool send_block(byte * p) {
  send_byte(p[0] + 2);
  if (read_byte() == -1) return false;

  send_byte( bc + 1);
  if (read_byte() == -1) return false;

  for (byte i = 0; i < p[0]; i++) {
    send_byte( p[ i + 1 ] );
    if (read_byte() == -1) return false;
  }
  send_byte( EOB );
  return true;
}

// Clear serial receive buffer
void clear_buffer() {
  while (Serial.available() > 0) {
    Serial.read();
  }
}

void serial_rx_off() {
  UCSR0B &= ~(_BV(RXEN0));  //disable UART RX
  delay(WAIT);                 //allow time for buffers to flush
}

void serial_tx_off() {
  UCSR0B &= ~(_BV(TXEN0));  //disable UART TX
  Serial.flush();  // or delay(WAIT);
}

void serial_rx_on() {
  Serial.begin(4800);   //setting enable bit didn't work, so do beginSerial
  Serial.setTimeout(50);
}

int read_byte() {
  int b = -1;
  byte t = 0;
  while (t != 125  && (b = Serial.read()) == -1) {
    delay(1);
    t++;
  }
  return b;
}

void send_byte(byte b) {
  serial_rx_off();
  Serial.write(b);
  Serial.flush();  // or delay(WAIT);
  serial_rx_on();
}

/* kw-71 init */
bool kw_init() {
  int b = 0;
  byte kw1, kw2, kw3, kw4, kw5;

  clear_buffer();

  delay(2600); //k line should be free of traffic for at least two secconds.

  // drive K line high for 300ms
  digitalWrite(K_TX, HIGH);
  delay(300);

  //send Motronic address(0x10)
  bitbang(0x10);

  // switch now to 4800 bauds
  Serial.begin(4800);

  // wait for 0x55 from the ECU (up to 300ms) ECUから0x55を待つ（最大300ms）
  //since our time out for reading is 125ms, we will try it three times
  byte tryc = 0;
  while (b != 0x55 && tryc < 6) {
    b = read_byte();
    tryc++;
  }
  if (b != 0x55) {
    initialized = false;
    return false;
  }
  // wait for kw1 and kw2
  kw1 = read_byte();
  kw2 = read_byte();
  kw3 = read_byte();
  kw4 = read_byte();
  kw5 = read_byte();

  delay(WAIT);
  //delay(50);
  // response to ECU
  if (kw2 != 0x00) {
    send_byte(kw2 ^ 0xFF);
  } else {
    return false;
  }

  // Receives initialization data block from ECU.
  // Number of repetitions depends on the ECU. Eg: 155 V6 twice, 155 16V four times.
  for (byte i = 0; i < NUMBER_INFO_BLOCKS ; i++) {
    if ( rcv_ecu_info() == false ) {
      initialized = false;
      clear_buffer();
      return false;
    }
  }

  rcv_block(data);  //Last ACK from ECU

  //init OK!
  initialized = true;
  return true;
}

void bitbang(byte b) {
  serial_tx_off();
  serial_rx_off();
  // send byte at 5 bauds
  // start bit
  digitalWrite(K_TX, LOW);
  delay(200);
  // data
  for (byte mask = 0x01; mask; mask <<= 1) {
    if (b & mask) { // choose bit
      digitalWrite(K_TX, HIGH); // send 1
    } else {
      digitalWrite(K_TX, LOW); // send 0
    }
    delay(200);
  }
  // stop bit(200ms) + 190 ms delay
  digitalWrite(K_TX, HIGH);
  delay(390);
}
