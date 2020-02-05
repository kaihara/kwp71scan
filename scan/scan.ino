    initialized = false;
    Serial.println("H/W init fail"); //DEBUG
    clear_buffer();
    return - 1;
  }

  //recieve ECU Software version
  if (! rcv_data()) {
    initialized = false;
    Serial.println("S/W init fail"); //DEBUG
    clear_buffer();
    return - 1;
  }

  //recieve ECU Software version
  if (! rcv_data2()) {
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
bool rcv_data() {
  byte bsize = 0x00;  //block data size
  while (mySerial.available() == 0) {}  //wait data

  bsize = read_byte();
  Serial.println(mySerial.available()); //debug
  Serial.println("bsize:" + String(bsize, HEX)); //DEBUG

  send_data( bsize ^ 0xFF );  //return

  byte b[24];
  for (byte i = 0; i < bsize; i++) {
    b[i] = read_byte();

    Serial.println("b:" + i); //DEBUG
    //03 = last は返信しない
    if ( i != (bsize - 1) ) {
    //if ( i < bsize ){
      send_data( b[i] ^ 0xFF );
    }
  }

  //最終0x03を受け取れていたら正常とみなす
  if ( b[(bsize-1)] == EOM ) {
    Serial.println("rcv_data true"); //DEBUG
    bc = b[0];
    send_ack();
    return true;
  }
  Serial.println("rcv_data false"); //DEBUG
  return false;
}

// データ受信を行う。
bool rcv_data2() {
  byte bsize = 0x00;  //block data size
  while (mySerial.available() == 0) {}  //wait data

  bsize = read_byte();
  Serial.println(mySerial.available()); //debug
  Serial.println("bsize:" + String(bsize, HEX)); //DEBUG

  send_data( bsize ^ 0xFF );  //return

  byte b[24];
  for (byte i = 0; i < bsize; i++) {
    b[i] = read_byte();

    Serial.println("b:" + i); //DEBUG
    //03 = last は返信しない
    if ( i != (bsize - 1) ) {
    //if ( i < bsize ){
      send_data( b[i] ^ 0xFF );
    }
  }

  //最終0x03を受け取れていたら正常とみなす
  if ( b[(bsize-1)] == EOM ) {
    Serial.println("rcv_data true"); //DEBUG
    bc = b[0];
    send_ack();
    return true;
  }
  Serial.println("rcv_data false"); //DEBUG
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
    Serial.println("g " + String(b, HEX)); //DEBUG
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
  // stop bit + 60 ms delay
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

int read_byte() {
  int b;
  byte t = 0;
  while (t != 125  && (b = mySerial.read()) == -1) {
    delay(1);
    t++;
  }
  if (t >= 125) {
    Serial.println("r t/o 125ms"); //DEBUG
    b = 0;
  }
  if ( b == 0xFF) {
    Serial.println("readbyte ff "); //DEBUG
    b = read_byte();
  }
  return b;
}

int read_byte_ff() {
  int b;
  byte t = 0;
  while (t != 125  && (b = mySerial.read()) == -1) {
    delay(1);
    t++;
  }
  if (t >= 125) {
    Serial.println("r t/o 125ms"); //DEBUG
    b = 0;
  }
  return b;
}

void send_data(byte b) {
  serial_rx_off();
  mySerial.write(b);
  delay(12);    // ISO requires 5-20 ms delay between bytes.
  serial_rx_on();
}

void requestPID(byte pid)
{
  /*
    Byte   0: Message Header 1... 0x68
           1: Message header 2... 0x6A for OBDI-II request
           2: Source address ... 0xF1 for off-board tool
           3-9: Data
             with  3: 0x01, get PID
                   4: pid in hex
           Final: checksum
  */
  byte message[6];
  message[0] = 0x68;
  message[1] = 0x6A;
  message[2] = 0xF1;
  message[3] = 0x01;
  message[4] = pid;
  message[5] = iso_checksum(message, 5);

  // write message to ECU
  for (int i = 0; i < 6; i++)
  {
    send_data(message[i]);
    Serial.println(message[i]);
  }
}

// inspired by SternOBDII\code\checksum.c
byte iso_checksum(byte *message, byte index)
{
  byte i;
  byte crc;

  crc = 0;
  for (i = 0; i < index; i++)
    crc = crc + message[i];

  return crc;
}
