from django.shortcuts import render, HttpResponse
from django.http import HttpRequest
from django.views.decorators.csrf import csrf_exempt
from django.contrib.auth.models import User

from iotoro import crypto_utils
from device import models


def get_device(device_id: str) -> models.Device:
    device = list(filter(
                lambda device: crypto_utils.md5(device.device_id) == device_id,
                models.Device.objects.all())
            )
    return device[0] if len(device) > 0 else None


def device_exists(device_id: str) -> User:
    return get_device(device_id) is not None
    

def get_device_owner(device_id: str) -> User:
    pass

def get_device_key(user: User, device_id: str) -> str:
    user_devices = models.Device.objects.filter(user=user)
    #device = list(filter(
                #lambda device: device_md5_comp(device, device_id), 
                #user_devices))
    #return device[0] if len(device) > 0 else None
    return None

def requires_device_owner(func):

    @csrf_exempt
    def wrapper(request: HttpRequest, device_id: str):
        if device_exists(device_id):
            return func(request, device_id)
        else:
            print(f'No owner for device id {device_id}')
            return HttpResponse('No such device exists.')
    
    return wrapper

# Create your views here.
@csrf_exempt
def index(request: HttpRequest):
    packet: crypto_utils.Packet = crypto_utils.decrypt_packet(request.body)
    return HttpResponse('Ok')

@requires_device_owner
def device_push(request: HttpRequest, device_id: str):
    # Get device from the id.
    device = get_device(device_id)

    # Turn the raw data to a packet.
    encrypted_packet = crypto_utils.get_packet(request.body)

    # Try to decrypt the packet
    packet = crypto_utils.decode_packet(encrypted_packet, device.device_key)

    # Find the device id in the packet.
    device_id = crypto_utils.get_device_id(packet)
    
    if device_id == device.device_id:
        # Auth OK
        return HttpResponse('Ok')
    else:
        # Auth not ok!
        return HttpResponse('Not authorized')



"""
iv is last 16 bytes
|      |     8     | 16 |
| data | device_id | iv |

"""