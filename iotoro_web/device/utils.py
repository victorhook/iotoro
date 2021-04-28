from django.conf import settings
from django.contrib.auth.models import User


from . import models
from . import forms
from restapi import models as api_models


def get_device_form(device: models.Device = None) -> forms.DeviceForm:
    if device is None:
        device_form = forms.DeviceForm(initial={
            'token': 'ASD'
        })
    else:
        device_form = forms.DeviceForm(instance=device)

    return device_form


def get_devices(user: User) -> list:
    return list(models.Device.objects.filter(user=user))


def get_device(user: User, device_name: str) -> list:
    return models.Device.objects.get(user=user,
                                     name=device_name)


def get_latest_packet(device: models.Device) -> api_models.Message:
    try:
        return api_models.Message.objects.filter(
                                        device=device).latest('timestamp')
    except api_models.Message.DoesNotExist:
        return None


def get_data_length_of_message(msg: api_models.Message) -> int:
    if msg.data is None:
        return 0
    else:
        return len(msg.data) + settings.PACKET_HEADER_SIZE


def get_total_data_send(device: models.Device) -> int:
    try:
        messages = api_models.Message.objects.filter(device=device)
        message_lengths = map(get_data_length_of_message, messages)
        return sum(message_lengths)
    except api_models.Message.DoesNotExist:
        return 0
