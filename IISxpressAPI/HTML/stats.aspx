<%@ Language="C#" %>
<%@ Assembly name="IISxpressAPI" %>

<html>

<head>

<title>IISxpress Compression Statistics</title>

<style>

.statsblock
{
    margin-left: 6px; 
	margin-right: 6px; 
	background-color: white; 
	border-width: 2px; 
	border-color: white; 
	border-style: outset; 
	width: 340px;
	height: 8em;
}

.statsheader
{
    font-size: 20px;
	background-color: #eaeaea;
	padding-left: 4px;
	margin-bottom: 4px;
	padding-top: 0px;
	margin-top: 0px;
	display: block;
}

#numberofresponses, #uncompresseddatasize, #compresseddatasize, #compressionratio
{
    font-weight: bolder;
    width: 200px;
    padding-left: 4px;
    padding-top: 2px;
    padding-bottom: 2px;
    float: left;
}

#numberofresponses_value, #uncompresseddatasize_value, #compresseddatasize_value, #compressionratio_value
{   
    margin-right: 4px;    
    float: right;
}

#timestamp
{
    clear: both;
    font-size: x-small; 
    position: relative; 
    top: 1em;
}

</style>

</head>

<body>

<script runat="server">
    
    UInt32 filesCompressed, rawSize, compressedSize;
    double ratio;    
    
    /// <summary>
    /// This method is executed by ASP.NET when the page is loaded
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    protected void Page_Load(object sender, System.EventArgs e)
    {
        try
        {
            // get hold of the stats object
            IISxpressAPI.CompressionStats stats = GetStatsObject();

            // get the information from the server
            stats.GetStats(out filesCompressed, out rawSize, out compressedSize);

            // calculate the compression ratio
            ratio = 0;
            if (rawSize > 0)
            {
                double r = compressedSize;
                r /= rawSize;
                r *= 100;
                ratio = 100 - r;
            }
        }
        catch (IISxpressAPI.IISxpressAPIException x)
        {
            // we failed for some reason, so ensure the stats object is 
            // no longer being held in the cache
            ResetStatsObject();
        }                
    }

    /// <summary>
    /// Gets the CompressionStats object from the web server's cache
    /// or by creating a new instance of the object
    /// </summary>
    /// <returns></returns>
    protected IISxpressAPI.CompressionStats GetStatsObject()
    {
        // try to get the CompressionStats object from the cache
        IISxpressAPI.CompressionStats stats = Cache["stats"] as IISxpressAPI.CompressionStats;
                
        // if the object reference is null then it wasn't in the cache, so we
        // need to create a new instance of the object and store it in the 
        // cache for next time
        if (stats == null)
        {
            stats = new IISxpressAPI.CompressionStats();
            Cache["stats"] = stats;
        }

        // return the CompressionStats object
        return stats;
    }

    /// <summary>
    /// Resets the CompressionStats object held in the web server's cache
    /// </summary>
    protected void ResetStatsObject()
    {
        Cache.Remove("stats");
    }
    
</script>

<div class="statsblock">
<span class="statsheader">Compression Statistics</span>
<span id="numberofresponses">Number of responses:</span><span id="numberofresponses_value"><% = filesCompressed.ToString("#,#") %></span>
<span id="uncompresseddatasize">Uncompressed data size:</span><span id="uncompresseddatasize_value"><% = rawSize.ToString("#,#") %> KB</span>
<span id="compresseddatasize">Compressed data size:</span><span id="compresseddatasize_value"><% = compressedSize.ToString("#,#") %> KB</span>
<span id="compressionratio">Compression ratio:</span><span id="compressionratio_value"><% = ratio.ToString("f1") %> %</span>
</div>

<div id="timestamp">Generated at: <% = DateTime.Now.ToString() %></div>

</body>

</html>