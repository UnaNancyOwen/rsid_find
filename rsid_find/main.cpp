#include <iostream>
#include <sstream>
#include <vector>

#include "RealSenseID/DiscoverDevices.h"

int main( int argc, char* argv[] )
{
    try{
        std::cout << "Find RealSense ID Devices" << std::endl;
        std::cout << "-------------------------" << std::endl;

        std::vector<RealSenseID::DeviceInfo> device_infos = RealSenseID::DiscoverDevices();

        std::cout << "* number of devices : " << device_infos.size() << std::endl;

        for( int32_t i = 0; i < device_infos.size(); i++ ){
            const RealSenseID::DeviceInfo& device_info = device_infos[i];
            std::cout << "* device " << i + 1 << std::endl;
            std::cout << "  - serial type : " << ( device_info.serialPortType == RealSenseID::SerialType::USB ? "USB" : "UART" ) << std::endl;
            std::cout << "  - port        : " << device_info.serialPort << std::endl;
        }
    }
    catch( std::exception& ex ){
        std::cout << ex.what() << std::endl;
    }

    system( "pause" );

    return 0;
}