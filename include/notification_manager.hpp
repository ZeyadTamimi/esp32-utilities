/**
 * @file   notification_manager.hpp
 *
 * @brief  Utility class used for abstracting FreeRTOS task notifications and IPC.
 * @date   03/04/2019
 * @author Zeyad Tamimi (ZeyadTamimi@Outlook.com)
 * @copyright Copyright (c) 2019 Zeyad Tamimi. All rights reserved.
 *            This Source Code Form is subject to the terms of the Mozilla Public
 *            License, v. 2.0. If a copy of the MPL was not distributed with this
 *            file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef COMPONENTS_UTILITIES_NOTIFICATION_MANAGER_HPP
#define COMPONENTS_UTILITIES_NOTIFICATION_MANAGER_HPP

#include <algorithm>
#include <cstdint>
#include <functional>
#include <optional>
#include <unordered_map>
#include <utility>
#include <vector>

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"


namespace Utilities
{

/**
 * @brief A manager for FreeRTOS task notification and inter task communication.
 * @detail This class allows tasks to suspend themselves in a "wait channel" like structure until
 *         another task wakes them up either by specifying their exact ID and operation
 *         they are waiting on, or by waking up all threads that are waiting on a specific
 *         operation. It also specifies a mechanism for the waking task to supply a value to all
 *         other sleeping tasks.
 * @note Waking task means the task that notifies other tasks. Sleeping task means the tasks that
 * @note Usage of this class will cause the FreeRTOS notification value to be clobbered.
 * sleep and wait from a notification from waking tasks.
 * @tparam I The type of the ID that the tasks will use to address each other in the manager.
 * @tparam T The type of the operations that the threads will use to wait on.
 */
template<typename I, typename T>
class Notification_Manager
{
public:
    using ProtectedFunction = std::function<bool()>;


    Notification_Manager(void);

    /**
     * @brief Places the current task in a wait state until a call to notify is made.
     * @param [in] id The ID of the current task which will be placed in the waiting queue.
     * @param [in] type The type of operation that this task is waiting on.
     * @param [in] fn A protected function that will be executed after the task is placed in the
     *             queue but before it actually sleeps. This is useful if the protected function is
     *             going to trigger the waking task to wake us.
     * @param [in] timeout (default=portMAX_DELAY) The timeout for the wait operation.
     * @return The uint32_t value supplied by the waking task or std::nullopt if the operation timed
     *         out.
     */
    std::optional<uint32_t> wait(I id, T type, ProtectedFunction fn,
                                 TickType_t timeout=portMAX_DELAY);

    /**
     * @brief Wakes up a task with a specific ID and which is waiting on a specific operation and
     *        supplies a value to the sleeping task.
     * @param [in] id The ID of the task to wake.
     * @param [in] type The type of operation that the waking tasking is signalling to the sleeping
     *             task.
     * @param [in] value A uint32_t value which will be supplied to the sleeping task. The sleeping
     *             task can use it or ignore it.
     * @return True if notify managed to wake up a task with the supplied ID and type, false
     *         otherwise.
     */
    bool notify(I id, T type, uint32_t value);

    /**
     * @brief Wakes up a task with all tasks waiting on a specific operation and supplies them with
     *        a value.
     * @param [in] type The type of operation that the waking task is signalling.
     * @param [in] value A uint32_t value which will be supplied all sleeping task. The sleeping
     *             tasks can use it or ignore it.
     * @return True if notify managed to wake up any task with the supplied type, false
     *         otherwise.
     */
    bool notify(T type, uint32_t value);

private:
    struct notification_entry_t
    {
        TaskHandle_t task;
        I id;
        T type;

        bool operator==(const notification_entry_t& lhs) { return (lhs.task == task &&
                                                                   id == lhs.id &&
                                                                   type == lhs.type); }
    };

    using Waiting_ID_Map = std::unordered_map<I, notification_entry_t>;
    using Waiting_Type_Map = std::unordered_map<T, std::vector<notification_entry_t>>;


    Waiting_ID_Map      m_waiting_ids;
    Waiting_Type_Map    m_waiting_types;

    SemaphoreHandle_t   m_waiting_semaphore = xSemaphoreCreateBinary();
};

#include "notification_manager.tpp"

};

#endif // COMPONENTS_UTILITIES_NOTIFICATION_MANAGER_HPP

