
#!/bin/bash
APP_ROOT=/home/victor/coding/projects/iotoro
APP_ENV=/home/victor/coding/projects/iotoro/iotoro

cd /home/victor/coding/projects/iotoro
source env/bin/activate

cd /home/victor/coding/projects/iotoro/iotoro
python manage.py livereload & python manage.py runserver

