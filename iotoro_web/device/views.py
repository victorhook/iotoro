from datetime import timedelta, datetime
from django.shortcuts import render, redirect
from django.utils import timezone
from django.http import HttpRequest
from django.contrib.auth.decorators import login_required


from . import models
from . import forms
from . import utils


@login_required
def new_device(request: HttpRequest):
    if request.method == 'POST':
        device_obj = models.Device(user=request.user)
        device_form = forms.DeviceForm(request.POST, instance=device_obj)
        if device_form.is_valid():
            device_form.save()
            return redirect('device')
        else:
            #TODO: Handle error here?
            return render(request, 'new_device.html', {'form': device_form})
    else:
        device_form = utils.get_device_form()
        return render(request, 'new_device.html', {'form': device_form})


@login_required
def device(request: HttpRequest):
    """ First page that the user gets linked to when pressing on Devices. """
    devices = utils.get_devices(request.user)
    selected_device = devices[0].name if len(devices) > 0 else None
    if selected_device is None:
        return render(request, 'no_devices.html')
    else:
        return redirect('overview', selected_device)


@login_required
def base_device_view(request: HttpRequest, endpoint: str, 
                     selected_device: str, data: dict):
    devices = utils.get_devices(request.user)
    selected_device = utils.get_device(request.user, selected_device)

    context = {
        'devices': devices,
        'selected_device': selected_device
    }
    context.update(data)

    return render(request, endpoint, context)

def data(request: HttpRequest, device_name: str):
    return base_device_view(request, 'data.html', device_name, {})
def attributes(request: HttpRequest, device_name: str):
    return base_device_view(request, 'attributes.html', device_name, {})

def settings(request: HttpRequest, device_name: str):
    if request.method == 'POST':
        # Save settings.
        device_obj = models.Device.objects.get(id=request.POST['id'])
        device_form = forms.DeviceForm(request.POST, instance=device_obj)
        if device_form.is_valid():
            device_form.save()
            device_form.clean()
            device_name = device_form.cleaned_data['name']
            return redirect('settings', device_name)
        else:
            #TODO: Handle error here?
            pass   
    else:
        device_form = utils.get_device_form(utils.get_device(request.user, 
                                            device_name))
        return base_device_view(request, 'settings.html', device_name, 
                            {'form': device_form})

def triggers(request: HttpRequest, device_name: str):
    return base_device_view(request, 'triggers.html', device_name, {})


def get_time_diff(t1: timezone, t2: timezone) -> str:
    return (t1 - t2).seconds


@login_required
def overview(request: HttpRequest, device_name: str):
    device = utils.get_device(request.user, device_name)
    latest_packet = utils.get_latest_packet(device)

    if latest_packet is not None:
        latest_packet.time_diff = get_time_diff(timezone.now(),
                                                latest_packet.timestamp)

    total_data = utils.get_total_data_send(device)
    print(total_data)

    return base_device_view(request, 'overview.html', device_name,
                            {'latest_packet': latest_packet})
