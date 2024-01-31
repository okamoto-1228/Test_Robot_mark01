#!/usr/bin/env python3
import pigpio
import time

Motor1In1 = 14
Motor1In2 = 15
Motor2In1 = 18
Motor2In2 = 23


def setup():
    global pi
    pi = pigpio.pi()
    pi.set_mode(Motor1In1, pigpio.OUTPUT)
    pi.set_mode(Motor1In2, pigpio.OUTPUT)
    pi.set_mode(Motor2In1, pigpio.OUTPUT)
    pi.set_mode(Motor2In2, pigpio.OUTPUT)


def main():
    while True:
        print("Motor1+2In1:High")
        pi.write(Motor1In1, 1)
        pi.write(Motor1In2, 0)
        pi.write(Motor2In1, 1)
        pi.write(Motor2In2, 0)
        time.sleep(3)

        print("Motor1+2In2:High")
        pi.write(Motor1In1, 0)
        pi.write(Motor1In2, 1)
        pi.write(Motor2In1, 0)
        pi.write(Motor2In2, 1)
        time.sleep(3)


def destroy():
    pi.write(Motor1In1, 0)
    pi.write(Motor1In2, 0)
    pi.write(Motor2In1, 0)
    pi.write(Motor2In2, 0)
    pi.stop()


if __name__ == "__main__":
    try:
        setup()
        main()
    except KeyboardInterrupt:
        destroy()
