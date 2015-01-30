// IISxpressAPI.h

#pragma once

/// <summary>
/// The IISxpressAPI namepace is used by all the types defined by the IISxpress API.
/// </summary>
namespace IISxpressAPI 
{
	/// <summary>
	/// <c>IISxpressAPIException</c> is the base exception class for the IISxpress API. 
	/// This exception is thrown when errors specific to IISxpress are encountered.
	/// </summary>
	/// <remarks>
	/// <c>IISxpressAPI</c> methods throw <c>IISxpressAPIException</c> when an error condition is encountered.
	/// Some method calls populate the <see cref="HResult">HResult</see> member with appropriate error information, you can therefore use
	/// the value of this field to determine the source of the error.	
	/// <note>
	/// All <c>IISxpressAPI</c> methods can throw .NET Framework <c>System.Exception</c> exceptions in addition to
	/// <c>IISxpressAPIException</c>.	
	/// </note>
	/// </remarks>	
	/// <preliminary/>
	public ref class IISxpressAPIException : System::ApplicationException
	{

	public:

		IISxpressAPIException() : System::ApplicationException() {}

		IISxpressAPIException(HRESULT hr) : System::ApplicationException(hr.ToString("X"))
		{
			this->HResult = hr;
		}

		IISxpressAPIException(System::String^ msg, System::Exception^ e) : System::ApplicationException(msg, e) {}	

		/// <summary>
		/// The read only <c>HResult</c> property can be used to determine the COM error code which 
		/// caused the exception to be thrown.
		/// </summary>
		/// <remarks>
		/// The <c>HResult</c> property is present in the <c>System.Exception</c> class but does not have 
		/// public scope. You must therefore handle the exception as a <c>IISxpressAPIException</c> to get
		/// access to this value.
		/// </remarks>
		/// <example><code>
		/// try
		/// {
		///		// your code here ....
		/// }
		/// catch (IISxpressAPIException e)
		/// {
		///		// IISxpressAPIException.HResult is public so we can use it here
		///		Console.WriteLine(e.HResult);
		/// }
		/// catch (System.Exception e)
		/// {
		///		// HResult is protected in System.Exception so we can't query for its value here
		/// }
		/// </code></example>
		property int HResult
		{
			int get() { return System::ApplicationException::HResult; }
		}
	};
}