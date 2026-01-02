# ESPHome Dynamixel Component

External ESPHome component for Dynamixel servos with Protocol 1.0 and 2.0 support over UART half-duplex (TTL).

## Features
- Protocol 1.0 and 2.0 packet support
- Full instruction set (ping/read/write/sync/bulk/reboot)
- Device register definitions (built-in or custom)
- Half-duplex direction pin support

## Install from GitHub
```yaml
external_components:
  - source:
      type: github
      repository: yourname/esphome-dynamixel
      ref: main
    components: [dynamixel]
```

## Example ESPHome config
```yaml
external_components:
  - source: local
    path: ./components

uart:
  id: uart_dxl
  tx_pin: 17
  rx_pin: 16
  baud_rate: 57600

dynamixel:
  id: dxl
  uart_id: uart_dxl
  protocol: 2
  direction_pin: 4
  devices:
    - name: "xm430"
      device_id: 1
      protocol: 2
      registers:
        - { name: "torque_enable", address: 64, length: 1, access: "rw" }
        - { name: "goal_position", address: 116, length: 4, access: "rw" }
        - { name: "present_position", address: 132, length: 4, access: "r" }
```

## XM430 example
See `example_xm430.yaml` for a more complete XM430 (servo 431) register set.
