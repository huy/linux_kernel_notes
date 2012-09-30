# Get kernel state using  sysrq


Enable


    # echo 1 > /proc/sys/kernel/sysrq


Generate dump


    # echo 'm' > /proc/sysrq-trigger


Trigger

1. m - dump information about memory allocation
2. t - dump thread state information
3. p - dump current CPU registers and flags
4. c - intentionally crash the system (useful for forcing a disk or netdump)
5. s - immediately sync all mounted filesystems
6. u - immediately remount all filesystems read-only
7. b - immediately reboot the machine
8. o - immediately power off the machine (if configured and supported)


View dump


    # vi /var/log/messages


References

1. https://access.redhat.com/kb/docs/DOC-2024