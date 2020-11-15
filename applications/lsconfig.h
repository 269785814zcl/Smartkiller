/*
mtd_erase /dev/mtd0
devcp tftp://193.169.2.215/rtthread.elf /dev/mtd0
set al /dev/mtd0
reboot

set al tftp://193.169.2.215/rtthread.elf
reboot

ifaddr syn0 193.169.2.230
set ifconfig syn0:193.169.2.230
 */
#ifndef __LSCONFIG_H
#define __LSCONFIG_H

#define USING_BLUET 1
#define USING_PWM 1
#define USING_GPIO 1
#define USING_ROS 1
#define PRINT_SPEED 0

#endif
