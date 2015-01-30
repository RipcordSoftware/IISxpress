#pragma once

namespace IISxpressAPI 
{		
	/// <summary>
	/// The <c>CompressionStats</c> class allows access to the compression statistics of an IISxpress
	/// Compression Server.
	/// </summary>
	/// <remarks>
	/// Each <c>CompressionStats</c> object that is created establishes a connection to the IISxpress
	/// Compression Server. It is therefore worthwhile creating only a small number of these objects
	/// (preferably one) in your application.
	/// <para>See <see cref="GetStats">GetStats()</see> for an example of how to use this class.</para>
	/// <note type="implementnotes">
	/// If you are using <c>CompressionStats</c> in ASP.NET you can use <c>Page.Cache</c> to store
	/// the object instance between calls.
	/// </note>
	/// </remarks>
	/// <preliminary/>
	public ref class CompressionStats
	{

	public: 

		/// <summary>
		/// This constructor establishes a connection to the IISxpress Compression Server process
		/// allowing the caller to query for the compression statistics.
		/// </summary>
		/// <exception cref="IISxpressAPIException">
		/// Thrown when the IISxpress Compression Server could not be located. The <c>HRESULT</c> of the
		/// exception details the COM error code and can be used to diagnose the source of the problem.
		/// </exception>
		CompressionStats();

		!CompressionStats();
		~CompressionStats();

		/// <summary>
		/// Call this method to reset the compression statistics to zero.
		/// </summary>
		/// <remarks>
		/// This method connects to the IISxpress Compression Server and resets the compression statistics
		/// to zero. To get the current compression statistics see <see cref="GetStats">GetStats</see>.
		/// </remarks>
		/// <seealso cref="GetStats">CompressionStats.GetStats</seealso>
		/// <exception cref="IISxpressAPIException">
		/// Thrown if an error occurs when making the call to the compression server. The <c>HRESULT</c> 
		/// will contain the COM error code and can be used to diagnose the source of the problem.
		/// </exception>		
		/// <example><code>		
		///	try
		/// {
		///		CompressionStats stats = new CompressionStats();
		///		stats.ResetStats();
		/// }
		/// catch (IISxpressAPIException e)
		///	{
		///		// do something about the exception....
		/// }
		/// catch (System.Exception e)
		/// {
		///		// do something about the exception....
		/// }
		/// </code></example>
		void ResetStats();		

		/// <summary>
		/// Call this method to get the compression statistics from the IISxpress Compression Server.
		/// </summary>
		/// <remarks>
		/// This method connects to the IISxpress Compression Server and retrieves the current compression
		/// statistics. To reset the compression statistics see <see cref="ResetStats">ResetStats</see>.
		/// </remarks>
		/// <param name="filesCompressed">
		/// On return will contain the total number of files compressed by IISxpress
		/// </param>
		/// <param name="rawSize">
		/// On return will contain the total number of Kbytes of raw data before being compressed by IISxpress
		/// </param>
		/// <param name="compressedSize">
		/// On return will contain the total number of Kbytes of data compressed by IISxpress
		/// </param>
		/// <seealso cref="ResetStats">CompressionStats.ResetStats</seealso>
		/// <exception cref="IISxpressAPIException">
		/// Thrown if an error occurs when making the call to the compression server. The <c>HRESULT</c> 
		/// will contain the COM error code and can be used to diagnose the source of the problem.
		/// </exception>
		/// <example><code>		
		///	try
		/// {
		///		CompressionStats stats = new CompressionStats();
		///
		///		UInt32 filesCompressed, rawSize, compressedSize;
		///		stats.GetStats(out filesCompressed, out rawSize, out compressedSize);
		///
		///		// calculate the compression ratio
        ///		double ratio = 0;
        ///		if (rawSize > 0)
        ///     {
        ///			double r = compressedSize;
		///			r /= rawSize;
		///			r *= 100;
		///			ratio = 100 - r;
		///		}
		/// }
		/// catch (IISxpressAPIException e)
		///	{
		///		// do something about the exception....
		/// }
		/// catch (System.Exception e)
		/// {
		///		// do something about the exception....
		/// }
		/// </code></example>
		void GetStats(
			[System::Runtime::InteropServices::Out] unsigned% filesCompressed, 
			[System::Runtime::InteropServices::Out] unsigned% rawSize, 
			[System::Runtime::InteropServices::Out] unsigned% compressedSize);		

	private: 

		HRESULT GetCompressionStatsObject(ICompressionStats** ppCompressionStats);		

		ICompressionStats* m_pCompressionStats;
	};	
}
