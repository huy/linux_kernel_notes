# udev

udev dynamically create inode by default in `/dev/` for hotplug devices. udev runs as userspace deamon listening on netlink socket for hardware hotplug event and create/remove a device's inode according to predefined rules as well as load kernel module(s) if it is required.

**references**

* https://www.linux.com/news/hardware/peripherals/180950-udev
