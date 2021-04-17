from django.forms import ModelForm

from . import models


class DeviceForm(ModelForm):
    class Meta:
        model = models.Device
        fields = ['name', 'type', 'token']
