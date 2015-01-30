rem ******************************************************************************
rem Configure IISxpress to run under Vista/Longhorn
rem
rem (c) Copyright Ripcord Software 2006
rem ******************************************************************************

rem registry the proxy DLL
regsvr32 /s iisxpressserverps.dll

rem ensure the proxy DLL can be loaded by IIS
icacls iisxpressserverps.dll /grant IIS_IUSRS:(RX)

rem install the IISxpress service
iisxpressserver /service
net start iisxpress

rem install the IISxpressNativeModule
%systemroot%\system32\inetsrv\appcmd.exe install module /name:IISxpressNativeModule /image:c:\iisxpress\IISxpressNativeModule.dll