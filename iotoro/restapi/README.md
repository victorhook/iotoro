# iotoro API protocol

There are different types of messages:
- Ping / Pong
- Push upstream
- Push downstream
- Read upstream
- Read downstream


## Packet format
|  x   |   x - x+8   |      x+8 - x+8+16     |
|------|-------------|-----------------------|
| Data | device_id   | initialization_vector |

### Data format:
|  0 - 3  | 4 - 7  | 8 - *   |
|---------|--------|---------|
| Version | Action | Content |

## Messages

### Device -> Server

#### PING
- Version: X
- Action: PING
- Content: []

#### Push upstream
- Version: X
- Action: PUSH_UP
- Content: [XXX]

#### Read upstream
- Version: X
- Action: READ_UP
- Content: [XXX]

### Server -> Device

#### Pong
- Version: X
- Action: PONG
- Content: []

#### Push downstream
- Version: X
- Action: PUSH_DOWN
- Content: [XXX]

#### Push downstream
- Version: X
- Action: READ_DOWN
- Content: [XXX]


## Encryption