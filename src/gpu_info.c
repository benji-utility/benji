#include "include/hardware/gpu_info.h"

result_t* get_gpu_info() {
    gpu_info_t* info = malloc(sizeof(gpu_info_t));

    result_t* gpu_name_result = get_gpu_name();
    return_if_error(gpu_name_result);
    info->name = strdup((char*) result_unwrap_value(gpu_name_result));
    strtrim(info->name);

    result_t* gpu_vendor_result = get_gpu_vendor();
    return_if_error(gpu_vendor_result);
    info->vendor = strdup((char*) result_unwrap_value(gpu_vendor_result));
    strtrim(info->vendor);

    result_t* gpu_dedicated_video_memory_result = get_gpu_dedicated_video_memory();
    return_if_error(gpu_dedicated_video_memory_result);
    info->dedicated_video_memory = *(double*) result_unwrap_value(gpu_dedicated_video_memory_result);

    result_t* gpu_dedicated_system_memory_result = get_gpu_dedicated_system_memory();
    return_if_error(gpu_dedicated_system_memory_result);
    info->dedicated_system_memory = *(double*) result_unwrap_value(gpu_dedicated_system_memory_result);

    result_t* gpu_shared_system_memory_result = get_gpu_shared_system_memory();
    return_if_error(gpu_shared_system_memory_result);
    info->shared_system_memory = *(double*) result_unwrap_value(gpu_shared_system_memory_result);

    return result_success(info);
}

result_t* get_gpu_name() {
    #if defined(_WIN32)
        DISPLAY_DEVICEW device;
        device.cb = sizeof(DISPLAY_DEVICEW);

        int i = 0;
        while (true) {
            if (!EnumDisplayDevicesW(NULL, i, &device, 0)) {
                return result_error(GetLastError(), "EnumDisplayDevicesW() failed", BENJI_ERROR_PACKET);
            }

            // get the primary GPU
            if (device.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE) {
                return result_success(wcharp_to_charp(device.DeviceString));
            }

            i++;
        }

        return result_error(-1, "no primary GPU found", BENJI_ERROR_PACKET);
    #elif defined(__linux__)
        /* TODO: add linux stuff */
    #endif
}

result_t* get_gpu_vendor() {
    #if defined(_WIN32)
        char* vendor;

        result_t* description_result = get_gpu_description();
        return_if_error(description_result);

        DXGI_ADAPTER_DESC description = *(DXGI_ADAPTER_DESC*) result_unwrap_value(description_result);

        switch (description.VendorId) {
            case BENJI_GPU_VENDOR_INTEL: vendor = "Intel"; break;
            case BENJI_GPU_VENDOR_AMD: vendor = "AMD"; break;
            case BENJI_GPU_VENDOR_NVIDIA: vendor = "NVIDIA"; break;

            default: vendor = "???"; break;
        }

        return result_success(vendor);
    #elif defined(__linux__)
        /* TODO: add linux stuff */
    #endif
}

result_t* get_gpu_dedicated_video_memory() {
    #if defined(_WIN32)
        result_t* description_result = get_gpu_description();
        return_if_error(description_result);

        DXGI_ADAPTER_DESC description = *(DXGI_ADAPTER_DESC*) result_unwrap_value(description_result);

        void* memory = malloc(sizeof(double));

        if (memory) {
            *(double*) memory = description.DedicatedVideoMemory / (1024.0 * 1024.0 * 1024.0);
        }
        else {
            return result_error(-1, "malloc() failed", BENJI_ERROR_PACKET);
        }

        return result_success(memory);
    #elif defined(__linux__)
        /* TODO: add linux stuff */
    #endif
}

result_t* get_gpu_dedicated_system_memory() {
    #if defined(_WIN32)
        result_t* description_result = get_gpu_description();
        return_if_error(description_result);

        DXGI_ADAPTER_DESC description = *(DXGI_ADAPTER_DESC*) result_unwrap_value(description_result);

        void* memory = malloc(sizeof(double));

        if (memory) {
            *(double*) memory = description.DedicatedSystemMemory / (1024.0 * 1024.0 * 1024.0);
        }
        else {
            return result_error(-1, "malloc() failed", BENJI_ERROR_PACKET);
        }

        return result_success(memory);
    #elif defined(__linux__)
        /* TODO: add linux stuff */
    #endif
}

result_t* get_gpu_shared_system_memory() {
    #if defined(_WIN32)
        result_t* description_result = get_gpu_description();
        return_if_error(description_result);

        DXGI_ADAPTER_DESC description = *(DXGI_ADAPTER_DESC*) result_unwrap_value(description_result);

        void* memory = malloc(sizeof(double));

        if (memory) {
            *(double*) memory = description.SharedSystemMemory / (1024.0 * 1024.0 * 1024.0);
        }
        else {
            return result_error(-1, "malloc() failed", BENJI_ERROR_PACKET);
        }

        return result_success(memory);
    #elif defined(__linux__)
        /* TODO: add linux stuff */
    #endif
}

#ifdef _WIN32
    result_t* get_gpu_description() {
        HRESULT hresult = CoInitializeEx(NULL, COINIT_MULTITHREADED);

        if (FAILED(hresult)) {
            return result_error(hresult, "CoInitializeEx() failed", BENJI_ERROR_PACKET);
        }

        IDXGIFactory* factory = NULL;
        hresult = CreateDXGIFactory(&IID_IDXGIFactory, (void**) &factory);

        if (FAILED(hresult)) {
            return result_error(hresult, "CreateDXGIFactory() failed", BENJI_ERROR_PACKET);
        }

        IDXGIAdapter* primary_adapter = NULL;
        DXGI_ADAPTER_DESC* primary_adapter_description = malloc(sizeof(DXGI_ADAPTER_DESC));

        if (!primary_adapter_description) {
            return result_error(-1, "malloc() failed", BENJI_ERROR_PACKET);
        }

        IDXGIAdapter* adapter = NULL;
        UINT index = 0;
        while (factory->lpVtbl->EnumAdapters(factory, index, &adapter) != DXGI_ERROR_NOT_FOUND) {
            IDXGIOutput* output = NULL;

            if ((hresult = adapter->lpVtbl->EnumOutputs(adapter, 0, &output)) == S_OK) {
                DXGI_OUTPUT_DESC output_description;
                hresult = output->lpVtbl->GetDesc(output, &output_description);

                if (SUCCEEDED(hresult) && output_description.AttachedToDesktop) {
                    primary_adapter = adapter;
                    primary_adapter->lpVtbl->AddRef(primary_adapter);

                    output->lpVtbl->Release(output);

                    break;
                }
                else {
                    return result_error(hresult, "GetDesc() failed", BENJI_ERROR_PACKET);
                }

                output->lpVtbl->Release(output);
            }
            else {
                return result_error(hresult, "EnumOutputs() failed", BENJI_ERROR_PACKET);
            }

            adapter->lpVtbl->Release(adapter);

            index++;
        }

        hresult = primary_adapter->lpVtbl->GetDesc(primary_adapter, primary_adapter_description);

        if (primary_adapter == NULL) {
            return result_error(hresult, "GetDesc() failed", BENJI_ERROR_PACKET);
        }

        primary_adapter->lpVtbl->Release(primary_adapter);
        factory->lpVtbl->Release(factory);

        CoUninitialize();

        return result_success((void*) primary_adapter_description);
    }
#endif

result_t* gpu_info_to_map(gpu_info_t gpu_info) {
    map_t* gpu_info_map = map_init();

    char* buffer = malloc(BENJI_CAPACITY(BENJI_BASIC_STRING_LENGTH, char));

    if (!buffer) {
        return result_error(-1, "malloc() failed", BENJI_ERROR_PACKET);
    }

    buffer[0] = '\0';

    map_insert(gpu_info_map, "name", gpu_info.name);
    map_insert(gpu_info_map, "vendor", gpu_info.vendor);

    sprintf(buffer, "%0.3f", gpu_info.dedicated_video_memory);
    map_insert(gpu_info_map, "dedicated_video_memory", buffer);

    sprintf(buffer, "%0.3f", gpu_info.dedicated_system_memory);
    map_insert(gpu_info_map, "dedicated_system_memory", buffer);

    sprintf(buffer, "%0.3f", gpu_info.shared_system_memory);
    map_insert(gpu_info_map, "shared_system_memory", buffer);

    free(buffer);

    return result_success(gpu_info_map);
}