import binascii
import hashlib

from Crypto.Cipher import AES
from Crypto.Util.Padding import pad, unpad
from Crypto.Random import get_random_bytes


def asbin(data: bytes):
    print(' '.join(hex(a)[2:] for a in data))

def asord(data: bytes):
    print(' '.join(str(a) for a in data))


id = get_random_bytes(8)
a = binascii.hexlify(id).decode('utf-8')
k = '13feffeb83805790'
print(asord(binascii.unhexlify(k)))