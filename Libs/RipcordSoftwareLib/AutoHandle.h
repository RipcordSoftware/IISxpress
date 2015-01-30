#pragma once

namespace Ripcord { namespace Types
{

namespace AutoHandle
{

	template <typename T> struct CloseHandle
	{
		void Close(T handle)
		{
			::CloseHandle(handle);
		}
	};

	template <typename T> struct ClosePerfCounterHandle
	{
		void Close(T handle)
		{
			::PdhRemoveCounter(handle);
		}
	};

	template <typename T> struct ClosePerfQueryHandle
	{
		void Close(T handle)
		{
			::PdhCloseQuery(handle);
		}
	};

	// ************************************************************************************

	template <typename HandleType, template <typename> class ReleaseAlgorithm, HandleType NULL_VALUE = NULL>
	class AutoHandleBase : public ReleaseAlgorithm<HandleType>
	{

	public:

		AutoHandleBase() : m_hHandle(NULL_VALUE) {}

		explicit AutoHandleBase(HandleType hHandle) { m_hHandle = hHandle; }

		~AutoHandleBase()
		{
			Close();
		}

		void Attach(HandleType hHandle)
		{
			m_hHandle = hHandle;
		}

		HandleType Detach() 
		{ 
			HandleType hHandle = m_hHandle;
			m_hHandle = NULL_VALUE; 
			return hHandle;
		}

		bool operator!() { return m_hHandle == NULL_VALUE; }

		operator HandleType() { return m_hHandle; }

		void Close()
		{
			if (m_hHandle != NULL_VALUE)
			{
				Close(m_hHandle);
				m_hHandle = NULL_VALUE;
			}
		}

	protected:	

		void Close(HandleType handle)
		{
			ReleaseAlgorithm<HandleType>::Close(handle);
		}

		HandleType m_hHandle;
	};

	// ************************************************************************************

	typedef AutoHandleBase<HANDLE, CloseHandle> AutoEventHandle;
	typedef AutoHandleBase<HANDLE, CloseHandle, INVALID_HANDLE_VALUE> AutoFileHandle;

#ifdef _PDH_H_
	typedef AutoHandleBase<HQUERY, ClosePerfQueryHandle> AutoPerfQueryHandle;
	typedef AutoHandleBase<HCOUNTER, ClosePerfCounterHandle> AutoPerfCounterHandle;
#endif
};

}}