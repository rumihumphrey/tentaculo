import random
import time
from pythonosc import udp_client

client = udp_client.SimpleUDPClient("192.168.86.247", 1234) 

# Define the B minor pentatonic scale as MIDI notes
# B3 to B5 includes the MIDI notes: B, D, E, F#, A
b_minor_pentatonic_notes = [47, 50, 52, 54, 57, 59, 62, 64]  # B3-B5

# Function to send a MIDI message
def send_midi_note(note, velocity, is_note_on):
    status = 144 if is_note_on else 128  # 144 = Note On, 128 = Note Off
    client.send_message("/rnbo/inst/0/midi/in", [status, note, velocity])
    print(f"Sent {'Note On' if is_note_on else 'Note Off'}: Note={note}, Velocity={velocity}")

# Main loop to send random MIDI notes
try:
    while True:
        # Pick a random note from the B minor pentatonic scale
        note = random.choice(b_minor_pentatonic_notes)
        velocity = random.randint(90, 127)  # Random velocity

        # Send a Note On message
        send_midi_note(note, velocity, True)
        
        # Wait for a short duration
        time.sleep(random.uniform(0.2, 2.0))
        
        # Send a Note Off message
        send_midi_note(note, 0, False)
        
        # Pause between notes
        time.sleep(random.uniform(0.0, 0.25))
except KeyboardInterrupt:
    print("Stopped sending MIDI messages.")

