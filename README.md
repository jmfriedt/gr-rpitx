# gr-rpitx 

Using the Raspberry Pi PLL as radiofrequency source controlled from GNU Radio.

# Compiling

Easiest: gr-rpitx is available as a Buildroot BR2_EXTERNAL package at 
https://github.com/oscimp/oscimp_br2_external and can be selected from 
the ``External options``.

We assume ``librpitx`` to have been compiled and installed, most probably on
the cross-compilation framework Buildroot as described at https://github.com/oscimp/oscimp_br2_external 
(tested with Buildroot 2020.11.1 and above). The benefits of using Buildroot for compiling GNU
Radio to the Raspberry Pi single board computers are detailed at
https://fosdem.org/2021/schedule/event/fsr_gnu_radio_on_embedded_using_buildroot/

See ``examples/README`` on the ``cmake`` command to run in the ``build_RP`` directory
for compiling and linking gr-rpitx with Buildroot supporting GNU Radio (configuration
files for RPi3 and RPi4 are for example found in the ``configs`` directory
of https://github.com/oscimp/oscimp_br2_external). 

To summarize the content of ``examples/README``, assuming Buildoot is installed in
``$BUILDROOT`` and the Raspberry Pi IP address is ``$IP_RPI``:

```bash
mkdir -p gr-rpitx/build_BR
cd gr-rpitx/build_BR
mkdir /tmp/tmpdir     # empty directory where compilation output will be stored
cmake -DCMAKE_INSTALL_PREFIX:PATH=/tmp/tmpdir -DCMAKE_TOOLCHAIN_FILE=$BUILDROOT/output/host/usr/share/buildroot/toolchainfile.cmake  -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON ../
make
make install
scp -r /tmp/tmpdir/* root@$IP_RPI:/usr
```
will install gr-rpitx to the Raspberry Pi

For GNU Radio Companion on the host PC to be aware of the ``gr-rpitx`` sink block,
this tool must also be compiled for the host after installing ``librpitx`` on the host (``git clone https://github.com/F5OEO/librpitx/ && cd librpitx/src && LDFLAGS="-lm -lrt -lpthread" make && sudo cp librpitx.so /usr/lib``):
```bash
mkdir -p gr-rpitx/build_PC
cd gr-rpitx/build_PC
sudo cp -r $BUILDROOT/output/host/aarch64-buildroot-linux-gnu/sysroot/usr/include/librpitx /usr/include/
cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON ../
make 
sudo make install
```

meaning that for the host PC to know about gr-rpitx source when generating the flowgraph, 
the header files must be copied to an accessible directory (most commonly ``/usr/include``) and 
the dynamic library also found on the host system (even though gr-rpitx can of course not be 
executed on the PC). To avoid transfering a new SD card image to the RPi, we have selected to 
install the ARM binaries in a temporary directory on the host, e.g. ``/tmp/tempdir`` thanks to 
``make install`` after completing ``make`` and then ``scp -r /tmp/tempdir $IP_RPI:/usr`` since 
the tree structure in the installation directory matches the target tree structure starting 
from ``/usr``.

# Usage

SampleRate 10000-250000 

Complex float input I,Q samples

Carrier frequency [Hz] in the 50 kHz to 1500 MHz range

Application example: AM modulated signal using the following flowchart, generated from GNU Radio
Companion on the host PC and executed with No GUI on the target Raspberry Pi 4:

<img src="examples/rpi_am.png">

resulting in the following measurements at fundamental frequency (left) and overtone 5:

<img src="examples/AM5kHz_fundamental.png">
<img src="examples/AM5kHz_overtone5.png">

Experimental setup: NEVER EVER radiate the RPiTX output on an antenna, only measure using a wired
connection to the receiver:

<img src="examples/DSC_0587ann_small.jpg">

Movie demonstrating the use of gr-rpitx for emitting an FM signal received by
a DVB-T dongle: https://github.com/jmfriedt/gr-rpitx/tree/main/examples/gr-rpitx_demo.mp4
