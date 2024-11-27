from pythonosc import udp_client

client = udp_client.SimpleUDPClient("192.168.86.247", 1234) 
parameter_address = "/rnbo/inst/0/params/sahgainin"

def send_value(value):
    client.send_message(parameter_address, value)
    print(f"Sent value {value} to {parameter_address}")

send_value(0.5)