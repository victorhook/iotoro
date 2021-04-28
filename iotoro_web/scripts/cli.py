import binascii
from dataclasses import dataclass, field
from datetime import datetime
import hashlib

from Crypto.Cipher import AES
from Crypto.Util.Padding import pad, unpad
from Crypto.Random import get_random_bytes

def ashex(data):
    return ' '.join(hex(a)[2:].zfill(2) for a in data)

device_key = 'c0c04877f74d69c68e120d45ddca18d0'
device_key = binascii.unhexlify(device_key)
iv = [152, 5, 221, 216, 222, 120, 83, 136, 168, 55, 182, 30, 195, 225, 18, 41]
data = [17, 0, 0, 19, 254, 255, 235, 131, 128, 87, 144, 5, 5, 5, 5, 5]

print(ashex(device_key))

key = AES.new(bytes(device_key), AES.MODE_CBC, bytes(iv))
encrypted = key.encrypt(bytes(data))
print(' '.join(hex(a)[2:].zfill(2) for a in encrypted))


import random
r = [random.randint(0, 255) for a in range(16)]
#print(', '.join(str(a) for a in r))