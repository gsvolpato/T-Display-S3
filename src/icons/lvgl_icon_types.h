#ifndef LVGL_ICON_TYPES_H
#define LVGL_ICON_TYPES_H

#ifdef __has_include
    #if __has_include("lvgl.h")
        #ifndef LV_LVGL_H_INCLUDE_SIMPLE
            #define LV_LVGL_H_INCLUDE_SIMPLE
        #endif
    #endif
#endif

#if defined(LV_LVGL_H_INCLUDE_SIMPLE)
    #include "lvgl.h"
#else
    #include "lvgl/lvgl.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

// Base memory alignment attribute
#ifndef LV_ATTRIBUTE_MEM_ALIGN
#define LV_ATTRIBUTE_MEM_ALIGN
#endif

#ifndef LV_ATTRIBUTE_LARGE_CONST
#define LV_ATTRIBUTE_LARGE_CONST
#endif

// Individual icon attributes
#ifndef LV_ATTRIBUTE_IMAGE_WIFI
#define LV_ATTRIBUTE_IMAGE_WIFI
#endif

#ifndef LV_ATTRIBUTE_IMAGE_BLUETOOTH 
#define LV_ATTRIBUTE_IMAGE_BLUETOOTH
#endif

#ifndef LV_ATTRIBUTE_IMAGE_FOLDER
#define LV_ATTRIBUTE_IMAGE_FOLDER
#endif

#ifndef LV_ATTRIBUTE_IMAGE_TV
#define LV_ATTRIBUTE_IMAGE_TV
#endif

#ifndef LV_ATTRIBUTE_IMAGE_SUN
#define LV_ATTRIBUTE_IMAGE_SUN
#endif

#ifndef LV_ATTRIBUTE_IMAGE_POWER
#define LV_ATTRIBUTE_IMAGE_POWER
#endif

#ifndef LV_ATTRIBUTE_IMAGE_RADIO
#define LV_ATTRIBUTE_IMAGE_RADIO
#endif

#ifndef LV_ATTRIBUTE_IMAGE_GITHUB
#define LV_ATTRIBUTE_IMAGE_GITHUB
#endif

#ifndef LV_ATTRIBUTE_IMAGE_SERVER
#define LV_ATTRIBUTE_IMAGE_SERVER
#endif

#ifndef LV_ATTRIBUTE_IMAGE_SETTINGS
#define LV_ATTRIBUTE_IMAGE_SETTINGS
#endif

#ifdef __cplusplus
}
#endif

#endif // LVGL_ICON_TYPES_H