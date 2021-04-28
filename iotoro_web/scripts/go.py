from restapi.api_client import IotoroClient

device_id = '6e795abf2d397b25'
device_key = '2a7056cfc228272140867b6a69a50d81'


def run():
    with IotoroClient(device_id, device_key) as client:    
        client.ping()
