rsid_find
=========

This is tool and module for find all connected RealSense ID devices.  

Sample
------
You can use this tool with following command.  
### Command
```
rsid_find.exe
```
### Output
```
Find RealSense ID Devices
-------------------------
* number of devices : 1
* deivice 1
  - serial type : USB
  - port        : COM3
```

Module
------
You can use find devices module in your C++ code.  
This module returns serial configurations of all connected devices.  
You can easily connect to RealSenseID::FaceAuthenticator with this configuration.  
```cpp
#include "RealSenseID/FindDevices.h"
```
```cpp
RealSenseID::FindDevices find_devices;
std::vector<RealSenseID::SerialConfig> serial_configs = find_devices.get_serialconfigs();
```

Environment
-----------
* Windows 10
* Visual Studio 2019 (C++)
* CMake 3.17 (or later)
* RealSense ID SDK v0.13.0 (or later)

License
-------
Copyright &copy; 2021 Tsukasa SUGIURA  
Distributed under the [MIT License](http://www.opensource.org/licenses/mit-license.php "MIT License | Open Source Initiative").

Contact
-------
* Tsukasa Sugiura  
    * <t.sugiura0204@gmail.com>  
    * <http://unanancyowen.com>  