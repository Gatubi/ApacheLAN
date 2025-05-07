# ScriptHookRDR ApacheLAN Mod

## Purpose
This repository seeks to create a Multiplayer LAN experience to enjoy RDR1 with friends in PC

## Contributing

This still in an early development there's still many errors and things to improve, so if you want to contribute in any way to this project, feel free to create a pull request.

## Configuration and Operation

To use the mod you'll have to make sure RDR1 have firewall permissions in the network. Then you can configure the ApacheConfig.ini file (you can find it in /src) with your IP address of the Server computer and your PC identification number.

Then you can open the game, first you have to turn on the server with the F9 key, after this, you do the same in the second computer and you'll have two players


## Prerequisites
- [CMake](https://cmake.org/download/)

## Build
CMake is used to build the project and generate solution files.

```
mkdir build
cd build
cmake ..
```

Solution files will be available in the **/build** directory
Compiled binary files will be available in the **/bin** directory

## Credits
- [TheRouletteBoi](https://github.com/therouletteboi) for his native database
- [CabooseSayzWTF](https://x.com/CabooseSayzWTF) for his years of native research on console
- [Foxxyyy](https://github.com/Foxxyyy) for his years of native research on console
- [EvilBlunt](https://github.com/EvilBlunt) for his strings and enums
