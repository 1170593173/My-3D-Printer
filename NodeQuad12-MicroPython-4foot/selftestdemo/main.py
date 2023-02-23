import time
from machine import I2C,Pin
from servo import Servos
i2c=I2C(sda=Pin(21),scl=Pin(22),freq=10000)
servos=Servos(i2c,address=0x40)

while True:
    for i in range(2, 150,10):
        servos.position(0,i)
        servos.position(1,i)
        time.sleep_us(100000)
    time.sleep_us(100000)
    for i in range(150, 2, -1):
        servos.position(0,i)
        servos.position(1,i)
        time.sleep_us(10000)
    time.sleep_us(100000)
