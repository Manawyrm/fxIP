```
sudo slattach -s 115200 -p slip -v -d -L /dev/ttyUSB1
sudo ip addr add 10.10.10.1/24 dev sl0
sudo ip link set mtu 576 dev sl0
sudo ip link set sl0 up
```