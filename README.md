# Motorola Moto Series' Boot Logo

> The logo encoding and decoding are based on the script by _carock_[1].
> Use these scripts at your own risk!

## Dependencies

* Rooted Motorola Moto (tested on Moto G [2013]);
* `fastboot`;
* `libpng`.

## Build

```bash
make
```

## Usage

### Extract the original bin file

You only need to do so once.

```bash
adb shell su -c "dd if=/dev/block/platform/msm_sdcc.1/by-name/logo of=/sdcard/logo.bin count=1 bs=634418"
adb pull /sdcard/logo.bin .
```

### Modify the bin file

Choose one of the following options.

#### Extract the logos from a bin file

```bash
moto-bootlogo logo.bin
```

#### Replace the 'unlocked bootloader' logo with the original logo

```bash
moto-bootlogo -f logo.bin
```

#### Replace the bootlogo with your own logo

```bash
moto-bootlogo -i bootlogo.png logo.bin
```

### Flash the modified bin file

Make sure your device is connected in _fastboot mode_.

```bash
fastboot flash logo logo-custom.bin
```

[1] <http://forum.xda-developers.com/showpost.php?p=48891456&postcount=140>
