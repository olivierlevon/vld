#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

// CriticalSection - Wrapper for Windows CRITICAL_SECTION
//
// IMPORTANT: For global/static objects, you MUST call Initialize() explicitly
// before use and Delete() before program exit. This is because
// InitializeCriticalSection may not be safe during static initialization.
//
// Use CriticalSectionLocker<> whenever possible instead of directly working
// with CriticalSection class - it is safer.
class CriticalSection
{
public:
	CriticalSection()
		: m_isInitialized(false)
		, m_ownerThreadId(0)
	{
		ZeroMemory(&m_critRegion, sizeof(m_critRegion));
	}

	~CriticalSection()
	{
		Delete();
	}

	// Initialize the critical section. Safe to call multiple times.
	// Returns true on success, false on failure (out of memory).
	bool Initialize()
	{
		if (m_isInitialized)
			return true;

		__try {
			InitializeCriticalSection(&m_critRegion);
			m_isInitialized = true;
		} __except (GetExceptionCode() == STATUS_NO_MEMORY ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) {
			m_isInitialized = false;
		}
		return m_isInitialized;
	}

	// Delete the critical section. Safe to call multiple times.
	void Delete()
	{
		if (m_isInitialized) {
			DeleteCriticalSection(&m_critRegion);
			m_isInitialized = false;
			m_ownerThreadId = 0;
		}
	}

	// Check if successfully initialized
	bool IsInitialized() const { return m_isInitialized; }

	// Enter the critical section
	void Enter()
	{
		if (m_isInitialized) {
			EnterCriticalSection(&m_critRegion);
			m_ownerThreadId = GetCurrentThreadId();
		}
	}

	// Check if locked by any thread
	bool IsLocked() const
	{
		return (m_ownerThreadId != 0);
	}

	// Check if locked by the current thread
	bool IsLockedByCurrentThread() const
	{
		return (m_ownerThreadId == GetCurrentThreadId());
	}

	// Try to enter the critical section (non-blocking)
	bool TryEnter()
	{
		if (!m_isInitialized)
			return false;
		if (TryEnterCriticalSection(&m_critRegion) != 0) {
			m_ownerThreadId = GetCurrentThreadId();
			return true;
		}
		return false;
	}

	// Leave the critical section
	void Leave()
	{
		if (m_isInitialized) {
			m_ownerThreadId = 0;
			LeaveCriticalSection(&m_critRegion);
		}
	}

private:
	// Non-copyable
	CriticalSection(const CriticalSection&) = delete;
	CriticalSection& operator=(const CriticalSection&) = delete;

	CRITICAL_SECTION m_critRegion;
	bool m_isInitialized;
	volatile DWORD m_ownerThreadId;  // Track owner thread ID ourselves
};

template<typename T = CriticalSection>
class CriticalSectionLocker
{
public:
	explicit CriticalSectionLocker(T& cs)
		: m_hasLeft(false)
		, m_critSect(cs)
	{
		m_critSect.Enter();
	}

	~CriticalSectionLocker()
	{
		LeaveLock();
	}

	void Leave()
	{
		LeaveLock();
	}

private:
	void LeaveLock()
	{
		if (!m_hasLeft)
		{
			m_critSect.Leave();
			m_hasLeft = true;
		}
	}

	// Non-copyable and non-movable
	CriticalSectionLocker() = delete;
	CriticalSectionLocker(const CriticalSectionLocker&) = delete;
	CriticalSectionLocker& operator=(const CriticalSectionLocker&) = delete;
	CriticalSectionLocker(CriticalSectionLocker&&) = delete;
	CriticalSectionLocker& operator=(CriticalSectionLocker&&) = delete;

	bool m_hasLeft;
	T& m_critSect;
};
