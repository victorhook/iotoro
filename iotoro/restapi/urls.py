from django.urls import path


from . import views


urlpatterns = [
    path('<str:device_id>/', views.device_push, name='device_push')
]
