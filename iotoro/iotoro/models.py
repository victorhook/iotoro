from django.contrib.auth.models import User
from django.db import models
from django.utils.datetime_safe import datetime


class Profile(models.Model):
    user = models.OneToOneField(User, on_delete=models.CASCADE)
    avatar = models.ImageField(upload_to='avatars/')
    nickname = models.CharField(max_length=50)
    def __str__(self):
        return self.user

"""
class Packet(models.Model):
    device = models.ForeignKey(Device, on_delete=models.CASCADE)
    data = models.BinaryField()
    timestamp = models.DateTimeField(default=datetime.now)

    def __str__(self):
        return f'Length: {len(self.data)} - {self.data}'
"""
