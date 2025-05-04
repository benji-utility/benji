#include "include/hardware/storage_info.h"

result_t* get_storage_info() {
    storage_info_t* info = malloc(sizeof(storage_info_t));

    if (!info) {
        return result_error(-1, "malloc() failed", BENJI_ERROR_PACKET);
    }

    size_t device_count = 0;

    result_t* models_result = get_storage_models(&device_count);
    return_if_error(models_result);
    info->models = strdup((char*) result_unwrap_value(models_result));
    strtrim(info->models);

    info->device_count = device_count;

    info->serial_numbers = "";
    info->bus_types = "";
    info->sizes = "";

    return result_success(info);
}

result_t* get_storage_models(size_t* device_count) {
    #if defined(_WIN32)
        char* models = malloc(BENJI_MAX_STORAGE_DEVICES * BENJI_BASIC_STRING_LENGTH);

        if (!models) {
            return result_error(-1, "malloc() failed", BENJI_ERROR_PACKET);
        }

        models[0] = '\0';

        for (*device_count; *device_count < BENJI_MAX_STORAGE_DEVICES; (*device_count)++) {
            HANDLE handle = open_storage_device_handle(*device_count);

            if (handle == INVALID_HANDLE_VALUE) {
                continue;
            }

            unsigned char* buffer = NULL;

            result_t* storage_device_descriptor_result = get_storage_device_descriptor(handle, &buffer);

            if (storage_device_descriptor_result->is_error) {
                log_warning(result_unwrap_error(storage_device_descriptor_result));
                strcat(models, "???");
            }
            else {
                STORAGE_DEVICE_DESCRIPTOR storage_device_descriptor = *(STORAGE_DEVICE_DESCRIPTOR*) result_unwrap_value(
                    storage_device_descriptor_result
                );

                strcat(models, (char*) (buffer + storage_device_descriptor.ProductIdOffset));
            }

            if (buffer) {
                free(buffer);
            }

            CloseHandle(handle);

            strcat(models, ",");
        }

        size_t models_length = strlen(models);

        // remove trailing comma
        if (models_length > 0 && models[models_length - 1] == ',') {
            models[models_length - 1] = '\0';
        }

        return result_success(models);
    #elif defined(__linux__)
        /* TODO: add linux stuff */
    #endif
}

result_t* get_storage_serial_numbers();

result_t* get_storage_bus_types();

result_t* get_storage_sizes();

#ifdef _WIN32
    HANDLE open_storage_device_handle(size_t device_index) {
        char device_path[BENJI_BASIC_STRING_LENGTH];

        sprintf(device_path, "\\\\.\\PhysicalDrive%d", device_index);

        return CreateFileA(device_path, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
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
            return result_error(GetLastError(), "DeviceIoControl() failed", BENJI_ERROR_PACKET);
        }

        size_t required_size = storage_device_descriptor.Size;

        if (required_size == 0) {
            required_size = BENJI_DEFAULT_STORAGE_DEVICE_DESCRIPTOR_SIZE;
        }

        *buffer = malloc(required_size);

        if (!(*buffer)) {
            return result_error(-1, "malloc() failed", BENJI_ERROR_PACKET);
        }

        result = DeviceIoControl(
            handle, IOCTL_STORAGE_QUERY_PROPERTY,
            &query, sizeof(query),
            *buffer, required_size,
            &bytes_returned, NULL
        );

        if (!result) {
            free(*buffer);

            return result_error(GetLastError(), "DeviceIoControl() failed", BENJI_ERROR_PACKET);
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
                case BusTypeNvme: return "NVME";
                case BusTypeSCM: return "SCM";
                case BusTypeUfs: return "UFS";
            #endif

            default: return "???";
        }
    }
#endif

result_t* storage_info_to_map(storage_info_t storage_info) {
    map_t* storage_info_map = map_init();

    char* buffer = malloc(BENJI_CAPACITY(BENJI_BASIC_STRING_LENGTH, char));

    if (!buffer) {
        return result_error(-1, "malloc() failed", BENJI_ERROR_PACKET);
    }

    buffer[0] = '\0';

    map_insert(storage_info_map, "models", storage_info.models);
    map_insert(storage_info_map, "serial_numbers", storage_info.serial_numbers);
    map_insert(storage_info_map, "bus_types", storage_info.bus_types);
    map_insert(storage_info_map, "sizes", storage_info.sizes);

    return result_success(storage_info_map);
}