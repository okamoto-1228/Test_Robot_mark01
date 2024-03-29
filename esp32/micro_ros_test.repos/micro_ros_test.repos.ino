#include <micro_ros_arduino.h>
#include <stdio.h>
#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <geometry_msgs/msg/twist.h>

rcl_subscription_t subscriber;
geometry_msgs__msg__Twist msg;
rclc_executor_t executor;
rcl_allocator_t allocator;
rclc_support_t support;
rcl_node_t node;

// モーターのピンを設定
#define RIGHT_MOTOR_IN1 13
#define RIGHT_MOTOR_IN2 12
#define LEFT_MOTOR_IN1 14
#define LEFT_MOTOR_IN2 27
#define RIGHT_ENABLE 26
#define LEFT_ENABLE 25

// micro-ros
#define LED_PIN 2
#define RCCHECK(fn) \
  { \
    rcl_ret_t temp_rc = fn; \
    if ((temp_rc != RCL_RET_OK)) { error_loop(); } \
  }
#define RCSOFTCHECK(fn) \
  { \
    rcl_ret_t temp_rc = fn; \
    if ((temp_rc != RCL_RET_OK)) { error_loop(); } \
  }

#define TREAD 0.1 //トレッド長 [m]
#define SPEED 1 //速度

// PWM制御の設定
const int freq = 30000;
const int rightpwmChannel = 0;
const int leftpwmChannel = 1;
const int resolution = 8;
int dutyCycle = 200;

void error_loop() {
  while (1) {
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    delay(100);
  }
}

//twist message cb
void subscription_callback(const void *msgin) {
  // duty比
  int right_duty = 0;
  int left_duty = 0;

  const geometry_msgs__msg__Twist *vel = (const geometry_msgs__msg__Twist *)msgin;
  // if velocity in x direction is 0 turn off LED, if 1 turn on LED
  digitalWrite(LED_PIN, (vel->linear.x == 0) ? LOW : HIGH);

  right_duty += (vel->linear.x + vel->angular.z * TREAD / 2) * 255 / SPEED;
  left_duty += (vel->linear.x - vel->angular.z * TREAD / 2) * 255 / SPEED;

  // duty比の調整
  if (right_duty > 255) right_duty = 255;
  else if (right_duty < -255) right_duty = -255;
  if (left_duty > 255) left_duty = 255;
  else if (left_duty < -255) left_duty = -255;

  Serial.print("right_duty: ");
  Serial.println(right_duty);
  Serial.print("left_duty: ");
  Serial.println(left_duty);

  // 回転
  if (right_duty > 0) {  //前方向
    digitalWrite(RIGHT_MOTOR_IN1, HIGH);
    digitalWrite(RIGHT_MOTOR_IN2, LOW);
    ledcWrite(rightpwmChannel, right_duty);
  } else if (right_duty < 0) {  //後ろ方向
    digitalWrite(RIGHT_MOTOR_IN1, LOW);
    digitalWrite(RIGHT_MOTOR_IN2, HIGH);
    ledcWrite(rightpwmChannel, right_duty);
  } else {  //モーター停止
    digitalWrite(RIGHT_MOTOR_IN1, LOW);
    digitalWrite(RIGHT_MOTOR_IN2, LOW);
    ledcWrite(rightpwmChannel, 0);
  }

  if (left_duty > 0) {  //前方向
    digitalWrite(LEFT_MOTOR_IN1, HIGH);
    digitalWrite(LEFT_MOTOR_IN2, LOW);
    ledcWrite(leftpwmChannel, left_duty);
  } else if (left_duty < 0) {  //後ろ方向
    digitalWrite(LEFT_MOTOR_IN1, LOW);
    digitalWrite(LEFT_MOTOR_IN2, HIGH);
    ledcWrite(leftpwmChannel, left_duty);
  } else {  //モーター停止
    digitalWrite(LEFT_MOTOR_IN1, LOW);
    digitalWrite(LEFT_MOTOR_IN2, LOW);
    ledcWrite(leftpwmChannel, 0);
  }
}

void setup() {
  // micro-ros
  set_microros_wifi_transports("Buffalo-G-9EC8", "j6pmvtvgfy48k", "192.168.12.33", 8888);


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

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  delay(2000);

  allocator = rcl_get_default_allocator();
  //create init_options
  RCCHECK(rclc_support_init(&support, 0, NULL, &allocator));
  // create node
  RCCHECK(rclc_node_init_default(&node, "micro_ros_arduino_node", "", &support));
  // create subscriber
  RCCHECK(rclc_subscription_init_default(
    &subscriber,
    &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(geometry_msgs, msg, Twist),
    "cmd_vel"));

  // create executor
  RCCHECK(rclc_executor_init(&executor, &support.context, 1, &allocator));
  RCCHECK(rclc_executor_add_subscription(&executor, &subscriber, &msg, &subscription_callback, ON_NEW_DATA));

  // シリアルモニターで確認
  Serial.print("Testing DC Motor...");
}

void loop() {
  digitalWrite(RIGHT_MOTOR_IN1, LOW);
  digitalWrite(RIGHT_MOTOR_IN2, LOW);
  digitalWrite(LEFT_MOTOR_IN1, LOW);
  digitalWrite(LEFT_MOTOR_IN2, LOW);

  delay(100);
  RCCHECK(rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100)));


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
  // digitalWrite(RIGHT_MOTOR_IN1, HIGH);
  // digitalWrite(RIGHT_MOTOR_IN2, LOW);
  // digitalWrite(LEFT_MOTOR_IN1, HIGH);
  // digitalWrite(LEFT_MOTOR_IN2, LOW);
  // while (dutyCycle <= 255) {
  //   ledcWrite(rightpwmChannel, dutyCycle);
  //   ledcWrite(leftpwmChannel, dutyCycle);
  //   Serial.print("Forward with duty cycle: ");
  //   Serial.println(dutyCycle);
  //   dutyCycle = dutyCycle + 5;
  //   delay(500);
  // }
  // dutyCycle = 200;

  // PWM制御を使って徐々に加速
  // digitalWrite(RIGHT_MOTOR_IN1, LOW);
  // digitalWrite(RIGHT_MOTOR_IN2, HIGH);
  // digitalWrite(LEFT_MOTOR_IN1, LOW);
  // digitalWrite(LEFT_MOTOR_IN2, HIGH);
  // while (dutyCycle <= 255) {
  //   ledcWrite(rightpwmChannel, dutyCycle);
  //   ledcWrite(leftpwmChannel, dutyCycle);
  //   Serial.print("Forward with duty cycle: ");
  //   Serial.println(dutyCycle);
  //   dutyCycle = dutyCycle + 5;
  //   delay(500);
  // }
  // dutyCycle = 200;
}
