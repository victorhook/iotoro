from device import models
from django.shortcuts import render, HttpResponse
from django.http import HttpRequest
from django.views.decorators.csrf import csrf_exempt
from django.contrib.auth.models import User

from iotoro_web.crypto_utils import Packet
from iotoro_web import api
from device.models import Device

from . import utils
from . import models


# Create your views here.
@csrf_exempt
def index(request: HttpRequest):
    return HttpResponse('Ok')


def pong(device: Device) -> HttpResponse:
    models.Message(device=device, type='PING').save()
    models.Message(device=device, type='PONG').save()
    return HttpResponse(utils.make_pong())


def write_up(request: HttpRequest, device: Device, packet: Packet):
    pass


def read_up(request: HttpRequest, device: Device, packet: Packet):
    pass


@utils.requires_device_auth
def device_push(request: HttpRequest, device: Device, packet: Packet):
    if packet.action == api.Action.PING:
        return pong(device)
    elif packet.action == api.Action.WRITE_UP:
        return write_up()
    elif packet.action == api.Action.READ_UP:
        return read_up()

    return HttpResponse(f'Failed to recognize command {packet.data.action}')


