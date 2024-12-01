import random
import time
import json
from pythonosc import udp_client
import serial

# Set up Serial communication for Arduino
SERIAL_PORT = '/dev/tty.usbmodem2101'  # USB address for Arduino
BAUD_RATE = 115200
arduino_serial = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)

# Set up the OSC client to send MIDI messages to the Raspberry Pi
client_ip = "192.168.86.247"
client_port = 1234
client = udp_client.SimpleUDPClient(client_ip, client_port)

# Define the B minor pentatonic scale as MIDI notes
# High notes (B4 to B5)
high_b_minor_pentatonic_notes = [59, 62, 64, 66, 69, 71, 74, 76]  # B4-B5
# Low notes (B2 to B3)
low_b_minor_pentatonic_notes = [35, 38, 40, 42, 45, 47, 50, 52]  # B2-B3

# Function to send a MIDI message
def send_midi_note(note, velocity, is_note_on):
    status = 144 if is_note_on else 128  # 144 = Note On, 128 = Note Off
    client.send_message("/rnbo/inst/0/midi/in", [status, note, velocity])
    print(f"Sent {'Note On' if is_note_on else 'Note Off'}: Note={note}, Velocity={velocity}")

# Function to send chase animation parameters to Arduino over Serial
def send_chase_animation(color, delay, width, reverse):
    message = json.dumps({
        "r": color[0],
        "g": color[1],
        "b": color[2],
        "delay": delay,
        "width": width,
        "reverse": reverse
    })
    try:
        arduino_serial.write((message + '\n').encode())
        print(f"Sent chase animation: Color={color}, Delay={delay}, Width={width}, Reverse={reverse}")
    except Exception as e:
        print(f"Error sending serial message: {e}")

# Main loop to trigger animations and MIDI notes
try:
    active_notes = []
    while True:
        # Send random chase animations to Arduino
        color = (random.randint(0, 255), random.randint(0, 255), random.randint(0, 255))
        delay = random.randint(3, 15)  # Faster delay for more frequent animations
        width = random.randint(1, 5)
        reverse = random.choices([True, False], weights=[70, 30], k=1)[0]
        send_chase_animation(color, delay, width, reverse)

        # Generate random MIDI notes and send them to the Raspberry Pi
        note_list = random.choice([high_b_minor_pentatonic_notes, low_b_minor_pentatonic_notes])
        note = random.choice(note_list)
        velocity = random.randint(75, 127)

        # Send a Note On message
        send_midi_note(note, velocity, True)
        active_notes.append(note)

        # Wait for a shorter duration between 0.05 and 0.3 seconds
        time.sleep(random.uniform(0.5, 3.2))

        # Randomly decide to end some of the active notes
        if active_notes and random.random() < 1.8:
            note_to_end = random.choice(active_notes)
            send_midi_note(note_to_end, 0, False)
            active_notes.remove(note_to_end)

except KeyboardInterrupt:
    print("Stopped sending animations and MIDI messages.")
    # End all active notes when stopping the script
    for note in active_notes:
        send_midi_note(note, 0, False)
    active_notes.clear()
