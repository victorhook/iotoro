from django.conf import settings
from django.urls import path


from . import views


urlpatterns = [
    path('', views.device, name='device'),
    path('new/', views.new_device, name='new_device'),
]