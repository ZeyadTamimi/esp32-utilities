/**
 * @file   utilities_esp.hpp
 *
 * @brief  ESP C++ utilities library.
 * @date   03/04/2019
 * @author Zeyad Tamimi (ZeyadTamimi@Outlook.com)
 * @copyright Copyright (c) 2019 Zeyad Tamimi. All rights reserved.
 *            This Source Code Form is subject to the terms of the Mozilla Public
 *            License, v. 2.0. If a copy of the MPL was not distributed with this
 *            file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef COMPONENTS_UTILITIES_UTILITIES_ESP_HPP
#define COMPONENTS_UTILITIES_UTILITIES_ESP_HPP

// Useful macro for logging the instance of an object that caused the log as well as the message.
#define INTANCE_LOG(LVL, TAG, VAR_STR, VAR, MSG, ...)\
    ESP_LOG_LEVEL(LVL, TAG, VAR_STR " " MSG, VAR, ##__VA_ARGS__)

#endif // COMPONENTS_UTILITIES_ESP_HPP

