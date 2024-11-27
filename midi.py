import random
import time
from pythonosc import udp_client

client = udp_client.SimpleUDPClient("192.168.86.247", 1234) 

# Function to send a MIDI message
def send_midi_note(note, velocity, is_note_on):
    status = 144 if is_note_on else 128  # 144 = Note On, 128 = Note Off
    client.send_message("/rnbo/inst/0/midi/in", [status, note, velocity])
    print(f"Sent {'Note On' if is_note_on else 'Note Off'}: Note={note}, Velocity={velocity}")

# Main loop to send random MIDI notes
try:
    while True:
        # Generate random MIDI note and velocity
        note = random.randint(60, 72)      # Notes between middle C and octave above
        velocity = random.randint(60, 127) # Random velocity

        # Send a Note On message
        send_midi_note(note, velocity, True)
        
        # Wait for a short duration
        time.sleep(random.uniform(0.2, 5.0))
        
        # Send a Note Off message
        send_midi_note(note, 0, False)
        
        # Pause between notes
        time.sleep(random.uniform(0.5, 1.5))
except KeyboardInterrupt:
    print("Stopped sending MIDI messages.")
