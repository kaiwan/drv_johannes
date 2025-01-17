**Raspberry Pi**
------------
- Run and follow the run_on_rpi helper script

**TI BBB**
------------
- `make`
- `sudo cp BBB-TESTOVERLAY.dtbo /boot/dtbs/$(uname -r)/overlays/`
- `sudo vi /boot/uEnv.txt`
  - Enable it by adding the line:
    `dtb_overlay=/boot/dtbs/<uname -r>/overlays/BBB-TESTOVERLAY.dtbo`
    (replace the <uname -r> with the output of `uname -r`)
- Reboot and check by inserting the `dt_probe.ko` module; the 'probe' method
  should get invoked and reveal some DT details.

*Tip*
To auto-load the kernel module, simply add it's name in the file
 `/etc/modules-load.d/modules.conf`

FYI: the `Device_Tree_and_DTOverlays-Trg.pdf` PDF doc is a presentation originally made for
the Bangalore Kernel Meetup in Nov 2024; it's been expanded upon since.
