import random
import time
import socket
import threading
import json
from pythonosc import udp_client

# Define the UDP server settings
UDP_IP = "0.0.0.0"
UDP_PORT = 4210

# Create a UDP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((UDP_IP, UDP_PORT))

# Define the B minor pentatonic scale as MIDI notes
# High notes (B4 to B5)
high_b_minor_pentatonic_notes = [59, 62, 64, 66, 69, 71, 74, 76]  # B4-B5
# Low notes (B2 to B3)
low_b_minor_pentatonic_notes = [35, 38, 40, 42, 45, 47, 50, 52]  # B2-B3

# Set up the OSC client to send MIDI messages to the Raspberry Pi
client_ip = "192.168.86.247"
client_port = 1234
client = udp_client.SimpleUDPClient(client_ip, client_port)

# Function to send a MIDI message
def send_midi_note(note, velocity, is_note_on):
    status = 144 if is_note_on else 128  # 144 = Note On, 128 = Note Off
    client.send_message("/rnbo/inst/0/midi/in", [status, note, velocity])
    print(f"Sent {'Note On' if is_note_on else 'Note Off'}: Note={note}, Velocity={velocity}")

# Function to handle incoming UDP messages
def handle_udp_messages():
    print(f"UDP server is listening on {UDP_IP}:{UDP_PORT}")
    while True:
        try:
            message, addr = sock.recvfrom(1024)  # Buffer size is 1024 bytes
            data = message.decode('utf-8')
            print(f"Received message from {addr}: {data}")

            # Parse the incoming message as JSON
            parsed_data = json.loads(data)
            message_type = parsed_data.get("message", "end")
            color = parsed_data.get("color", 0)
            delay = parsed_data.get("delay", 100)
            width = parsed_data.get("width", 1)

            # Choose note array based on start or end message
            if message_type == "start":
                note_list = high_b_minor_pentatonic_notes
            else:
                note_list = low_b_minor_pentatonic_notes

            # Use the parsed data to determine the MIDI note and velocity
            note_index = color % len(note_list)  # Use color to pick a note from the scale
            note = note_list[note_index]
            velocity = min(max(int(delay), 75), 127)  # Use delay as velocity, clamped between 75 and 127

            # Send a Note On message
            send_midi_note(note, velocity, True)

            # Wait for a short duration based on the width value
            time.sleep(width * 0.1)

            # Send a Note Off message
            send_midi_note(note, 0, False)

        except json.JSONDecodeError:
            print(f"Error decoding JSON message: {data}")
        except Exception as e:
            print(f"Error receiving message: {e}")

# Start the UDP server in a separate thread
udp_thread = threading.Thread(target=handle_udp_messages)
udp_thread.daemon = True
udp_thread.start()

# Main loop is no longer needed since messages are triggered by incoming UDP messages
try:
    while True:
        time.sleep(1)  # Keep the main thread alive
except KeyboardInterrupt:
    print("Stopped receiving MIDI messages.")
