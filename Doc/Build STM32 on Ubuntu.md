## Build STM32 on Ubuntu

Go to the ARM official website to download the `gcc-arm-none-eabi` compiler:
https://developer.arm.com/downloads/-/gnu-rm

Download `gcc-arm-none-eabi-10.3-2021.10-x86_64-linux.tar.bz2`

Decompress compiler:
```bash
tar xjf ./gcc-arm-none-eabi-10.3-2021.10-x86_64-linux.tar.bz2
```
Verify that the compiler can be executed on your computer:
```bash
./gcc-arm-none-eabi-10.3-2021.10/bin/arm-none-eabi-gcc --version
```
Move gcc compiler to home directory:
```bash
mv ./gcc-arm-none-eabi-10.3-2021.10 ~/gcc-arm-none-eabi-10.3-2021.10
```
Add gcc folder into system $PATH:
```bash
echo "export PATH=$PATH:~/gcc-arm-none-eabi-10.3-2021.10/bin" >> ~/.bashrc
cat ~/.bashrc |grep gcc
```
Build firmware:
```bash
./build.sh
```

