#include <SPI.h>
#include <SD.h>
File datalog;

String data_A1;
String data_A2;
String u_A1, v_A1, w_A1, t_A1;
String u_A2, v_A2, w_A2, t_A2;
String A_1, A_2;

int counter = 0;

unsigned long ntime;

String data_IMU;
char data_cmp_IMU[100];
String IMU;
String IMU_data;

String data_GPS;
char data_GPS_cmp[100];
String GPS_GPGGA;
String GPS_GPVTG;
String GPS_data;

String data1;
String data2;

RingBuffer rx_buffer5;
RingBuffer tx_buffer5;
USARTClass Serial4(USART2, USART2_IRQn, ID_USART2, &rx_buffer5, &tx_buffer5);
void USART2_Handler(void)
{
  Serial4.IrqHandler();
}


void setup() {

  Serial.begin(115200);
  Serial1.begin(9600, SERIAL_7E1);
  Serial2.begin(9600, SERIAL_7E1);
  {
    Serial3.begin(9600);
    //    Serial3.write("$VNWRG,06,14*59");  // START
    //    Serial3.write("$VNWRG,07,4*69");   // CHANGE FREQUENCY
    //    Serial3.write("$VNWRG,06,0*6C");  // STOP
  }
  {
    PIO_Configure(PIOB, PIO_PERIPH_A, PIO_PB20A_TXD2 | PIO_PB21A_RXD2, PIO_DEFAULT);
    Serial4.begin(9600);
    //    Serial4.write("UNLOGALL\n"); Serial4.write("LOG COM1 GPGGA ONTIME 1\n");
    //    Serial4.write("LOG COM1 GPVTG ONTIME 1\n"); Serial4.write("SAVECONFIG\n");
  }


  Serial.println("Initializing the SD Card.....");
  if (!SD.begin(4))
  {
    Serial.println("Failed....");
    return;
  }
  Serial.println("Done...");
  Serial.println("------------------------------------------NEW Begining------------------------------------------------------");

  File datalog = SD.open("datalog.txt", FILE_WRITE);
  if (datalog) {
    datalog.println("------------------------------------------NEW Begining------------------------------------------------------");
    datalog.close();
  }
}

void loop()
{
  ntime = millis();
  if (Serial1.available() > 0)
  {
    data_A1 = Serial1.readStringUntil('\n');
    u_A1 = data_A1.substring(0, 20);
  }
  if (Serial2.available() > 0)
  {
    data_A2 = Serial2.readStringUntil('\n');
    u_A2 = data_A2.substring(0, 20);
  }
  if (Serial4.available() > 0)
  {
    data_GPS = Serial4.readStringUntil('\n');
    data_GPS.toCharArray(data_GPS_cmp, 100);
    if (strncmp(data_GPS_cmp, "$GPGGA", 6) == 0)
    {
      GPS_GPGGA = data_GPS.substring(0, 120);
    }
    if (strncmp(data_GPS_cmp, "$GPVTG", 6) == 0)
    {
      GPS_GPVTG = data_GPS.substring(0, 120);
    }

  }
  if (Serial3.available() > 0)
  {
    data_IMU = Serial3.readStringUntil('\n');
    IMU = data_IMU.substring(0, 115);

    data1 = String(ntime) + "," + u_A1 + "," + u_A2 + "," + IMU + ",";
    data2 = GPS_GPGGA + "," + GPS_GPVTG + ",";
    counter = 1 + counter;

    File datalog = SD.open("datalog.txt", FILE_WRITE);
    if (datalog) {
      datalog.print(ntime);
      datalog.print("--");
      datalog.print(counter);
      datalog.print(",");
      datalog.print(data1);
      datalog.print(",");
      datalog.println(data2);
      datalog.close();
    }
    if (counter == 2)
    {
      Serial.print(ntime);
      Serial.print("--");
      Serial.println(counter);
      Serial.println(data1);
      Serial.println(data2);
      counter = 0;
    }
  }
  Serial4.flush();
  Serial3.flush();
  Serial2.flush();
  Serial1.flush();
  Serial.flush();
}
