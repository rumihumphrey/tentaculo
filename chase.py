import requests

ESP32_IP = "192.168.86.248"  

def start_chase():
    url = f"http://{ESP32_IP}/chase"
    response = requests.get(url)
    if response.status_code == 200:
        print("Successfully started chase animation")
    else:
        print("Failed to start chase animation")

start_chase()

