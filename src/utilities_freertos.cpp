/**
 * @file   utilities_freertos.cpp
 *
 * @brief  FreeRTOS C++ utilities library.
 * @date   03/04/2019
 * @author Zeyad Tamimi (ZeyadTamimi@Outlook.com)
 * @copyright Copyright (c) 2019 Zeyad Tamimi. All rights reserved.
 *            This Source Code Form is subject to the terms of the Mozilla Public
 *            License, v. 2.0. If a copy of the MPL was not distributed with this
 *            file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <stdexcept>

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include "utilities_freertos.hpp"


namespace Utilities
{

AnchorSemaphore::AnchorSemaphore(SemaphoreHandle_t sem, TickType_t timeout) : m_sem(sem)
{
    if (xSemaphoreTake(m_sem, timeout) != pdTRUE)
        throw std::runtime_error("Failed to grab semaphore");
}

AnchorSemaphore::~AnchorSemaphore(void)
{
    xSemaphoreGive(m_sem);
}

};

