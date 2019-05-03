/**
 * @file   utilities_freertos.hpp
 *
 * @brief  FreeRTOS C++ utilities library.
 * @date   03/04/2019
 * @author Zeyad Tamimi (ZeyadTamimi@Outlook.com)
 * @copyright Copyright (c) 2019 Zeyad Tamimi. All rights reserved.
 *            This Source Code Form is subject to the terms of the Mozilla Public
 *            License, v. 2.0. If a copy of the MPL was not distributed with this
 *            file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef COMPONENTS_UTILITIES_UTILITIES_FREERTOS_HPP
#define COMPONENTS_UTILITIES_UTILITIES_FREERTOS_HPP

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"


namespace Utilities
{

/**
 * @brief RAII wrapper for FreRTOS semaphores.
 * @detail This class attempts to take the semaphore upon construction and automatically releases it
 *         upon destruction.
 */
class AnchorSemaphore
{
public:
    AnchorSemaphore(SemaphoreHandle_t sem, TickType_t timeout=portMAX_DELAY);
    ~AnchorSemaphore(void);

private:
    SemaphoreHandle_t   m_sem;
};

};

#endif // COMPONENTS_UTILITIES_UTILITIES_FREERTOS_HPP

