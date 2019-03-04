## HOW-TO-RUN

1.  you need to be running PulseAudio (on your linux machine)
2.  Create a bash script and save this to it(e.g audio.sh), note that you will need to update source based on your device
```bash
#!/bin/sh
case "$1" in
  start)
    $0 stop 
    pactl load-module module-simple-protocol-tcp rate=48000 format=s16le channels=2 source=alsa_output.pci-0000_00_01.1.hdmi-stereo.monitor record=true port=12345
    ;;
  stop)
    pactl unload-module `pactl list | grep tcp -B1 | grep M | sed 's/[^0-9]//g'`
    ;;
  *)
    echo "Usage: $0 start|stop" >&2
    ;;
esac
```
3.  run the script with `start` arugment, aka `./audio.sh start`
4.  Run the android app, fill in the your PC IP address, port = 12345 and click on the stream switch
