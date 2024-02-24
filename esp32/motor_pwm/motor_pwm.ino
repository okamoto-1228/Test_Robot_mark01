// モーターのピンを設定
#define RIGHT_MOTOR_IN1 13
#define RIGHT_MOTOR_IN2 12
#define LEFT_MOTOR_IN1 14
#define LEFT_MOTOR_IN2 27

#define RIGHT_ENABLE 26
#define LEFT_ENABLE 25


// PWM制御の設定
const int freq = 30000;
const int rightpwmChannel = 0;
const int leftpwmChannel = 1;
const int resolution = 8;
int dutyCycle = 200;

void setup() {
  // ピンのセットアップ
  pinMode(RIGHT_MOTOR_IN1, OUTPUT);
  pinMode(RIGHT_MOTOR_IN2, OUTPUT);
  pinMode(LEFT_MOTOR_IN1, OUTPUT);
  pinMode(LEFT_MOTOR_IN2, OUTPUT);

  // PWM制御のセットアップ
  ledcSetup(rightpwmChannel, freq, resolution);
  ledcSetup(leftpwmChannel, freq, resolution);


  // PWM制御でモーターをアタッチ
  ledcAttachPin(RIGHT_ENABLE, rightpwmChannel);
  ledcAttachPin(LEFT_ENABLE, leftpwmChannel);


  //シリアルモニターに表示
  Serial.begin(115200);
 
  // シリアルモニターで確認
  Serial.print("Testing DC Motor...");
}

void loop() {
  digitalWrite(RIGHT_MOTOR_IN1, LOW);
  digitalWrite(RIGHT_MOTOR_IN2, LOW);
  digitalWrite(LEFT_MOTOR_IN1, LOW);
  digitalWrite(LEFT_MOTOR_IN2, LOW);
  // digitalWrite(RIGHT_MOTOR_IN1, HIGH);
  // digitalWrite(RIGHT_MOTOR_IN2, LOW);
  // digitalWrite(LEFT_MOTOR_IN1, HIGH);
  // digitalWrite(LEFT_MOTOR_IN2, LOW);
  // delay(5000);

  // digitalWrite(RIGHT_MOTOR_IN1, LOW);
  // digitalWrite(RIGHT_MOTOR_IN2, HIGH);
  // digitalWrite(LEFT_MOTOR_IN1, LOW);
  // digitalWrite(LEFT_MOTOR_IN2, HIGH);
  // delay(5000);

  // PWM制御を使って徐々に加速
  digitalWrite(RIGHT_MOTOR_IN1, HIGH);
  digitalWrite(RIGHT_MOTOR_IN2, LOW);
  digitalWrite(LEFT_MOTOR_IN1, HIGH);
  digitalWrite(LEFT_MOTOR_IN2, LOW);
  while (dutyCycle <= 255){
    ledcWrite(rightpwmChannel, dutyCycle);
    ledcWrite(leftpwmChannel, dutyCycle);
    Serial.print("Forward with duty cycle: ");
    Serial.println(dutyCycle);
    dutyCycle = dutyCycle + 5;
    delay(500);
  }
  dutyCycle = 200;

  // PWM制御を使って徐々に加速
  digitalWrite(RIGHT_MOTOR_IN1, LOW);
  digitalWrite(RIGHT_MOTOR_IN2, HIGH);
  digitalWrite(LEFT_MOTOR_IN1, LOW);
  digitalWrite(LEFT_MOTOR_IN2, HIGH);
  while (dutyCycle <= 255){
    ledcWrite(rightpwmChannel, dutyCycle);
    ledcWrite(leftpwmChannel, dutyCycle);
    Serial.print("Forward with duty cycle: ");
    Serial.println(dutyCycle);
    dutyCycle = dutyCycle + 5;
    delay(500);
  }
  dutyCycle = 200;

}
