<%@ Language="C#" %>
<%@ Assembly name="IISxpressAPI" %>

<html>

<head>

<title>IISxpress Clear History</title>

</head>

<body>

<script runat="server">       
    
    String status;
    
    protected void Page_Load(object sender, System.EventArgs e)
    {              
        IISxpressAPI.ResponseHistory history = Application["history"] as IISxpressAPI.ResponseHistory;

        if (history != null)
        {
            history.ClearResponses();
            status = "Response history cleared ok";
        }
        else
        {
            status = "Unable to clear response history";
        }
    }         
    
</script>

<div>
<% = status %>
</div>

</body>

</html>