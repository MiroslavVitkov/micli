#!/usr/bin/env python

"""Handle serial communication with the device and do callbacks.

"""

import serial


class Serial(object):
    """Serial duplex communication."""
    def __init__(self, clb):
        self.clb = clb  # callback for new available measurement
        self.comm = serial.Serial(port='/dev/ttyUSB0',
                                  baudrate=9600,  # 115200 is highest standard
                                  bytesize=serial.EIGHTBITS,
                                  parity=serial.PARITY_NONE,
                                  stopbits=serial.STOPBITS_ONE,
                                  timeout=None,  # None==forever,
                                                 # 0=non-blocking,
                                                 # foat=seconds
                                  xonxoff=False,  # sw flow control
                                  rtscts=False,  # hw flow control
                                  writeTimeout=None,  # see 'timeout'
                                  dsrdtr=False,  # hw flow control
                                  interCharTimeout=None,
                                  )

    def listen_forever(self):
        """Blocking! Forever!"""
        while True:
            measurement = self.comm.readline()
            self.clb(measurement)

    def _generate_random_data(self):
        """For debug purposes."""
        import random
        while True:
            measurement = str(random.randint(-30000, 50000)) + "\n"
            self.clb(measurement)

    def read_line(self):
        """Blocks until newline is received."""
        self.comm.readline()


def main():
    pass

if __name__ == "__main__":
    main()
