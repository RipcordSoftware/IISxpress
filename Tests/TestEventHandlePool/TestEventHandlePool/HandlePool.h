#pragma once

class HandleObject
{

public:

	HandleObject() : m_hHandle(NULL), m_dwCookie(0), m_nSlot(-1) {}

	explicit HandleObject(HANDLE hHandle) : m_hHandle(hHandle), m_dwCookie(0), m_nSlot(-1) {}

	explicit HandleObject(HANDLE hHandle, int nSlot) : m_hHandle(hHandle), m_dwCookie(0), m_nSlot(nSlot) {}

	HandleObject(HandleObject& other) : 
		m_hHandle(other.Detach()), m_dwCookie(other.m_dwCookie), m_nSlot(other.m_nSlot) {}	

	~HandleObject() 
	{ 
		HANDLE hHandle = Detach();
		if (hHandle != NULL)
		{
			::CloseHandle(hHandle);
		}
	}

	HANDLE Detach()
	{
		return Exchange(NULL);
	}

	bool IsValid()
	{
		return m_hHandle != NULL ? true : false;
	}

	HandleObject& operator=(HandleObject& other) 
	{ 
		Exchange(other.Detach());
		m_dwCookie = other.m_dwCookie; 
		m_nSlot = other.m_nSlot;
		return *this; 
	}

	operator HANDLE() { return m_hHandle; }

	DWORD GetCookie() { return m_dwCookie; }
	void SetCookie(DWORD dwCookie) { m_dwCookie = dwCookie; }

	int GetSlot() { return m_nSlot; }

private:

	HANDLE Exchange(HANDLE newval)
	{
#pragma warning (push)
#pragma warning (disable: 4311 4312)
#ifdef _M_X64		
		return (HANDLE) InterlockedExchange64((LONGLONG*) &m_hHandle, (LONGLONG) newval);
#elif _WIN32
		return (HANDLE) InterlockedExchange((LONG*) &m_hHandle, (LONG) newval);		
#endif	
#pragma warning (pop)
	}

	volatile HANDLE m_hHandle;

	DWORD			m_dwCookie;
	int				m_nSlot;

};

class EventHandlePool
{

public:

	EventHandlePool(LONG nSize = 16) : 
		m_pHandleObjects(new HandleObject[nSize]), m_nSize(nSize), m_nGetOffset(0), m_nAvailableHandleObjects(nSize)			
	{
		// create a fairly unique cookie which will allow us to track ownership of the handle objects
		LARGE_INTEGER nCounter;
		::QueryPerformanceCounter(&nCounter);
		m_dwMasterCookie = nCounter.LowPart;

		for (LONG i = 0; i < m_nSize; i++)
		{
			m_pHandleObjects[i] = HandleObject(::CreateEvent(NULL, FALSE, FALSE, NULL), i);
			m_pHandleObjects[i].SetCookie(m_dwMasterCookie);
		}
	}	

	~EventHandlePool()
	{
		if (m_pHandleObjects != NULL)
		{
			delete[] m_pHandleObjects;
			m_pHandleObjects = NULL;
		}
	}

	LONG GetSize() { return m_nSize; }

	bool GetHandleObject(HandleObject& handleObject)
	{
		bool bStatus = false;		

		// loop as long as there are available handle objects
		while (m_nAvailableHandleObjects != 0)
		{
			LONG nOffset = ::InterlockedExchangeAdd(&m_nGetOffset, 1);
			nOffset %= m_nSize;

			// get the handle object from the array
			handleObject = m_pHandleObjects[nOffset];

			// if the handle is valid we are finished
			if (handleObject.IsValid() == true)
			{
				bStatus = true;
				::InterlockedDecrement(&m_nAvailableHandleObjects);
				break;
			}
		}

		return bStatus;
	}

	bool ReturnHandleObject(HandleObject& handleObject)
	{
		// don't allow the caller to return a NULL handle
		if (handleObject.IsValid() == false)
			return false;

		// if the handle object didn't come from this instance then just free it and return like all is well
		if (handleObject.GetCookie() != m_dwMasterCookie)
		{
			HandleObject temp(handleObject);
			return true;
		}

		int nOffset = handleObject.GetSlot();		

		// return the handle object
		m_pHandleObjects[nOffset] = handleObject;		

		// there is one more handle object available
		::InterlockedIncrement(&m_nAvailableHandleObjects);

		return true;
	}

private:

	const LONG		m_nSize;	
	HandleObject*	m_pHandleObjects;	
	volatile LONG	m_nAvailableHandleObjects;

	volatile LONG	m_nGetOffset;	

	DWORD			m_dwMasterCookie;

};