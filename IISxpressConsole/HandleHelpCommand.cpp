#include "stdafx.h"

void HandleHelpAddCommand(void);
void HandleHelpRemoveCommand(void);
void HandleHelpListCommand(void);
void HandleHelpFilterCommand(void);
void HandleHelpCompModeCommand(void);
void HandleHelpMonitorCommand(void);

void HandleHelpCommand(const CStringArray& saArgs)
{
	if (saArgs.GetCount() == 1 && saArgs[0].CompareNoCase(_T("add")) == 0)
	{
		HandleHelpAddCommand();
	}
	else if (saArgs.GetCount() == 1 && saArgs[0].CompareNoCase(_T("remove")) == 0)
	{
		HandleHelpRemoveCommand();
	}
	else if (saArgs.GetCount() == 1 && saArgs[0].CompareNoCase(_T("list")) == 0)
	{
		HandleHelpListCommand();
	}
	else if (saArgs.GetCount() == 1 && saArgs[0].CompareNoCase(_T("filter")) == 0)
	{
		HandleHelpFilterCommand();
	}
	else if (saArgs.GetCount() == 1 && saArgs[0].CompareNoCase(_T("compmode")) == 0)
	{
		HandleHelpCompModeCommand();
	}
	else if (saArgs.GetCount() == 1 && saArgs[0].CompareNoCase(_T("monitor")) == 0)
	{
		HandleHelpMonitorCommand();
	}
	else
	{
		CString sMsg;
		sMsg.LoadString(IDS_ERROR_COMMANDLINEINCORRECT);		
		throw sMsg;
	}
}

void HandleHelpAddCommand(void)
{
	_tprintf(
		_T("Command syntax:\n\n")
		_T("add extn [EXTNLIST]\n")		
		_T(" Add a list of one or more extensions to the extension exclusion list.\n")
		_T("\n")
		_T(" examples:\n")
		_T("  add extn mp3            - do not compress requests for files with the\n")
		_T("                            mp3 extension\n")
		_T("  add extn jpg gif        - do not compress requests for files with either\n")
		_T("                            the jpg or gif extension\n")
		_T("\n")
		_T("add ct [CONTEXTTYPELIST]\n")
		_T(" Add a list of one or more content types to the content type exclusion list.\n")
		_T("\n")
		_T(" examples:\n")
		_T("  add ct audio/mp3            - do not compress responses which have the\n")
		_T("                                content type audio/mp3\n")
		_T("  add ct image/jpeg image/gif - do not compress responses which have the\n")
		_T("                                content type images/jpeg or image/gif\n")
		_T("\n")
		_T("add uri [SERVER] [URILIST]\n")
		_T(" Add a list of one or more URIs to the URI exclusion list. You must specify\n")
		_T(" the name or the instance number of the web server.\n")
		_T("\n")
		_T(" examples:\n")
		_T("  add uri 1 /audio                   - do not compress requests for files in\n")
		_T("                                       /audio on web server 1\n")
		_T("  add uri \"Default Web Site\" /images - do not compress requests for files in\n")		
		_T("                                       /images on \"Default Web Site\"\n")		
		_T("  add uri \"Default Web Site\" /data/* - do not compress requests for files in\n")		
		_T("                                       /data or of its sub directories on\n")
		_T("                                       \"Default Web Site\"\n")
		_T("\n")
		_T("add ip [IPLIST]\n")
		_T(" Add a list of one or more IP addresses to the IP address exclusion list.\n")
		_T("\n")
		_T(" examples:\n")
		_T("  add ip 10               - exclude all requests from clients on network 10\n")
		_T("  add ip 172.16 192.168.0 - exclude all requests from clients on networks\n")		
		_T("                            172.16 and 192.168.0\n")
		_T("\n")
		_T("add file [FILELIST]\n")
		_T(" Add a list of one or more files to the file exclusion list.\n")
		_T("\n")
		_T(" examples:\n")
		_T("  add file c:\\inetpub\\wwwroot\\default.aspx\n")		
		_T("  add file c:\\inetpub\\wwwroot\\test.asp c:\\inetpub\\wwwroot\\test.xml\n")		
		);
}

void HandleHelpRemoveCommand(void)
{
	_tprintf(
		_T("Command syntax:\n\n")
		_T("remove extn [EXTNLIST]\n")		
		_T(" Remove one or more extensions from the extension exclusion list.\n")
		_T("\n")
		_T(" example:\n")
		_T("  remove extn gif jpg     - remove extensions gif and jpg from the exclusion\n")
		_T("                            list\n")
		_T("\n")
		_T("remove ct [CONTEXTTYPELIST]\n")
		_T(" Remove one or more content types from the content type exclusion list.\n")		
		_T("\n")
		_T(" example:\n")
		_T("  remove ct image/jpg     - remove content type image/jpg from the exclusion\n")
		_T("                            list\n")
		_T("\n")
		_T("remove uri [SERVER] [URILIST]\n")
		_T(" Remove one or more URIs from the URI exclusion list.\n")
		_T("\n")
		_T(" examples:\n")
		_T("  remove uri 1 /audio                   - remove /audio from the URI exclusion\n")
		_T("                                          list on server 1\n")
		_T("  remove uri \"Default Web Site\" /images - remove /images from the URI exclusion\n")
		_T("                                          list on \"Default Web Site\"\n")
		_T("\n")
		_T("add ip [IPLIST]\n")
		_T(" Remove one or more IP addresses from the IP address exclusion list.\n")		
		_T("\n")
		_T(" example:\n")
		_T("  remove ip 192.168.0     - remove network 192.168.0 from the exclusion list\n")
		_T("\n")
		_T("remove file [FILELIST]\n")
		_T(" Remove one or more files from the file exclusion list.\n")
		_T("\n")
		_T(" examples:\n")
		_T("  remove file c:\\inetpub\\wwwroot\\default.aspx\n")		
		_T("  remove file c:\\inetpub\\wwwroot\\test.asp c:\\inetpub\\wwwroot\\test.xml\n")		
		);
}

void HandleHelpListCommand(void)
{
	_tprintf(
		_T("Command syntax:\n\n")
		_T("list extn\n")		
		_T(" List all the excluded extensions.\n")
		_T("\n")
		_T("list ct\n")
		_T(" List all the excluded content types.\n")
		_T("\n")
		_T("list uri [opt:SERVER]\n")
		_T(" Either list all the excluded URIs or only the URIs for a specific server.\n")
		_T(" If the URI has a /* on the end then it is a wildcard exclusion rule.\n")
		_T(" example: list uri \"Default Web Site\"\n")
		_T("\n")
		_T("list ip\n")
		_T(" List all the excluded IP addresses.\n")
		_T("\n")
		_T("list file [FILEPATH]\n")		
		_T(" List all the excluded files on a given filepath.\n")
		_T("\n")
		_T(" example: list file c:\\inetpub\\wwwroot\n")		
		);
}

void HandleHelpFilterCommand(void)
{
	_tprintf(
		_T("Command syntax:\n\n")		
		_T("filter enable\n")
		_T(" Enable the ISAPI filter. When enabled IISxpress will attempt to compress\n")
		_T(" responses.\n")
		_T("\n")
		_T("filter disable\n")
		_T(" Disable the ISAPI filter. When disabled IISxpress will not attempt to compress\n")
		_T(" responses. This mode is useful when diagnosing server problems.\n")
		_T("\n")
		_T("filter\n")		
		_T(" When the filter command is specified without any parameters the current mode\n")
		_T(" is displayed.\n")
		);
}

void HandleHelpCompModeCommand(void)
{
	_tprintf(
		_T("Command syntax:\n\n")
		_T("compmode default\n")
		_T(" Set the compression mode to 'default'. This mode offers the best compromise\n")
		_T(" between response size and compression time. Typically this mode will be 20%%\n")
		_T(" slower than 'speed' mode but will still exhibit very good compression\n")
		_T(" performance.\n")
		_T("\n")
		_T("compmode speed\n")
		_T(" Set the compression mode to 'speed'. This mode ensures the fastest possible\n")
		_T(" response time at the expense of compression performance.\n")
		_T("\n")
		_T("compmode size\n")
		_T(" Set the compression mode to 'size'. The response will be compressed to the\n")
		_T(" smallest possible size regardless of time used.\n")
		);
}

void HandleHelpMonitorCommand(void)
{
	_tprintf(
		_T("Command syntax:\n\n")
		_T("monitor\n")
		_T(" The 'monitor' command allows you to track compression activity on your web\n")
		_T(" server. When responses are compressed their details will be displayed in the\n")
		_T(" console window. To exit from monitor mode press CTRL+C.\n")
		);
}