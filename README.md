## EagleSat II Telemetry & Telecommand System
EagleSat II ESTTS shared library source code and associated examples

### Building From Source

The EagleSat Telemetry & Telecommand System is built as a shared 
library - called ```libestts``` - using ```CMake```. The reccomended IDE
for working with the library is ```JetBrains CLion``` - as this IDE has
native support for CMake applications. With CLion, building the library
is as simple as cloning the repository and setting the ```CMakeLists.txt```
at the root directory as the active project. If working with another IDE,
you can follow the following steps:

1. Install CMake and the relevant build tools for your platform. Ensure 
these are available in your executable path.
2. Change to the root repository directory and create a 
```build``` directory

    ```asm
    cd /build 
    mkdir build 
    cd build
    ```
2. Build the project using CMake
   * For Make build systems:
       ```asm
       cmake .. -DCMAKE_BUILD_TYPE=Debug
       make
       ```
   * More architectures to come..

3. After building, executables are found in the ```/examples``` 
and ```tests``` directories. If you wish to install the library,
see the *installing* section.

### Installing

Installing the library installs a binary executable and include directories
to your computer's library directory. This is unnecessary for now..

1. Install CMake and the relevant build tools for your platform. Ensure
   these are available in your executable path.
2. Change to the root repository directory and create a
   ```build``` directory

    ```asm
    cd /build 
    mkdir build 
    cd build
    ```
3. Build and install the library
    ```asm
    cmake .. -DCMAKE_BUILD_TYPE=Debug
    make
    make install
    ```

### Contributing

Please follow the guidelines in the 
[contributions](https://github.com/ERAU-EagleSat-PR/es2-estts-cpp/blob/main/CONTRIBUTING.md) 
page.

### Other topics
* [Changelog](https://github.com/ERAU-EagleSat-PR/es2-estts-cpp/blob/main/CHANGELOG.md)
* [License](https://github.com/ERAU-EagleSat-PR/es2-estts-cpp/blob/main/LICENSE)