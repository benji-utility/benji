#include "include/telemetry/gpu_telemetry.h"

result_t* get_gpu_info() {
    gpu_info_t* gpu_info = malloc(sizeof(gpu_info_t));

    if (!gpu_info) {
        return result_error(-1, BENJI_ERROR_PACKET, "malloc() failed");
    }

    get_telemetry_info_string(
        gpu_info,
        gpu_info->name,
        get_gpu_name,
        free_gpu_info
    );

    get_telemetry_info_string(
        gpu_info,
        gpu_info->vendor,
        get_gpu_vendor,
        free_gpu_info
    );

    #ifdef _WIN32
        get_telemetry_info_double(
            gpu_info,
            gpu_info->dedicated_video_memory,
            get_gpu_memory,
            free_gpu_info,
            BENJI_GPU_WIN32_DEDICATED_VIDEO_MEMORY
        );

        get_telemetry_info_double(
            gpu_info,
            gpu_info->dedicated_system_memory,
            get_gpu_memory,
            free_gpu_info,
            BENJI_GPU_WIN32_DEDICATED_SYSTEM_MEMORY
        );

        get_telemetry_info_double(
            gpu_info,
            gpu_info->shared_system_memory,
            get_gpu_memory,
            free_gpu_info,
            BENJI_GPU_WIN32_SHARED_SYSTEM_MEMORY
        );
    #endif

    return result_success(gpu_info);
}

result_t* get_gpu_name() {
    #if defined(_WIN32)
        return _get_gpu_name_windows();
    #elif defined(__linux__)
        return _get_gpu_name_linux();
    #endif
}

result_t* get_gpu_vendor() {
    #if defined(_WIN32)
        return _get_gpu_vendor_windows();
    #elif defined(__linux__)
        return _get_gpu_vendor_linux();
    #endif
}

result_t* get_gpu_memory(gpu_memory_type_t memory_type) {
    #ifdef _WIN32
        result_t* description_result = _get_gpu_description();
        return_if_error(description_result);

        DXGI_ADAPTER_DESC* adapter_description = (DXGI_ADAPTER_DESC*) result_unwrap_value(description_result);
    #endif

    void* memory_value = malloc(sizeof(double));

    if (!memory_value) {
        #ifdef _WIN32
            free(adapter_description);
        #endif

        return result_error(-1, BENJI_ERROR_PACKET, "malloc() failed");
    }

    switch (memory_type) {
        #if defined(_WIN32)
            case BENJI_GPU_WIN32_DEDICATED_VIDEO_MEMORY: {
                *(double*) memory_value = bytes_to_gigabytes(adapter_description->DedicatedVideoMemory);
                break;
            }

            case BENJI_GPU_WIN32_DEDICATED_SYSTEM_MEMORY: {
                *(double*) memory_value = bytes_to_gigabytes(adapter_description->DedicatedSystemMemory);
                break;
            }

            case BENJI_GPU_WIN32_SHARED_SYSTEM_MEMORY: {
                *(double*) memory_value = bytes_to_gigabytes(adapter_description->SharedSystemMemory);
                break;
            }
        #endif
    }

    #ifdef _WIN32
        free(adapter_description);
    #endif

    return result_success(memory_value);
}

#if defined(_WIN32)
    result_t* _get_gpu_name_windows() {
        result_t* description_result = _get_gpu_description();
        return_if_error(description_result);

        DXGI_ADAPTER_DESC* adapter_description = (DXGI_ADAPTER_DESC*) result_unwrap_value(description_result);

        // make the assumption that the description value contains the name (it usually does)
        const char* name = wcharp_to_charp(adapter_description->Description);

        free(adapter_description);

        return result_success(name);
    }

    result_t* _get_gpu_vendor_windows() {
        char* vendor;

        result_t* description_result = _get_gpu_description();
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
    }

    result_t* _get_gpu_description() {
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
#elif defined(__linux__)
    result_t* _get_gpu_name_linux() {
        // TODO: add linux stuff
    }

    result_t* _get_gpu_vendor_linux() {
        // TODO: add linux stuff
    }
#endif

result_t* gpu_info_to_map(const gpu_info_t gpu_info) {
    map_t* gpu_info_map = map_init();

    char* buffer = malloc(BENJI_CAPACITY(BENJI_BASIC_STRING_LENGTH, char));

    if (!buffer) {
        return result_error(-1, BENJI_ERROR_PACKET, "malloc() failed");
    }

    buffer[0] = '\0';

    map_insert(gpu_info_map, "name", gpu_info.name);
    map_insert(gpu_info_map, "vendor", gpu_info.vendor);

    #ifdef _WIN32
        sprintf(buffer, "%0.3f", gpu_info.dedicated_video_memory);
        map_insert(gpu_info_map, "dedicated_video_memory", strdup(buffer));

        sprintf(buffer, "%0.3f", gpu_info.dedicated_system_memory);
        map_insert(gpu_info_map, "dedicated_system_memory", strdup(buffer));

        sprintf(buffer, "%0.3f", gpu_info.shared_system_memory);
        map_insert(gpu_info_map, "shared_system_memory", strdup(buffer));
    #endif

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