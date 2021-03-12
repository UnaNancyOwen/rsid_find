#include <iostream>
#include <sstream>
#include <vector>

#include "RealSenseID/SerialConfig.h"
#include "RealSenseID/FindDevices.h"

int main( int argc, char* argv[] )
{
    try{
        std::cout << "Find RealSense ID Devices" << std::endl;
        std::cout << "-------------------------" << std::endl;

        RealSenseID::FindDevices find_devices;
        std::vector<RealSenseID::SerialConfig> serial_configs = find_devices.get_serialconfigs();

        std::cout << "* number of devices : " << serial_configs.size() << std::endl;

        for( int32_t i = 0; i < serial_configs.size(); i++ ){
            const RealSenseID::SerialConfig& serial_config = serial_configs[i];
            std::cout << "* device " << i + 1 << std::endl;
            std::cout << "  - serial type : " << ( serial_config.serType == RealSenseID::SerialType::USB ? "USB" : "UART" ) << std::endl;
            std::cout << "  - port        : " << serial_config.port << std::endl;
        }
    }
    catch( std::exception& ex ){
        std::cout << ex.what() << std::endl;
    }

    system( "pause" );

    return 0;
}