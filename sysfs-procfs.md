## Overview

In general `procfs` export kernel information of processess while `sysfs` export information about devices and other kernel objects. 
However `procfs` also export kernel subsystem information not related to proceses, which make `procfs` procfs cluttered with lots of non-process information. These should be replaced by  `sysfs`.

e.g.

      root@gerrit01:/home/vagrant# ls /proc/ | egrep -v -E "[0-9]+" | more
      acpi
      buddyinfo
      bus
      cgroups
      cmdline
      consoles
      cpuinfo
      crypto
      devices
      diskstats
      ...


References

* http://people.ee.ethz.ch/~arkeller/linux/multi/kernel_user_space_howto-2.html
* https://www.kernel.org/doc/Documentation/filesystems/sysfs.txt
* https://lwn.net/Articles/51437/
* https://www.kernel.org/doc/Documentation/filesystems/proc.txt

