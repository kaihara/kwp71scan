#include <SoftwareSerial.h>

int K_IN = 2;
int K_OUT = 3;

SoftwareSerial mySerial(K_IN, K_OUT, false); // RX, TX

boolean initialized = false;  // 5baud init status
byte bc = 1;                   // block counter

/* Supported PIDs */
#define ENGINE_RPM 0x0C
#define VEHICLE_SPEED 0x0D
#define ENGINE_COOLANT_TEMP 0x05
#define MAF_AIRFLOW 0x10
#define THROTTLE_POS 0x11


#define WAIT 0x0B       // wait time
#define EOM 0x03        // メッセージエンド


void setup() {
  pinMode(K_OUT, OUTPUT);
  pinMode(K_IN, INPUT);

  Serial.begin(115200);
  mySerial.begin(4800);

}

void loop() {
  // put your main code here, to run repeatedly:
  delay(3000);

  //init
  if ( !initialized ) {
    kw_init();
  }

  //Get information

}

/* kw-71 init */
void kw_init() {
  byte b = 0;
  byte kw1, kw2, kw3, kw4, kw5;

  clear_buffer();
  delay(1000);
  //serial_tx_off(); //disable UART so we can "bit-Bang" the slow init.
  //serial_rx_off();

  //bitbang(0x00);
  delay(2600); //k line should be free of traffic for at least two secconds.

  // drive K line high for 300ms
  digitalWrite(K_OUT, HIGH);
  delay(300);

  //ECU アドレス送信
  bitbang(0x10);

  // switch now to 4800 bauds
  mySerial.begin(4800);
  Serial.begin(115200);   //for Debug

  // wait for 0x55 from the ECU (up to 300ms) ECUから0x55を待つ（最大300ms）
  //since our time out for reading is 125ms, we will try it three times
  byte tryc = 0;
  while (b != 0x55 && tryc < 6) {
    b = read_byte();
    tryc++;
    Serial.println("# b=" + String(b, HEX)); //DEBUG
    Serial.println("# 55 try =" + tryc); //DEBUG
  }
  if (b != 0x55) {
    initialized = false;
    return -1;
  }
  // wait for kw1 and kw2
  kw1 = read_byte();
  kw2 = read_byte();
  kw3 = read_byte();
  kw4 = read_byte();
  kw5 = read_byte();

  delay(WAIT);
  //response to ECU
  if (kw2 != 0x00) {
    send_data(kw2 ^ 0xFF);
  }

  //recieve ECU hardware version
  if (! rcv_block()) {
    initialized = false;
    Serial.println("H/W init fail"); //DEBUG
    clear_buffer();
    return - 1;
  }

  //recieve ECU Software version
  if (! rcv_block()) {
    initialized = false;
    Serial.println("S/W init fail"); //DEBUG
    clear_buffer();
    return - 1;
  }

  //recieve ECU Software version
  if (! rcv_block()) {
    initialized = false;
    Serial.println("??? init fail"); //DEBUG
    clear_buffer();
    return - 1;
  }

  // init OK!
  initialized = false;
  return 0;
}


// データ受信を行う。
bool rcv_block() {
  byte bsize = 0x00;  //block data size
  while (mySerial.available() == 0) {}  //wait data

  bsize = read_byte();
  delay(WAIT);
  send_data( bsize ^ 0xFF );  //return

  byte b[24];
  for (byte i = 0; i < bsize; i++) {
    b[i] = read_byte();

    //03 = last は返信しない
    if ( i != (bsize - 1) ) {
      send_data( b[i] ^ 0xFF );
    }
  }

  //最終0x03を受け取れていたら正常とみなす
  if ( b[(bsize - 1)] == EOM ) {
    bc = b[0];
    send_ack();
    return true;
  }
  Serial.println("rcv_block false"); //DEBUG
  return false;
}

void send_ack() {
  send_data( 0x03 );
  read_byte();
  send_data( bc + 1 );
  read_byte();
  send_data( 0x09 );
  read_byte();
  send_data( 0x03 );
}

void clear_buffer() {
  byte b;
  Serial.println(mySerial.available()); //debug
  while (mySerial.available() > 0) {
    b = mySerial.read();
    Serial.println("clear: " + b); //DEBUG
  }
}

void bitbang(byte b) {
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
  // stop bit + 190 ms delay
  digitalWrite(K_OUT, HIGH);
  delay(390);
}

void serial_rx_off() {
  UCSR0B &= ~(_BV(RXEN0));  //disable UART RX
}

void serial_tx_off() {
  UCSR0B &= ~(_BV(TXEN0));  //disable UART TX
  delay(15);                 //allow time for buffers to flush
}

void serial_rx_on() {
  mySerial.begin(4800);   //setting enable bit didn't work, so do beginSerial
}

byte read_byte() {
  byte b = -1;
  byte t = 0;
  while (t != 125  && (b = mySerial.read()) == -1) {
    delay(1);
    t++;
  }
  if (t >= 125) {
//    Serial.println("r t/o 125ms"); //DEBUG
    b = 0;
  }
  if ( b == 0xFF) {
    Serial.println("readbyte " + b); //DEBUG
    b = read_byte();
  }
  return b;
}

void send_data(byte b) {
  serial_rx_off();
  mySerial.write(b);
  delay(WAIT);    // ISO requires 5-20 ms delay between bytes.
  serial_rx_on();
}
