import serial
import serial.tools.list_ports
import csv
import os
import sys
import time
from datetime import datetime

EXPERIMENT_RUN = 1


# --- CONFIGURATION ---
# Set the correct port below. On Windows, it's usually COM3, COM4, etc.
# IMPORTANT: Change 'COM4' to your Arduino's serial port
COM_PORT = 'COM4'
BAUD_RATE = 9600
# Use absolute path to user's home directory (works even when running with sudo)
# If running with sudo, get the original user's home directory
if os.geteuid() == 0 and 'SUDO_USER' in os.environ:
    import pwd
    user_home = pwd.getpwnam(os.environ['SUDO_USER']).pw_dir
else:
    user_home = os.path.expanduser('~')
filename = os.path.join(user_home, f"co2_data_{EXPERIMENT_RUN}.csv")
# ---------------------

def find_port(com_port):
    """Find the correct port path based on OS and available devices"""
    if sys.platform.startswith('win'):
        # Windows - use COM port directly
        return com_port
    
    # Linux/WSL - need to convert COM port to device path
    # When using usbipd, devices typically appear as /dev/ttyUSB0 or /dev/ttyACM0
    # Standard mapping: COM4 -> /dev/ttyS3 (but usbipd devices use USB/ACM)
    
    # First, check for actual USB/ACM device files (from usbipd)
    import glob
    usb_devices = glob.glob('/dev/ttyUSB*') + glob.glob('/dev/ttyACM*')
    if usb_devices:
        print(f"Found USB/ACM devices (likely from usbipd): {usb_devices}")
        return sorted(usb_devices)[0]  # Use the first one found
    
    # Also try list_ports (sometimes more reliable)
    try:
        available_ports = serial.tools.list_ports.comports()
        usb_ports = [p.device for p in available_ports if 'USB' in p.device or 'ACM' in p.device]
        if usb_ports:
            print(f"Found USB/ACM devices via list_ports: {usb_ports}")
            return usb_ports[0]
    except Exception:
        pass
    
    # Fallback: try standard ttyS mapping (requires dialout group or sudo)
    if com_port.upper().startswith('COM'):
        com_num = int(com_port[3:])
        ttyS_port = f'/dev/ttyS{com_num - 1}'
        if os.path.exists(ttyS_port):
            print(f"Using standard serial port mapping: {ttyS_port}")
            print("  Note: This may require dialout group permissions or sudo")
            return ttyS_port
    
    # If nothing found, return the original (might be a direct path)
    return com_port

# Write headers if file doesn't exist
if not os.path.exists(filename):
    with open(filename, mode='w', newline='') as f:
        writer = csv.writer(f)
        # Updated headers to match the new CO2-only output
        writer.writerow(["Timestamp(UTC)", "CO2(ppm)"])

# Find the correct port
port = find_port(COM_PORT)

print(f"Logging data to: {filename} (absolute path)")
print(f"Connecting to {port} (Windows: {COM_PORT}) at {BAUD_RATE} baud...")

# Arduino Uno resets when serial connection opens (DTR toggles)
# Open with explicit timeout and wait for Arduino to complete setup()
try:
    ser = serial.Serial(port, BAUD_RATE, timeout=1)
    print(f"Serial port opened. Waiting for Arduino to initialize...")
    
    # Wait for Arduino to reset and complete setup() function
    # The Arduino sketch has delay(1000) in setup() and prints "MH-Z19C starting..."
    time.sleep(3)  # Give Arduino time to reset, run setup(), and start sending data
    
    # Flush any startup messages or partial data
    ser.reset_input_buffer()
    print("Ready to read data!")
    
except serial.SerialException as e:
    print(f"Error opening serial port {port}: {e}")
    print(f"  Windows port: {COM_PORT}")
    print(f"  Linux/WSL path: {port}")
    print("\nTroubleshooting:")
    print("  1. Make sure the Arduino is connected to", COM_PORT, "on Windows")
    if not sys.platform.startswith('win'):
        print("  2. If using usbipd, ensure the device is properly attached:")
        print("     a. In PowerShell (Windows): usbipd bind --busid <bus-id>")
        print("     b. In PowerShell: usbipd wsl attach --busid <bus-id>")
        print("     c. Check status: usbipd wsl list")
        print("  3. Check available ports in WSL:")
        import glob
        usb_devs = glob.glob('/dev/ttyUSB*') + glob.glob('/dev/ttyACM*')
        ttyS_devs = glob.glob('/dev/ttyS*')
        if usb_devs:
            print(f"     USB/ACM devices: {usb_devs}")
        if ttyS_devs:
            print(f"     Serial ports: {sorted(ttyS_devs)}")
        try:
            available = serial.tools.list_ports.comports()
            if available:
                print(f"     Via list_ports: {[p.device for p in available]}")
        except:
            pass
        if "Permission denied" in str(e):
            print("  4. PERMISSION ISSUE DETECTED:")
            print("     Option A (Recommended): Add yourself to dialout group:")
            print("       sudo usermod -a -G dialout $USER")
            print("       Then restart WSL (exit terminal and reopen)")
            print("     Option B (Temporary): Run with sudo:")
            print("       sudo uv run log.py")
            print("     Note: If usbipd is properly set up, USB/ACM devices")
            print("           usually don't require special permissions")
    print("  5. Close Arduino IDE Serial Monitor if it's open")
    print("  6. Close any other programs using the serial port")
    print("  7. Try unplugging and replugging the Arduino USB cable")
    raise

try:
    
    while True:
        try:
            line = ser.readline().decode('utf-8', errors='ignore').strip()
            if not line:  # Skip empty lines
                continue
            print("Read:", line)
        except UnicodeDecodeError:
            # Skip lines that can't be decoded
            continue

        # The Arduino sketch outputs: "CO2: 1234 ppm"
        # We need to extract the number (e.g., "1234")
        
        # Check if the line starts with "CO2:"
        if line.startswith("CO2:"):
            try:
                # Split the line by spaces and get the second element (the CO2 value)
                # Example: "CO2: 1234 ppm" -> ["CO2:", "1234", "ppm"] -> "1234"
                co2_value = line.split()[1] 
                
                # Replace Arduino output time with current UTC timestamp
                utc_timestamp = datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S')
                
                # Data to write: [Timestamp, CO2 Value]
                data_row = [utc_timestamp, co2_value]
                
                with open(filename, mode='a', newline='') as f:
                    writer = csv.writer(f)
                    writer.writerow(data_row)
                    f.flush()  # Ensure data is written immediately
                    os.fsync(f.fileno())  # Force write to disk
                    print("Logged:", data_row)

            except IndexError:
                # Handle cases where the line format isn't as expected (e.g., just "No response...")
                print("Error parsing data line.")
            except Exception as e:
                print(f"An unexpected error occurred: {e}")


except KeyboardInterrupt:
    print("Logging stopped.")
finally:
    ser.close()
    print("Serial port closed.")