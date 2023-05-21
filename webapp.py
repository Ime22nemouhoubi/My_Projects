from fastapi import FastAPI, HTTPException
from pydantic import BaseModel
import requests
from datetime import datetime
import uvicorn

app = FastAPI()

# Grafana Cloud API URL
GRAFANA_CLOUD_API_URL = "https://graphite-prod-13-prod-us-east-0.grafana.net/graphite"

# Grafana Cloud API authentication
GRAFANA_CLOUD_API_AUTH = ("977486", "")

# Models for request body
class DistanceData(BaseModel):
    distance: float
    timestamp: int

class PhData(BaseModel):
    ph: float
    timestamp: int

# Routes
@app.post('/distance_data')
def post_distance_data(distance_data: DistanceData):
    try:
        # Prepare data for Grafana Cloud API
        data = f"distance_data value={distance_data.distance} {distance_data.timestamp}\n"
        # Send data to Grafana Cloud API
        response = requests.post(GRAFANA_CLOUD_API_URL, data=data, auth=GRAFANA_CLOUD_API_AUTH)
        response.raise_for_status()
        return {'message': 'Data added to Grafana Cloud for distance_data'}
    except requests.exceptions.RequestException as e:
        raise HTTPException(status_code=500, detail=f'Error adding data to Grafana Cloud: {str(e)}')

@app.post('/ph_data')
def post_ph_data(ph_data: PhData):
    try:
        # Prepare data for Grafana Cloud API
        data = f"ph_data value={ph_data.ph} {ph_data.timestamp}\n"
        # Send data to Grafana Cloud API
        response = requests.post(GRAFANA_CLOUD_API_URL, data=data, auth=GRAFANA_CLOUD_API_AUTH)
        response.raise_for_status()
        return {'message': 'Data added to Grafana Cloud for ph_data'}
    except requests.exceptions.RequestException as e:
        raise HTTPException(status_code=500, detail=f'Error adding data to Grafana Cloud: {str(e)}')

# Check Grafana Cloud API on startup
@app.on_event('startup')
def startup_event():
    try:
        # TODO: Check if data source for distance_data exists and create it if it doesn't
        # TODO: Check if data source for ph_data exists and create it if it doesn't
        pass
    except requests.exceptions.RequestException as e:
        raise HTTPException(status_code=500, detail=f'Error checking Grafana Cloud API: {str(e)}')



uvicorn.run(app, host="0.0.0.0", port=8080) #set host to local ip address
