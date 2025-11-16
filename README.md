# SensorSim_LinuxDriver
Linux device driver that simulates a sensor. This module generates random values and puts them into sys filesystem. 

This module can be useful if you develop some project that requires some sensors but you don't have them yet.

In future I plan to add more options for the sensor like generating function signals like sin or something like that. 

## Compilation and installation

After cloning repository, build the module with *make*:
```shell
foo@bar:~/SensorSim_LinuxDriver$ make
```
To install the module on your machine use command *insmod* (don't forget *sudo*)
```shell
foo@bar:~/SensorSim_LinuxDriver$ sudo insmod sensorsim.ko
```
You can check if module is installed by typing *lsmod*
```shell
foo@bar:~/SensorSim_LinuxDriver$ lsmod
Module                  Size  Used by
sensorsim              12288  0
...
```
To remove the module from your machine use command *rmmod* (Notice that there is no file extension this time)
```shell
foo@bar:~/SensorSim_LinuxDriver$ sudo rmmod sensorsim
```
## Usage
To get the value from your sensor just access file /sys/kernel/sensorsim/value
```shell
foo@bar:~/SensorSim_LinuxDriver$ cat /sys/kernel/sensorsim/sensorValue
12345
```


