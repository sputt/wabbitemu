# Wabbitemu Windows setup for the Zelda toolchain

Wabbitemu runs the generated TI-83 Plus application.  This branch also exposes
COM automation for calculator input, emulated time, PNG captures, and GIF
recording, which is used by Zelda testing scripts and the map editor.

## Prerequisites

- Visual Studio 2022 Community (or newer) with **Desktop development with C++**,
  ATL, and a Windows 10/11 SDK.
- A TI-83 Plus ROM that you are legally entitled to use. It is not stored in
  this repository.

## Build and register

Open `Wabbitemu.sln`, select **Release / x64**, and build it. Then register the
resulting COM server from an elevated Developer PowerShell if required:

```powershell
.\x64\Release\Wabbitemu.exe /RegServer
```

If COM metadata changes, close running Wabbitemu instances, rebuild, and run
the registration command again.

## Run Zelda

First build `Zelda-pete.8xk` in the Zelda checkout. Start Wabbitemu, choose
your ROM when prompted, then open or drag `Zelda-pete.8xk` onto the emulator.
Set `WABBITEMU_EXE` before running Zelda's `build.bat` to open it automatically:

```powershell
$env:WABBITEMU_EXE = 'C:\Path\To\Wabbitemu.exe'
```

The automation additions are `PressKeyCode`, `PressKeyFor`, `SetKeyState`,
`AdvanceMilliseconds`, and `SavePNG`; calculator key codes include Up=0,
Left=1, Right=2, Down=3, APPS=16, and `2`=42.
