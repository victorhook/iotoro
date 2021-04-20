from django.db import models
from django.contrib.auth.models import User
from django.core.validators import RegexValidator
from django.conf import settings

from iotoro import crypto_utils


def get_length_validator(length: int) -> RegexValidator:
    return RegexValidator(regex='^.{%s}' % length,
                          message=f'Length has to be {length}',
                          code='nomatch')


def get_default_id() -> str:
    """ Wrapper around utils default id, to ensure that NO other model
        has the same id.
    """
    found_id = False
    while not found_id:
        try:
            id = crypto_utils.get_default_id()
            Device.objects.get(device_id=id)

        except Device.DoesNotExist:
            # If no device with the given id can be found, we know it's unique.
            # However, the md5 can still collide with any other device_id.
            # While being VERY unlikely, we'll ensure that this won't happen.
            md5_id_collision = filter(
                lambda device: crypto_utils.md5(device.device_id) ==
                               crypto_utils.md5(id),     # noqa
                Device.objects.all()
            )

            if len(list(md5_id_collision)) == 0:
                found_id = True

    return id


class DeviceType(models.Model):
    name = models.CharField(max_length=100)

    def __str__(self):
        return self.name


class Device(models.Model):
    name = models.CharField(max_length=32)
    type = models.ForeignKey(DeviceType, on_delete=models.CASCADE)
    user = models.ForeignKey(User, on_delete=models.CASCADE)

    device_id = models.CharField(validators=[get_length_validator(
                                                settings.DEVICE_ID_SIZE*2)],
                                 default=get_default_id,
                                 max_length=settings.DEVICE_ID_SIZE*2)

    device_key = models.CharField(validators=[get_length_validator(
                                                settings.DEVICE_KEY_SIZE*2)],
                                  default=crypto_utils.get_default_key,
                                  max_length=settings.DEVICE_KEY_SIZE*2)

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
    