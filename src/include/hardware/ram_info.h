#ifndef __BENJI_RAM_INFO_H
#define __BENJI_RAM_INFO_H

#ifndef BENJI_USE_SYS_INFO_UTILS
    #define BENJI_USE_SYS_INFO_UTILS
#endif

#include "../utils.h"

#include "hardware_base.h"

#ifndef BENJI_SMBIOS_MEMORY_DEVICE_TYPE
    #define BENJI_SMBIOS_MEMORY_DEVICE_TYPE (17)
#endif

#ifndef BENJI_SMBIOS_SPEED_OFFSET
    #define BENJI_SMBIOS_SPEED_OFFSET (0x15)
#endif

#ifndef BENJI_SYSTEM_FIRMWARE_TABLE
    #define BENJI_SYSTEM_FIRMWARE_TABLE 'RSMB'
#endif

#ifndef BENJI_RAM_FIELDS
    #define BENJI_RAM_FIELDS(_field_getter_impl) \
        _field_getter_impl(ram, total_memory) \
        _field_getter_impl(ram, memory_load) \
        _field_getter_impl(ram, free_memory) \
        _field_getter_impl(ram, speed)
#endif

BENJI_START_HARDWARE_STRUCT(RAM)
    double total_memory; // in GB
    double memory_load; // in GB
    double free_memory; // in GB
    uint16_t speed; // in MHz
BENJI_END_HARDWARE_STRUCT(ram)

#ifdef _WIN32
    #pragma pack(push, 1)
        typedef struct _SMBIOS_MEMORY_DEVICE {
            uint8_t type;
            uint8_t length;
            uint16_t handle;

            uint16_t physical_memory_array_handle;
            uint16_t error_information_handle;

            uint16_t total_width;
            uint16_t data_width;
            uint16_t size;

            uint8_t form_factor;
            uint8_t device_set;

            char device_locator;
            char bank_locator;

            uint8_t  memory_type;
            uint16_t type_detail;

            uint16_t speed;
        } SMBIOS_MEMORY_DEVICE;

        typedef struct _RAW_SMBIOS_DATA {
            uint32_t signature;
            uint32_t length;
            uint8_t data[1];
        } RAW_SMBIOS_DATA;
    #pragma pack(pop)
#endif

BENJI_CREATE_HARDWARE_BASE(ram)

BENJI_RAM_FIELDS(BENJI_CREATE_HARDWARE_GETTER_IMPL)

#ifdef _WIN32
    result_t* get_memory_status();
#endif

#endif