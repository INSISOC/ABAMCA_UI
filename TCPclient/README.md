THIS IS A COPY OF THE REPOSITORY 
https://github.com/Sasisekhar/DEVS_IO
There are moficications to adapt it to our project.
Therefore, all the content of this readme file is not applicable.

# Blank Cadmium I/O Project (RT)
|  Linux | Windows| ESP32 | MSP432 |
|:--|:--|:--|:--|
|:heavy_check_mark:|:heavy_check_mark:|:question:|:question:|
## Introduction
This is a template DEVS model that showcases the use of asynchronous I/O. Apart from the DEVS model, the repo also contains a `sender.cpp` file, and `recv.cpp` file. `sender.cpp` defines a TCP client that can be used to inject packets into the DEVS model. `recv.cpp` defines a TCP client that recieves packets from the DEVS model.

## Dependencies
This project assumes that you have Cadmium installed in a location accessible by the environment variable `$CADMIUM`.
_This dependency would be met by default if you are using the ARSLAB servers. To check, try `echo $CADMIUM` in the terminal_

**PLEASE NOTE THAT THIS EXAMPLE REQUIRES YOU TO HAVE INSTALLED THE 'DEVS_IO' BRANCH OF CADMIUM**
To do so, go to  your Cadmium installation by opening up your terminal (or wsl), and typing
```
cd $CADMIUM
```
This will take you to the `/include` folder.
Now, execute the following commands to get the DEVS_IO branch.
```
cd ..
git pull
git switch -b DEVS_IO
git pull
```
_At the moment, DEVS\_IO is only available in Sasisekhar's version of Cadmium, which can be found [here](https://github.com/Sasisekhar/cadmium_v2/tree/DEVS_IO)._

## Build
To build this project, run:
```sh
source build_sim.sh
```
__NOTE__: Everytime you run build_sim.sh, the contents of `build/` and `bin/` will be replaced.

To build this project for the ESP32, run:
```sh
source build_esp.sh
```

## Execute
To run the project, run:
```sh
./bin/sample_project
```

To flash the project onto the esp32, run:
```sh
idf.py -p $ESP_PORT flash
```

## Modify
You can modify this project per your requirement. Change the project name defined in the topmost CMakeLists.txt file here:
```cmake
set(projectName "sample_project")
```
If you want to add other include directories, add the following to the CMakeLists.txt file in the `main` directory:
```cmake
target_include_directories(${projectName} PRIVATE "/path/to/dependency")
```
