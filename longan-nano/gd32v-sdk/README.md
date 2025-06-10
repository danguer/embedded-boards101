The Longan nano is a board that contains a RISC-V 
[GD32VF103CBT6](https://www.gigadevice.com/product/mcu/main-stream-mcus/gd32vf103-series). 
Also have a RGB led, TFT Display connector and an SD card reader.

## SDK
The MCU manufacturer (GigaDevice) has a library to handle the features of the CPU.

In this example it will be using a riscv gcc toolchain and make to compile.

The example is based on the blink example from SDK

## Setup
### Windows
You can download and install the riscv-toolchaing from [here](https://gnutoolchains.com/risc-v/) 
you need to choose [GCC 10.1.0](https://sysprogs.com/getfile/1107/risc-v-gcc10.1.0.exe)

#### Setup GD32v SDK
[Download SDK](https://www.gd32mcu.com/download/down/document_id/386/path_type/1) and use 
[7-Zip](https://www.7-zip.org/download.html) to open the zip file, note that inside the 
`GD32VF103_Firmware_Library_V1.5.0` there is another 7z file that you need to open and there
it will be a `GD32VF103_Firmware_Library_V1.5.0` folder that you need to unzip in your system.

### Linux
#### Ubuntu 24.04
##### Dependencies
```
apt install make gcc-riscv64-unknown-elf wget p7zip
```

##### Setup GD32v SDK

The following code download the firmware into `/opt` folder, change the `mv` command to use 
any other folder

```
cd /tmp
wget --content-disposition https://www.gd32mcu.com/download/down/document_id/386/path_type/1
7z x GD32VF103_Firmware_Library_V1.5.0.7z
# zip file just contain another zip -_- so extract it
cd GD32VF103_Firmware_Library_V1.5.0
7z x GD32VF103_Firmware_Library_V1.5.0.7z
mv GD32VF103_Firmware_Library_V1.5.0 /opt/GD32VF103_Firmware_Library
cd /opt
rm -rf /tmp/GD32VF103_Firmware_Library_V1.5.0*
```

##### Building Newlib
I haven't found any suitable package that contains newlib nano which is needed, so this will 
compile it and save into `/opt/newlib-nano`

```
cd /tmp
wget https://github.com/bminor/newlib/archive/refs/tags/newlib-4.5.0.tar.gz
tar zxf newlib-4.5.0.tar.gz
cd newlib-newlib-4.5.0/
./configure \
    --target=riscv64-unknown-elf \
    --prefix=/opt/newlib-nano \
    --enable-newlib-reent-small \
    --disable-newlib-fvwrite-in-streamio \
    --disable-newlib-fseek-optimization \
    --disable-newlib-wide-orient \
    --enable-newlib-nano-malloc \
    --disable-newlib-unbuf-stream-opt \
    --enable-lite-exit \
    --enable-newlib-global-atexit \
    --enable-newlib-nano-formatted-io \
    --disable-newlib-supplied-syscalls \
    --disable-nls \
    CFLAGS_FOR_TARGET="-Os -ffunction-sections -fdata-sections -mcmodel=medlow" \
    CXXFLAGS_FOR_TARGET="-Os -ffunction-sections -fdata-sections -mcmodel=medlow"

make -j$(nproc)
make install
cp /opt/newlib-nano/riscv64-unknown-elf/lib/rv32imac/ilp32/libc.a \
    /opt/newlib-nano/riscv64-unknown-elf/lib/rv32imac/ilp32/libc_nano.a 
```

## Compile
### Windows
Open a command prompt ( `CMD` ) and go into `code` folder and run:
```
make GD32_LIBRARY=PATH_TO_LIBRARY\GD32VF103_Firmware_Library_V1.5.0 RISCV_PATH=PATH_TO_RISCV_SETUP\risc-v\riscv64-unknown-elf
```
Replace `PATH_TO_LIBRARY` to the path where SDK is unzipped and `PATH_TO_RISCV_SETUP` to the path where toolchain was installed

### Linux
```
cd code
make GD32_LIBRARY=/opt/GD32VF103_Firmware_Library RISCV_PATH=/opt/newlib-nano/riscv64-unknown-elf
```

### Download to device
In the `build` folder will be created a `blink.bin` file that can be used in the **GD32 All in One Programmer** to download into device.

The code will show blue, green and red leds similar to the stock program in the board.
