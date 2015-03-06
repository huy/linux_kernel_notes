## Overview

In general `procfs` export kernel information of processess while `sysfs` export information about devices. However `procfs` 
also export kernel information not related to proceses.

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
* https://www.kernel.org/doc/Documentation/filesystems/proc.txt
