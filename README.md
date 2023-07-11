# gamef

My game engine and game made with SDL3.

## Platforms

![macOS](https://img.shields.io/badge/mac%20os-000000?style=for-the-badge&logo=macos&logoColor=F0F0F0)
![Linux](https://img.shields.io/badge/Linux-FCC624?style=for-the-badge&logo=linux&logoColor=black)
![Windows](https://img.shields.io/badge/Windows-0078D6?style=for-the-badge&logo=windows&logoColor=white)
![Android](https://img.shields.io/badge/Android-3DDC84?style=for-the-badge&logo=android&logoColor=white)

## Features

- Graphics
- Sounds
- GUI
- Simple gameplay

## Installation

Game requires:

- [SDL3](http://libsdl.org/) v3.0.0+
- [Cmake](https://cmake.org) 3.16+

For android:

- Gradle
- SDK
- NDK

### [Build](https://drive.google.com/drive/folders/1xNcAWjovcvdE2pLFfY6g788OUNJTqxKG)

Linux or windows:

```
mkdir build
cd build
cmake ..
cmake --build .
```

Android:

1. Open folder android-project in Android Studio.
2. Provide path to SDL with variable `sdl3.dir` in local.properties.
3. Provide paths to SDK and NDK if needed.
4. Build.
