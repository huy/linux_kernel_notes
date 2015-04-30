# udev

`udev` dynamically create inode by default in `/dev/` for hotplug devices. udev runs as userspace deamon listening on netlink socket for hardware hotplug event and create/remove a device's inode according to predefined rules as well as load kernel module(s) if it is required. 

`udev` also create meaningful symlink to easy identification devices. e.g.

    # ls -l /dev/disk/by-uuid/
    total 0
    lrwxrwxrwx 1 root root 10 Apr 30 16:08 30051059-44e1-4425-9bc8-9b9ade27822c -> ../../dm-1
    lrwxrwxrwx 1 root root 10 Apr 30 16:08 a975dd09-15f7-4945-a2f8-f59de9af125a -> ../../dm-0
    lrwxrwxrwx 1 root root 10 Apr 30 16:08 f012a222-cb8d-436f-9291-d559e99ce304 -> ../../sda1


**references**

* https://www.linux.com/news/hardware/peripherals/180950-udev
* https://www.kernel.org/doc/pending/hotplug.txt
