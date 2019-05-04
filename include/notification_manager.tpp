/**
 * @file   notification_manager.tpp
 *
 * @brief  Utility class used for abstracting FreeRTOS task notifications and IPC.
 * @date   03/04/2019
 * @author Zeyad Tamimi (ZeyadTamimi@Outlook.com)
 * @copyright Copyright (c) 2019 Zeyad Tamimi. All rights reserved.
 *            This Source Code Form is subject to the terms of the Mozilla Public
 *            License, v. 2.0. If a copy of the MPL was not distributed with this
 *            file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef COMPONENTS_UTILITIES_NOTIFICATION_MANAGER_TPP
#define COMPONENTS_UTILITIES_NOTIFICATION_MANAGER_TPP

template<typename I, typename T>
Notification_Manager<I,T>::Notification_Manager(void)
{
    if (m_waiting_semaphore == nullptr)
        throw std::bad_alloc();

    xSemaphoreGive(m_waiting_semaphore);
}


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
template<typename I, typename T>
std::optional<uint32_t>
Notification_Manager<I,T>::wait(I id, T type, ProtectedFunction fn, TickType_t timeout)
{
    typename std::vector<notification_entry_t>::iterator entry_loc;
    {
        AnchorSemaphore anchor(m_waiting_semaphore);
        if (m_waiting_ids.count(id))
            return std::nullopt;

        notification_entry_t entry = {xTaskGetCurrentTaskHandle(), id, type};

        if (!m_waiting_types.count(type))
            m_waiting_types.insert(std::make_pair(type, std::vector<notification_entry_t>()));

        std::vector<notification_entry_t>& type_entries = m_waiting_types[type];
        entry_loc = std::find(type_entries.begin(), type_entries.end(), entry);
        if (entry_loc !=  type_entries.end())
            return std::nullopt;

        entry_loc = type_entries.insert(type_entries.end(), entry);
        m_waiting_ids.insert(std::make_pair(id, entry));
    }

    bool fn_status = fn();
    if (!fn_status)
        return std::nullopt;

    uint32_t ret;
    BaseType_t status = xTaskNotifyWait(0x00000000, ULONG_MAX, &ret, timeout);
    if (status == pdFALSE)
        return std::nullopt;

    AnchorSemaphore anchor(m_waiting_semaphore);
    m_waiting_ids.erase(id);
    m_waiting_types[type].erase(entry_loc);

    return ret;
}


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
template<typename I, typename T>
bool
Notification_Manager<I,T>::notify(I id, T type, uint32_t value)
{
    AnchorSemaphore anchor(m_waiting_semaphore);
    if (!m_waiting_ids.count(id))
        return false;

    if (m_waiting_ids[id].type != type)
        return false;

    xTaskNotify(m_waiting_ids[id].task, value, eSetValueWithOverwrite);
    return true;

};


/**
 * @brief Wakes up a task with all tasks waiting on a specific operation and supplies them with
 *        a value.
 * @param [in] type The type of operation that the waking task is signalling.
 * @param [in] value A uint32_t value which will be supplied all sleeping task. The sleeping
 *             tasks can use it or ignore it.
 * @return True if notify managed to wake up any task with the supplied type, false
 *         otherwise.
 */
template<typename I, typename T>
bool
Notification_Manager<I,T>::notify(T type, uint32_t value)
{
    AnchorSemaphore anchor(m_waiting_semaphore);
    if (!m_waiting_types.count(type))
        return false;

    for (auto waiting_type : m_waiting_types[type])
        xTaskNotify(waiting_type.task, value, eSetValueWithOverwrite);

    return true;
};

#endif // COMPONENTS_UTILITIES_NOTIFICATION_MANAGER_TPP

