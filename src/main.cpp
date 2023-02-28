#include <Arduino.h>
#include <TinyGPSPlus.h>
int last_millis{-5000};
int last_time_gps{};

void command(String cmd);
void sendMessage(char *number, char *message);
void getMessage();
void getGPS();
static void printFloat(float val, bool valid, int len, int prec);
static void printDateTime(TinyGPSDate &d, TinyGPSTime &t);
static void printStr(const char *str, int len);
static void printFloat(float val, bool valid, int len, int prec);
static void printInt(unsigned long val, bool valid, int len);

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
TinyGPSPlus gps;

void setup()
{
  // put your setup code here, to run once:
  Serial2.begin(115200);
  Serial.begin(115200);
  Serial.println("OK");
  //
  //
  //
  // Setup SMS
  command("ATI");
  command("AT+CMEE=1");
  command("AT+CMGF=1");
  command("AT+CSCS=\"GSM\"");
  // setup GPS
  command("AT$MYGPSPWR=1");

  // sendMessage("+919106594440", "heyyyyyyyyyyyyy shawty");
  command("AT+CSQ");
  delay(1000);
  getMessage();
  delay(1000);
  getGPS();
}

void loop()
{
  // put your main code here, to run repeatedly:
  if (Serial.available())
  {
    command(Serial.readString());
  }
  if (millis() - last_time_gps > 10000)
  {
    getGPS();
    last_time_gps = millis();
  }
}
// AT+CGNSPWR?
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
        last_millis = millis();
        count++;
      }
    }
  }
  Serial.println("Recieved Data: ");

  while (Serial2.available())
  {
    String resp = Serial2.readString();
    Serial.println(resp);
  }
  Serial.println();
}

void getMessage()
{
  command("AT+CMGL=\"ALL\"");
}

void getGPS()
{
  Serial.println("AT$MYGPSPOS=6");
  Serial2.println("AT$MYGPSPOS=6");
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
        last_millis = millis();
        count++;
      }
    }
  }
  Serial.println("Recieved Data: ");

  while (Serial2.available())
  {
    char resp = Serial2.read();
    // Serial.println(resp);
    gps.encode(resp);
  }
  printInt(gps.satellites.value(), gps.satellites.isValid(), 5);
  printFloat(gps.hdop.hdop(), gps.hdop.isValid(), 6, 1);
  printFloat(gps.location.lat(), gps.location.isValid(), 11, 6);
  printFloat(gps.location.lng(), gps.location.isValid(), 12, 6);
  printInt(gps.location.age(), gps.location.isValid(), 5);
  printDateTime(gps.date, gps.time);
  printFloat(gps.altitude.meters(), gps.altitude.isValid(), 7, 2);
  printFloat(gps.course.deg(), gps.course.isValid(), 7, 2);
  printFloat(gps.speed.kmph(), gps.speed.isValid(), 6, 2);
  printStr(gps.course.isValid() ? TinyGPSPlus::cardinal(gps.course.deg()) : "*** ", 6);

  Serial.println();
  // +CGNSINF: <enabled>,<fixstatus>,<utcdate>,<utctime>,<lat>,<ns>,<lon>,<ew>,<posfix>,<satellites>,<hdop>,<altitude>,<undulation>,<diffage>,<hdop>,<vdop>,<tdop>,<gnssmode>
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

static void printFloat(float val, bool valid, int len, int prec)
{
  if (!valid)
  {
    while (len-- > 1)
      Serial.print('*');
    Serial.print(' ');
  }
  else
  {
    Serial.print(val, prec);
    int vi = abs((int)val);
    int flen = prec + (val < 0.0 ? 2 : 1); // . and -
    flen += vi >= 1000 ? 4 : vi >= 100 ? 3
                         : vi >= 10    ? 2
                                       : 1;
    for (int i = flen; i < len; ++i)
      Serial.print(' ');
  }
}

static void printInt(unsigned long val, bool valid, int len)
{
  char sz[32] = "*****************";
  if (valid)
    sprintf(sz, "%ld", val);
  sz[len] = 0;
  for (int i = strlen(sz); i < len; ++i)
    sz[i] = ' ';
  if (len > 0)
    sz[len - 1] = ' ';
  Serial.print(sz);
}

static void printDateTime(TinyGPSDate &d, TinyGPSTime &t)
{
  if (!d.isValid())
  {
    Serial.print(F("********** "));
  }
  else
  {
    char sz[32];
    sprintf(sz, "%02d/%02d/%02d ", d.month(), d.day(), d.year());
    Serial.print(sz);
  }

  if (!t.isValid())
  {
    Serial.print(F("******** "));
  }
  else
  {
    char sz[32];
    sprintf(sz, "%02d:%02d:%02d ", t.hour(), t.minute(), t.second());
    Serial.print(sz);
  }

  printInt(d.age(), d.isValid(), 5);
}

static void printStr(const char *str, int len)
{
  int slen = strlen(str);
  for (int i = 0; i < len; ++i)
    Serial.print(i < slen ? str[i] : ' ');
}