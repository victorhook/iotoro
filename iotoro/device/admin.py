from django.contrib import admin

from . import models


# Register your models here.
admin.site.register(models.DeviceType)
admin.site.register(models.Device)