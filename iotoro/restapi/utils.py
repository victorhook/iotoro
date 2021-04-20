from django.http import HttpRequest, HttpResponse
from django.views.decorators.csrf import csrf_exempt
from django.contrib.auth.models import User
from django.conf import settings

from iotoro import crypto_utils
from iotoro import api
from device import models


def make_pong() -> bytes:
    return api.make_data_packet(
        settings.IOTORO_VERSION,
        api.Action.PONG,
        b''
    )

def get_device(device_id: str) -> models.Device:
    device = list(filter(
                lambda device: crypto_utils.md5(device.device_id) == device_id,
                models.Device.objects.all())
            )
    return device[0] if len(device) > 0 else None


def device_exists(device_id: str) -> User:
    return get_device(device_id) is not None


def requires_device_auth(func: callable):
    """ Decorator to ensure that the device message is authenticated. """

    @csrf_exempt
    def wrapper(request: HttpRequest, device_id: str):
        if device_exists(device_id):
            # Get device from the id.
            device = get_device(device_id)

            # Try to decrypt the packet
            packet = crypto_utils.decode_packet(request.body,
                                                device.device_key)

            if packet.device_id == device.device_id:
                print(f'Device id {packet.device_id} authorized')

                # Decode that data in the packet and put in the body.
                data_packet = api.decode_data_packet(packet.data)
                packet.version = data_packet.version
                packet.action = data_packet.action
                packet.data = data_packet.content

                # Call the view function.
                return func(request, device, packet)

            else:
                print('Incorrect authorization')
                return HttpResponse('Incorrect authorization')
        else:
            print(f'No owner for device id {device_id}')
            return HttpResponse('No such device exists.')

    return wrapper
