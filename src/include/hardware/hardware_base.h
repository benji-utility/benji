#ifndef __BENJI_HARDWARE_BASE_H
#define __BENJI_HARDWARE_BASE_H

#ifndef BENJI_START_HARDWARE_STRUCT
    #define BENJI_START_HARDWARE_STRUCT(_hardware_upper) \
        typedef struct _BENJI_##_hardware_upper##_INFO {
#endif

#ifndef BENJI_END_HARDWARE_STRUCT
    #define BENJI_END_HARDWARE_STRUCT(_hardware_lower) \
        } _hardware_lower##_info_t;
#endif

#ifndef BENJI_CREATE_HARDWARE_BASE
    #define BENJI_CREATE_HARDWARE_BASE(_hardware) \
        result_t* get_##_hardware##_info(); \
        result_t* _hardware##_info_to_map(_hardware##_info_t info); \
        void free_##_hardware##_info(_hardware##_info_t* info);
#endif

#ifndef BENJI_CREATE_HARDWARE_GETTER_IMPL
    #define BENJI_CREATE_HARDWARE_GETTER_IMPL(_hardware, _field) \
        result_t* get_##_hardware##_##_field();
#endif

#endif