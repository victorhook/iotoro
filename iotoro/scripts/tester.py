import binascii
import hashlib

from Crypto.Cipher import AES
from Crypto.Util.Padding import pad, unpad
from Crypto.Random import get_random_bytes


def asbin(data: bytes):
    print(' '.join(hex(a)[2:] for a in data))

data = get_random_bytes(18)
asbin(data)
data = pad(data, 16)
asbin(data)