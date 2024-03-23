#include<Wire.h>
#include<Servo.h>
Servo myservo;
int servoPin=7;
int led=5;

const int MPU_ADDR = 0x68;    // I2C통신을 위한 MPU6050의 주소
int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;   // 가속도(Acceleration)와 자이로(Gyro)
double angleAcX;
double angleAcY;
// double angleAcZ;
const double RADIAN_TO_DEGREE = 180 / 3.14159;
void setup() {
  initSensor();
  Serial.begin(9600);
  myservo.attach(servoPin);
  
}

void loop() {
  getAngleXY(); 
  Serial.print("Angle x : ");
  Serial.print(angleAcX);
  Serial.print("\t\t Angle y : ");
  Serial.println(angleAcY);
  delay(1);
 
  if(angleAcX<-26){
    myservo.write(135);
    delay(15);
  }
  if(angleAcX<1&&angleAcX>-1){
    myservo.write(90);
    delay(15);
  }
  if(angleAcX>26){
    myservo.write(45);
    delay(15);
  }
  
}

double getAngleXY() {
  getData();  
  // 삼각함수를 이용한 롤(Roll)의 각도 구하기 
  angleAcX = atan(AcY / sqrt(pow(AcX, 2) + pow(AcZ, 2)));
  angleAcX *= RADIAN_TO_DEGREE;
  // 삼각함수를 이용한 피치(Pitch)의 각도 구하기
  angleAcY = atan(-AcX / sqrt(pow(AcY, 2) + pow(AcZ, 2)));
  angleAcY *= RADIAN_TO_DEGREE;
  // angleAcZ값(Yaw)은 아래의 삼각함수 공식은 있으나, 가속도 센서만 이용해서는 원하는 데이터를 얻을 수 없어 생략
// angleAcZ = atan(sqrt(pow(AcX, 2) + pow(AcY, 2)) / AcZ );
// angleAcZ *= RADIAN_TO_DEGREE;
}

void initSensor() {
  Wire.begin();
  Wire.beginTransmission(MPU_ADDR);   // I2C 통신용 어드레스(주소)
  Wire.write(0x6B);    // MPU6050과 통신을 시작하기 위해서는 0x6B번지에    
  Wire.write(0);
  Wire.endTransmission(true);
}

void getData() {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);   // AcX 레지스터 위치(주소)를 지칭합니다
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, 14, true);  // AcX 주소 이후의 14byte의 데이터를 요청
  AcX = Wire.read() << 8 | Wire.read(); //두 개의 나뉘어진 바이트를 하나로 이어 붙여서 각 변수에 저장
  AcY = Wire.read() << 8 | Wire.read();
  AcZ = Wire.read() << 8 | Wire.read();
  Tmp = Wire.read() << 8 | Wire.read();
  GyX = Wire.read() << 8 | Wire.read();
  GyY = Wire.read() << 8 | Wire.read();
  GyZ = Wire.read() << 8 | Wire.read();
}