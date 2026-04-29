''' Subclass of Car for radio-controlled Mini-z'''
import logging
import struct
from dataclasses import dataclass
from time import sleep, time
import serial
from math import sin, radians

logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)
logger.setLevel(logging.DEBUG)

#from buzzracer.cars.car import Car, CarParams

@dataclass
class CarParams:
    min_pwm_left: int
    max_pwm_right: int
    max_steering_left: float
    max_steering_right: float


class FHSS():
    car_count = 0
    cars = []
    pwm_values = [1500] * 12 # 6 car, 2 val each (steering, throttle)
    frame_header = bytes([0xAA, 0x55])

    def __init__(self, param: CarParams):
        # Car.__init__(self, main)
        self.param = param
        self.throttle = 0.0
        self.steering = 0.0

        self.index = FHSS.car_count
        FHSS.car_count += 1
        FHSS.cars.append(self)

    @classmethod
    def init(cls):
        serial_port = '/dev/ttyUSB0'
        try:
            FHSS.serial_port = serial.Serial(
                serial_port, 115200, timeout=0.001, writeTimeout=0)
        except (FileNotFoundError, serial.serialutil.SerialException):
            print(f'interface {serial_port} not found')
            raise

    def actuate(self):
        # Car.actuate(self)
        steering_pwm = int(self.mapdata(self.steering,
                                        self.param.max_steering_left,
                                        -self.param.max_steering_right,
                                        self.param.min_pwm_left,
                                        self.param.max_pwm_right))
        throttle_pwm = int(self.mapdata(self.throttle, -1.0, 1.0, 1900, 1100))
        FHSS.pwm_values[2*self.index] = steering_pwm
        FHSS.pwm_values[2*self.index + 1] = throttle_pwm


    @classmethod
    def send_pwm_array(cls) -> bool:
        if len(FHSS.pwm_values) != 12:
            raise ValueError("PWM array must contain exactly 12 elements")
            
        try:
            # Pack 10 unsigned 16-bit integers (Little-Endian)
            # Result is exactly 20 bytes
            payload = struct.pack('<12H', *FHSS.pwm_values)
            
            # Calculate CRC over the payload
            crc = FHSS.calculate_crc8(payload)
            # print(f'payload {payload} crc: {hex(crc)}')
            
            # Construct the final 27-byte frame
            frame = bytearray(FHSS.frame_header)
            frame.extend(payload)
            frame.append(crc)
            
            count = FHSS.serial_port.write(frame)
            return count == 27
            
        except serial.SerialException as e:
            print(f"Serial write error: {e}")
            return False


    def mapdata(self, x, a, b, c, d):
        y = (x-a)/(b-a)*(d-c)+c
        return int(y)

    @classmethod
    def calculate_crc8(cls, data: bytes) -> int:
        """Calculates CRC-8-CCITT (Poly: 0x07) for a given byte array."""
        crc = 0x00
        for byte in data:
            crc ^= byte
            for _ in range(8):
                if crc & 0x80:
                    crc = (crc << 1) ^ 0x07
                else:
                    crc <<= 1
                crc &= 0xFF
        return crc

    @classmethod
    def read_serial_monitor(cls):
        """
        Reads any waiting bytes from the Arduino and prints them to the console.
        Non-blocking: returns immediately if there's nothing to read.
        """
        if FHSS.serial_port.in_waiting > 0:
            try:
                # Read everything sitting in the OS buffer
                raw_bytes = FHSS.serial_port.read(FHSS.serial_port.in_waiting)
                
                # Decode as ASCII. We use errors='replace' so that if a random 
                # corrupted byte or binary artifact comes through, it prints a '?' 
                # instead of crashing the Python script with a UnicodeDecodeError.
                text = raw_bytes.decode('ascii', errors='replace')
                
                # Print without adding an extra newline, since Arduino's println 
                # already sends \r\n
                print(text, end='', flush=True)
                
            except serial.SerialException as e:
                print(f"\n[Serial Read Error]: {e}")

if __name__=="__main__":
    pi = 3.14159
    param = CarParams(
        min_pwm_left=1200,
        max_pwm_right=1800,
        max_steering_left=radians(27),
        max_steering_right=radians(27))
    car0 = FHSS(param)
    car1 = FHSS(param)
    car2 = FHSS(param)

    T = 3.0
    FHSS.init()
    for i in range(10000):
        # car0.steering = sin(2*pi/T*(time())) * radians(26.1)
        # car1.steering = sin(2*pi/T*(time()+0.3)) * radians(26.1)
        # car2.steering = sin(2*pi/T*(time()+0.3)) * radians(26.1)
        # car0.steering = radians(26)
        # car1.steering = radians(26)
        # car2.steering = radians(26)
        # car0.actuate()
        # car1.actuate()
        # car2.actuate()

        FHSS.pwm_values = [2000,1500] * 6
        FHSS.send_pwm_array()
        FHSS.read_serial_monitor()
        print(FHSS.pwm_values)
        sleep(0.01)