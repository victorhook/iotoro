import logging
import socket

from django.conf import settings

from iotoro_web import crypto_utils
from iotoro_web import api


class IotoroClient:

    def __init__(self, device_id: str, device_key: str):
        self._device_id = device_id
        self._device_key = device_key
        self._host = settings.IOTORO_HOST_IP
        self._port = settings.IOTORO_HOST_PORT
        self._base_endpoint = settings.IOTORO_API_BASE_ENDPOINT
        self._sock = None

    def _make_http_packet(self, method: str, endpoint: str,
                          body: bytes) -> bytes:
        headers = f'{method} {endpoint} HTTP/1.1\r\n' \
                  f'Content-length: {len(body)}\r\n' \
                  'Content-Type: application/x-www-form-urlencoded\r\n'

        packet = headers.encode('utf-8') + b'\r\n' + body + b'\r\n\r\n'
        return packet

    def __enter__(self):
        self.connect()
        return self

    def __exit__(self, *ignore):
        self.disconnect()

    def connect(self) -> None:
        if self._sock is None:
            self._sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self._sock.connect((self._host, self._port))

    def disconnect(self) -> None:
        if self._sock is not None:
            self._sock.close()

    def ping(self):
        self._send_data(api.Action.PING, b'')

    def write_up(self):
        pass

    def read_up(self):
        pass

    def _send_data(self, action: int, data: bytes) -> None:
        if self._sock is None:
            logging.error('Not connected yet, cant send data!')
            return

        data = api.make_data_packet(settings.IOTORO_VERSION,
                                    action,
                                    data)

        # Encrypt the packet
        body = crypto_utils.encode_packet(self._device_key,
                                          self._device_id,
                                          data)

        # Endpoint is always BASE/md5 hash of device id.
        endpoint = f'{self._base_endpoint}/{crypto_utils.md5(self._device_id)}/'

        packet = self._make_http_packet('POST', endpoint, body.as_bytes())

        self._sock.send(packet)
        response = self._sock.recv(1024).decode('utf-8')
        print(response)
