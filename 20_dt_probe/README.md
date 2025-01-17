Raspberry Pi
------------
- Run and follow the run_on_rpi helper script

TI BBB
------------
make
sudo cp BBB-TESTOVERLAY.dtbo /boot/dtbs/$(uname -r)/overlays/
sudo vi /boot/uEnv.txt
- Enable it by adding the line:
dtb_overlay=/boot/dtbs/5.10.168-ti-r71/overlays/BBB-TESTOVERLAY.dtbo
- Reboot and check by running the dt_probe.ko module; the 'probe' method
  should get invoked and reveal some DT details

The 'Device Tree Overlay.pdf' PDF doc is a presentation originally made for
the Bangalore Kernel Meetup in Nov 2024; it's been expanded upon since.
