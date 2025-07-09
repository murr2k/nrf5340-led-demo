#!/usr/bin/env python3
"""
Simple test script to demonstrate nRF5340 LED demo serial commands
Usage: python3 test_commands.py
"""

import serial
import time
import sys

def send_command(ser, cmd):
    """Send a command and read the response"""
    print(f">>> {cmd}")
    ser.write(f"{cmd}\n".encode())
    time.sleep(0.1)
    
    # Read any available response
    while ser.in_waiting > 0:
        try:
            response = ser.readline().decode().strip()
            if response:
                print(f"<<< {response}")
        except:
            break

def main():
    # Try to connect to nRF5340DK
    try:
        ser = serial.Serial('/dev/ttyACM0', 115200, timeout=1)
        print("Connected to nRF5340DK on /dev/ttyACM0")
    except Exception as e:
        print(f"Failed to connect: {e}")
        print("Make sure the nRF5340DK is connected and the firmware is running")
        sys.exit(1)
    
    try:
        # Wait for device to initialize
        time.sleep(2)
        
        # Test basic commands
        commands = [
            "help",
            "status", 
            "manual",
            "pattern 2",  # Chase pattern
            "all 1",      # All LEDs on
            "all 0",      # All LEDs off
            "led 0 1",    # Turn on LED 0
            "led 2 1",    # Turn on LED 2
            "leds 5",     # LEDs 0 and 2 on (binary: 0101)
            "leds 10",    # LEDs 1 and 3 on (binary: 1010)
            "pattern 0",  # Blink pattern
            "auto",       # Return to auto mode
            "status"
        ]
        
        print("\n🔬 Testing nRF5340 LED Demo Commands\n")
        
        for cmd in commands:
            send_command(ser, cmd)
            time.sleep(1)
            print()
            
        print("✅ Command test completed!")
        
    except KeyboardInterrupt:
        print("\n⚠️  Test interrupted by user")
    except Exception as e:
        print(f"❌ Error during test: {e}")
    finally:
        ser.close()
        print("🔌 Serial connection closed")

if __name__ == "__main__":
    main()