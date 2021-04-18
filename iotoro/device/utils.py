from django.contrib.auth.models import User


from . import models
from . import forms


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
