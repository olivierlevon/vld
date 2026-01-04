#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include "ntapi.h"

//
// LdrLockLoaderLock Flags
//
#define LDR_LOCK_LOADER_LOCK_FLAG_DEFAULT           0x00000000
#define LDR_LOCK_LOADER_LOCK_FLAG_RAISE_ON_ERRORS   0x00000001
#define LDR_LOCK_LOADER_LOCK_FLAG_TRY_ONLY          0x00000002

//
// LdrUnlockLoaderLock Flags
//
#define LDR_UNLOCK_LOADER_LOCK_FLAG_DEFAULT         0x00000000
#define LDR_UNLOCK_LOADER_LOCK_FLAG_RAISE_ON_ERRORS 0x00000001

//
// LdrLockLoaderLock State
//
#define LDR_LOCK_LOADER_LOCK_STATE_INVALID                   0
#define LDR_LOCK_LOADER_LOCK_STATE_LOCK_ACQUIRED             1
#define LDR_LOCK_LOADER_LOCK_STATE_LOCK_NOT_ACQUIRED         2

class LoaderLock
{
public:
    LoaderLock()
        : m_cookie(0)
        , m_threadId(0)
        , m_locked(false)
    {
        if (LdrLockLoaderLock == NULL) {
            return;
        }

        ULONG state = 0;
        NTSTATUS status = LdrLockLoaderLock(LDR_LOCK_LOADER_LOCK_FLAG_DEFAULT, &state, &m_cookie);
        if (status == STATUS_SUCCESS && m_cookie != 0) {
            m_threadId = GetCurrentThreadId();
            m_locked = true;
        }
    }

    ~LoaderLock() {
        if (m_locked && m_cookie != 0 && LdrUnlockLoaderLock != NULL) {
            LdrUnlockLoaderLock(LDR_UNLOCK_LOADER_LOCK_FLAG_DEFAULT, m_cookie);
        }
    }

    // Check if the lock was successfully acquired
    bool IsLocked() const {
        return m_locked;
    }

    // Check if the lock is held by the current thread
    bool IsLockedByCurrentThread() const {
        return m_locked && (m_threadId == GetCurrentThreadId());
    }

private:
    // Non-copyable
    LoaderLock(const LoaderLock&) = delete;
    LoaderLock& operator=(const LoaderLock&) = delete;

    ULONG_PTR m_cookie;
    DWORD m_threadId;
    bool m_locked;
};
