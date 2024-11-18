#include <Adafruit_Fingerprint.h>
#include <LiquidCrystal.h>
#include <Keypad.h>
#include <Wire.h>

// Define pin connections
const int rs = 12, en = 11, d4 = 7, d5 = 6, d6 = 5, d7 = 4;
const byte ROWS = 4;
const byte COLS = 4;

// Define keypad layout
char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

// Define keypad pin connections
byte rowPins[ROWS] = {13, 10, 9, 8};
byte colPins[COLS] = {A0, A1, A2, A3};

// Initialize keypad
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

// Initialize fingerprint sensor
SoftwareSerial mySerial(2, 3); // RX/TX
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
uint8_t fingerprintID = 0;
bool fingerprintRegistered[10] = {false};

// Initialize LCD
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

String studentPrefix = "SE";

bool fingerDetected = false; // Biến để theo dõi trạng thái phát hiện ngón tay

struct Student {
  String studentID;
  uint8_t fingerprintID;
};

// Define an array to store student information
Student students[10];

int MAX_FINGERPRINTS = 10;

// Function prototypes
void setup();
void loop();
void enrollFingerprint();
void checkAttendance();
uint8_t readnumber();
bool readStudentID();
uint8_t getFingerprintEnroll();
uint8_t getFingerprintID();

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // Initialize fingerprint sensor
  finger.begin(57600);
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor");
    while (1);
  }

  // Initialize LCD
  lcd.begin(16, 2);
  lcd.print("Attendance");
  lcd.setCursor(0, 1);
  lcd.print("Check System");
  delay(4000);

  lcd.clear();
  lcd.print("1.Enroll");
  lcd.setCursor(0, 1);
  lcd.print("2.Check 3.Empty");
}

void loop() {
  char key = customKeypad.getKey();
  if (key) {
    if (key == '1') {
      enrollFingerprint();
    } else if (key == '2') {
      checkAttendance();
    }
    else if (key == '3') {
      emptyData();
    }
  }
}

uint8_t readnumber(void) {
  char key = customKeypad.getKey();
  if (key) {
    if (key >= '0' && key <= '9') {
      fingerprintID = fingerprintID * 10 + (key - '0'); // Cập nhật giá trị ID khi nhập các số từ 0 đến 9
      lcd.print(key); // Hiển thị số vừa nhập lên LCD
    } else if (key == '#') {
      if (fingerprintID >= 1 && fingerprintID <= 10) {
        uint8_t tempId = fingerprintID; // Lưu trữ giá trị ID tạm thời
        fingerprintID = 0; // Đặt lại biến ID về 0 để chuẩn bị cho quá trình nhập tiếp theo
        return tempId; // Trả về ID tạm thời
      } else {
        fingerprintID = 0; // Reset ID nếu không nằm trong phạm vi cho phép
        lcd.clear();
        lcd.print("Invalid ID");
        delay(1000);
        lcd.clear();
        lcd.print("Enter ID (0-9):");
      }
    }
  }
  return 0; // Trả về 0 nếu không có phím được nhấn
}




void enrollFingerprint() {
  lcd.clear();
  lcd.print("Enroll Finger");
  delay(4000);
  lcd.clear();
  lcd.print("Please Enter ID:");
  lcd.setCursor(0, 1);

  char key = customKeypad.getKey();
  while (true) {
    if (key) {
      if (key == '#') {
        if (fingerprintID == 0) {
          lcd.clear();
          lcd.print("ID not allowed");
          delay(2000);
          lcd.clear();
          lcd.print("Please Enter ID:");
          lcd.setCursor(0, 1);
        } else if (fingerprintRegistered[fingerprintID]) {
          lcd.clear();
          Serial.println("This fingerprint ID has already been registered. Please choose another ID.");
          lcd.print("ID already");
          lcd.setCursor(0, 1);
          lcd.print("exists");
          delay(2000);
          fingerprintID = 0;
          lcd.clear();
          lcd.print("Please Enter ID:");
          lcd.setCursor(0, 1);
          break;
        } else {
          lcd.clear();
          lcd.print("Enter student ID:");
          lcd.setCursor(0, 1);
          lcd.print("SE");
          String studentPrefix = "";
          while (true) {
            char studentKey = customKeypad.getKey();
            if (studentKey) {
              if (studentKey >= '0' && studentKey <= '9') {
                // Chỉ cho phép nhập số
                studentPrefix += studentKey;
                lcd.print(studentKey);
              } else if (studentKey == '*') {
                // Xóa ký tự cuối cùng
                if (studentPrefix.length() > 0) {
                  studentPrefix = studentPrefix.substring(0, studentPrefix.length() - 1);
                  lcd.clear();
                  lcd.print("Enter student ID:");
                  lcd.setCursor(0, 1);
                  lcd.print("SE" + studentPrefix);
                }
              } else if (studentKey == '#') {
                // Kết thúc nhập
                lcd.clear();
                break;
              }
            }
          }

          // Kiểm tra xem studentPrefix đã được sử dụng cho ID nào chưa
          bool studentIDExists = false;
          for (int i = 0; i < MAX_FINGERPRINTS; ++i) {
            if (students[i].studentID == studentPrefix) {
              studentIDExists = true;
              break;
            }
          }
          if (studentIDExists) {
            lcd.clear();
            lcd.print("Student ID");
            lcd.setCursor(0, 1);
            lcd.print("already exists");
            delay(2000);

            fingerprintID = 0;
            studentPrefix = "";
            lcd.clear();
            lcd.print("1.Enroll");
            lcd.setCursor(0, 1);
            lcd.print("2.Check 3.Empty");
            break;
          }

          // In thông tin đăng ký
          lcd.clear();
          lcd.print("Enrolling ID: ");
          lcd.print(fingerprintID); // Hiển thị ID đã nhập, không hiển thị #0 mặc định
          delay(2000);
          lcd.clear();
          lcd.print("Student ID: ");
          lcd.setCursor(0, 1);
          lcd.print("SE" + studentPrefix);
          delay(2000);

          // Lưu thông tin sinh viên vào mảng students
          students[fingerprintID].studentID = studentPrefix;
          students[fingerprintID].fingerprintID = fingerprintID;

          getFingerprintEnroll(fingerprintID);
          lcd.clear();
          lcd.print("1.Enroll");
          lcd.setCursor(0, 1);
          lcd.print("2.Check 3.Empty");
          fingerprintID = 0;
          break;
        }
      } else if (key >= '0' && key <= '9') {
        fingerprintID = fingerprintID * 10 + (key - '0');
        lcd.print(key);
      }
    }
    key = customKeypad.getKey(); // Lấy phím tiếp theo từ bàn phím
  }
}






void checkAttendance() {
  lcd.clear();
  lcd.print("Checking");
  lcd.setCursor(0, 1);
  lcd.print("attendance...");
  delay(4000);

  lcd.clear();
  lcd.print("Place finger");
  lcd.setCursor(0, 1);
  lcd.print("on sensor");

  while (!isFingerDetected()) {
    // Kiểm tra trạng thái của cảm biến vân tay
    delay(500); // Đợi một chút trước khi kiểm tra lại
  }

  uint8_t fingerprintID = getFingerprintID();

  if (fingerprintID != FINGERPRINT_NOTFOUND) {
    // Search for matching student ID
    String studentID = "";
    for (int i = 0; i < 10; ++i) {
      if (students[i].fingerprintID == fingerprintID) {
        studentID = students[i].studentID;
        break;
      }
    }

    lcd.clear();
    lcd.print("Matching:");
    lcd.setCursor(0, 1);
    if (studentID != "") {
      lcd.print("ID: SE" + studentID);
      Serial.print("SE" + studentID); // Bắt đầu bằng một ký tự đặc biệt để phân biệt dữ liệu
      delay(3000);
      lcd.clear();
      lcd.print("Returning to");
      lcd.setCursor(0, 1);
      lcd.print("main menu...");
    } else {
      lcd.clear();
      lcd.print("No student");
      lcd.setCursor(0, 1);
      lcd.print("ID found");
    }
    delay(3000);
  } else {
    lcd.clear();
    lcd.print("No fingerprint");
    lcd.setCursor(0, 1);
    lcd.print("detected");
    delay(2000);
  }


  lcd.clear();
  lcd.print("1.Enroll");
  lcd.setCursor(0, 1);
  lcd.print("2.Check 3.Empty");
}

bool isFingerDetected() {
  // Kiểm tra xem có dấu vân tay nào được phát hiện không
  return finger.getImage() == FINGERPRINT_OK;
}

void emptyData() {
  finger.emptyDatabase();
  lcd.clear();
  lcd.print("Data Empty");
  delay(2000);

  lcd.clear();
  lcd.print("1.Enroll");
  lcd.setCursor(0, 1);
  lcd.print("2.Check 3.Empty");
}






uint8_t getFingerprintEnroll(uint8_t id) {
  int p = -1;
  lcd.clear();
  lcd.print("Waiting finger");
  lcd.setCursor(0, 1);
  lcd.print("enroll as #");
  lcd.print(id);

  Serial.print("Waiting for valid finger to enroll as #");
  Serial.println(id);

  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        lcd.clear();
        lcd.print("Image taken");
        delay(2000);
        Serial.println("Image taken");
        break;
      case FINGERPRINT_NOFINGER:
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        lcd.clear();
        lcd.print("Communication error");
        delay(2000);
        Serial.println("Communication error");
        break;
      case FINGERPRINT_IMAGEFAIL:
        lcd.clear();
        lcd.print("Imaging error");
        delay(2000);
        Serial.println("Imaging error");
        break;
      default:
        lcd.clear();
        lcd.print("Unknown error");
        delay(2000);
        Serial.println("Unknown error");
        break;
    }
  }

  // OK success!
  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      lcd.clear();
      lcd.print("Image converted");
      delay(2000);
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      lcd.clear();
      lcd.print("Image too messy");
      delay(2000);
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      lcd.clear();
      lcd.print("Communication error");
      delay(2000);
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      lcd.clear();
      lcd.print("Could not find features");
      delay(2000);
      Serial.println("Could not find features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      lcd.clear();
      lcd.print("Invalid image");
      delay(2000);
      Serial.println("Invalid image");
      return p;
    default:
      lcd.clear();
      lcd.print("Unknown error");
      delay(2000);
      Serial.println("Unknown error");
      return p;
  }

  lcd.clear();
  lcd.print("Remove finger");
  Serial.println("Remove finger");
  delay(2000);

  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }

  lcd.clear();
  lcd.print("Place same");
  lcd.setCursor(0, 1);
  lcd.print("finger again");

  Serial.println("Place same finger again");

  p = -1;
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        lcd.clear();
        lcd.print("Image taken");
        delay(2000);
        Serial.println("Image taken");
        break;
      case FINGERPRINT_NOFINGER:
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        lcd.clear();
        lcd.print("Communication error");
        delay(2000);
        Serial.println("Communication error");
        break;
      case FINGERPRINT_IMAGEFAIL:
        lcd.clear();
        lcd.print("Imaging error");
        delay(2000);
        Serial.println("Imaging error");
        break;
      default:
        lcd.clear();
        lcd.print("Unknown error");
        delay(2000);
        Serial.println("Unknown error");
        break;
    }
  }

  // OK success!
  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      lcd.clear();
      lcd.print("Image converted");
      delay(2000);
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      lcd.clear();
      lcd.print("Image too messy");
      delay(2000);
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      lcd.clear();
      lcd.print("Communication error");
      delay(2000);
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      lcd.clear();
      lcd.print("Could not find features");
      delay(2000);
      Serial.println("Could not find features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      lcd.clear();
      lcd.print("Invalid image");
      delay(2000);
      Serial.println("Invalid image");
      return p;
    default:
      lcd.clear();
      lcd.print("Unknown error");
      delay(2000);
      Serial.println("Unknown error");
      return p;
  }

  // OK converted!
  lcd.clear();
  lcd.print("Creating model");
  delay(2000);
  Serial.println("Creating model");


  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    lcd.clear();
    lcd.print("Prints matched!");
    delay(2000);
    Serial.println("Prints matched!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    lcd.clear();
    lcd.print("Communication error");
    delay(2000);
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    lcd.clear();
    lcd.print("Fingerprints");
    lcd.setCursor(0, 1);
    lcd.print("mismatch");
    delay(2000);
    Serial.println("Fingerprints mismatch");
    return p;
  } else {
    lcd.clear();
    lcd.print("Unknown error");
    delay(2000);
    Serial.println("Unknown error");
    return p;
  }

  lcd.clear();
  lcd.print("Storing...");
  Serial.println("Storing...");
  delay(2000);

  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    lcd.clear();
    lcd.print("Stored!");
    delay(2000);
    Serial.println("Stored!");
    fingerprintRegistered[id] = true; // Mark ID as registered
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    lcd.clear();
    lcd.print("Communication error");
    delay(2000);
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    lcd.clear();
    lcd.print("Cannot store in that location");
    delay(2000);
    Serial.println("Cannot store in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    lcd.clear();
    lcd.print("Flash write error");
    delay(2000);
    Serial.println("Flash write error");
    return p;
  } else {
    lcd.clear();
    lcd.print("Unknown error");
    delay(2000);
    Serial.println("Unknown error");
    return p;
  }
}

uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("No finger detected");
      return FINGERPRINT_NOTFOUND; // Thay vì return p, return FINGERPRINT_NOTFOUND
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK success!

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK converted!
  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);

  return finger.fingerID;
}

// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;

  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  return finger.fingerID;
}
