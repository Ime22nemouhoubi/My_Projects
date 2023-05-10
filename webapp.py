from fastapi import FastAPI, HTTPException
from pydantic import BaseModel
import redis
from redis.exceptions import RedisError
from datetime import datetime
import uvicorn

app = FastAPI()

# Redis client
r = redis.Redis(host='localhost', port=6379)# redis specifications

# Redis Timeseries configuration
TIMESERIES_RETENTION_TIME = 3600  # in seconds
TIMESERIES_CHUNK_SIZE = 1000

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
        # Add data to Redis Timeseries
        r.execute_command('TS.ADD', 'distance_data', distance_data.timestamp, distance_data.distance, 'RETENTION', TIMESERIES_RETENTION_TIME, 'CHUNK_SIZE', TIMESERIES_CHUNK_SIZE)
        return {'message': 'Data added to Redis Timeseries for distance_data'}
    except RedisError as e:
        raise HTTPException(status_code=500, detail=f'Error adding data to Redis: {str(e)}')

@app.post('/ph_data')
def post_ph_data(ph_data: PhData):
    try:
        # Add data to Redis Timeseries
        r.execute_command('TS.ADD', 'ph_data', ph_data.timestamp, ph_data.ph, 'RETENTION', TIMESERIES_RETENTION_TIME, 'CHUNK_SIZE', TIMESERIES_CHUNK_SIZE)
        return {'message': 'Data added to Redis Timeseries for ph_data'}
    except RedisError as e:
        raise HTTPException(status_code=500, detail=f'Error adding data to Redis: {str(e)}')

# Check Redis Timeseries on startup
@app.on_event('startup')
def startup_event():
    try:
        # Check if Timeseries for distance_data exists and create it if it doesn't
        if not r.exists('distance_data'):
            r.execute_command('TS.CREATE', 'distance_data', 'RETENTION', TIMESERIES_RETENTION_TIME, 'CHUNK_SIZE', TIMESERIES_CHUNK_SIZE)
        # Check if Timeseries for ph_data exists and create it if it doesn't
        if not r.exists('ph_data'):
            r.execute_command('TS.CREATE', 'ph_data', 'RETENTION', TIMESERIES_RETENTION_TIME, 'CHUNK_SIZE', TIMESERIES_CHUNK_SIZE)
    except RedisError as e:
        raise HTTPException(status_code=500, detail=f'Error checking Redis Timeseries: {str(e)}')


uvicorn.run(app,host="41.220.149.120",port=8000) #set host to local ip ad