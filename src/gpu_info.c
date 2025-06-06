#include "include/hardware/gpu_info.h"

result_t* get_gpu_info() {
    gpu_info_t* info = malloc(sizeof(gpu_info_t));

    if (!info) {
        return result_error(-1, BENJI_ERROR_PACKET, "malloc() failed");
    }

    result_t* gpu_name_result = get_gpu_name();
    return_if_error(gpu_name_result);
    info->name = strdup((char*) result_unwrap_value(gpu_name_result));
    strtrim(info->name);

    result_t* gpu_vendor_result = get_gpu_vendor();
    return_if_error(gpu_vendor_result);
    info->vendor = strdup((char*) result_unwrap_value(gpu_vendor_result));
    strtrim(info->vendor);

    result_t* gpu_dedicated_video_memory_result = get_gpu_memory(BENJI_GPU_DEDICATED_VIDEO_MEMORY);
    return_if_error(gpu_dedicated_video_memory_result);
    info->dedicated_video_memory = *(double*) result_unwrap_value(gpu_dedicated_video_memory_result);

    result_t* gpu_dedicated_system_memory_result = get_gpu_memory(BENJI_GPU_DEDICATED_SYSTEM_MEMORY);
    return_if_error(gpu_dedicated_system_memory_result);
    info->dedicated_system_memory = *(double*) result_unwrap_value(gpu_dedicated_system_memory_result);

    result_t* gpu_shared_system_memory_result = get_gpu_memory(BENJI_GPU_SHARED_SYSTEM_MEMORY);
    return_if_error(gpu_shared_system_memory_result);
    info->shared_system_memory = *(double*) result_unwrap_value(gpu_shared_system_memory_result);

    return result_success(info);
}

result_t* get_gpu_name() {
    #if defined(_WIN32)
        result_t* description_result = get_gpu_description();
        return_if_error(description_result);

        DXGI_ADAPTER_DESC* adapter_description = (DXGI_ADAPTER_DESC*) result_unwrap_value(description_result);

        // make the assumption that the description value contains the name (it usually does)
        const char* name = wcharp_to_charp(adapter_description->Description);

        free(adapter_description);

        return result_success(name);
    #elif defined(__linux__)
        /* TODO: add linux stuff */
    #endif
}

result_t* get_gpu_vendor() {
    #if defined(_WIN32)
        char* vendor;

        result_t* description_result = get_gpu_description();
        return_if_error(description_result);

        DXGI_ADAPTER_DESC* adapter_description = (DXGI_ADAPTER_DESC*) result_unwrap_value(description_result);

        switch (adapter_description->VendorId) {
            case BENJI_GPU_VENDOR_NVIDIA: vendor = "NVIDIA"; break;
            case BENJI_GPU_VENDOR_AMD: vendor = "AMD"; break;
            case BENJI_GPU_VENDOR_INTEL: vendor = "Intel"; break;
            case BENJI_GPU_VENDOR_MICROSOFT: vendor = "Microsoft"; break;
            case BENJI_GPU_VENDOR_QUALCOMM: vendor = "Qualcomm"; break;
            case BENJI_GPU_VENDOR_ARM: vendor = "ARM"; break;
            case BENJI_GPU_VENDOR_APPLE: vendor = "Apple"; break;
            case BENJI_GPU_VENDOR_IMAGINATION: vendor = "Imagination Technologies"; break;
            case BENJI_GPU_VENDOR_S3: vendor = "S3 Graphics"; break;
            case BENJI_GPU_VENDOR_MATROX: vendor = "Matrox"; break;
            case BENJI_GPU_VENDOR_VIA: vendor = "VIA"; break;
            case BENJI_GPU_VENDOR_SIS: vendor = "SiS"; break;
            case BENJI_GPU_VENDOR_3DFX: vendor = "3Dfx"; break;
            case BENJI_GPU_VENDOR_TRIDENT: vendor = "Trident Microsystems"; break;
            case BENJI_GPU_VENDOR_XGI: vendor = "XGI Technology"; break;
            case BENJI_GPU_VENDOR_ASPEED: vendor = "ASPEED"; break;
            case BENJI_GPU_VENDOR_VMWARE: vendor = "VMware"; break;
            case BENJI_GPU_VENDOR_PARALLELS: vendor = "Parallels"; break;
            case BENJI_GPU_VENDOR_VIRTUALBOX: vendor = "VirtualBox"; break;
            case BENJI_GPU_VENDOR_BOCHS: vendor = "Bochs/QEMU"; break;
            case BENJI_GPU_VENDOR_REDHAT: vendor = "Red Hat (virtio)"; break;
            case BENJI_GPU_VENDOR_NUMBER_NINE: vendor = "Number Nine"; break;
            case BENJI_GPU_VENDOR_TSENG_LABS: vendor = "Tseng Labs"; break;
            case BENJI_GPU_VENDOR_RENDITION: vendor = "Rendition"; break;
            case BENJI_GPU_VENDOR_CIRRUS_LOGIC: vendor = "Cirrus Logic"; break;

            default: vendor = "???"; break;
        }

        free(adapter_description);

        return result_success(vendor);
    #elif defined(__linux__)
        /* TODO: add linux stuff */
    #endif
}

result_t* get_gpu_memory(gpu_memory_type_t memory_type) {
    #if defined(_WIN32)
        result_t* description_result = get_gpu_description();
        return_if_error(description_result);

        DXGI_ADAPTER_DESC* adapter_description = (DXGI_ADAPTER_DESC*) result_unwrap_value(description_result);

        void* memory_value = malloc(sizeof(double));

        if (!memory_value) {
            free(adapter_description);

            return result_error(-1, BENJI_ERROR_PACKET, "malloc() failed");
        }

        switch (memory_type) {
            case BENJI_GPU_DEDICATED_VIDEO_MEMORY: {
                *(double*) memory_value = bytes_to_gigabytes(adapter_description->DedicatedVideoMemory);
                break;
            }

            case BENJI_GPU_DEDICATED_SYSTEM_MEMORY: {
                *(double*) memory_value = bytes_to_gigabytes(adapter_description->DedicatedSystemMemory);
                break;
            }

            case BENJI_GPU_SHARED_SYSTEM_MEMORY: {
                *(double*) memory_value = bytes_to_gigabytes(adapter_description->SharedSystemMemory);
                break;
            }
        }

        free(adapter_description);

        return result_success(memory_value);
    #elif defined(__linux__)
        /* TODO: add linux stuff */
    #endif
}

#ifdef _WIN32
    result_t* get_gpu_description() {
        HRESULT hresult;

        hresult = CoInitializeEx(NULL, COINIT_MULTITHREADED);

        if (FAILED(hresult)) {
            return result_error(hresult, BENJI_ERROR_PACKET, "CoInitializeEx() failed");
        }

        IDXGIFactory* factory = NULL;

        hresult = CreateDXGIFactory(&IID_IDXGIFactory, (void**) &factory);

        if (FAILED(hresult)) {
            return result_error(hresult, BENJI_ERROR_PACKET, "CreateDXGIFactory() failed");
        }

        IDXGIAdapter* primary_adapter = NULL;

        hresult = factory->lpVtbl->EnumAdapters(factory, 0, &primary_adapter);

        if (FAILED(hresult)) {
            return result_error(hresult, BENJI_ERROR_PACKET, "EnumAdapters() failed");
        }

        DXGI_ADAPTER_DESC primary_adapter_description;

        hresult = primary_adapter->lpVtbl->GetDesc(primary_adapter, &primary_adapter_description);

        if (FAILED(hresult)) {
            return result_error(hresult, BENJI_ERROR_PACKET, "GetDesc() failed");
        }

        primary_adapter->lpVtbl->Release(primary_adapter);
        factory->lpVtbl->Release(factory);

        CoUninitialize();

        return result_success(memdup(&primary_adapter_description, sizeof(DXGI_ADAPTER_DESC)));
    }
#endif

result_t* gpu_info_to_map(gpu_info_t gpu_info) {
    map_t* gpu_info_map = map_init();

    char* buffer = malloc(BENJI_CAPACITY(BENJI_BASIC_STRING_LENGTH, char));

    if (!buffer) {
        return result_error(-1, BENJI_ERROR_PACKET, "malloc() failed");
    }

    buffer[0] = '\0';

    map_insert(gpu_info_map, "name", gpu_info.name);
    map_insert(gpu_info_map, "vendor", gpu_info.vendor);

    sprintf(buffer, "%0.3f", gpu_info.dedicated_video_memory);
    map_insert(gpu_info_map, "dedicated_video_memory", strdup(buffer));

    sprintf(buffer, "%0.3f", gpu_info.dedicated_system_memory);
    map_insert(gpu_info_map, "dedicated_system_memory", strdup(buffer));

    sprintf(buffer, "%0.3f", gpu_info.shared_system_memory);
    map_insert(gpu_info_map, "shared_system_memory", strdup(buffer));

    free(buffer);

    return result_success(gpu_info_map);
}

void free_gpu_info(gpu_info_t* info) {
    if (!info) {
        return;
    }

    free(info->name);
    info->name = NULL;

    free(info->vendor);
    info->vendor = NULL;

    free(info);
    info = NULL;
}