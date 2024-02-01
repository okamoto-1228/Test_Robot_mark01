#!/usr/bin/env python3
import rclpy  # ROS2 python
from rclpy.node import Node
from geometry_msgs.msg import Twist

import pigpio  # pigpio

Right_Wheel1 = 14
Right_Wheel2 = 15
Left_Wheel1 = 18
Left_Wheel2 = 23


TREAD = 1
SPEED = 1


class TestRobotMark01Master(Node):  # Nodeクラスを継承したクラスを作成
    def __init__(self):
        super().__init__("test_robot_mark01_master")  # Nodeクラスのコンストラクタを呼び出す
        # pigpioのセットアップ
        self.pi = pigpio.pi()
        self.setup()

        self.sub_cmd_vel = self.create_subscription(
            Twist, "cmd_vel", self.callback_cmd_vel, 10
        )

    def setup(self):
        self.pi.set_mode(Right_Wheel1, pigpio.OUTPUT)
        self.pi.set_mode(Right_Wheel2, pigpio.OUTPUT)
        self.pi.set_mode(Left_Wheel1, pigpio.OUTPUT)
        self.pi.set_mode(Left_Wheel2, pigpio.OUTPUT)

        # PWM：800Hz
        self.pi.set_PWM_frequency(Left_Wheel1, 800)
        self.pi.set_PWM_frequency(Left_Wheel2, 800)
        self.pi.set_PWM_frequency(Right_Wheel1, 800)
        self.pi.set_PWM_frequency(Right_Wheel2, 800)

        # PWM：Duty 0~100
        self.pi.set_PWM_range(Left_Wheel1, 100)
        self.pi.set_PWM_range(Left_Wheel2, 100)
        self.pi.set_PWM_range(Right_Wheel1, 100)
        self.pi.set_PWM_range(Right_Wheel2, 100)

    def callback_cmd_vel(self, vel):
        right_duty = 0
        left_duty = 0
        right_duty += (vel.linear.x + vel.angular.z * TREAD / 2.0) * 100 / SPEED
        left_duty += (vel.linear.x - vel.angular.z * TREAD / 2.0) * 100 / SPEED

        # Duty比の調整
        if right_duty > 100:
            right_duty = 100
        elif right_duty < -100:
            right_duty = -100
        if left_duty > 100:
            left_duty = 100
        elif left_duty < -100:
            left_duty = -100

        print("right_duty: ", right_duty)
        print("left_duty: ", left_duty)

        # 回転
        if right_duty > 0:
            self.pi.set_PWM_dutycycle(Right_Wheel1, right_duty)
            self.pi.set_PWM_dutycycle(Right_Wheel2, 0)
        elif right_duty < 0:
            self.pi.set_PWM_dutycycle(Right_Wheel2, -right_duty)
            self.pi.set_PWM_dutycycle(Right_Wheel1, 0)
        else:
            self.pi.set_PWM_dutycycle(Right_Wheel2, 0)
            self.pi.set_PWM_dutycycle(Right_Wheel1, 0)

        if left_duty > 0:
            self.pi.set_PWM_dutycycle(Left_Wheel1, left_duty)
            self.pi.set_PWM_dutycycle(Left_Wheel2, 0)
        elif left_duty < 0:
            self.pi.set_PWM_dutycycle(Left_Wheel2, -left_duty)
            self.pi.set_PWM_dutycycle(Left_Wheel1, 0)
        else:
            self.pi.set_PWM_dutycycle(Left_Wheel2, 0)
            self.pi.set_PWM_dutycycle(Left_Wheel1, 0)

    def destroy(self):
        self.pi.write(Right_Wheel1, 0)
        self.pi.write(Right_Wheel2, 0)
        self.pi.write(Left_Wheel1, 0)
        self.pi.write(Left_Wheel2, 0)
        self.pi.stop()
        super().destroy()


def main(args=None):
    rclpy.init(args=args)
    node = TestRobotMark01Master()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        node.destroy()
        rclpy.shutdown()
    else:
        rclpy.shutdown()


if __name__ == "__main__":
    main()
