# Wabbitemu

## For Windows, Mac OS X, and Android

By Spencer Putt, Chris Shappell, and James Montelongo

Wabbitemu creates a Texas Instruments graphing calculator right on your Windows, Mac, or Android device. Wabbitemu supports the TI-73, TI-81, TI-82, TI-83, TI-83 Plus, TI-83 Plus Silver Edition, TI-84 Plus, TI-84 Plus Silver Edition, TI-85, and TI-86. Fast and convenient, Wabbitemu allows you to always have your trusty calculator with you. Because Wabbitemu is an emulator, the calculator it creates will act exactly like the real thing.

Wabbitemu, like all emulators, requires a ROM image. Wabbitemu includes an easy to use setup wizard that will help you create a ROM image for your TI.

### Developers

To build for Windows, install the latest Windows SDK and build the Windows Release x64 configuration:

    msbuild -p:Configuration="Wabbitemu Release" -p:Platform=x64

This needs some major cleanup and migration to newer Visual Studio community editions.
