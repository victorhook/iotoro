from django.conf.urls.static import static
from django.conf import settings
from django.contrib import admin
from django.urls import path, include


from . import views


urlpatterns = [
    path('admin/', admin.site.urls),
    path('accounts/', include('django.contrib.auth.urls')),
    path('', views.index, name='index'),
    path('api/', include('restapi.urls')),
    path('device/', include('device.urls')),
] + static(settings.MEDIA_URL, document_root=settings.MEDIA_ROOT)

