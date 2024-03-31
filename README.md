# dumpmidi-rt

[![Build on Linux](https://github.com/pedrolcl/dumpmidi-rt/actions/workflows/linux-build.yml/badge.svg)](https://github.com/pedrolcl/dumpmidi-rt/actions/workflows/linux-build.yml)

Multiplatform Command Line MIDI Monitor program using Drumstick::RT

This program is equivalent to the `drumstick-dumpmid` utility that is included 
in the Drumstick source packages, but instead of using Drumstick::ALSA it uses 
the Drumstick::RT library, so it may be used on several operating systems.

~~~
./dumpmidi-rt --help
Usage: ./dumpmidi-rt [options]
Drumstick command line utility for decoding MIDI events

Options:
  -h, --help             Displays help on commandline options.
  --help-all             Displays help including Qt specific options.
  -v, --version          Displays version information.
  -d, --driver <driver>  MIDI Driver.
  -p, --port <port>      MIDI Port.
  -l, --list             List available MIDI Ports.  
~~~

The available drivers for each operating system are the following. The first on
each group is the default.

* Linux: ALSA, OSS, Network
* Windows: Windows MM, Network
* macOS: CoreMIDI, Network
* Unix: OSS, Network

~~~
./dumpmidi-rt -p nanoKEY2
driver: ALSA
port: nanoKEY2
Press ^C to stop the program...
Event_________________ Ch _Data__
note on                 0  60  71
note off                0  60  64
control change          0  64 127
control change          0  64   0
pitch bend              0  -327
pitch bend              0  -654
pitch bend              0  -981
pitch bend              0  -572
pitch bend              0  -163
pitch bend              0     0
^CReceived a SIGINT. Exiting
~~~
