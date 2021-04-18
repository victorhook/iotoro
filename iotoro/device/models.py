from django.db import models
from django.contrib.auth.models import User
# Create your models here.

def default_token():
    return 'asd'

class DeviceType(models.Model):
    name = models.CharField(max_length=100)

    def __str__(self):
        return self.name


class Device(models.Model):
    name = models.CharField(max_length=32)
    type = models.ForeignKey(DeviceType, on_delete=models.CASCADE)
    token = models.CharField(max_length=64, default=default_token)
    user = models.ForeignKey(User, on_delete=models.CASCADE)

    class Meta:
        constraints = [
            models.UniqueConstraint(fields=['name', 'user'], 
                                    name='unique devicename')
        ]

    def __str__(self):
        return self.name


class Param(models.Model):
    type = models.CharField(max_length=50)


class Attribute(models.Model):
    device = models.ForeignKey(Device, on_delete=models.CASCADE)
    param = models.ForeignKey(Param, on_delete=models.CASCADE)