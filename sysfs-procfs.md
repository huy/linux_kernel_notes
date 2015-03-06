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

