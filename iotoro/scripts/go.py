from restapi.api_client import IotoroClient

device_id = '5d6b848e6dad8d83'
device_key = 'bb248ae52007756df14a9a86fd7c51ee'


def run():
    print('running!')
    with IotoroClient(device_id, device_key) as client:    
        client.send_data(b'hello!')
