#include <Arduino.h>
int last_millis{-5000};

void command(String cmd);
void sendMessage(char *number, char *message);

#define ATI "ATI";
#define SLEEP "AT+ENPWRSAVE";
#define SET_SMS "AT+CSMS=1";
#define SMS_STORAGE 'AT+CPMS="SM"';
#define SMS_MODE "AT+CMGF=1";

// Serial.print(char(26));
int count{0};
// #define   "AT+CNMI: 2,1,0,0,0";
// loaction
// gsm messages trasescton start time rfid meter value;

void setup()
{
  // put your setup code here, to run once:
  Serial2.begin(115200);
  Serial.begin(115200);
  Serial.println("OK");
  //
  //
  //
  command("ATI");
  command("AT+CMEE=1");
  command("AT+CMGF=1");
  command("AT+CSCS=\"GSM\"");

  delay(10000);
  sendMessage("+919106594440", "heyyyyyyyyyyyyy shawty");
}

void loop()
{
  // put your main code here, to run repeatedly:
  if (Serial.available())
  {
    command(Serial.readString());
  }
}

void command(String cmd)
{
  Serial.print("Sent Command: ");
  Serial.println(cmd);
  Serial2.println(cmd);
  Serial2.write(0x0d);

  while (!Serial2.available())
  {
    if (millis() - last_millis > 1000)
    {
      if (count > 15)
      {
        count = 0;
        return;
      }
      else
      {
        Serial.println(".");
        last_millis = millis();
        count++;
      }
    }
  }
  // AT+CMGS="+919106594440""hey"

  Serial.println();
  Serial.println("Recieved Data: ");

  while (Serial2.available())
  {
    String resp = Serial2.readString();
    Serial.println(resp);
  }
  Serial.println();
}

void sendMessage(char *number, char *message)
{
  String cmd = "AT+CMGS=\"" + String(number) + "\"";
  Serial.print("Sent Command: ");
  Serial.println(cmd);

  Serial2.println(cmd);
  Serial2.write(0x0d);

  while (!Serial2.available())
  {
    if (millis() - last_millis > 1000)
    {
      if (count > 15)
      {
        count = 0;
        return;
      }
      else
      {
        Serial.println(".");
        last_millis = millis();
        count++;
      }
    }
  }
  while (Serial2.available())
  {
    String resp = Serial2.readString();
    Serial.println(resp);
  }

  Serial.println("waiting for message ackknowledgement");
  Serial2.println(message);
  Serial2.println((char)26);
  Serial.write(0x0d);

  while (!Serial2.available())
  {
    if (millis() - last_millis > 1000)
    {
      if (count > 15)
      {
        count = 0;
        return;
      }
      else
      {
        Serial.println(".");
        last_millis = millis();
        count++;
      }
    }
  }
  while (Serial2.available())
  {
    String resp = Serial2.readString();
    Serial.println(resp);
  }
}