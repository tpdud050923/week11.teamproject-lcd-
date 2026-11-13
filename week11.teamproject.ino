
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

// 문자 코드 정의
byte ch0[8] = { /* 가 */ };
byte ch1[8] = { /* 나 */ };
byte ch2[8] = { /* 다 */ };
byte ch3[8] = { /* 네모 */ };
byte ch4[8] = { /* 화살표 */ };

// 상태 변수
int state = 0;  // 0 = 15cm 초과, 1 = 15cm 이하
int button = 0; // 15cm 이내에서 버튼 : 0 = 경보, 1 = 안전

// 부저 제어 
int buzzerOn = 0;
unsigned long buzzerstart;

void setup() {

  Serial.begin(9600);

  pinMode(8, OUTPUT);
  pinMode(9, INPUT);
  pinMode(6, OUTPUT);
  pinMode(7, INPUT_PULLUP);

  lcd.init();
  lcd.backlight();

  lcd.createChar(0, ch0); // 가 등록
  lcd.createChar(1, ch1); // 나 등록
  lcd.createChar(2, ch2); // 다 등록
  lcd.createChar(3, ch3); // 네모 등록
  lcd.createChar(4, ch4); // 화살표 등록

  lcd.clear();
}

void loop() {

  // 1. 거리 측정 
  digitalWrite(8, LOW);
  delayMicroseconds(2);
  digitalWrite(8, HIGH);
  delayMicroseconds(10);
  digitalWrite(8, LOW);

  unsigned long duration = pulseIn(9, HIGH, 30000);
  float distance = duration / 29.41 / 2.0;

  Serial.print(distance);
  Serial.println(" cm");

  // 2. 15cm 초과 -> lcd, 부저 초기화
  if (distance > 15) {

    if (state == 1) {
      lcd.clear();
    }

    state = 0;
    button = 0;
    buzzerOn = 0;
    noTone(6);
  }

  // 3. 15cm 이하 -> lcd 에 '가나다 네모' 출력 + 부저 시작
  else if (state == 0) {

    state = 1; 
    button = 0;

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("'");
    lcd.write(byte(0));
    lcd.write(byte(1));
    lcd.write(byte(2));
    lcd.write(byte(3));
    lcd.print("'"); 

    tone(6, 1000);
    buzzerOn = 1;
    buzzerstart = millis();

  }

  // 4. 버튼을 누르면 안전 상태
  if (state == 1 && button == 0 && digitalRead(7) == LOW) {

    button = 1;

    noTone(6);
    buzzerOn = 0;

    lcd.clear();
    lcd.setCursor(0, 0);

    lcd.print("\"Safe " );
    lcd.write(byte(4));
    lcd.print("\"");
    lcd.write(byte(4));

    delay(200);
  }

  // 5. 부저 1초 울림, 1초 정지
  if (state == 1 && button == 0) {

    if (millis() - buzzerstart >= 1000) {

      buzzerstart = millis();

      if (buzzerOn == 1) {
        noTone(6);
        buzzerOn = 0;
      }
      else {
        tone(6, 1000);
        buzzerOn = 1;
      }
    }
  }
}

