# fxIP
### TCP/IP stack and IRC client for Casio fx-9860/9750 calculators


<img src="https://screenshot.tbspace.de/miceroxwgfj.jpg" width="350"/>&nbsp;<img src="https://screenshot.tbspace.de/bjwklegfnmp.jpg" width="350"/>

![Screenshot of ping](https://screenshot.tbspace.de/uwaontfeqlk.png)

[YouTube video of fxIP's IRC client, connecting to irc.libera.chat](https://www.youtube.com/watch?v=afkrucsMMrc)  
[YouTube video of fxTerm, a simple RS232 terminal application](https://www.youtube.com/watch?v=epFX8K0dhdY)

fxIP is a port of [uIP 1.0](https://github.com/adamdunkels/uip) to Casio fx-9750/fx-9860 calculators.  
It (currently) requires calculators with a SuperH SH4a CPU, needs quite a bit of SRAM.  

fxIP connects to IP networks via SLIP encapsulation over the 3-pin 2.5mm 5V serial port/UART.  
By default, it connects to irc.libera.chat (without SSL).

### Network setup (needs root)
```
slattach -s 115200 -p slip -v -d -L /dev/ttyUSB0
ip addr add 10.10.10.1/24 dev sl0
ip link set mtu 576 dev sl0
ip link set sl0 up
iptables -t nat -A POSTROUTING -o eth0 -j MASQUERADE
echo "1" > /proc/sys/net/ipv4/ip_forward
```

### Building Add-In
Requires [fxsdk](https://gitea.planet-casio.com/Lephenixnoir/fxsdk) to build.  
```
touch src/main.c && make clean && fxsdk build-fx && cp -f Fxip.g1a ~/Exchange/Test/
```


### Credits
fxIP uses the following fantastic projects:  
[fxsdk](https://gitea.planet-casio.com/Lephenixnoir/fxsdk) by [Lephenixnoir](https://gitea.planet-casio.com/Lephenixnoir)  
[c-object-log](https://github.com/TobleMiner/c-object-log) by [TobleMiner](https://github.com/TobleMiner)  
[uIP 1.0](https://github.com/adamdunkels/uip) by [Adam Dunkels](https://github.com/adamdunkels)  
