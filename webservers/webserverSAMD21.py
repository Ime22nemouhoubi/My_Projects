from flask import Flask, request
import requests

app = Flask(__name__)

@app.route('/sensor_data', methods=['POST'])
def receive_data():
    distance = float(request.form['Distance_Data'])
    temperature = float(request.form['Temp_Data'])
    # Forward the data to the Node-RED server
    url = "http://10.3.2.72:1880/receive-SAMD21" 
    payload = {
        'distance': distance,
        'temp': temperature,
    }
    
    response = requests.post(url, json=payload)
    
    if response.status_code == 200:
        return "Data sent to Node-RED"
    else:
        return "Failed to send data to Node-RED"

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=8002)