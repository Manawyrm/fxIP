```
sudo slattach -s 115200 -p slip -v -d -L /dev/ttyUSB0
sudo ip addr add 10.10.10.1/24 dev sl0
sudo ip link set mtu 576 dev sl0
sudo ip link set sl0 up
```

```
touch src/main.c && make clean && fxsdk build-fx && cp -f Fxip.g1a ~/Exchange/Test/
```
