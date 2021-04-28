from dataclasses import dataclass, field
import struct


class Action:
    PING = 1
    WRITE_UP = 2
    WRITE_DOWN = 3
    PONG = 4
    READ_UP = 5
    READ_DOWN = 6


def get_action_choices() -> list:
    return [
        ('Pi', 'PING'),
        ('Wu', 'WRITE_UP'),
        ('Wd', 'WRITE_DOWN'),
        ('Po', 'PONG'),
        ('Ru', 'READ_UP'),
        ('Rd', 'READ_DOWN'),
    ]


@dataclass
class DataPacket:
    version: int
    action: int
    content: field(default_factory=bytes)


def make_data_packet(version: int, action: int, content: bytes) -> bytes:
    first_byte = (version << 4) | action
    first_byte = struct.pack('B', first_byte)
    return first_byte + content


def decode_data_packet(data: bytes) -> DataPacket:
    packet = DataPacket(
        version=(data[0] & 0xf0) >> 4,
        action=data[0] & 0x0f,
        content=data[1:]
    )
    return packet


def print_binary(data: bytes) -> None:
    data = [bin(a)[2:].zfill(8) for a in data]
    print(' '.join(data))


raw_packet = make_data_packet(1, Action.PONG, b'hello world!')
print_binary(raw_packet)
packet = decode_data_packet(raw_packet)
print(packet)