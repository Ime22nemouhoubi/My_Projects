from flask import Flask, request
import requests

app = Flask(__name__)

@app.route('/endpoint', methods=['POST'])
def receive_data():
    main_flow_rate = float(request.form['main_flow_rate'])
    secondary_flow_rate = float(request.form['secondary_flow_rate'])
    
    # Forward the data to the Node-RED server
    url = "http://172.17.0.1/receive-data" 
    payload = {
        'main_flow_rate': main_flow_rate,
        'secondary_flow_rate': secondary_flow_rate
    }
    
    response = requests.post(url, json=payload)
    
    if response.status_code == 200:
        return "Data sent to Node-RED"
    else:
        return "Failed to send data to Node-RED"

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=8001)
