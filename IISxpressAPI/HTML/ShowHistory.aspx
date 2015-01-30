<%@ Language="C#" Debug="true" %>
<%@ Assembly name="IISxpressAPI" %>

<html>

<head>
<title>IISxpress Response History</title>
</head>

<body>

<script runat="server">            

    IISxpressAPI.ResponseDetails[] responses;
    
    protected void Page_Load(object sender, System.EventArgs e)
    {                
        sourceResponses.TypeName = this.GetType().AssemblyQualifiedName;        
        sourceResponses.ObjectCreating += sourceResponses_ObjectCreating;
        sourceResponses.SelectMethod = "GetResponses";
        
        try
        {
            IISxpressAPI.ResponseHistory history = GetHistoryObject();

            responses = history.GetResponses();            

            System.Array.Reverse(responses);
        }
        catch (IISxpressAPI.IISxpressAPIException x)
        {
        }        

        this.DataBind();
    }    

    protected IISxpressAPI.ResponseHistory GetHistoryObject()
    {
        IISxpressAPI.ResponseHistory history = Application["history"] as IISxpressAPI.ResponseHistory;
        if (history == null)
        {
            history = new IISxpressAPI.ResponseHistory();
            Application["history"] = history;
        }

        return history;
    }

    protected void sourceResponses_ObjectCreating(object sender, ObjectDataSourceEventArgs e)
    {
        e.ObjectInstance = this;
    }

    public IISxpressAPI.ResponseDetails[] GetResponses() { return responses; }
    
</script>

<form runat="server">

<asp:ObjectDataSource ID="sourceResponses" runat=server></asp:ObjectDataSource>
<asp:GridView ID="gridview1" runat=server DataSourceID="sourceResponses" AutoGenerateColumns="false" HeaderStyle-BackColor="LightGrey">
<Columns>
<asp:BoundField DataField="Time" HeaderText="Time" />
<asp:BoundField DataField="URI" HeaderText="URI" />
<asp:BoundField DataField="RemoteAddress" HeaderText="IP Address" />
<asp:BoundField DataField="ContentType" HeaderText="Content Type" />
<asp:BoundField DataField="IsCompressed" HeaderText="Compressed" />
<asp:BoundField DataField="RawSize" HeaderText="Raw Size" />
<asp:BoundField DataField="CompressedSize" HeaderText="Compressed Size" />
</Columns>
</asp:GridView>

</form>

</body>

</html>