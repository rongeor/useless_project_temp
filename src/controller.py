import serial
import time

SERIAL_PORT = "COM7"  # Change if your Arduino is on a different COM port
BAUD_RATE = 9600

def main():
    try:
        ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
        print(f"Connected to {SERIAL_PORT}. Initializing...")
        time.sleep(2)  # Give Uno time to reset after opening serial
    except Exception as e:
        print(f"Could not open {SERIAL_PORT}: {e}")
        return

    print("\n--- DIRECT-ROUTE GLOBAL DISPATCH ---")
    dest = input("Enter your target destination (e.g., Paris, Kitchen, Mars): ").strip()
    if not dest:
        dest = "UNKNOWN REALM"

    # Send destination packet to the Arduino
    ser.write(f"START:{dest}\n".encode('utf-8'))
    print(f"\n[+] Destination '{dest}' transmitted.")
    print("[+] Follow instructions on Arduino button or watch status below:\n")

    # Read and print live updates coming back from the Uno
    try:
        while True:
            if ser.in_waiting > 0:
                line = ser.readline().decode('utf-8', errors='ignore').strip()
                if line:
                    print(f"STATUS >> {line}")
                    if "HOME" in line:
                        print("\nMission Complete: Returned with 100% positional accuracy.")
                        break
            time.sleep(0.05)
    except KeyboardInterrupt:
        print("\nDisconnected.")
    finally:
        ser.close()

if __name__ == "__main__":
    main()