#include "stdafx.h"

#include "..\Common\IISxpressVersion.h"

using namespace System;
using namespace System::Reflection;
using namespace System::Runtime::CompilerServices;
using namespace System::Runtime::InteropServices;
using namespace System::Security::Permissions;

//
// General Information about an assembly is controlled through the following
// set of attributes. Change these attribute values to modify the information
// associated with an assembly.
//
[assembly:AssemblyTitleAttribute("IISxpress API")];
[assembly:AssemblyDescriptionAttribute("A .NET API for consuming IISxpress services")];
[assembly:AssemblyConfigurationAttribute("")];
[assembly:AssemblyCompanyAttribute(IISXPRESS_COMPANYNAME_STRING)];
[assembly:AssemblyProductAttribute("IISxpress")];
[assembly:AssemblyCopyrightAttribute(IISXPRESS_COPYRIGHT_STRING)];
[assembly:AssemblyTrademarkAttribute("")];
[assembly:AssemblyCultureAttribute("")];

//
// Version information for an assembly consists of the following four values:
//
//      Major Version
//      Minor Version
//      Build Number
//      Revision
//
// You can specify all the value or you can default the Revision and Build Numbers
// by using the '*' as shown below:

[assembly:AssemblyVersionAttribute(IISXPRESS_FILEVERSION_STRING)];

[assembly:ComVisible(false)];

[assembly:CLSCompliantAttribute(true)];

[assembly:SecurityPermission(SecurityAction::RequestMinimum, UnmanagedCode = true)];

// we need to delay load since the auto manifest mashes the linker's hash
[assembly:AssemblyDelaySignAttribute(true)];
[assembly:AssemblyKeyFileAttribute("IISxpressKeyPair.snk")];
[assembly:AssemblyKeyNameAttribute("")];

