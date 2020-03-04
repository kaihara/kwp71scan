#include <LiquidCrystal.h>

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

*/

/* Settiong parameter */
byte NUMBER_INFO_BLOCKS = 2; // Number of information blocks at initialization 155 V6 -> 2 ,155 16V -> 4
/* Settiong parameter */

const int K_IN = 0;
const int K_OUT = 1;

boolean initialized = false;  // 5baud init status
byte bc = 1;                   // block counter
byte data[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

const byte WAIT = 8;        // wait time.Waiting time settings may need to be fine-tuned for each model.
const int TIME_OUT = 1000;  // loop time out(ms)
const byte EOM = 0x03;      // byte of block end.

/* Parameters for obtaining vehicle information */
const byte P_ACK[] = { 1, 0x09};
const byte P_BATTERY[] = { 2, 0x08, 0x01};
const byte P_WATER_TEMP[] = { 2, 0x08, 0x03};

/* LCD Setting */
LiquidCrystal lcd( 4, 6, 10, 11, 12, 13 );


void setup() {
  pinMode(K_OUT, OUTPUT);
  pinMode(K_IN, INPUT);

  Serial.begin(4800);

  //clear rx buffer
  clear_buffer();

  initialized = false;

  //LCD print
  lcd.begin( 16, 2 );
}

void loop() {
  //TODO 起動前のdelay時間 要調整
  // delay(1000);
  // Wait for ECU startup
  // delay(3000);

  //init
  if ( initialized == false ) {
    lcd.setCursor(0, 0);
    lcd.print("Initializing");

    kw_init();
  }


  //Get information
  if (initialized == true) {

    //TODO 4つのzoneにわけてデータを表示する。
    //TODO 通信が途絶した場合は、INITからやり直す
    //battery v
    //TODO The bug lives here.
    if ( rcv_info(P_BATTERY) == false ) {
      initialized = false;
      clear_buffer();
    } else {
      lcd.setCursor(0, 0);
      lcd.print("BA ");
      lcd.print( data[3] * 0.0681 + 0.0019 , 1);
    }

    delay(30);

    /*
        rcv_block(data2, P_WATER_TEMP);
        lcd.setCursor(8, 0);
        lcd.print("WT ");
        //lcd.print( (-0.000014482 * pow(data2[3], 3) + 0.006319247 * pow(data2[3], 2) - 1.35140625 * data2[3] + 144.4095455), 1);
        lcd.print( String(data2[3], HEX));

        delay(30);


            if (! rcv_block(data1,P_BATTERY)) {
              initialized = false;
              clear_buffer();
            } else {
              lcd.setCursor(0, 0);
              lcd.print("BA ");
              //lcd.print( (data1[3] * 0.0681 + 0.0019 ), 1);
              double d1 = data1[3] * 0.0681 + 0.0019;
              lcd.print( String(data1[3], HEX));
            }

            if (! rcv_block(data2, P_WATER_TEMP)) {
              initialized = false;
              clear_buffer();
            } else {
              lcd.setCursor(8, 0);
              lcd.print("WT ");
              //lcd.print( (-0.000014482 * pow(data2[3], 3) + 0.006319247 * pow(data2[3], 2) - 1.35140625 * data2[3] + 144.4095455), 1);
              lcd.print( String(data2[3], HEX));
            }
    */
    /*
        if (! rcv_block(data3, P_BATTERY)) {
          initialized = false;
          clear_buffer();
        } else {
          lcd.setCursor(0, 1);
          lcd.print("BA ");
          double d3 = data3[3] * 0.0681 + 0.0019;
          //lcd.print( (data3[3] * 0.0681 + 0.0019 ), 1);
          //lcd.print( d3, 1);
          lcd.print(String(data3[3], HEX));

          lcd.setCursor(8, 1);
          lcd.print( d3, 1);
        }

        if (! rcv_block(data4, P_WATER_TEMP)) {
          initialized = false;
          clear_buffer();
        } else {
          lcd.setCursor(8, 1);
          lcd.print("WT ");
          //lcd.print( (-0.000014482 * pow(data4[3], 3) + 0.006319247 * pow(data4[3], 2) - 1.35140625 * data4[3] + 144.4095455), 1);
          lcd.print( String(data4[3], HEX));
        }
    */
    //delay(20);  // 50msにするとアウト TODO あとで調整

  }

}


bool rcv_ecu_info() {
  if ( rcv_block(data) == true && send_block(P_ACK) == true ) {
    return true;
  }
  return false;
}

bool rcv_info(byte *para) {
  if ( send_block(para) == true && rcv_block(data) == true ) {
    send_block(P_ACK);
    return true;
  }
  return false;
}

// Recieve block data.
bool rcv_block(byte *b) {
  byte bsize = 0x00;  //block data size
  byte t = 0;
  while (t != TIME_OUT  && (Serial.available() == 0)) {  //wait data
    delay(1);
    t++;
  }

  // In kw-71, the first byte of block data is the number of data bytes
  bsize = read_byte();
  delay(WAIT);
  send_byte( bsize ^ 0xFF );  //return byte

  for (byte i = 0; i < bsize; i++) {
    b[i] = read_byte();

    //03 = last は返信しない
    if ( i != (bsize - 1) ) {
      send_byte( b[i] ^ 0xFF );
    }
  }

  // When receiving 03 at the end, block reception is regarded as normal end
  if ( b[(bsize - 1)] == EOM ) {
    bc = b[0];
    return true;
  }
  return false;
}

bool send_block(byte *p) {
  send_byte(p[0] + 2);
  read_byte();
  send_byte( bc + 1 );
  read_byte();
  for (byte i = 0; i < p[0]; i++) {
    send_byte( p[ i + 1 ] );
    read_byte();
  }
  send_byte( EOM );
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
  delay(WAIT);                 //allow time for buffers to flush
}

void serial_rx_on() {
  Serial.begin(4800);   //setting enable bit didn't work, so do beginSerial
}

int read_byte() {
  int b = -1;
  byte t = 0;
  while (t != 125  && (b = Serial.read()) == -1) {
    delay(1);
    t++;
  }
  //TODO return -1の場合は処理を止めるのを入れたほうが良いかも。
  return b;
}

void send_byte(byte b) {
  serial_rx_off();
  Serial.write(b);
  delay(WAIT);    // ISO requires 5-20 ms delay between bytes.
  serial_rx_on();
}

/* kw-71 init */
bool kw_init() {
  int b = 0;
  byte kw1, kw2, kw3, kw4, kw5;
  byte data[12];

  clear_buffer();

  delay(2600); //k line should be free of traffic for at least two secconds.

  // drive K line high for 300ms
  digitalWrite(K_OUT, HIGH);
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
  // TODO kw2以外不要なので消す
  kw1 = read_byte();
  kw2 = read_byte();
  kw3 = read_byte();
  kw4 = read_byte();
  kw5 = read_byte();

  delay(WAIT);
  //response to ECU
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
  digitalWrite(K_OUT, LOW);
  delay(200);
  // data
  for (byte mask = 0x01; mask; mask <<= 1) {
    if (b & mask) { // choose bit
      digitalWrite(K_OUT, HIGH); // send 1
    } else {
      digitalWrite(K_OUT, LOW); // send 0
    }
    delay(200);
  }
  // stop bit(200ms) + 190 ms delay
  digitalWrite(K_OUT, HIGH);
  delay(390);
}
