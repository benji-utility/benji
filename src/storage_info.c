#include "include/hardware/storage_info.h"

result_t* get_storage_info() {
    storage_info_t* info = malloc(sizeof(storage_info_t));

    if (!info) {
        return result_error(-1, BENJI_ERROR_PACKET, "malloc() failed");
    }

    info->device_count = count_storage_devices();

    result_t* models_result = get_storage_devices_info(info->device_count, BENJI_STORAGE_DEVICE_MODEL_NAME);
    return_if_error(models_result);
    info->models = strdup((char*) result_unwrap_value(models_result));
    strtrim(info->models);

    result_t* serial_numbers_result = get_storage_devices_info(info->device_count, BENJI_STORAGE_DEVICE_SERIAL_NUMBER);
    return_if_error(serial_numbers_result);
    info->serial_numbers = strdup((char*) result_unwrap_value(serial_numbers_result));
    strtrim(info->serial_numbers);

    result_t* bus_types_result = get_storage_devices_info(info->device_count, BENJI_STORAGE_DEVICE_BUS_TYPE);
    return_if_error(bus_types_result);
    info->bus_types = strdup((char*) result_unwrap_value(bus_types_result));
    strtrim(info->bus_types);

    result_t* sizes_result = get_storage_devices_size(info->device_count);
    return_if_error(sizes_result);
    info->sizes = strdup((char*) result_unwrap_value(sizes_result));
    strtrim(info->sizes);

    return result_success(info);
}

result_t* get_storage_devices_info(size_t device_count, enum BENJI_STORAGE_DEVICE_MODEL_INFO_TYPE info_type) {
    #if defined(_WIN32)
        char* devices_info = malloc(BENJI_MAX_STORAGE_DEVICES * BENJI_BASIC_STRING_LENGTH);

        if (!devices_info) {
            return result_error(-1, BENJI_ERROR_PACKET, "malloc() failed");
        }

        devices_info[0] = '\0';

        for (int i = 0; i < device_count; i++) {
            HANDLE handle = open_storage_device_handle(i);

            unsigned char* buffer = NULL;

            result_t* storage_device_descriptor_result = get_storage_device_descriptor(handle, &buffer);

            if (storage_device_descriptor_result->is_error) {
                log_warning(result_unwrap_error(storage_device_descriptor_result));
                strcat(devices_info, "???");
            }
            else {
                STORAGE_DEVICE_DESCRIPTOR storage_device_descriptor = *(STORAGE_DEVICE_DESCRIPTOR*) result_unwrap_value(
                    storage_device_descriptor_result
                );

                // model name or serial number
                if (info_type != BENJI_STORAGE_DEVICE_BUS_TYPE) {
                    unsigned long offset;

                    switch (info_type) {
                        case BENJI_STORAGE_DEVICE_MODEL_NAME: {
                            offset = storage_device_descriptor.ProductIdOffset;
                            break;
                        }

                        case BENJI_STORAGE_DEVICE_SERIAL_NUMBER: {
                            offset = storage_device_descriptor.SerialNumberOffset;
                            break;
                        }
                    }

                    if (offset) {
                        strcat(devices_info, (char*) (buffer + offset));
                    }
                    else {
                        strcat(devices_info, "???");
                    }
                }
                else { // bus type
                    strcat(devices_info, get_bus_type(storage_device_descriptor.BusType));
                }
            }

            if (buffer) {
                free(buffer);
            }

            CloseHandle(handle);

            strcat(devices_info, ",");
        }

        size_t models_length = strlen(devices_info);

        // remove trailing comma
        if (models_length > 0 && devices_info[models_length - 1] == ',') {
            devices_info[models_length - 1] = '\0';
        }

        return result_success(devices_info);
    #elif defined(__linux__)
        /* TODO: add linux stuff */
    #endif
}

result_t* get_storage_devices_size(size_t device_count) {
    #if defined(_WIN32)
        char* sizes = malloc(BENJI_MAX_STORAGE_DEVICES * BENJI_BASIC_STRING_LENGTH);

        if (!sizes) {
            return result_error(-1, BENJI_ERROR_PACKET, "malloc() failed");
        }

        sizes[0] = '\0';

        for (int i = 0; i < device_count; i++) {
            HANDLE handle = open_storage_device_handle(i);

            DISK_GEOMETRY_EX storage_device_geometry = { 0 };

            unsigned long bytes_returned;

            boolean result = DeviceIoControl(
                handle, IOCTL_DISK_GET_DRIVE_GEOMETRY_EX,
                NULL, 0,
                &storage_device_geometry, sizeof(storage_device_geometry),
                &bytes_returned, NULL
            );

            if (result) {
                char* buffer = malloc(BENJI_CAPACITY(BENJI_BASIC_STRING_LENGTH, char));

                if (!buffer) {
                    return result_error(-1, BENJI_ERROR_PACKET, "malloc() failed");
                }

                buffer[0] = '\0';

                sprintf(buffer, "%0.3f", storage_device_geometry.DiskSize.QuadPart / (1024.0 * 1024.0 * 1024.0));
                strcat(sizes, buffer);

                free(buffer);
            }

            CloseHandle(handle);

            strcat(sizes, ",");
        }

        size_t sizes_length = strlen(sizes);

        if (sizes_length > 0 && sizes[sizes_length - 1] == ',') {
            sizes[sizes_length - 1] = '\0';
        }

        return result_success(sizes);
    #elif defined(__linux__)
        /* TODO: add linux stuff */
    #endif
}

#ifdef _WIN32
    HANDLE open_storage_device_handle(size_t device_index) {
        char device_path[BENJI_BASIC_STRING_LENGTH];

        sprintf(device_path, "\\\\.\\PhysicalDrive%d", device_index);

        return CreateFileA(
            device_path,
            GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL, OPEN_EXISTING, 0, NULL
        );
    }

    result_t* get_storage_device_descriptor(HANDLE handle, unsigned char** buffer) {
        STORAGE_PROPERTY_QUERY query = {
            .PropertyId = StorageDeviceProperty,
            .QueryType = PropertyStandardQuery
        };

        STORAGE_DEVICE_DESCRIPTOR storage_device_descriptor = { 0 };
        unsigned long bytes_returned = 0;

        boolean result = DeviceIoControl(
            handle, IOCTL_STORAGE_QUERY_PROPERTY,
            &query, sizeof(query),
            &storage_device_descriptor, sizeof(storage_device_descriptor),
            &bytes_returned, NULL
        );

        if (!result) {
            return result_error(GetLastError(), BENJI_ERROR_PACKET, "DeviceIoControl() failed");
        }

        size_t required_size = storage_device_descriptor.Size;

        if (required_size == 0) {
            required_size = BENJI_DEFAULT_STORAGE_DEVICE_DESCRIPTOR_SIZE;
        }

        *buffer = malloc(required_size);

        if (!(*buffer)) {
            return result_error(-1, BENJI_ERROR_PACKET, "malloc() failed");
        }

        result = DeviceIoControl(
            handle, IOCTL_STORAGE_QUERY_PROPERTY,
            &query, sizeof(query),
            *buffer, required_size,
            &bytes_returned, NULL
        );

        if (!result) {
            free(*buffer);

            return result_error(GetLastError(), BENJI_ERROR_PACKET, "DeviceIoControl() failed");
        }

        return result_success((STORAGE_DEVICE_DESCRIPTOR*) *buffer);
    }

    // largely copied from the "winioctl.h" header, hence the jank win32 macro usage
    const char* get_bus_type(STORAGE_BUS_TYPE bus_type) {
        switch (bus_type) {
            case BusTypeScsi: return "SCSI";
            case BusTypeAtapi: return "ATAPI";
            case BusTypeAta: return "ATA";
            case BusType1394: return "1394";
            case BusTypeSsa: return "SSA";
            case BusTypeFibre: return "Fibre";
            case BusTypeUsb: return "USB";
            case BusTypeRAID: return "RAID";

            #if (_WIN32_WINNT >= 0x0600)
                case BusTypeiScsi: return "iSCSI";
                case BusTypeSas: return "SAS";
                case BusTypeSata: return "SATA";
                case BusTypeSd: return "SD";
                case BusTypeMmc: return "MMC";
            #endif

            #if (_WIN32_WINNT >= 0x0601)
                case BusTypeVirtual: return "Virtual";
                case BusTypeFileBackedVirtual: return "File-Backed Virtual";
                case BusTypeSpaces: return "Spaces";
                case BusTypeNvme: return "NVMe";
                case BusTypeSCM: return "SCM";
                case BusTypeUfs: return "UFS";
            #endif

            default: return "???";
        }
    }
#endif

size_t count_storage_devices() {
    size_t device_count = 0;

    for (size_t i = 0; i < BENJI_MAX_STORAGE_DEVICES; i++) {
        HANDLE handle = open_storage_device_handle(i);

        if (handle == INVALID_HANDLE_VALUE) {
            continue;
        }

        device_count++;

        CloseHandle(handle);
    }

    return device_count;
}

result_t* storage_info_to_map(storage_info_t storage_info) {
    map_t* storage_info_map = map_init();

    char* buffer = malloc(BENJI_CAPACITY(BENJI_BASIC_STRING_LENGTH, char));

    if (!buffer) {
        return result_error(-1, BENJI_ERROR_PACKET, "malloc() failed");
    }

    buffer[0] = '\0';

    sprintf(buffer, "%lu", storage_info.device_count);
    map_insert(storage_info_map, "device_count", buffer);

    map_insert(storage_info_map, "models", storage_info.models);
    map_insert(storage_info_map, "serial_numbers", storage_info.serial_numbers);
    map_insert(storage_info_map, "bus_types", storage_info.bus_types);
    map_insert(storage_info_map, "sizes", storage_info.sizes);

    free(buffer);

    return result_success(storage_info_map);
}