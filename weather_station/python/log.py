import serial
import csv
import os
import time
from datetime import datetime

# Set the correct port below. On Windows, it's usually COM3, COM4, etc.
ser = serial.Serial('COM4', 9600)  # Change to your Arduino port
filename = os.path.expanduser("~/weather_data.csv")

# Write headers if file doesn't exist
if not os.path.exists(filename):
    with open(filename, mode='w', newline='') as f:
        writer = csv.writer(f)
        writer.writerow(["Timestamp(UTC)", "DHT_Temp(°C)", "Humidity(%)", "DS18B20_Temp(°C)", "Thermistor_Temp(°C)", "Gas_ppm"])

print("Logging data to:", filename)

try:
    while True:
        line = ser.readline().decode().strip()
        print("Read:", line)
        fields = line.split(",")
        if len(fields) == 6:
            # Replace Arduino timestamp with UTC timestamp
            utc_timestamp = datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S')
            fields[0] = utc_timestamp
            with open(filename, mode='a', newline='') as f:
                writer = csv.writer(f)
                writer.writerow(fields)
except KeyboardInterrupt:
    print("Logging stopped.")
finally:
    ser.close()
