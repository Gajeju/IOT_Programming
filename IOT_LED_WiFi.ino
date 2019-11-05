#include <WiFi.h>
#include <IoTStarterKit_WiFi.h>

/*
  Arduino
*/
#define LED   2
#define TAG_ID    "LED"
#define TAG_ID_2  "state"
/*
  WIFI Shield
*/
#define WIFI_SSID   "WOWEDU001"                       //와이파이 SSID
#define WIFI_PASS   "00000000"                        //와이파이 비밀번호


/*
  IoTMakers
*/
IoTMakers g_im;

#define deviceID    "qwert6D1572675402636"            //디바이스 아이디
#define authnRqtNo  "73y0ayefb"            //디바이스 패스워드
#define extrSysID   "OPEN_TCP_001PTL001_1000007985"   //게이트웨이 연결 ID


//IoT Makers 접속을 초기화 하는 함수
void init_iotmakers()
{
  while (1)
  {
    // AP 접속
    Serial.print(F("Connect to AP..."));
    while (g_im.begin(WIFI_SSID, WIFI_PASS) < 0) {
      Serial.println(F("retrying."));
      delay(1000);
    }
    Serial.println(F("success"));

    // 인자값으로 받은 정보로 KT IoT Makers 접속
    g_im.init(deviceID, authnRqtNo, extrSysID);

    // String type 제어 핸들러
    g_im.set_strdata_handler(mycb_strdata_handler);

    // IoTMakers 플랫폼 연결
    Serial.print(F("Connect to platform... "));
    while ( g_im.connect() < 0 ) {
      Serial.println(F("retrying."));
      delay(1000);
    }
    Serial.println(F("success"));

    //IoTMakers 플랫폼 인증
    Serial.print(F("Auth... "));
    if (g_im.auth_device() >= 0) {
      Serial.println(F("success "));
      return;
    }
    Serial.println(F("fail"));
  }
}

void setup()
{
  Serial.begin(9600);

  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);

  init_iotmakers();
}


void loop()
{
  static unsigned long tick = millis();

  // 만약 플랫폼 접속이 종료되었을 경우 다시 접속
  if (g_im.isServerDisconnected() == 1)
  {
    init_iotmakers();
  }

  // 센서 값을 읽어오는 시간 설정
  if ( ( millis() - tick) > 1000 )
  {
    send_LED();
    tick = millis();
  }

  // IoTMakers 플랫폼 수신처리 및 keepalive 송신
  g_im.loop();
}


// 플랫폼에 보내는 내용
void mycb_strdata_handler(char *tagid, char *strval)
{
  if ( strcmp(TAG_ID, tagid) == 0 && strcmp(strval, "on") == 0
       || strcmp(TAG_ID, tagid) == 0 && strcmp(strval, "ON") == 0)
  {
    digitalWrite(LED, LOW);
    Serial.println(F("LED ON"));
  }

  else if ( strcmp(TAG_ID, tagid) == 0 && strcmp(strval, "off") == 0
            || strcmp(TAG_ID, tagid) == 0 && strcmp(strval, "OFF") == 0)
  {
    digitalWrite(LED, HIGH);
    Serial.println(F("LED OFF"));
  }
}

int send_LED()
{
  // 센서 값을 읽어온다.
  char* led_state = digitalRead(LED) ? "off" : "on";
  
  Serial.print(F("LED : "));
  Serial.println(led_state);

  if ( g_im.send_strdata(TAG_ID_2, led_state) < 0 ) {
    Serial.println(F("fail"));
    return -1;
  }
  return 0;
}
