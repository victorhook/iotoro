from django.shortcuts import render

from . import models, forms


# Create your views here.
def device(request):
    return render(request, 'device.html')


def new_device(request):
    device_form = forms.DeviceForm(initial={
        'token': 'ASD'
    })

    return render(request, 'new_device.html', {'form': device_form})