import serial
import csv
import os

# Set the correct port below. On Windows, it's usually COM3, COM4, etc.
ser = serial.Serial('COM4', 9600)  # Change to your Arduino port
filename = os.path.expanduser("~/weather_data.csv")

# Write headers if file doesn't exist
if not os.path.exists(filename):
    with open(filename, mode='w', newline='') as f:
        writer = csv.writer(f)
        writer.writerow(["Timestamp(ms)", "DHT_Temp(°C)", "Humidity(%)", "DS18B20_Temp(°C)", "Thermistor_Temp(°C)", "Gas_ppm"])

print("Logging data to:", filename)

try:
    while True:
        line = ser.readline().decode().strip()
        print("Read:", line)
        fields = line.split(",")
        if len(fields) == 6:
            with open(filename, mode='a', newline='') as f:
                writer = csv.writer(f)
                writer.writerow(fields)
except KeyboardInterrupt:
    print("Logging stopped.")
finally:
    ser.close()
