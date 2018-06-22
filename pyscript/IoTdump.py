import boto3
import json

client = boto3.client('iot-data')

def lambda_handler(event, context):
    response = client.publish(
    topic='$aws/things/IoTDoor/shadow/update',
    qos=0,
    payload=json.dumps(event)
)
