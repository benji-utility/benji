#ifndef __BENJI_RAM_INFO_H
#define __BENJI_RAM_INFO_H

#define BENJI_USE_SYS_INFO_UTILS

#include "utils.h"

typedef struct _BENJI_RAM_INFO {
    double total_memory; // in GB
    double memory_load; // in GB
    double free_memory; // in GB
    uint16_t speed; // in MHz
} ram_info_t;

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

result_t* get_ram_info();

result_t* get_ram_total_memory();
result_t* get_ram_memory_load();
result_t* get_ram_free_memory();
result_t* get_ram_speed();

#ifdef _WIN32
    result_t* get_memory_status();
#endif

map_t* ram_info_to_map(ram_info_t ram_info);

#endif