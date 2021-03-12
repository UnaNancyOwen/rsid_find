/*
 This is module for auto find realsense id devices.
 This module returns serial configurations of all connected devices.
 You can easily connect to RealSenseID::FaceAuthenticator with this configuration.

 #include "RealSenseID/FindDevices.h"

 RealSenseID::FindDevices find_devices;
 std::vector<RealSenseID::SerialConfig> serial_configs = find_devices.get_serialconfigs();

 Copyright (c) 2021 Tsukasa Sugiura <t.sugiura0204@gmail.com>
 Licensed under the MIT license.

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
*/

#ifndef __FIND_DEVICES__
#define __FIND_DEVICES__

#include <iostream>
#include <cassert>
#include <string>
#include <vector>
#include <regex>

#include <windows.h>
#include <tchar.h>
#include <setupapi.h>
#pragma comment( lib, "setupapi.lib" )

#include "RealSenseID/SerialConfig.h"

namespace RealSenseID
{
    class FindDevices
    {
    private:
        HDEVINFO hdevinfo;
        std::vector<RealSenseID::SerialConfig> serial_configs;

    public:
        FindDevices()
            : hdevinfo( INVALID_HANDLE_VALUE )
        {
        }

        ~FindDevices()
        {
            // Clear Serial Configs
            for( RealSenseID::SerialConfig& serial_config : this->serial_configs ){
                if( serial_config.port ){
                    delete[] serial_config.port;
                    serial_config.port = nullptr;
                }
            }
            
            // Destroy Device Info List
            if( hdevinfo != INVALID_HANDLE_VALUE ){
                SetupDiDestroyDeviceInfoList( hdevinfo );
                hdevinfo = INVALID_HANDLE_VALUE;
            }
        }

        std::vector<RealSenseID::SerialConfig> get_serialconfigs()
        {
            find_serialconfigs();
            return serial_configs;
        }

    private:
        inline void find_serialconfigs()
        {
            // Clear Serial Configs
            for( RealSenseID::SerialConfig& serial_config : this->serial_configs ){
                if( serial_config.port ){
                    delete[] serial_config.port;
                    serial_config.port = nullptr;
                }
            }

            // Get GUID of Ports
            BOOL result;
            GUID guid;
            DWORD size = 0;
            result = SetupDiClassGuidsFromName( _T( "Ports" ), (LPGUID)&guid, 1, &size );
            if( result == FALSE ){
                return;
            }

            // Get Device Info List of Ports
            hdevinfo = SetupDiGetClassDevs( &guid, NULL, NULL, DIGCF_PRESENT | DIGCF_PROFILE );
            if( hdevinfo == INVALID_HANDLE_VALUE ){
                return;
            }

            if( hdevinfo == INVALID_HANDLE_VALUE ){
                this->serial_configs.swap( std::vector<RealSenseID::SerialConfig>() );
                return;
            }

            SP_DEVINFO_DATA devinfo_data;
            ZeroMemory( &devinfo_data, sizeof( SP_DEVINFO_DATA ) );
            devinfo_data.cbSize = sizeof( SP_DEVINFO_DATA );

            DWORD index = 0;
            std::vector<RealSenseID::SerialConfig> serial_configs;
            while( true ){
                // Get Device Infor Data
                BOOL result = SetupDiEnumDeviceInfo( hdevinfo, index++, &devinfo_data );
                if( result == FALSE ){
                    break;
                }

                // Get Serial Config
                RealSenseID::SerialConfig serial_config;
                try{
                    serial_config.serType = get_setialtype( devinfo_data );
                    std::string port = get_port( devinfo_data );
                    serial_config.port = new char[port.size() + 1];
                    memcpy( (void*)serial_config.port, port.c_str(), port.size() + 1 );
                }
                catch( const std::runtime_error& error ){
                    continue;
                }

                // Add Serial Config
                serial_configs.push_back( serial_config );
            }

            // Destroy Device Info List
            if( hdevinfo != INVALID_HANDLE_VALUE ){
                SetupDiDestroyDeviceInfoList( hdevinfo );
                hdevinfo = INVALID_HANDLE_VALUE;
            }

            // Swap Serial Configs
            this->serial_configs.swap( serial_configs );

            return;
        }

        inline RealSenseID::SerialType get_setialtype( SP_DEVINFO_DATA& devinfo_data )
        {
            assert( hdevinfo != INVALID_HANDLE_VALUE );

            // Get HardWare ID
            TCHAR hardware_id[MAX_PATH];
            BOOL result = SetupDiGetDeviceRegistryProperty( hdevinfo, &devinfo_data, SPDRP_HARDWAREID, NULL, (BYTE*)hardware_id, MAX_PATH, NULL );
            if( result == FALSE ){
                throw std::runtime_error( "failed get hardware id! (" + std::to_string( GetLastError() ) + ")" );
            }

            // Get Serial Type
            const std::string id = to_string( hardware_id );
            if( std::regex_search( id, std::regex( "VID_2AAD" ) ) && std::regex_search( id, std::regex( "PID_6373" ) ) ){
                return RealSenseID::SerialType::USB;
            }
            if( std::regex_search( id, std::regex( "VID_04D8" ) ) && std::regex_search( id, std::regex( "PID_00DD" ) ) ){
                return RealSenseID::SerialType::UART;
            }

            throw std::runtime_error( "failed get serial type!" );
        }

        inline std::string get_port( SP_DEVINFO_DATA& devinfo_data )
        {
            assert( hdevinfo != INVALID_HANDLE_VALUE );

            HKEY hkey = NULL;
            TCHAR port_name[MAX_PATH];
            try{
                // Open Registry Key
                hkey = SetupDiOpenDevRegKey( hdevinfo, &devinfo_data, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_READ );
                if( !hkey ){
                    throw std::runtime_error( "failed open registory key! (" + std::to_string( GetLastError() ) + ")" );
                }

                // Get Registry Value
                LSTATUS status;
                DWORD reg_type = REG_SZ;
                DWORD size = sizeof( port_name );
                status = RegQueryValueEx( hkey, _T( "PortName" ), NULL, &reg_type, (LPBYTE)port_name, &size );
                if( status != ERROR_SUCCESS ){
                    throw std::runtime_error( "failed get registry value!" + std::to_string( GetLastError() ) + ")" );
                }
            }
            catch( const std::runtime_error& error ){
                if( hkey ){
                    RegCloseKey( hkey );
                    hkey = NULL;
                }
                throw error;
            }

            if( hkey ){
                RegCloseKey( hkey );
                hkey = NULL;
            }

            return to_string( port_name );
        }

        inline std::string to_string( const TCHAR* str ) const
        {
            std::string out;
            #ifdef UNICODE
            const int32_t size = WideCharToMultiByte( CP_ACP, WC_COMPOSITECHECK | WC_DEFAULTCHAR, str, -1, NULL, 0, NULL, NULL );
            std::vector<char> multibyte_str( size, '\0' );
            WideCharToMultiByte( CP_ACP, WC_COMPOSITECHECK | WC_DEFAULTCHAR, str, -1, multibyte_str.data(), multibyte_str.size(), NULL, NULL );
            multibyte_str.resize( std::strlen( multibyte_str.data() ) );
            multibyte_str.shrink_to_fit();
            return std::string( multibyte_str.begin(), multibyte_str.end() );
            #else
            out = std::string( str );
            return out;
            #endif
        }
    };
}

#endif // __FIND_DEVICES__