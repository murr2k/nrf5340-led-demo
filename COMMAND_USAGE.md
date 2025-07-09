# nRF5340 LED Demo - Command Interface Usage

## 🔌 Connection Setup

Connect to the nRF5340DK serial console:

```bash
# Option 1: Using screen
screen /dev/ttyACM0 115200

# Option 2: Using minicom
minicom -D /dev/ttyACM0 -b 115200

# Option 3: Using Python
python3 -c "
import serial
ser = serial.Serial('/dev/ttyACM0', 115200)
ser.write(b'help\n')
print(ser.readline().decode())
"
```

## 📋 Available Commands

### **Basic Commands**
| Command | Description | Example |
|---------|-------------|---------|
| `help` | Show all available commands | `help` |
| `status` | Display current system status | `status` |
| `auto` | Enable automatic pattern cycling | `auto` |
| `manual` | Switch to manual control mode | `manual` |

### **Pattern Control**
| Command | Description | Pattern |
|---------|-------------|---------|
| `pattern 0` | All LEDs blink together | BLINK |
| `pattern 1` | LEDs light up sequentially | SEQUENCE |
| `pattern 2` | Single LED moves back/forth | CHASE |
| `pattern 3` | Progressive fill with bounce | BOUNCE |
| `pattern 4` | Custom manual control | CUSTOM |

### **Individual LED Control**
| Command | Description | Example |
|---------|-------------|---------|
| `led <0-3> <0-1>` | Control single LED | `led 2 1` (turn on LED 2) |
| `all <0-1>` | Turn all LEDs on/off | `all 1` (all LEDs on) |

### **Multi-LED Control (Bitmask)**
| Command | Description | Binary | LEDs On |
|---------|-------------|--------|---------|
| `leds 1` | LED 0 only | 0001 | LED 0 |
| `leds 3` | LEDs 0,1 | 0011 | LED 0, LED 1 |
| `leds 5` | LEDs 0,2 | 0101 | LED 0, LED 2 |
| `leds 10` | LEDs 1,3 | 1010 | LED 1, LED 3 |
| `leds 15` | All LEDs | 1111 | All LEDs |

## 🎮 Interactive Usage Examples

### **Basic LED Control**
```
> help
=== nRF5340 LED Demo Commands ===
...

> status
=== Status ===
Mode: Auto
Pattern: BLINK (0)
LED States: LED0=1 LED1=1 LED2=1 LED3=1
==============

> manual
Manual mode enabled

> led 0 1
LED 0 set to ON

> led 2 1
LED 2 set to ON

> leds 5
LEDs set to mask: 0x05
```

### **Pattern Demonstration**
```
> pattern 2
Pattern set to: CHASE

> pattern 1
Pattern set to: SEQUENCE

> auto
Auto mode enabled
```

### **Advanced Examples**
```
# Create custom patterns
> manual
> leds 1      # Only LED 0
> leds 3      # LEDs 0,1
> leds 7      # LEDs 0,1,2
> leds 15     # All LEDs
> leds 0      # All LEDs off

# Individual control
> led 0 1
> led 1 0
> led 2 1
> led 3 0
```

## 🔄 Mode Behavior

### **Auto Mode**
- Automatically cycles through patterns 0-3 every 10 seconds
- Patterns: BLINK → SEQUENCE → CHASE → BOUNCE → repeat
- Status updates logged to serial console

### **Manual Mode**
- Manual control of all LEDs and patterns
- No automatic pattern changes
- Immediate response to commands
- Perfect for interactive control

## 🧪 Testing Script

Run the included test script to demonstrate all commands:

```bash
# Make executable
chmod +x test_commands.py

# Run test (requires pyserial)
python3 test_commands.py
```

## 📊 Status Information

The `status` command provides:
- Current mode (Auto/Manual)
- Active pattern name and number
- Individual LED states (0/1)
- Real-time system information

## 🔧 Hardware Information

- **Board**: nRF5340DK (PCA10095)
- **LEDs**: 4 Green LEDs (P0.28, P0.29, P0.30, P0.31)
- **Serial**: 115200 baud, 8N1
- **Firmware**: 53,124 bytes (5.07% flash usage)

## 💡 Tips

1. **Command Echo**: Commands are echoed back for confirmation
2. **Backspace**: Use backspace to correct typing errors
3. **Case Sensitive**: Commands are case-sensitive (lowercase)
4. **Immediate Response**: LED changes happen instantly
5. **Help Available**: Type `help` anytime for command reference

## 🚀 Advanced Usage

### **Creating Light Shows**
```bash
# Rapid fire commands for light shows
echo -e "manual\nleds 1\nleds 2\nleds 4\nleds 8\nleds 15\nleds 0" | \
screen /dev/ttyACM0 115200
```

### **Scripted Control**
```python
import serial
import time

ser = serial.Serial('/dev/ttyACM0', 115200)

# Create a bouncing pattern
for i in range(4):
    ser.write(f"led {i} 1\n".encode())
    time.sleep(0.2)
    ser.write(f"led {i} 0\n".encode())
    time.sleep(0.2)
```

The command interface provides full real-time control over the nRF5340 LED demo with both automatic and manual operation modes!