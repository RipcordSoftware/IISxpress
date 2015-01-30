<%@ Language="C#" %>
<%@ Assembly name="IISxpressAPI" %>

<html>

<head>

<title>IISxpress Unload History</title>

</head>

<body>

<script runat="server">       
    
    String status;
    
    protected void Page_Load(object sender, System.EventArgs e)
    {              
        IISxpressAPI.ResponseHistory history = Application["history"] as IISxpressAPI.ResponseHistory;

        if (history != null)
        {
            Application["history"] = null;
            history.Dispose();

            status = "Response history unloaded ok";
        }
        else
        {
            status = "Unable to unload response history";
        }        
    }          
    
</script>

<div>
<% = status %>
</div>

</body>

</html>