/**
 * @file        service_mode.h
 * @brief       Provide mode switching service layer.
 * @author      Rakwireless
 * @version     0.0.0
 * @date        2021.6
 */

#ifndef __SERVICE_MODE_H__
#define __SERVICE_MODE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

typedef enum _SERVICE_MODE_TYPE
{
    SERVICE_MODE_TYPE_CLI,
    SERVICE_MODE_TYPE_PROTOCOL,
    SERVICE_MODE_TYPE_TRANSPARENT,
    SERVICE_MODE_TYPE_CUSTOM,
} SERVICE_MODE_TYPE;

#ifdef __cplusplus
}
#endif

#endif  // __SERVICE_MODE_H__

