import random
import time
import socket
import json
from pythonosc import udp_client

# Define the UDP server settings for Arduino communication
arduino_ip = "192.168.86.27"  # Replace with your Arduino IP address
arduino_port = 4210
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

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

# Function to send chase animation parameters to Arduino
def send_chase_animation(color, delay, width):
    message = json.dumps({
        "r": color[0],
        "g": color[1],
        "b": color[2],
        "delay": delay,
        "width": width
    })
    sock.sendto(message.encode(), (arduino_ip, arduino_port))
    print(f"Sent chase animation: Color={color}, Delay={delay}, Width={width}")

# Main loop to trigger animations and MIDI notes
try:
    while True:
        # Send random chase animations to Arduino
        color = (random.randint(0, 255), random.randint(0, 255), random.randint(0, 255))
        delay = random.randint(10, 30)
        width = random.randint(1, 5)
        send_chase_animation(color, delay, width)

        # Generate random MIDI notes and send them to the Raspberry Pi
        note_list = random.choice([high_b_minor_pentatonic_notes, low_b_minor_pentatonic_notes])
        note = random.choice(note_list)
        velocity = random.randint(75, 127)

        # Send a Note On message
        send_midi_note(note, velocity, True)

        # Wait for a random duration between 0.1 and 1 second
        time.sleep(random.uniform(0.1, 2.0))

        # Send a Note Off message
        send_midi_note(note, 0, False)

        # Pause before sending another command
        time.sleep(random.uniform(0.5, 1.0))

except KeyboardInterrupt:
    print("Stopped sending animations and MIDI messages.")
