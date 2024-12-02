import requests
import time
from flask import Flask, request, jsonify

ESP32_IP = "192.168.86.248"  # Replace with the actual IP address of your ESP32

app = Flask(__name__)

def set_color(color):
    url = f"http://{ESP32_IP}/set_color"
    params = {'color': color}
    response = requests.get(url, params=params)
    if response.status_code == 200:
        print(f"Successfully set color to {color}")
    else:
        print("Failed to set color")

def start_rainbow():
    url = f"http://{ESP32_IP}/rainbow"
    response = requests.get(url)
    if response.status_code == 200:
        print("Successfully started rainbow animation")
    else:
        print("Failed to start rainbow animation")

def start_chase():
    url = f"http://{ESP32_IP}/chase"
    response = requests.get(url)
    if response.status_code == 200:
        print("Successfully started chase animation")
    else:
        print("Failed to start chase animation")

@app.route('/chase', methods=['POST'])
def chase_endpoint():
    client_ip = request.remote_addr
    data = request.get_json()
    message = data.get('message', 'No message received')
    print(f"{message}")
    #start_chase()
    return jsonify({"status": "Bang back"}), 200

if __name__ == '__main__':
    # Run the Flask server
    app.run(host='0.0.0.0', port=5002)
# Duration in seconds for each animation
animation_duration = 20


start_chase()

# start_chase()
# time.sleep(animation_duration)

# while True:
#     set_color('green')
#     time.sleep(animation_duration)

#     start_rainbow()rm -rf venv
#     time.sleep(animation_duration)

#     start_chase()
#     time.sleep(animation_duration)