if [%1]==[] goto end
if [%2]==[] goto end

rem %1 is the platform
rem %2 is the configuration

set PRERELEASE=c:\PRERELEASE
set TIMESTAMP=20100610000000

rem ensure we always have a prerelease directory
if NOT EXIST %PRERELEASE% ( mkdir %PRERELEASE% )

set DEBUGPOSTFIX=""
if /I "%2"=="DEBUG" ( set DEBUGPOSTFIX="D" )

goto iisxpress
rem ***********************************************************************************
rem * IIS Metabase Explorer 

set MBEXP_TARGET=%PRERELEASE%\IISMetabaseExplorer
mkdir %MBEXP_TARGET%
xcopy ..\IISMetabaseExplorer\%1\IISMetabaseExplorer.exe %MBEXP_TARGET%
xcopy ..\IISMetabaseExplorer\%1\IISMetabaseExplorer.chm %MBEXP_TARGET%
rem xcopy %SYSTEMROOT%\System32\mfc80%DEBUGPOSTFIX%.dll %MBEXP_TARGET%
rem xcopy %SYSTEMROOT%\System32\msvcr80%DEBUGPOSTFIX%.dll %MBEXP_TARGET%
rem xcopy %SYSTEMROOT%\System32\msvcp80%DEBUGPOSTFIX%.dll %MBEXP_TARGET%
xcopy "..\IISMetabaseExplorer\Installer License.txt" %MBEXP_TARGET%
xcopy ..\IISMetabaseExplorer\License.txt %MBEXP_TARGET%

:iisxpress
rem ***********************************************************************************
rem * IISxpress

set IISX_TARGET=%PRERELEASE%\IISxpress_%1
rmdir /s /q %IISX_TARGET%
mkdir %IISX_TARGET%

set IISX_RESTARGET=%IISX_TARGET%\res
mkdir %IISX_RESTARGET%

set IISX_APITARGET=%IISX_TARGET%\API
mkdir %IISX_APITARGET%

rem Until InstallShield is 64bit the DLL is always Win32
xcopy ..\IISxpressInstallerExtensions\Win32\%2\IISxpressInstallerExtensions.dll %IISX_TARGET%
xcopy ..\IISxpressIIS7ModuleInstaller\%1\%2\IISxpressIIS7ModuleInstaller.exe %IISX_TARGET%

xcopy ..\IISxpress\%1\%2\IISxpress.dll %IISX_TARGET%
xcopy ..\IISxpress\%1\%2\IISxpressPerf.dll %IISX_TARGET%

xcopy ..\IISxpressNativeModule\%1\%2\IISxpressNativeModule.dll %IISX_TARGET%
xcopy ..\IISxpressNativeModule\IISxpressNativeModulePerf.xml %IISX_TARGET%

xcopy ..\IISxpressServer\%1\%2\IISxpressServer.exe %IISX_TARGET%
xcopy ..\IISxpressServer\%1\%2PS\IISxpressServerPS.dll %IISX_TARGET%
xcopy ..\IISxpressAHAComp\%1\%2\IISxpressAHAComp.dll %IISX_TARGET%
xcopy ..\IISxpressServer\IISxpress.config %IISX_TARGET%

xcopy ..\IISxpressTray\%1\%2\IISxpressTray.exe %IISX_TARGET%
xcopy ..\IISxpressTray\hlp\IISxpressTray.chm %IISX_TARGET%
xcopy ..\IISxpressTray\res\compass.ico %IISX_RESTARGET%

xcopy ..\IISxpressConsole\%1\%2\IISxpress.exe %IISX_TARGET%

xcopy ..\IISxpressAPI\%1\%2\IISxpressAPI.dll %IISX_APITARGET%
xcopy ..\IISxpressAPI\IISxpressAPI.pdf %IISX_APITARGET%
xcopy ..\IISxpressAPI\%1\%2\doc\IISxpressAPI.chm %IISX_APITARGET%

xcopy ..\IISxpressCompressionStudio\%1\%2\IISxpressCompressionStudio.exe %IISX_TARGET%

rem ValidateIISxpress is always Win32
xcopy ..\ValidateIISxpress\win32\%2\ValidateIISxpress.exe %IISX_TARGET%

mkdir %IISX_APITARGET%\html
xcopy ..\IISxpressAPI\HTML\web.config %IISX_APITARGET%\html
xcopy ..\IISxpressAPI\HTML\stats.aspx %IISX_APITARGET%\html

if /I [%1]==[Win32] (  set ZLIB_DLL=..\IISxpressServer\ZLIB\DLL32\zlibwapi.dll ) else ( set ZLIB_DLL=..\IISxpressServer\ZLIB\DLL64\zlibwapi.dll )
xcopy %ZLIB_DLL% %IISX_TARGET%

xcopy ..\License.rtf %IISX_TARGET%
xcopy ..\EvaluationLicense.rtf %IISX_TARGET%
xcopy ..\CommunityLicense.rtf %IISX_TARGET%

xcopy "..\docs\IISxpress User Manual.pdf" %IISX_TARGET%

xcopy "..\RipcordSoftware.com.url" %IISX_TARGET%

xcopy "..\LogFiles.txt" %IISX_TARGET%

rem xcopy ..\IISxpressServer\Rules.NeverRules.Upgrade2.xml %IISX_TARGET%

set IISX_KEYTARGET=%IISX_TARGET%\keys
mkdir %IISX_KEYTARGET%
xcopy ..\Certificates\Software\mykey.pvk %IISX_KEYTARGET%
xcopy ..\Certificates\Software\mypfx.pfx %IISX_KEYTARGET%

rem ***********************************************************************************
rem * Sign binaries

if /I not "%2" == "RELEASE" goto symbol_store

set PFX_FILEPATH=%IISX_KEYTARGET%\mypfx.pfx
set PFX_PWD=bismark123
set TIMESTAMP_URL=http://timestamp.comodoca.com/authenticode

set IISX_SIGN_TARGETS=%IISX_TARGET%\IISxpress.dll %IISX_TARGET%\IISxpressPerf.dll
set IISX_SIGN_TARGETS=%IISX_SIGN_TARGETS% %IISX_TARGET%\IISxpress.exe %IISX_TARGET%\IISxpressServer.exe 
set IISX_SIGN_TARGETS=%IISX_SIGN_TARGETS% %IISX_TARGET%\IISxpressAHAComp.dll %IISX_TARGET%\IISxpressServerPS.dll
set IISX_SIGN_TARGETS=%IISX_SIGN_TARGETS% %IISX_TARGET%\IISxpressTray.exe 
set IISX_SIGN_TARGETS=%IISX_SIGN_TARGETS% %IISX_TARGET%\IISxpressInstallerExtensions.dll %IISX_TARGET%\IISxpressIIS7ModuleInstaller.exe
set IISX_SIGN_TARGETS=%IISX_SIGN_TARGETS% %IISX_TARGET%\IISxpressNativeModule.dll
set IISX_SIGN_TARGETS=%IISX_SIGN_TARGETS% %IISX_APITARGET%\IISxpressAPI.dll
set IISX_SIGN_TARGETS=%IISX_SIGN_TARGETS% %IISX_TARGET%\IISxpressCompressionStudio.exe
set IISX_SIGN_TARGETS=%IISX_SIGN_TARGETS% %IISX_TARGET%\ValidateIISxpress.exe
signtool sign /f %PFX_FILEPATH% /p %PFX_PWD% /t %TIMESTAMP_URL% %IISX_SIGN_TARGETS%

set IISX_TIMESTAMP_TARGETS=%IISX_SIGN_TARGETS%
set IISX_TIMESTAMP_TARGETS=%IISX_TIMESTAMP_TARGETS% %IISX_TARGET%\IISxpressTray.chm 
set IISX_TIMESTAMP_TARGETS=%IISX_TIMESTAMP_TARGETS% %IISX_TARGET%\License.rtf 
set IISX_TIMESTAMP_TARGETS=%IISX_TIMESTAMP_TARGETS% %IISX_TARGET%\CommunityLicense.rtf 
set IISX_TIMESTAMP_TARGETS=%IISX_TIMESTAMP_TARGETS% %IISX_TARGET%\EvaluationLicense.rtf 
set IISX_TIMESTAMP_TARGETS=%IISX_TIMESTAMP_TARGETS% "%IISX_TARGET%\IISxpress User Manual.pdf" 
set IISX_TIMESTAMP_TARGETS=%IISX_TIMESTAMP_TARGETS% %IISX_TARGET%\RipcordSoftware.com.url
SetFileTime %TIMESTAMP% %IISX_TIMESTAMP_TARGETS% 

SetFileTime 20050101000000 %IISX_TARGET%\IISxpress.config

:symbol_store

rem ***********************************************************************************
rem * Symbol store

if /I not "%2" == "RELEASE" goto end
set SYMSHARE=\\westwaynas\Ripcord\Symbols\RipcordSoftware

set SYMSTORE="C:\Program Files\Debugging Tools for Windows\symstore.exe"
if exist %SYMSTORE% goto symbackup
set SYMSTORE="C:\Program Files\Debugging Tools for Windows (x86)\symstore.exe"

:symbackup
%SYMSTORE% add /t IISxpress /f ..\IISxpressServer\%1\%2\IISxpressServer.pdb /s %SYMSHARE%
%SYMSTORE% add /t IISxpress /f ..\IISxpressAHAComp\%1\%2\IISxpressAHAComp.pdb /s %SYMSHARE%
%SYMSTORE% add /t IISxpress /f ..\IISxpress\%1\%2\IISxpress.pdb /s %SYMSHARE%
%SYMSTORE% add /t IISxpress /f ..\IISxpressConsole\%1\%2\IISxpressConsole.pdb /s %SYMSHARE%
%SYMSTORE% add /t IISxpress /f ..\IISxpressTray\%1\%2\IISxpressTray.pdb /s %SYMSHARE%
%SYMSTORE% add /t IISxpress /f ..\IISxpressNativeModule\%1\%2\IISxpressNativeModule.pdb /s %SYMSHARE%
%SYMSTORE% add /t IISxpress /f ..\IISxpressAPI\%1\%2\IISxpressAPI.pdb /s %SYMSHARE%
%SYMSTORE% add /t IISxpress /f ..\IISxpressCompressionStudio\%1\%2\IISxpressCompressionStudio.pdb /s %SYMSHARE%

rem *** Always WIN32 ***
%SYMSTORE% add /t IISxpress /f ..\IISxpressInstallerExtensions\Win32\%2\IISxpressInstallerExtensions.pdb /s %SYMSHARE%

:end