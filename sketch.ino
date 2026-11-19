#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
#include <Adafruit_Fingerprint.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
SoftwareSerial fingerPrint(2, 3);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&fingerPrint);

uint8_t id;

#define enroll A0
#define del A1
#define up A2
#define down A3
#define openLight 4
#define closeLight 5
#define door 10

void setup() {
  // put your setup code here, to run once:
  lcd.init();
  delay(100);
  Serial.begin(9600);

  pinMode(enroll, INPUT_PULLUP);
  pinMode(up, INPUT_PULLUP);
  pinMode(down, INPUT_PULLUP);
  pinMode(del, INPUT_PULLUP);
  pinMode(openLight, OUTPUT);
  pinMode(closeLight, OUTPUT);
  pinMode(door, OUTPUT);

  digitalWrite(door, LOW);

  lcd.init();
  delay(2000);

  finger.begin(57600);
  Serial.begin(9600);

  lcd.clear();
  lcd.print("Finding Module");
  lcd.setCursor(0, 1);
  delay(1000);

  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
    lcd.clear();
    lcd.print("Found Module ");
    delay(1000);
  } else {
    lcd.clear();
    lcd.print("Module not Found");
    lcd.setCursor(0, 1);
    lcd.print("Check Connections");
    while (1);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  lcd.setCursor(0, 0);
  lcd.print("Press UP/Down ");
  lcd.setCursor(0, 1);
  lcd.print("to start System");
  Serial.print("Press UP/Down to start System\n");
  lcd.backlight();

  if (digitalRead(up) == 0 || digitalRead(down) == 0) {
    while (digitalRead(down) == 1) {
      for (int i = 0; i < 5; i++) {
        Serial.println("Place Finger");
        int result = getFingerprintIDez();
        if (result >= 0) {
          digitalWrite(openLight, HIGH);
          digitalWrite(door, HIGH);
          digitalWrite(closeLight, LOW);
          lcd.clear();
          lcd.print("Allowed");
          lcd.setCursor(0, 1);
          lcd.print("Gate Opened   ");
          Serial.println("Allowed, gate opened");
          delay(5000);
          //digitalWrite(closeLight, HIGH);
          //digitalWrite(openLight, LOW);
          //lcd.setCursor(0,1);
          digitalWrite(door, LOW);
          //lcd.print("Gate Closed   ");
          lcd.println("Gate closed");
          return;
        } else {
          Serial.println("enroll");
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("enroll");
        }
      }
    }
  }

  checkKeys();
  delay(1000);
}

void checkKeys() {
  if (digitalRead(enroll) == 0) {
    Serial.println("Dang ky van tay");
    lcd.clear();
    lcd.print("Please Wait");
    delay(1000);
    while (digitalRead(enroll) == 0);
    Enroll();
  } else if (digitalRead(del) == 0) {
    Serial.println("Xoa van tay");
    lcd.clear();
    lcd.print("Please Wait");
    delay(1000);
    delet();
  }
}

void Enroll() {
  int count = 0;
  lcd.clear();
  lcd.print("Enroll Finger    ");
  Serial.println("Enroll Finger    ");
  lcd.setCursor(0, 1);
  lcd.print("Location:");
  Serial.println("Location:");

  while (1) {
    lcd.setCursor(9, 1);
    lcd.print(count);
    lcd.print("  ");

    if (digitalRead(up) == 0) {
      count++;
      if (count > 25)
        count = 0;
      Serial.println(count);
      delay(500);
    } else if (digitalRead(down) == 0) {
      count--;
      if (count < 0)
        count = 25;
      Serial.println(count);
      delay(500);
    } else if (digitalRead(del) == 0) {
      id = count;
      getFingerprintEnroll();
      return;
    } else if (digitalRead(enroll) == 0) {
      return;
    }
  }
}

void delet() {
  int count = 0;
  lcd.clear();
  lcd.print("Delete Finger    ");
  lcd.setCursor(0, 1);
  lcd.print("Location:");

  while (1) {
    lcd.setCursor(9, 1);
    lcd.print(count);
    lcd.print("  ");
    Serial.println(count);

    if (digitalRead(up) == 0) {
      count++;
      if (count > 25)
        count = 0;
      delay(500);
    } else if (digitalRead(down) == 0) {
      count--;
      if (count < 0)
        count = 25;
      delay(500);
    } else if (digitalRead(del) == 0) {
      id = count;
      deleteFingerprint(id);
      return;
    } else if (digitalRead(enroll) == 0) {
      return;
    }
  }
}

uint8_t getFingerprintEnroll() {
  int p = -1;
  lcd.clear();
  lcd.print("finger ID:");
  lcd.print(id);
  Serial.println("Finger ID: ");
  Serial.println(id);
  lcd.setCursor(0, 1);
  lcd.print("Place Finger");
  delay(2000);

  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        lcd.clear();
        lcd.print("Image taken");
        Serial.println("Image taken");
        break;
      case FINGERPRINT_NOFINGER:
        lcd.clear();
        lcd.print("No Finger");
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        lcd.clear();
        lcd.print("Comm Error");
        break;
      case FINGERPRINT_IMAGEFAIL:
        lcd.clear();
        lcd.print("Imaging Error");
        break;
      default:
        lcd.clear();
        lcd.print("Unknown Error");
        break;
    }
  }

  // OK success!
  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      lcd.clear();
      lcd.print("Image converted");
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      lcd.clear();
      lcd.print("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      lcd.clear();
      lcd.print("Comm Error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      lcd.clear();
      lcd.print("Feature Not Found");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      lcd.clear();
      lcd.print("Feature Not Found");
      return p;
    default:
      lcd.clear();
      lcd.print("Unknown Error");
      return p;
  }

  lcd.clear();
  lcd.print("Remove Finger");
  Serial.println("Remove Finger");
  delay(2000);

  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }

  p = -1;
  lcd.clear();
  lcd.print("Place Finger");
  lcd.setCursor(0, 1);
  lcd.print("   Again");
  Serial.println("Place Finger Again");

  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
  }

  // OK success!
  p = finger.image2Tz(2);
  p = finger.createModel();
  p = finger.storeModel(id);

  if (p == FINGERPRINT_OK) {
    //lcd.clear();
    //lcd.print("Stored!");
    Serial.println("Stored!");
    //delay(2000);
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    return p;
  } else {
    return p;
  }
}

int getFingerprintIDez() {
  uint8_t p = finger.getImage();

  if (p != FINGERPRINT_OK)
    return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)
    return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK) {
    lcd.clear();
    lcd.print("Finger Not Found");
    lcd.setCursor(0, 1);
    lcd.print("Try Later");
    delay(2000);
    return -1;
  }

  return finger.fingerID;
}

uint8_t deleteFingerprint(uint8_t id) {
  uint8_t p = -1;
  lcd.clear();
  lcd.print("Please wait");
  p = finger.deleteModel(id);

  if (p == FINGERPRINT_OK) {
    lcd.clear();
    lcd.print("Figer Deleted");
    lcd.setCursor(0, 1);
    lcd.print("Successfully");
    delay(1000);
  } else {
    lcd.clear();
    lcd.print("Something Wrong");
    lcd.setCursor(0, 1);
    lcd.print("Try Again Later");
    delay(2000);
    return p;
  }
}
