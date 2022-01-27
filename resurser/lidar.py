import serial
import matplotlib.pyplot as plt
import math

class Lidar:
    def __init__(self):
        self.ser = serial.Serial("/dev/ttyACM0")
        self.params = {}
        self.connect()
    
    def connect(self):
        self.query("SCIP2.0")

        response = self.query("PP")
        assert response[:2] == ["PP", "00P"]

        for entry in response[2:]:
            colon_idx = entry.index(":")
            semicolon_idx = entry.index(";")

            key = entry[:colon_idx]
            value = entry[colon_idx + 1:semicolon_idx]
            self.params[key] = value

    def query(self, query):
        print("W: ", query)
        self.ser.write((query + "\n").encode("utf8"))
        lines = []
        while True:
            line = self.ser.readline().decode("utf8").rstrip()
            print("R: ", line)
            if not line: break
            lines += [line]
        return lines
    
    def decode_number(self, encoded):
        decoded = 0

        for c in encoded:
            decoded = decoded * 64 + ord(c) - 0x30

        return decoded
    
    def laser_on(self):
        response = self.query("BM")
        assert response == ["BM", "00P"] or response == ["BM", "02R"]

    def laser_off(self):
        response = self.query("QT")
        assert response == ["QT", "00P"]

    def capture(self):
        self.laser_on()

        response = self.query("GD" + self.params["AMIN"].zfill(4) + self.params["AMAX"].zfill(4) + "01")

        print("Timestamp:", self.decode_number(response[2][:-1]))

        data = ""
        for line in response[3:]:
            data += line[:-1]
        
        values = []
        for i in range(0, len(data), 3):
            values += [self.decode_number(data[i:i+3])]
        
        return values

lidar = Lidar()
print(lidar.params)

try:
    while True:
        values = lidar.capture()

        plt.clf()
        plt.axis([-1000, 1000, -1000, 1000])

        angle_deg = [i / len(values) * 240 - 120 for i in range(len(values))]
        angle_rad = [angle / 180 * math.pi for angle in angle_deg]

        x_values = [val * -math.sin(angle) for val, angle in zip(values, angle_rad) if val > 30]
        y_values = [val * math.cos(angle) for val, angle in zip(values, angle_rad) if val > 30]

        plt.plot(x_values, y_values)
        plt.plot(0, 0, '*')

        plt.pause(0.05)

except:
    lidar.laser_off()
