from django.conf import settings
from django.urls import path
from django.contrib.auth.decorators import login_required


from . import views


urlpatterns = [
    path('', views.device, name='device'),
    path('new/', views.new_device, name='new_device'),
    path('data/<str:device_name>', views.data, name='data'),
    path('attributes/<str:device_name>', views.attributes, name='attributes'),
    path('settings/<str:device_name>', views.settings, name='settings'),
    path('triggers/<str:device_name>', views.triggers, name='triggers'),
    path('overview/<str:device_name>', views.overview, name='overview'),
]