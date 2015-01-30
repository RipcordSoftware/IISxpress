

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0500 */
/* at Mon Dec 13 22:55:14 2010
 */
/* Compiler settings for .\IISxpressServer.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __IISxpressServer_h__
#define __IISxpressServer_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IComIISxpressRegSettings_FWD_DEFINED__
#define __IComIISxpressRegSettings_FWD_DEFINED__
typedef interface IComIISxpressRegSettings IComIISxpressRegSettings;
#endif 	/* __IComIISxpressRegSettings_FWD_DEFINED__ */


#ifndef __ICompressionRuleManager_FWD_DEFINED__
#define __ICompressionRuleManager_FWD_DEFINED__
typedef interface ICompressionRuleManager ICompressionRuleManager;
#endif 	/* __ICompressionRuleManager_FWD_DEFINED__ */


#ifndef __IIISxpressSettings_FWD_DEFINED__
#define __IIISxpressSettings_FWD_DEFINED__
typedef interface IIISxpressSettings IIISxpressSettings;
#endif 	/* __IIISxpressSettings_FWD_DEFINED__ */


#ifndef __IEnumStaticFileInfo_FWD_DEFINED__
#define __IEnumStaticFileInfo_FWD_DEFINED__
typedef interface IEnumStaticFileInfo IEnumStaticFileInfo;
#endif 	/* __IEnumStaticFileInfo_FWD_DEFINED__ */


#ifndef __IStaticFileRules_FWD_DEFINED__
#define __IStaticFileRules_FWD_DEFINED__
typedef interface IStaticFileRules IStaticFileRules;
#endif 	/* __IStaticFileRules_FWD_DEFINED__ */


#ifndef __IStaticFileRules2_FWD_DEFINED__
#define __IStaticFileRules2_FWD_DEFINED__
typedef interface IStaticFileRules2 IStaticFileRules2;
#endif 	/* __IStaticFileRules2_FWD_DEFINED__ */


#ifndef __ICustomCompressionMode_FWD_DEFINED__
#define __ICustomCompressionMode_FWD_DEFINED__
typedef interface ICustomCompressionMode ICustomCompressionMode;
#endif 	/* __ICustomCompressionMode_FWD_DEFINED__ */


#ifndef __IEnumExclusionRule_FWD_DEFINED__
#define __IEnumExclusionRule_FWD_DEFINED__
typedef interface IEnumExclusionRule IEnumExclusionRule;
#endif 	/* __IEnumExclusionRule_FWD_DEFINED__ */


#ifndef __INeverCompressRules_FWD_DEFINED__
#define __INeverCompressRules_FWD_DEFINED__
typedef interface INeverCompressRules INeverCompressRules;
#endif 	/* __INeverCompressRules_FWD_DEFINED__ */


#ifndef __INeverCompressRulesNotify_FWD_DEFINED__
#define __INeverCompressRulesNotify_FWD_DEFINED__
typedef interface INeverCompressRulesNotify INeverCompressRulesNotify;
#endif 	/* __INeverCompressRulesNotify_FWD_DEFINED__ */


#ifndef __INotifyCookie_FWD_DEFINED__
#define __INotifyCookie_FWD_DEFINED__
typedef interface INotifyCookie INotifyCookie;
#endif 	/* __INotifyCookie_FWD_DEFINED__ */


#ifndef __ICompressionHistory_FWD_DEFINED__
#define __ICompressionHistory_FWD_DEFINED__
typedef interface ICompressionHistory ICompressionHistory;
#endif 	/* __ICompressionHistory_FWD_DEFINED__ */


#ifndef __ICompressionHistory2_FWD_DEFINED__
#define __ICompressionHistory2_FWD_DEFINED__
typedef interface ICompressionHistory2 ICompressionHistory2;
#endif 	/* __ICompressionHistory2_FWD_DEFINED__ */


#ifndef __ICompressionStats_FWD_DEFINED__
#define __ICompressionStats_FWD_DEFINED__
typedef interface ICompressionStats ICompressionStats;
#endif 	/* __ICompressionStats_FWD_DEFINED__ */


#ifndef __ICompressionStats2_FWD_DEFINED__
#define __ICompressionStats2_FWD_DEFINED__
typedef interface ICompressionStats2 ICompressionStats2;
#endif 	/* __ICompressionStats2_FWD_DEFINED__ */


#ifndef __IProductLicenseManager_FWD_DEFINED__
#define __IProductLicenseManager_FWD_DEFINED__
typedef interface IProductLicenseManager IProductLicenseManager;
#endif 	/* __IProductLicenseManager_FWD_DEFINED__ */


#ifndef __IAHAState_FWD_DEFINED__
#define __IAHAState_FWD_DEFINED__
typedef interface IAHAState IAHAState;
#endif 	/* __IAHAState_FWD_DEFINED__ */


#ifndef __IIISxpressHTTPRequest_FWD_DEFINED__
#define __IIISxpressHTTPRequest_FWD_DEFINED__
typedef interface IIISxpressHTTPRequest IIISxpressHTTPRequest;
#endif 	/* __IIISxpressHTTPRequest_FWD_DEFINED__ */


#ifndef __IIISxpressHTTPNotify_FWD_DEFINED__
#define __IIISxpressHTTPNotify_FWD_DEFINED__
typedef interface IIISxpressHTTPNotify IIISxpressHTTPNotify;
#endif 	/* __IIISxpressHTTPNotify_FWD_DEFINED__ */


#ifndef __AsyncIIISxpressHTTPNotify_FWD_DEFINED__
#define __AsyncIIISxpressHTTPNotify_FWD_DEFINED__
typedef interface AsyncIIISxpressHTTPNotify AsyncIIISxpressHTTPNotify;
#endif 	/* __AsyncIIISxpressHTTPNotify_FWD_DEFINED__ */


#ifndef __IISxpressHTTPRequest_FWD_DEFINED__
#define __IISxpressHTTPRequest_FWD_DEFINED__

#ifdef __cplusplus
typedef class IISxpressHTTPRequest IISxpressHTTPRequest;
#else
typedef struct IISxpressHTTPRequest IISxpressHTTPRequest;
#endif /* __cplusplus */

#endif 	/* __IISxpressHTTPRequest_FWD_DEFINED__ */


#ifndef __CompressionRuleManager_FWD_DEFINED__
#define __CompressionRuleManager_FWD_DEFINED__

#ifdef __cplusplus
typedef class CompressionRuleManager CompressionRuleManager;
#else
typedef struct CompressionRuleManager CompressionRuleManager;
#endif /* __cplusplus */

#endif 	/* __CompressionRuleManager_FWD_DEFINED__ */


#ifndef __EnumStaticFileInfo_FWD_DEFINED__
#define __EnumStaticFileInfo_FWD_DEFINED__

#ifdef __cplusplus
typedef class EnumStaticFileInfo EnumStaticFileInfo;
#else
typedef struct EnumStaticFileInfo EnumStaticFileInfo;
#endif /* __cplusplus */

#endif 	/* __EnumStaticFileInfo_FWD_DEFINED__ */


#ifndef __ComIISxpressRegSettings_FWD_DEFINED__
#define __ComIISxpressRegSettings_FWD_DEFINED__

#ifdef __cplusplus
typedef class ComIISxpressRegSettings ComIISxpressRegSettings;
#else
typedef struct ComIISxpressRegSettings ComIISxpressRegSettings;
#endif /* __cplusplus */

#endif 	/* __ComIISxpressRegSettings_FWD_DEFINED__ */


#ifndef __EnumExclusionRule_FWD_DEFINED__
#define __EnumExclusionRule_FWD_DEFINED__

#ifdef __cplusplus
typedef class EnumExclusionRule EnumExclusionRule;
#else
typedef struct EnumExclusionRule EnumExclusionRule;
#endif /* __cplusplus */

#endif 	/* __EnumExclusionRule_FWD_DEFINED__ */


#ifndef __ProductLicenseManager_FWD_DEFINED__
#define __ProductLicenseManager_FWD_DEFINED__

#ifdef __cplusplus
typedef class ProductLicenseManager ProductLicenseManager;
#else
typedef struct ProductLicenseManager ProductLicenseManager;
#endif /* __cplusplus */

#endif 	/* __ProductLicenseManager_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


/* interface __MIDL_itf_IISxpressServer_0000_0000 */
/* [local] */ 

typedef /* [public][public][public][public][public][public][public] */ struct __MIDL___MIDL_itf_IISxpressServer_0000_0000_0001
    {
    LPCSTR pszServerName;
    LPCSTR pszServerPort;
    LPCSTR pszInstanceId;
    LPCSTR pszURLMapPath;
    } 	IISInfo;

typedef /* [public][public][public][public][public][public][public] */ struct __MIDL___MIDL_itf_IISxpressServer_0000_0000_0002
    {
    LPCSTR pszUserAgent;
    LPCSTR pszHostname;
    LPCSTR pszURI;
    LPCSTR pszQueryString;
    LPCSTR pszAcceptEncoding;
    LPCSTR pszRemoteAddress;
    } 	RequestInfo;

typedef /* [public][public][public][public][public][public][public] */ struct __MIDL___MIDL_itf_IISxpressServer_0000_0000_0003
    {
    DWORD dwResponseCode;
    LPCSTR pszContentType;
    DWORD dwContentLength;
    LPCSTR pszLastModified;
    } 	ResponseInfo;

typedef struct tagStaticFileInfo
    {
    BSTR bsPath;
    BSTR bsFilename;
    int nUserCompression;
    int nSystemCompression;
    } 	StaticFileInfo;

typedef struct tagFileCompressionHistory
    {
    DWORD dwDownloads;
    VARIANT vLastDownload;
    DWORD dwRawSize;
    BOOL bCompressed;
    DWORD dwCompressedSize;
    } 	FileCompressionHistory;

typedef struct tagExclusionRuleInfo
    {
    BSTR bsRule;
    BSTR bsInstance;
    DWORD dwFlags;
    int nUserAgents;
    BSTR *pbsUserAgentIDs;
    } 	ExclusionRuleInfo;

#define	RULEFLAGS_FOLDER_NOMATCH	( 0 )

#define	RULEFLAGS_FOLDER_WILDCARDMATCH	( 1 )

#define	RULEFLAGS_FOLDER_EXACTMATCH	( 2 )

typedef /* [public][public][public] */ 
enum __MIDL___MIDL_itf_IISxpressServer_0000_0000_0004
    {	LicenseModeNone	= 0,
	LicenseModeCommunity	= 1,
	LicenseModeEvaluation	= 2,
	LicenseModeFull	= 3,
	LicenseModeAHA	= 4
    } 	LicenseMode;

typedef /* [public][public][public] */ 
enum __MIDL___MIDL_itf_IISxpressServer_0000_0000_0005
    {	BZIP2ModeDisabled	= 0,
	BZIP2ModeStaticOnly	= 1,
	BZIP2ModeAll	= 2
    } 	BZIP2Mode;



extern RPC_IF_HANDLE __MIDL_itf_IISxpressServer_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_IISxpressServer_0000_0000_v0_0_s_ifspec;

#ifndef __IComIISxpressRegSettings_INTERFACE_DEFINED__
#define __IComIISxpressRegSettings_INTERFACE_DEFINED__

/* interface IComIISxpressRegSettings */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IComIISxpressRegSettings;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("968AF704-C7F8-409F-821B-5B4EC9AA81A2")
    IComIISxpressRegSettings : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE get_Enabled( 
            /* [retval][out] */ BOOL *pbEnabled) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE put_Enabled( 
            /* [in] */ BOOL bEnabled) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE get_DebugEnabled( 
            /* [retval][out] */ BOOL *pbEnabled) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE put_DebugEnabled( 
            /* [in] */ BOOL bEnabled) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE get_LoggingLevel( 
            /* [retval][out] */ DWORD *pdwLevel) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE put_LoggingLevel( 
            /* [in] */ DWORD dwLevel) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE get_CheckPhysicalMemory( 
            /* [out] */ BOOL *pbCheck) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE get_CheckCPULoad( 
            /* [out] */ BOOL *pbCheck) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE get_CheckNeverRules( 
            /* [out] */ BOOL *pbCheck) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE get_CheckUserRules( 
            /* [out] */ BOOL *pbCheck) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE get_LicenseMode( 
            /* [out] */ LicenseMode *pMode) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE put_LicenseMode( 
            LicenseMode Mode) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE get_BZIP2Mode( 
            /* [retval][out] */ BZIP2Mode *pMode) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE put_BZIP2Mode( 
            /* [in] */ BZIP2Mode mode) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE get_AHAEnabled( 
            /* [retval][out] */ DWORD *pdwEnabled) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE put_AHAEnabled( 
            /* [in] */ DWORD dwEnabled) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE get_AHADetectOverheat( 
            /* [retval][out] */ DWORD *pdwDetectOverheat) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE put_AHADetectOverheat( 
            /* [in] */ DWORD dwDetectOverheat) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IComIISxpressRegSettingsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IComIISxpressRegSettings * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IComIISxpressRegSettings * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IComIISxpressRegSettings * This);
        
        HRESULT ( STDMETHODCALLTYPE *get_Enabled )( 
            IComIISxpressRegSettings * This,
            /* [retval][out] */ BOOL *pbEnabled);
        
        HRESULT ( STDMETHODCALLTYPE *put_Enabled )( 
            IComIISxpressRegSettings * This,
            /* [in] */ BOOL bEnabled);
        
        HRESULT ( STDMETHODCALLTYPE *get_DebugEnabled )( 
            IComIISxpressRegSettings * This,
            /* [retval][out] */ BOOL *pbEnabled);
        
        HRESULT ( STDMETHODCALLTYPE *put_DebugEnabled )( 
            IComIISxpressRegSettings * This,
            /* [in] */ BOOL bEnabled);
        
        HRESULT ( STDMETHODCALLTYPE *get_LoggingLevel )( 
            IComIISxpressRegSettings * This,
            /* [retval][out] */ DWORD *pdwLevel);
        
        HRESULT ( STDMETHODCALLTYPE *put_LoggingLevel )( 
            IComIISxpressRegSettings * This,
            /* [in] */ DWORD dwLevel);
        
        HRESULT ( STDMETHODCALLTYPE *get_CheckPhysicalMemory )( 
            IComIISxpressRegSettings * This,
            /* [out] */ BOOL *pbCheck);
        
        HRESULT ( STDMETHODCALLTYPE *get_CheckCPULoad )( 
            IComIISxpressRegSettings * This,
            /* [out] */ BOOL *pbCheck);
        
        HRESULT ( STDMETHODCALLTYPE *get_CheckNeverRules )( 
            IComIISxpressRegSettings * This,
            /* [out] */ BOOL *pbCheck);
        
        HRESULT ( STDMETHODCALLTYPE *get_CheckUserRules )( 
            IComIISxpressRegSettings * This,
            /* [out] */ BOOL *pbCheck);
        
        HRESULT ( STDMETHODCALLTYPE *get_LicenseMode )( 
            IComIISxpressRegSettings * This,
            /* [out] */ LicenseMode *pMode);
        
        HRESULT ( STDMETHODCALLTYPE *put_LicenseMode )( 
            IComIISxpressRegSettings * This,
            LicenseMode Mode);
        
        HRESULT ( STDMETHODCALLTYPE *get_BZIP2Mode )( 
            IComIISxpressRegSettings * This,
            /* [retval][out] */ BZIP2Mode *pMode);
        
        HRESULT ( STDMETHODCALLTYPE *put_BZIP2Mode )( 
            IComIISxpressRegSettings * This,
            /* [in] */ BZIP2Mode mode);
        
        HRESULT ( STDMETHODCALLTYPE *get_AHAEnabled )( 
            IComIISxpressRegSettings * This,
            /* [retval][out] */ DWORD *pdwEnabled);
        
        HRESULT ( STDMETHODCALLTYPE *put_AHAEnabled )( 
            IComIISxpressRegSettings * This,
            /* [in] */ DWORD dwEnabled);
        
        HRESULT ( STDMETHODCALLTYPE *get_AHADetectOverheat )( 
            IComIISxpressRegSettings * This,
            /* [retval][out] */ DWORD *pdwDetectOverheat);
        
        HRESULT ( STDMETHODCALLTYPE *put_AHADetectOverheat )( 
            IComIISxpressRegSettings * This,
            /* [in] */ DWORD dwDetectOverheat);
        
        END_INTERFACE
    } IComIISxpressRegSettingsVtbl;

    interface IComIISxpressRegSettings
    {
        CONST_VTBL struct IComIISxpressRegSettingsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IComIISxpressRegSettings_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IComIISxpressRegSettings_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IComIISxpressRegSettings_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IComIISxpressRegSettings_get_Enabled(This,pbEnabled)	\
    ( (This)->lpVtbl -> get_Enabled(This,pbEnabled) ) 

#define IComIISxpressRegSettings_put_Enabled(This,bEnabled)	\
    ( (This)->lpVtbl -> put_Enabled(This,bEnabled) ) 

#define IComIISxpressRegSettings_get_DebugEnabled(This,pbEnabled)	\
    ( (This)->lpVtbl -> get_DebugEnabled(This,pbEnabled) ) 

#define IComIISxpressRegSettings_put_DebugEnabled(This,bEnabled)	\
    ( (This)->lpVtbl -> put_DebugEnabled(This,bEnabled) ) 

#define IComIISxpressRegSettings_get_LoggingLevel(This,pdwLevel)	\
    ( (This)->lpVtbl -> get_LoggingLevel(This,pdwLevel) ) 

#define IComIISxpressRegSettings_put_LoggingLevel(This,dwLevel)	\
    ( (This)->lpVtbl -> put_LoggingLevel(This,dwLevel) ) 

#define IComIISxpressRegSettings_get_CheckPhysicalMemory(This,pbCheck)	\
    ( (This)->lpVtbl -> get_CheckPhysicalMemory(This,pbCheck) ) 

#define IComIISxpressRegSettings_get_CheckCPULoad(This,pbCheck)	\
    ( (This)->lpVtbl -> get_CheckCPULoad(This,pbCheck) ) 

#define IComIISxpressRegSettings_get_CheckNeverRules(This,pbCheck)	\
    ( (This)->lpVtbl -> get_CheckNeverRules(This,pbCheck) ) 

#define IComIISxpressRegSettings_get_CheckUserRules(This,pbCheck)	\
    ( (This)->lpVtbl -> get_CheckUserRules(This,pbCheck) ) 

#define IComIISxpressRegSettings_get_LicenseMode(This,pMode)	\
    ( (This)->lpVtbl -> get_LicenseMode(This,pMode) ) 

#define IComIISxpressRegSettings_put_LicenseMode(This,Mode)	\
    ( (This)->lpVtbl -> put_LicenseMode(This,Mode) ) 

#define IComIISxpressRegSettings_get_BZIP2Mode(This,pMode)	\
    ( (This)->lpVtbl -> get_BZIP2Mode(This,pMode) ) 

#define IComIISxpressRegSettings_put_BZIP2Mode(This,mode)	\
    ( (This)->lpVtbl -> put_BZIP2Mode(This,mode) ) 

#define IComIISxpressRegSettings_get_AHAEnabled(This,pdwEnabled)	\
    ( (This)->lpVtbl -> get_AHAEnabled(This,pdwEnabled) ) 

#define IComIISxpressRegSettings_put_AHAEnabled(This,dwEnabled)	\
    ( (This)->lpVtbl -> put_AHAEnabled(This,dwEnabled) ) 

#define IComIISxpressRegSettings_get_AHADetectOverheat(This,pdwDetectOverheat)	\
    ( (This)->lpVtbl -> get_AHADetectOverheat(This,pdwDetectOverheat) ) 

#define IComIISxpressRegSettings_put_AHADetectOverheat(This,dwDetectOverheat)	\
    ( (This)->lpVtbl -> put_AHADetectOverheat(This,dwDetectOverheat) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IComIISxpressRegSettings_INTERFACE_DEFINED__ */


#ifndef __ICompressionRuleManager_INTERFACE_DEFINED__
#define __ICompressionRuleManager_INTERFACE_DEFINED__

/* interface ICompressionRuleManager */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_ICompressionRuleManager;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("C371FBDC-9888-43BD-B1A6-46917A9630BE")
    ICompressionRuleManager : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Init( 
            /* [in] */ IStorage *pStorage,
            /* [in] */ IComIISxpressRegSettings *pRegSettings) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CanCompressFile( 
            /* [in] */ LPCSTR pszPath,
            /* [in] */ LPCSTR pszFilename,
            /* [in] */ DWORD dwFileSize) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICompressionRuleManagerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICompressionRuleManager * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICompressionRuleManager * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICompressionRuleManager * This);
        
        HRESULT ( STDMETHODCALLTYPE *Init )( 
            ICompressionRuleManager * This,
            /* [in] */ IStorage *pStorage,
            /* [in] */ IComIISxpressRegSettings *pRegSettings);
        
        HRESULT ( STDMETHODCALLTYPE *CanCompressFile )( 
            ICompressionRuleManager * This,
            /* [in] */ LPCSTR pszPath,
            /* [in] */ LPCSTR pszFilename,
            /* [in] */ DWORD dwFileSize);
        
        END_INTERFACE
    } ICompressionRuleManagerVtbl;

    interface ICompressionRuleManager
    {
        CONST_VTBL struct ICompressionRuleManagerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICompressionRuleManager_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ICompressionRuleManager_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ICompressionRuleManager_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ICompressionRuleManager_Init(This,pStorage,pRegSettings)	\
    ( (This)->lpVtbl -> Init(This,pStorage,pRegSettings) ) 

#define ICompressionRuleManager_CanCompressFile(This,pszPath,pszFilename,dwFileSize)	\
    ( (This)->lpVtbl -> CanCompressFile(This,pszPath,pszFilename,dwFileSize) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ICompressionRuleManager_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_IISxpressServer_0000_0002 */
/* [local] */ 

typedef /* [public][public][public][public][public][public] */ 
enum __MIDL___MIDL_itf_IISxpressServer_0000_0002_0001
    {	CompressionModeNone	= 0,
	CompressionModeSpeed	= 1,
	CompressionModeSize	= 2,
	CompressionModeDefault	= 3
    } 	CompressionMode;

#define	IISXPRESS_SERVER_PROCESSORAFFINITY_OFF	( 0 )

#define	IISXPRESS_SERVER_PROCESSORAFFINITY_AUTO	( -1 )



extern RPC_IF_HANDLE __MIDL_itf_IISxpressServer_0000_0002_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_IISxpressServer_0000_0002_v0_0_s_ifspec;

#ifndef __IIISxpressSettings_INTERFACE_DEFINED__
#define __IIISxpressSettings_INTERFACE_DEFINED__

/* interface IIISxpressSettings */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IIISxpressSettings;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("2371FBDC-288C-43BD-B1A3-46917A9630B4")
    IIISxpressSettings : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE get_CompressionMode( 
            /* [retval][out] */ CompressionMode *pMode) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE put_CompressionMode( 
            /* [in] */ CompressionMode Mode) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE get_ProcessorAffinity( 
            /* [out] */ DWORD *pdwProcessorAffinity) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE put_ProcessorAffinity( 
            /* [in] */ DWORD dwProcessorAffinity) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IIISxpressSettingsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IIISxpressSettings * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IIISxpressSettings * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IIISxpressSettings * This);
        
        HRESULT ( STDMETHODCALLTYPE *get_CompressionMode )( 
            IIISxpressSettings * This,
            /* [retval][out] */ CompressionMode *pMode);
        
        HRESULT ( STDMETHODCALLTYPE *put_CompressionMode )( 
            IIISxpressSettings * This,
            /* [in] */ CompressionMode Mode);
        
        HRESULT ( STDMETHODCALLTYPE *get_ProcessorAffinity )( 
            IIISxpressSettings * This,
            /* [out] */ DWORD *pdwProcessorAffinity);
        
        HRESULT ( STDMETHODCALLTYPE *put_ProcessorAffinity )( 
            IIISxpressSettings * This,
            /* [in] */ DWORD dwProcessorAffinity);
        
        END_INTERFACE
    } IIISxpressSettingsVtbl;

    interface IIISxpressSettings
    {
        CONST_VTBL struct IIISxpressSettingsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IIISxpressSettings_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IIISxpressSettings_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IIISxpressSettings_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IIISxpressSettings_get_CompressionMode(This,pMode)	\
    ( (This)->lpVtbl -> get_CompressionMode(This,pMode) ) 

#define IIISxpressSettings_put_CompressionMode(This,Mode)	\
    ( (This)->lpVtbl -> put_CompressionMode(This,Mode) ) 

#define IIISxpressSettings_get_ProcessorAffinity(This,pdwProcessorAffinity)	\
    ( (This)->lpVtbl -> get_ProcessorAffinity(This,pdwProcessorAffinity) ) 

#define IIISxpressSettings_put_ProcessorAffinity(This,dwProcessorAffinity)	\
    ( (This)->lpVtbl -> put_ProcessorAffinity(This,dwProcessorAffinity) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IIISxpressSettings_INTERFACE_DEFINED__ */


#ifndef __IEnumStaticFileInfo_INTERFACE_DEFINED__
#define __IEnumStaticFileInfo_INTERFACE_DEFINED__

/* interface IEnumStaticFileInfo */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IEnumStaticFileInfo;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("CE62A8D6-DA1D-4664-B170-172A59EDB676")
    IEnumStaticFileInfo : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Next( 
            /* [in] */ ULONG celt,
            /* [out] */ StaticFileInfo *rgelt,
            /* [out] */ ULONG *pceltFetched) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ ULONG celt) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Clone( 
            /* [out] */ IEnumStaticFileInfo **ppenum) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEnumStaticFileInfoVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IEnumStaticFileInfo * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IEnumStaticFileInfo * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IEnumStaticFileInfo * This);
        
        HRESULT ( STDMETHODCALLTYPE *Next )( 
            IEnumStaticFileInfo * This,
            /* [in] */ ULONG celt,
            /* [out] */ StaticFileInfo *rgelt,
            /* [out] */ ULONG *pceltFetched);
        
        HRESULT ( STDMETHODCALLTYPE *Skip )( 
            IEnumStaticFileInfo * This,
            /* [in] */ ULONG celt);
        
        HRESULT ( STDMETHODCALLTYPE *Reset )( 
            IEnumStaticFileInfo * This);
        
        HRESULT ( STDMETHODCALLTYPE *Clone )( 
            IEnumStaticFileInfo * This,
            /* [out] */ IEnumStaticFileInfo **ppenum);
        
        END_INTERFACE
    } IEnumStaticFileInfoVtbl;

    interface IEnumStaticFileInfo
    {
        CONST_VTBL struct IEnumStaticFileInfoVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnumStaticFileInfo_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IEnumStaticFileInfo_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IEnumStaticFileInfo_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IEnumStaticFileInfo_Next(This,celt,rgelt,pceltFetched)	\
    ( (This)->lpVtbl -> Next(This,celt,rgelt,pceltFetched) ) 

#define IEnumStaticFileInfo_Skip(This,celt)	\
    ( (This)->lpVtbl -> Skip(This,celt) ) 

#define IEnumStaticFileInfo_Reset(This)	\
    ( (This)->lpVtbl -> Reset(This) ) 

#define IEnumStaticFileInfo_Clone(This,ppenum)	\
    ( (This)->lpVtbl -> Clone(This,ppenum) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IEnumStaticFileInfo_INTERFACE_DEFINED__ */


#ifndef __IStaticFileRules_INTERFACE_DEFINED__
#define __IStaticFileRules_INTERFACE_DEFINED__

/* interface IStaticFileRules */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IStaticFileRules;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("A371FBDC-9888-43BD-11A6-46917A9630B2")
    IStaticFileRules : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Init( 
            /* [in] */ IStorage *pStorage) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE IsKnownFile( 
            /* [in] */ LPCSTR pszFullFilePath) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE AddFileRule( 
            /* [in] */ LPCSTR pszFullFilePath,
            /* [in] */ int nUserCompress,
            /* [in] */ int nSystemCompress) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RemoveFileRule( 
            /* [in] */ LPCSTR pszFullFilePath) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetFileRule( 
            /* [in] */ LPCSTR pszFullFilePath,
            /* [out] */ int *pnUserCompress,
            /* [out] */ int *pnSystemCompress) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetFileRule( 
            /* [in] */ LPCSTR pszFullFilePath,
            /* [in] */ int nUserCompress,
            /* [in] */ int nSystemCompress) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnumByPath( 
            /* [in] */ LPCSTR pszPath,
            /* [out] */ IEnumStaticFileInfo **ppEnum) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IStaticFileRulesVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IStaticFileRules * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IStaticFileRules * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IStaticFileRules * This);
        
        HRESULT ( STDMETHODCALLTYPE *Init )( 
            IStaticFileRules * This,
            /* [in] */ IStorage *pStorage);
        
        HRESULT ( STDMETHODCALLTYPE *IsKnownFile )( 
            IStaticFileRules * This,
            /* [in] */ LPCSTR pszFullFilePath);
        
        HRESULT ( STDMETHODCALLTYPE *AddFileRule )( 
            IStaticFileRules * This,
            /* [in] */ LPCSTR pszFullFilePath,
            /* [in] */ int nUserCompress,
            /* [in] */ int nSystemCompress);
        
        HRESULT ( STDMETHODCALLTYPE *RemoveFileRule )( 
            IStaticFileRules * This,
            /* [in] */ LPCSTR pszFullFilePath);
        
        HRESULT ( STDMETHODCALLTYPE *GetFileRule )( 
            IStaticFileRules * This,
            /* [in] */ LPCSTR pszFullFilePath,
            /* [out] */ int *pnUserCompress,
            /* [out] */ int *pnSystemCompress);
        
        HRESULT ( STDMETHODCALLTYPE *SetFileRule )( 
            IStaticFileRules * This,
            /* [in] */ LPCSTR pszFullFilePath,
            /* [in] */ int nUserCompress,
            /* [in] */ int nSystemCompress);
        
        HRESULT ( STDMETHODCALLTYPE *EnumByPath )( 
            IStaticFileRules * This,
            /* [in] */ LPCSTR pszPath,
            /* [out] */ IEnumStaticFileInfo **ppEnum);
        
        END_INTERFACE
    } IStaticFileRulesVtbl;

    interface IStaticFileRules
    {
        CONST_VTBL struct IStaticFileRulesVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IStaticFileRules_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IStaticFileRules_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IStaticFileRules_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IStaticFileRules_Init(This,pStorage)	\
    ( (This)->lpVtbl -> Init(This,pStorage) ) 

#define IStaticFileRules_IsKnownFile(This,pszFullFilePath)	\
    ( (This)->lpVtbl -> IsKnownFile(This,pszFullFilePath) ) 

#define IStaticFileRules_AddFileRule(This,pszFullFilePath,nUserCompress,nSystemCompress)	\
    ( (This)->lpVtbl -> AddFileRule(This,pszFullFilePath,nUserCompress,nSystemCompress) ) 

#define IStaticFileRules_RemoveFileRule(This,pszFullFilePath)	\
    ( (This)->lpVtbl -> RemoveFileRule(This,pszFullFilePath) ) 

#define IStaticFileRules_GetFileRule(This,pszFullFilePath,pnUserCompress,pnSystemCompress)	\
    ( (This)->lpVtbl -> GetFileRule(This,pszFullFilePath,pnUserCompress,pnSystemCompress) ) 

#define IStaticFileRules_SetFileRule(This,pszFullFilePath,nUserCompress,nSystemCompress)	\
    ( (This)->lpVtbl -> SetFileRule(This,pszFullFilePath,nUserCompress,nSystemCompress) ) 

#define IStaticFileRules_EnumByPath(This,pszPath,ppEnum)	\
    ( (This)->lpVtbl -> EnumByPath(This,pszPath,ppEnum) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IStaticFileRules_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_IISxpressServer_0000_0005 */
/* [local] */ 

typedef struct tagCustomCompressionSettings
    {
    BOOL bEnabled;
    CompressionMode mode;
    int nLevel;
    int nWindowBits;
    int nMemoryLevel;
    } 	CustomCompressionSettings;



extern RPC_IF_HANDLE __MIDL_itf_IISxpressServer_0000_0005_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_IISxpressServer_0000_0005_v0_0_s_ifspec;

#ifndef __IStaticFileRules2_INTERFACE_DEFINED__
#define __IStaticFileRules2_INTERFACE_DEFINED__

/* interface IStaticFileRules2 */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IStaticFileRules2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("A371FBDC-9888-43BD-11A6-469173963067")
    IStaticFileRules2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetFileRule( 
            /* [in] */ LPCSTR pszFullFilePath,
            /* [out] */ int *pnUserCompress,
            /* [out] */ int *pnSystemCompress,
            /* [out] */ CustomCompressionSettings *pCustomCompressionSettings) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IStaticFileRules2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IStaticFileRules2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IStaticFileRules2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IStaticFileRules2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetFileRule )( 
            IStaticFileRules2 * This,
            /* [in] */ LPCSTR pszFullFilePath,
            /* [out] */ int *pnUserCompress,
            /* [out] */ int *pnSystemCompress,
            /* [out] */ CustomCompressionSettings *pCustomCompressionSettings);
        
        END_INTERFACE
    } IStaticFileRules2Vtbl;

    interface IStaticFileRules2
    {
        CONST_VTBL struct IStaticFileRules2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IStaticFileRules2_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IStaticFileRules2_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IStaticFileRules2_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IStaticFileRules2_GetFileRule(This,pszFullFilePath,pnUserCompress,pnSystemCompress,pCustomCompressionSettings)	\
    ( (This)->lpVtbl -> GetFileRule(This,pszFullFilePath,pnUserCompress,pnSystemCompress,pCustomCompressionSettings) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IStaticFileRules2_INTERFACE_DEFINED__ */


#ifndef __ICustomCompressionMode_INTERFACE_DEFINED__
#define __ICustomCompressionMode_INTERFACE_DEFINED__

/* interface ICustomCompressionMode */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_ICustomCompressionMode;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("A371FBDC-9888-43BD-11A6-46917A963613")
    ICustomCompressionMode : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetFileCompressionParams( 
            /* [in] */ LPCSTR pszFullFilePath,
            /* [out] */ CustomCompressionSettings *pCustomCompressionSettings) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetFileCompressionParams( 
            /* [in] */ LPCSTR pszFullFilePath,
            /* [in] */ CustomCompressionSettings *pCustomCompressionSettings) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RemoveFileCompressionParams( 
            /* [in] */ LPCSTR pszFullFilePath) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICustomCompressionModeVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICustomCompressionMode * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICustomCompressionMode * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICustomCompressionMode * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetFileCompressionParams )( 
            ICustomCompressionMode * This,
            /* [in] */ LPCSTR pszFullFilePath,
            /* [out] */ CustomCompressionSettings *pCustomCompressionSettings);
        
        HRESULT ( STDMETHODCALLTYPE *SetFileCompressionParams )( 
            ICustomCompressionMode * This,
            /* [in] */ LPCSTR pszFullFilePath,
            /* [in] */ CustomCompressionSettings *pCustomCompressionSettings);
        
        HRESULT ( STDMETHODCALLTYPE *RemoveFileCompressionParams )( 
            ICustomCompressionMode * This,
            /* [in] */ LPCSTR pszFullFilePath);
        
        END_INTERFACE
    } ICustomCompressionModeVtbl;

    interface ICustomCompressionMode
    {
        CONST_VTBL struct ICustomCompressionModeVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICustomCompressionMode_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ICustomCompressionMode_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ICustomCompressionMode_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ICustomCompressionMode_GetFileCompressionParams(This,pszFullFilePath,pCustomCompressionSettings)	\
    ( (This)->lpVtbl -> GetFileCompressionParams(This,pszFullFilePath,pCustomCompressionSettings) ) 

#define ICustomCompressionMode_SetFileCompressionParams(This,pszFullFilePath,pCustomCompressionSettings)	\
    ( (This)->lpVtbl -> SetFileCompressionParams(This,pszFullFilePath,pCustomCompressionSettings) ) 

#define ICustomCompressionMode_RemoveFileCompressionParams(This,pszFullFilePath)	\
    ( (This)->lpVtbl -> RemoveFileCompressionParams(This,pszFullFilePath) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ICustomCompressionMode_INTERFACE_DEFINED__ */


#ifndef __IEnumExclusionRule_INTERFACE_DEFINED__
#define __IEnumExclusionRule_INTERFACE_DEFINED__

/* interface IEnumExclusionRule */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IEnumExclusionRule;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("088876BE-75D9-40E8-BA49-8BB7EE5677E6")
    IEnumExclusionRule : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Next( 
            /* [in] */ ULONG celt,
            /* [out] */ ExclusionRuleInfo *rgelt,
            /* [out] */ ULONG *pceltFetched) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ ULONG celt) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Clone( 
            /* [out] */ IEnumExclusionRule **ppenum) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEnumExclusionRuleVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IEnumExclusionRule * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IEnumExclusionRule * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IEnumExclusionRule * This);
        
        HRESULT ( STDMETHODCALLTYPE *Next )( 
            IEnumExclusionRule * This,
            /* [in] */ ULONG celt,
            /* [out] */ ExclusionRuleInfo *rgelt,
            /* [out] */ ULONG *pceltFetched);
        
        HRESULT ( STDMETHODCALLTYPE *Skip )( 
            IEnumExclusionRule * This,
            /* [in] */ ULONG celt);
        
        HRESULT ( STDMETHODCALLTYPE *Reset )( 
            IEnumExclusionRule * This);
        
        HRESULT ( STDMETHODCALLTYPE *Clone )( 
            IEnumExclusionRule * This,
            /* [out] */ IEnumExclusionRule **ppenum);
        
        END_INTERFACE
    } IEnumExclusionRuleVtbl;

    interface IEnumExclusionRule
    {
        CONST_VTBL struct IEnumExclusionRuleVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnumExclusionRule_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IEnumExclusionRule_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IEnumExclusionRule_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IEnumExclusionRule_Next(This,celt,rgelt,pceltFetched)	\
    ( (This)->lpVtbl -> Next(This,celt,rgelt,pceltFetched) ) 

#define IEnumExclusionRule_Skip(This,celt)	\
    ( (This)->lpVtbl -> Skip(This,celt) ) 

#define IEnumExclusionRule_Reset(This)	\
    ( (This)->lpVtbl -> Reset(This) ) 

#define IEnumExclusionRule_Clone(This,ppenum)	\
    ( (This)->lpVtbl -> Clone(This,ppenum) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IEnumExclusionRule_INTERFACE_DEFINED__ */


#ifndef __INeverCompressRules_INTERFACE_DEFINED__
#define __INeverCompressRules_INTERFACE_DEFINED__

/* interface INeverCompressRules */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_INeverCompressRules;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("4321FBDC-3888-43BD-11A6-46917A9630B1")
    INeverCompressRules : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Init( 
            /* [in] */ IStorage *pStorage) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE AddExtensionRule( 
            /* [in] */ LPCSTR pszExtension,
            /* [in] */ LPCSTR pszUserAgent,
            /* [in] */ LPCSTR pszTypeString,
            /* [in] */ DWORD dwUpdateCookie) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE AddContentTypeRule( 
            /* [in] */ LPCSTR pszContentType,
            /* [in] */ LPCSTR pszUserAgent,
            /* [in] */ LPCSTR pszTypeString,
            /* [in] */ DWORD dwUpdateCookie) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE AddFolderRule( 
            /* [in] */ LPCSTR pszInstance,
            /* [in] */ LPCSTR pszFolder,
            /* [in] */ DWORD dwFlags,
            /* [in] */ DWORD dwUpdateCookie) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE AddIPRule( 
            /* [in] */ LPCSTR pszIPAddress,
            /* [in] */ DWORD dwUpdateCookie) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RemoveExtensionRule( 
            /* [in] */ LPCSTR pszExtension,
            /* [in] */ LPCSTR pszUserAgent,
            /* [in] */ DWORD dwUpdateCookie) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RemoveContentTypeRule( 
            /* [in] */ LPCSTR pszContentType,
            /* [in] */ LPCSTR pszUserAgent,
            /* [in] */ DWORD dwUpdateCookie) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RemoveFolderRule( 
            /* [in] */ LPCSTR pszInstance,
            /* [in] */ LPCSTR pszFolder,
            /* [in] */ DWORD dwUpdateCookie) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RemoveIPRule( 
            /* [in] */ LPCSTR pszIPAddress,
            /* [in] */ DWORD dwUpdateCookie) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE MatchRule( 
            /* [in] */ LPCSTR pszExtension,
            /* [in] */ LPCSTR pszContentType,
            /* [in] */ LPCSTR pszInstance,
            /* [in] */ LPCSTR pszFolder,
            /* [in] */ LPCSTR pszUserAgent,
            /* [in] */ LPCSTR pszIPAddress) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnumExtensions( 
            /* [out] */ IEnumExclusionRule **ppEnum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnumContentTypes( 
            /* [out] */ IEnumExclusionRule **ppEnum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnumFolders( 
            /* [out] */ IEnumExclusionRule **ppEnum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnumIPAddresses( 
            /* [out] */ IEnumExclusionRule **ppEnum) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INeverCompressRulesVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INeverCompressRules * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INeverCompressRules * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INeverCompressRules * This);
        
        HRESULT ( STDMETHODCALLTYPE *Init )( 
            INeverCompressRules * This,
            /* [in] */ IStorage *pStorage);
        
        HRESULT ( STDMETHODCALLTYPE *AddExtensionRule )( 
            INeverCompressRules * This,
            /* [in] */ LPCSTR pszExtension,
            /* [in] */ LPCSTR pszUserAgent,
            /* [in] */ LPCSTR pszTypeString,
            /* [in] */ DWORD dwUpdateCookie);
        
        HRESULT ( STDMETHODCALLTYPE *AddContentTypeRule )( 
            INeverCompressRules * This,
            /* [in] */ LPCSTR pszContentType,
            /* [in] */ LPCSTR pszUserAgent,
            /* [in] */ LPCSTR pszTypeString,
            /* [in] */ DWORD dwUpdateCookie);
        
        HRESULT ( STDMETHODCALLTYPE *AddFolderRule )( 
            INeverCompressRules * This,
            /* [in] */ LPCSTR pszInstance,
            /* [in] */ LPCSTR pszFolder,
            /* [in] */ DWORD dwFlags,
            /* [in] */ DWORD dwUpdateCookie);
        
        HRESULT ( STDMETHODCALLTYPE *AddIPRule )( 
            INeverCompressRules * This,
            /* [in] */ LPCSTR pszIPAddress,
            /* [in] */ DWORD dwUpdateCookie);
        
        HRESULT ( STDMETHODCALLTYPE *RemoveExtensionRule )( 
            INeverCompressRules * This,
            /* [in] */ LPCSTR pszExtension,
            /* [in] */ LPCSTR pszUserAgent,
            /* [in] */ DWORD dwUpdateCookie);
        
        HRESULT ( STDMETHODCALLTYPE *RemoveContentTypeRule )( 
            INeverCompressRules * This,
            /* [in] */ LPCSTR pszContentType,
            /* [in] */ LPCSTR pszUserAgent,
            /* [in] */ DWORD dwUpdateCookie);
        
        HRESULT ( STDMETHODCALLTYPE *RemoveFolderRule )( 
            INeverCompressRules * This,
            /* [in] */ LPCSTR pszInstance,
            /* [in] */ LPCSTR pszFolder,
            /* [in] */ DWORD dwUpdateCookie);
        
        HRESULT ( STDMETHODCALLTYPE *RemoveIPRule )( 
            INeverCompressRules * This,
            /* [in] */ LPCSTR pszIPAddress,
            /* [in] */ DWORD dwUpdateCookie);
        
        HRESULT ( STDMETHODCALLTYPE *MatchRule )( 
            INeverCompressRules * This,
            /* [in] */ LPCSTR pszExtension,
            /* [in] */ LPCSTR pszContentType,
            /* [in] */ LPCSTR pszInstance,
            /* [in] */ LPCSTR pszFolder,
            /* [in] */ LPCSTR pszUserAgent,
            /* [in] */ LPCSTR pszIPAddress);
        
        HRESULT ( STDMETHODCALLTYPE *EnumExtensions )( 
            INeverCompressRules * This,
            /* [out] */ IEnumExclusionRule **ppEnum);
        
        HRESULT ( STDMETHODCALLTYPE *EnumContentTypes )( 
            INeverCompressRules * This,
            /* [out] */ IEnumExclusionRule **ppEnum);
        
        HRESULT ( STDMETHODCALLTYPE *EnumFolders )( 
            INeverCompressRules * This,
            /* [out] */ IEnumExclusionRule **ppEnum);
        
        HRESULT ( STDMETHODCALLTYPE *EnumIPAddresses )( 
            INeverCompressRules * This,
            /* [out] */ IEnumExclusionRule **ppEnum);
        
        END_INTERFACE
    } INeverCompressRulesVtbl;

    interface INeverCompressRules
    {
        CONST_VTBL struct INeverCompressRulesVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INeverCompressRules_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define INeverCompressRules_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define INeverCompressRules_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define INeverCompressRules_Init(This,pStorage)	\
    ( (This)->lpVtbl -> Init(This,pStorage) ) 

#define INeverCompressRules_AddExtensionRule(This,pszExtension,pszUserAgent,pszTypeString,dwUpdateCookie)	\
    ( (This)->lpVtbl -> AddExtensionRule(This,pszExtension,pszUserAgent,pszTypeString,dwUpdateCookie) ) 

#define INeverCompressRules_AddContentTypeRule(This,pszContentType,pszUserAgent,pszTypeString,dwUpdateCookie)	\
    ( (This)->lpVtbl -> AddContentTypeRule(This,pszContentType,pszUserAgent,pszTypeString,dwUpdateCookie) ) 

#define INeverCompressRules_AddFolderRule(This,pszInstance,pszFolder,dwFlags,dwUpdateCookie)	\
    ( (This)->lpVtbl -> AddFolderRule(This,pszInstance,pszFolder,dwFlags,dwUpdateCookie) ) 

#define INeverCompressRules_AddIPRule(This,pszIPAddress,dwUpdateCookie)	\
    ( (This)->lpVtbl -> AddIPRule(This,pszIPAddress,dwUpdateCookie) ) 

#define INeverCompressRules_RemoveExtensionRule(This,pszExtension,pszUserAgent,dwUpdateCookie)	\
    ( (This)->lpVtbl -> RemoveExtensionRule(This,pszExtension,pszUserAgent,dwUpdateCookie) ) 

#define INeverCompressRules_RemoveContentTypeRule(This,pszContentType,pszUserAgent,dwUpdateCookie)	\
    ( (This)->lpVtbl -> RemoveContentTypeRule(This,pszContentType,pszUserAgent,dwUpdateCookie) ) 

#define INeverCompressRules_RemoveFolderRule(This,pszInstance,pszFolder,dwUpdateCookie)	\
    ( (This)->lpVtbl -> RemoveFolderRule(This,pszInstance,pszFolder,dwUpdateCookie) ) 

#define INeverCompressRules_RemoveIPRule(This,pszIPAddress,dwUpdateCookie)	\
    ( (This)->lpVtbl -> RemoveIPRule(This,pszIPAddress,dwUpdateCookie) ) 

#define INeverCompressRules_MatchRule(This,pszExtension,pszContentType,pszInstance,pszFolder,pszUserAgent,pszIPAddress)	\
    ( (This)->lpVtbl -> MatchRule(This,pszExtension,pszContentType,pszInstance,pszFolder,pszUserAgent,pszIPAddress) ) 

#define INeverCompressRules_EnumExtensions(This,ppEnum)	\
    ( (This)->lpVtbl -> EnumExtensions(This,ppEnum) ) 

#define INeverCompressRules_EnumContentTypes(This,ppEnum)	\
    ( (This)->lpVtbl -> EnumContentTypes(This,ppEnum) ) 

#define INeverCompressRules_EnumFolders(This,ppEnum)	\
    ( (This)->lpVtbl -> EnumFolders(This,ppEnum) ) 

#define INeverCompressRules_EnumIPAddresses(This,ppEnum)	\
    ( (This)->lpVtbl -> EnumIPAddresses(This,ppEnum) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __INeverCompressRules_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_IISxpressServer_0000_0009 */
/* [local] */ 

typedef /* [public][public] */ 
enum __MIDL___MIDL_itf_IISxpressServer_0000_0009_0001
    {	NeverRuleChangeHintUnknown	= 0,
	NeverRuleChangeHintExtension	= 1,
	NeverRuleChangeHintContentType	= 2,
	NeverRuleChangeHintURI	= 3,
	NeverRuleChangeHintIP	= 4,
	NeverRuleChangeHintUserAgent	= 5
    } 	NeverRuleChangeHint;



extern RPC_IF_HANDLE __MIDL_itf_IISxpressServer_0000_0009_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_IISxpressServer_0000_0009_v0_0_s_ifspec;

#ifndef __INeverCompressRulesNotify_INTERFACE_DEFINED__
#define __INeverCompressRulesNotify_INTERFACE_DEFINED__

/* interface INeverCompressRulesNotify */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_INeverCompressRulesNotify;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("2321FBDC-3888-43BC-11AA-46947A9330B2")
    INeverCompressRulesNotify : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE OnRuleChanged( 
            /* [in] */ NeverRuleChangeHint hint,
            /* [in] */ DWORD dwUpdateCookie) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INeverCompressRulesNotifyVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INeverCompressRulesNotify * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INeverCompressRulesNotify * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INeverCompressRulesNotify * This);
        
        HRESULT ( STDMETHODCALLTYPE *OnRuleChanged )( 
            INeverCompressRulesNotify * This,
            /* [in] */ NeverRuleChangeHint hint,
            /* [in] */ DWORD dwUpdateCookie);
        
        END_INTERFACE
    } INeverCompressRulesNotifyVtbl;

    interface INeverCompressRulesNotify
    {
        CONST_VTBL struct INeverCompressRulesNotifyVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INeverCompressRulesNotify_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define INeverCompressRulesNotify_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define INeverCompressRulesNotify_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define INeverCompressRulesNotify_OnRuleChanged(This,hint,dwUpdateCookie)	\
    ( (This)->lpVtbl -> OnRuleChanged(This,hint,dwUpdateCookie) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __INeverCompressRulesNotify_INTERFACE_DEFINED__ */


#ifndef __INotifyCookie_INTERFACE_DEFINED__
#define __INotifyCookie_INTERFACE_DEFINED__

/* interface INotifyCookie */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_INotifyCookie;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("C321FBDC-3888-43B1-11AA-E6947A9330B8")
    INotifyCookie : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetCookie( 
            /* [out] */ DWORD *pdwCookie) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INotifyCookieVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INotifyCookie * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INotifyCookie * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INotifyCookie * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetCookie )( 
            INotifyCookie * This,
            /* [out] */ DWORD *pdwCookie);
        
        END_INTERFACE
    } INotifyCookieVtbl;

    interface INotifyCookie
    {
        CONST_VTBL struct INotifyCookieVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INotifyCookie_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define INotifyCookie_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define INotifyCookie_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define INotifyCookie_GetCookie(This,pdwCookie)	\
    ( (This)->lpVtbl -> GetCookie(This,pdwCookie) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __INotifyCookie_INTERFACE_DEFINED__ */


#ifndef __ICompressionHistory_INTERFACE_DEFINED__
#define __ICompressionHistory_INTERFACE_DEFINED__

/* interface ICompressionHistory */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_ICompressionHistory;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("F32AFBD1-3888-43BD-41A6-46917A9230B1")
    ICompressionHistory : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Init( 
            /* [in] */ IStorage *pStorage) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE UpdateEntry( 
            /* [in] */ LPCSTR pszFullFilePath,
            /* [in] */ DWORD dwRawSize,
            /* [in] */ BOOL bCompressed,
            /* [in] */ DWORD dwCompressedSize) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE QueryEntry( 
            /* [in] */ LPCSTR pszFullFilePath,
            /* [out] */ FileCompressionHistory *pHistory) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RemoveEntry( 
            /* [in] */ LPCSTR pszPath) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnumEntriesByPath( 
            /* [in] */ LPCSTR pszFullFilePath,
            /* [out] */ IEnumString **ppEnum) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICompressionHistoryVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICompressionHistory * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICompressionHistory * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICompressionHistory * This);
        
        HRESULT ( STDMETHODCALLTYPE *Init )( 
            ICompressionHistory * This,
            /* [in] */ IStorage *pStorage);
        
        HRESULT ( STDMETHODCALLTYPE *UpdateEntry )( 
            ICompressionHistory * This,
            /* [in] */ LPCSTR pszFullFilePath,
            /* [in] */ DWORD dwRawSize,
            /* [in] */ BOOL bCompressed,
            /* [in] */ DWORD dwCompressedSize);
        
        HRESULT ( STDMETHODCALLTYPE *QueryEntry )( 
            ICompressionHistory * This,
            /* [in] */ LPCSTR pszFullFilePath,
            /* [out] */ FileCompressionHistory *pHistory);
        
        HRESULT ( STDMETHODCALLTYPE *RemoveEntry )( 
            ICompressionHistory * This,
            /* [in] */ LPCSTR pszPath);
        
        HRESULT ( STDMETHODCALLTYPE *EnumEntriesByPath )( 
            ICompressionHistory * This,
            /* [in] */ LPCSTR pszFullFilePath,
            /* [out] */ IEnumString **ppEnum);
        
        END_INTERFACE
    } ICompressionHistoryVtbl;

    interface ICompressionHistory
    {
        CONST_VTBL struct ICompressionHistoryVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICompressionHistory_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ICompressionHistory_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ICompressionHistory_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ICompressionHistory_Init(This,pStorage)	\
    ( (This)->lpVtbl -> Init(This,pStorage) ) 

#define ICompressionHistory_UpdateEntry(This,pszFullFilePath,dwRawSize,bCompressed,dwCompressedSize)	\
    ( (This)->lpVtbl -> UpdateEntry(This,pszFullFilePath,dwRawSize,bCompressed,dwCompressedSize) ) 

#define ICompressionHistory_QueryEntry(This,pszFullFilePath,pHistory)	\
    ( (This)->lpVtbl -> QueryEntry(This,pszFullFilePath,pHistory) ) 

#define ICompressionHistory_RemoveEntry(This,pszPath)	\
    ( (This)->lpVtbl -> RemoveEntry(This,pszPath) ) 

#define ICompressionHistory_EnumEntriesByPath(This,pszFullFilePath,ppEnum)	\
    ( (This)->lpVtbl -> EnumEntriesByPath(This,pszFullFilePath,ppEnum) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ICompressionHistory_INTERFACE_DEFINED__ */


#ifndef __ICompressionHistory2_INTERFACE_DEFINED__
#define __ICompressionHistory2_INTERFACE_DEFINED__

/* interface ICompressionHistory2 */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_ICompressionHistory2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("F32AFBD1-3888-43BD-41A2-4691769230B2")
    ICompressionHistory2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE UpdateEntry( 
            /* [in] */ LPCSTR pszFullFilePath,
            /* [in] */ DWORD dwRawSize,
            /* [in] */ BOOL bCompressed,
            /* [in] */ DWORD dwCompressedSize,
            /* [in] */ BOOL bCacheHit) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICompressionHistory2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICompressionHistory2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICompressionHistory2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICompressionHistory2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *UpdateEntry )( 
            ICompressionHistory2 * This,
            /* [in] */ LPCSTR pszFullFilePath,
            /* [in] */ DWORD dwRawSize,
            /* [in] */ BOOL bCompressed,
            /* [in] */ DWORD dwCompressedSize,
            /* [in] */ BOOL bCacheHit);
        
        END_INTERFACE
    } ICompressionHistory2Vtbl;

    interface ICompressionHistory2
    {
        CONST_VTBL struct ICompressionHistory2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICompressionHistory2_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ICompressionHistory2_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ICompressionHistory2_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ICompressionHistory2_UpdateEntry(This,pszFullFilePath,dwRawSize,bCompressed,dwCompressedSize,bCacheHit)	\
    ( (This)->lpVtbl -> UpdateEntry(This,pszFullFilePath,dwRawSize,bCompressed,dwCompressedSize,bCacheHit) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ICompressionHistory2_INTERFACE_DEFINED__ */


#ifndef __ICompressionStats_INTERFACE_DEFINED__
#define __ICompressionStats_INTERFACE_DEFINED__

/* interface ICompressionStats */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_ICompressionStats;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("432AFBD3-3888-43BD-11A6-36917A9231B2")
    ICompressionStats : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE ResetStats( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetStats( 
            /* [out] */ DWORD *pdwFilesCompressed,
            /* [out] */ DWORD *pdwRawSize,
            /* [out] */ DWORD *pdwCompressedSize) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICompressionStatsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICompressionStats * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICompressionStats * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICompressionStats * This);
        
        HRESULT ( STDMETHODCALLTYPE *ResetStats )( 
            ICompressionStats * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetStats )( 
            ICompressionStats * This,
            /* [out] */ DWORD *pdwFilesCompressed,
            /* [out] */ DWORD *pdwRawSize,
            /* [out] */ DWORD *pdwCompressedSize);
        
        END_INTERFACE
    } ICompressionStatsVtbl;

    interface ICompressionStats
    {
        CONST_VTBL struct ICompressionStatsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICompressionStats_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ICompressionStats_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ICompressionStats_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ICompressionStats_ResetStats(This)	\
    ( (This)->lpVtbl -> ResetStats(This) ) 

#define ICompressionStats_GetStats(This,pdwFilesCompressed,pdwRawSize,pdwCompressedSize)	\
    ( (This)->lpVtbl -> GetStats(This,pdwFilesCompressed,pdwRawSize,pdwCompressedSize) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ICompressionStats_INTERFACE_DEFINED__ */


#ifndef __ICompressionStats2_INTERFACE_DEFINED__
#define __ICompressionStats2_INTERFACE_DEFINED__

/* interface ICompressionStats2 */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_ICompressionStats2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("4321FBD3-3882-43BD-91A6-36917A9231BC")
    ICompressionStats2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetCacheStats( 
            /* [out] */ DWORD *pdwCacheHits,
            /* [out] */ DWORD *pdwRawSize,
            /* [out] */ DWORD *pdwCompressedSize) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICompressionStats2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICompressionStats2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICompressionStats2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICompressionStats2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetCacheStats )( 
            ICompressionStats2 * This,
            /* [out] */ DWORD *pdwCacheHits,
            /* [out] */ DWORD *pdwRawSize,
            /* [out] */ DWORD *pdwCompressedSize);
        
        END_INTERFACE
    } ICompressionStats2Vtbl;

    interface ICompressionStats2
    {
        CONST_VTBL struct ICompressionStats2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICompressionStats2_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ICompressionStats2_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ICompressionStats2_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ICompressionStats2_GetCacheStats(This,pdwCacheHits,pdwRawSize,pdwCompressedSize)	\
    ( (This)->lpVtbl -> GetCacheStats(This,pdwCacheHits,pdwRawSize,pdwCompressedSize) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ICompressionStats2_INTERFACE_DEFINED__ */


#ifndef __IProductLicenseManager_INTERFACE_DEFINED__
#define __IProductLicenseManager_INTERFACE_DEFINED__

/* interface IProductLicenseManager */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IProductLicenseManager;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("C7C8114E-6767-43E7-A7EF-86C3C74B8554")
    IProductLicenseManager : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE IsValidKey( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE IsRegisteredKey( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetKey( 
            /* [in] */ LPCSTR pszKey,
            /* [in] */ BOOL bAllowTrial) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetExpiry( 
            /* [out] */ SYSTEMTIME *pstExpiry) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IProductLicenseManagerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IProductLicenseManager * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IProductLicenseManager * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IProductLicenseManager * This);
        
        HRESULT ( STDMETHODCALLTYPE *IsValidKey )( 
            IProductLicenseManager * This);
        
        HRESULT ( STDMETHODCALLTYPE *IsRegisteredKey )( 
            IProductLicenseManager * This);
        
        HRESULT ( STDMETHODCALLTYPE *SetKey )( 
            IProductLicenseManager * This,
            /* [in] */ LPCSTR pszKey,
            /* [in] */ BOOL bAllowTrial);
        
        HRESULT ( STDMETHODCALLTYPE *GetExpiry )( 
            IProductLicenseManager * This,
            /* [out] */ SYSTEMTIME *pstExpiry);
        
        END_INTERFACE
    } IProductLicenseManagerVtbl;

    interface IProductLicenseManager
    {
        CONST_VTBL struct IProductLicenseManagerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IProductLicenseManager_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IProductLicenseManager_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IProductLicenseManager_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IProductLicenseManager_IsValidKey(This)	\
    ( (This)->lpVtbl -> IsValidKey(This) ) 

#define IProductLicenseManager_IsRegisteredKey(This)	\
    ( (This)->lpVtbl -> IsRegisteredKey(This) ) 

#define IProductLicenseManager_SetKey(This,pszKey,bAllowTrial)	\
    ( (This)->lpVtbl -> SetKey(This,pszKey,bAllowTrial) ) 

#define IProductLicenseManager_GetExpiry(This,pstExpiry)	\
    ( (This)->lpVtbl -> GetExpiry(This,pstExpiry) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IProductLicenseManager_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_IISxpressServer_0000_0016 */
/* [local] */ 

typedef /* [public][public][public] */ 
enum __MIDL___MIDL_itf_IISxpressServer_0000_0016_0001
    {	Installed	= 0,
	Reinstall	= 1,
	FailedInstall	= 2,
	FinishInstall	= 3
    } 	AHABoardInstallState;

typedef /* [public][public] */ struct __MIDL___MIDL_itf_IISxpressServer_0000_0016_0002
    {
    BOOL bEnabled;
    WCHAR szDescription[ 256 ];
    AHABoardInstallState InstallState;
    } 	AHABoardInfo;



extern RPC_IF_HANDLE __MIDL_itf_IISxpressServer_0000_0016_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_IISxpressServer_0000_0016_v0_0_s_ifspec;

#ifndef __IAHAState_INTERFACE_DEFINED__
#define __IAHAState_INTERFACE_DEFINED__

/* interface IAHAState */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IAHAState;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("B13887AA-DC0A-4803-9148-8E1D24DB45EC")
    IAHAState : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetBoardTemps( 
            /* [out] */ DWORD *pdwFPGA,
            /* [out] */ DWORD *pdwBoard) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetBoardInfo( 
            /* [out] */ AHABoardInfo *pBoardInfo) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IAHAStateVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IAHAState * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IAHAState * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IAHAState * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetBoardTemps )( 
            IAHAState * This,
            /* [out] */ DWORD *pdwFPGA,
            /* [out] */ DWORD *pdwBoard);
        
        HRESULT ( STDMETHODCALLTYPE *GetBoardInfo )( 
            IAHAState * This,
            /* [out] */ AHABoardInfo *pBoardInfo);
        
        END_INTERFACE
    } IAHAStateVtbl;

    interface IAHAState
    {
        CONST_VTBL struct IAHAStateVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAHAState_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IAHAState_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IAHAState_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IAHAState_GetBoardTemps(This,pdwFPGA,pdwBoard)	\
    ( (This)->lpVtbl -> GetBoardTemps(This,pdwFPGA,pdwBoard) ) 

#define IAHAState_GetBoardInfo(This,pBoardInfo)	\
    ( (This)->lpVtbl -> GetBoardInfo(This,pBoardInfo) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IAHAState_INTERFACE_DEFINED__ */


#ifndef __IIISxpressHTTPRequest_INTERFACE_DEFINED__
#define __IIISxpressHTTPRequest_INTERFACE_DEFINED__

/* interface IIISxpressHTTPRequest */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IIISxpressHTTPRequest;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("86BEC801-0653-42BE-A20A-84D71DEECE51")
    IIISxpressHTTPRequest : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Init( 
            /* [in] */ IStorage *pStorage,
            /* [in] */ ICompressionRuleManager *pCompressionRuleManager,
            /* [in] */ IProductLicenseManager *pProductLicenseManager,
            /* [in] */ IComIISxpressRegSettings *pRegSettings,
            /* [in] */ IIISxpressSettings *pSettings) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetFilterVersion( 
            /* [in] */ IStream *pFilterVersion) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnSendResponse( 
            /* [in] */ IISInfo *pIISInfo,
            /* [in] */ RequestInfo *pRequestInfo,
            /* [in] */ ResponseInfo *pResponseInfo,
            /* [out] */ DWORD *pdwFilterContext) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnSendRawData( 
            /* [in] */ DWORD dwFilterContext,
            /* [in] */ IStream *pRawDataStream,
            /* [in] */ DWORD dwStreamDataLength,
            /* [in] */ BOOL bHasHeader,
            /* [size_is][out][in][full] */ signed char *pszResponseContentEncoding,
            /* [in] */ DWORD dwResponseContentEncodingBufferSize) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnEndOfRequest( 
            /* [in] */ DWORD dwFilterContext,
            /* [in] */ IStream *pRawDataStream,
            /* [in] */ DWORD dwStreamDataLength,
            /* [in] */ BOOL bHasHeader,
            /* [size_is][out][in][full] */ signed char *pszResponseContentEncoding,
            /* [in] */ DWORD dwResponseContentEncodingBufferSize) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnReadRawData( 
            /* [in] */ IStream *pRawDataStream) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnEndOfNetSession( 
            /* [in] */ DWORD dwFilterContext) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE IsAlive( 
            /* [out] */ DWORD *pdwData) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE AbortRequest( 
            /* [in] */ DWORD dwFilterContext) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE NotifyCacheHit( 
            /* [in] */ IISInfo *pIISInfo,
            /* [in] */ RequestInfo *pRequestInfo,
            /* [in] */ ResponseInfo *pResponseInfo,
            /* [in] */ DWORD dwCompressedSize) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IIISxpressHTTPRequestVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IIISxpressHTTPRequest * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IIISxpressHTTPRequest * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IIISxpressHTTPRequest * This);
        
        HRESULT ( STDMETHODCALLTYPE *Init )( 
            IIISxpressHTTPRequest * This,
            /* [in] */ IStorage *pStorage,
            /* [in] */ ICompressionRuleManager *pCompressionRuleManager,
            /* [in] */ IProductLicenseManager *pProductLicenseManager,
            /* [in] */ IComIISxpressRegSettings *pRegSettings,
            /* [in] */ IIISxpressSettings *pSettings);
        
        HRESULT ( STDMETHODCALLTYPE *GetFilterVersion )( 
            IIISxpressHTTPRequest * This,
            /* [in] */ IStream *pFilterVersion);
        
        HRESULT ( STDMETHODCALLTYPE *OnSendResponse )( 
            IIISxpressHTTPRequest * This,
            /* [in] */ IISInfo *pIISInfo,
            /* [in] */ RequestInfo *pRequestInfo,
            /* [in] */ ResponseInfo *pResponseInfo,
            /* [out] */ DWORD *pdwFilterContext);
        
        HRESULT ( STDMETHODCALLTYPE *OnSendRawData )( 
            IIISxpressHTTPRequest * This,
            /* [in] */ DWORD dwFilterContext,
            /* [in] */ IStream *pRawDataStream,
            /* [in] */ DWORD dwStreamDataLength,
            /* [in] */ BOOL bHasHeader,
            /* [size_is][out][in][full] */ signed char *pszResponseContentEncoding,
            /* [in] */ DWORD dwResponseContentEncodingBufferSize);
        
        HRESULT ( STDMETHODCALLTYPE *OnEndOfRequest )( 
            IIISxpressHTTPRequest * This,
            /* [in] */ DWORD dwFilterContext,
            /* [in] */ IStream *pRawDataStream,
            /* [in] */ DWORD dwStreamDataLength,
            /* [in] */ BOOL bHasHeader,
            /* [size_is][out][in][full] */ signed char *pszResponseContentEncoding,
            /* [in] */ DWORD dwResponseContentEncodingBufferSize);
        
        HRESULT ( STDMETHODCALLTYPE *OnReadRawData )( 
            IIISxpressHTTPRequest * This,
            /* [in] */ IStream *pRawDataStream);
        
        HRESULT ( STDMETHODCALLTYPE *OnEndOfNetSession )( 
            IIISxpressHTTPRequest * This,
            /* [in] */ DWORD dwFilterContext);
        
        HRESULT ( STDMETHODCALLTYPE *IsAlive )( 
            IIISxpressHTTPRequest * This,
            /* [out] */ DWORD *pdwData);
        
        HRESULT ( STDMETHODCALLTYPE *AbortRequest )( 
            IIISxpressHTTPRequest * This,
            /* [in] */ DWORD dwFilterContext);
        
        HRESULT ( STDMETHODCALLTYPE *NotifyCacheHit )( 
            IIISxpressHTTPRequest * This,
            /* [in] */ IISInfo *pIISInfo,
            /* [in] */ RequestInfo *pRequestInfo,
            /* [in] */ ResponseInfo *pResponseInfo,
            /* [in] */ DWORD dwCompressedSize);
        
        END_INTERFACE
    } IIISxpressHTTPRequestVtbl;

    interface IIISxpressHTTPRequest
    {
        CONST_VTBL struct IIISxpressHTTPRequestVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IIISxpressHTTPRequest_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IIISxpressHTTPRequest_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IIISxpressHTTPRequest_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IIISxpressHTTPRequest_Init(This,pStorage,pCompressionRuleManager,pProductLicenseManager,pRegSettings,pSettings)	\
    ( (This)->lpVtbl -> Init(This,pStorage,pCompressionRuleManager,pProductLicenseManager,pRegSettings,pSettings) ) 

#define IIISxpressHTTPRequest_GetFilterVersion(This,pFilterVersion)	\
    ( (This)->lpVtbl -> GetFilterVersion(This,pFilterVersion) ) 

#define IIISxpressHTTPRequest_OnSendResponse(This,pIISInfo,pRequestInfo,pResponseInfo,pdwFilterContext)	\
    ( (This)->lpVtbl -> OnSendResponse(This,pIISInfo,pRequestInfo,pResponseInfo,pdwFilterContext) ) 

#define IIISxpressHTTPRequest_OnSendRawData(This,dwFilterContext,pRawDataStream,dwStreamDataLength,bHasHeader,pszResponseContentEncoding,dwResponseContentEncodingBufferSize)	\
    ( (This)->lpVtbl -> OnSendRawData(This,dwFilterContext,pRawDataStream,dwStreamDataLength,bHasHeader,pszResponseContentEncoding,dwResponseContentEncodingBufferSize) ) 

#define IIISxpressHTTPRequest_OnEndOfRequest(This,dwFilterContext,pRawDataStream,dwStreamDataLength,bHasHeader,pszResponseContentEncoding,dwResponseContentEncodingBufferSize)	\
    ( (This)->lpVtbl -> OnEndOfRequest(This,dwFilterContext,pRawDataStream,dwStreamDataLength,bHasHeader,pszResponseContentEncoding,dwResponseContentEncodingBufferSize) ) 

#define IIISxpressHTTPRequest_OnReadRawData(This,pRawDataStream)	\
    ( (This)->lpVtbl -> OnReadRawData(This,pRawDataStream) ) 

#define IIISxpressHTTPRequest_OnEndOfNetSession(This,dwFilterContext)	\
    ( (This)->lpVtbl -> OnEndOfNetSession(This,dwFilterContext) ) 

#define IIISxpressHTTPRequest_IsAlive(This,pdwData)	\
    ( (This)->lpVtbl -> IsAlive(This,pdwData) ) 

#define IIISxpressHTTPRequest_AbortRequest(This,dwFilterContext)	\
    ( (This)->lpVtbl -> AbortRequest(This,dwFilterContext) ) 

#define IIISxpressHTTPRequest_NotifyCacheHit(This,pIISInfo,pRequestInfo,pResponseInfo,dwCompressedSize)	\
    ( (This)->lpVtbl -> NotifyCacheHit(This,pIISInfo,pRequestInfo,pResponseInfo,dwCompressedSize) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IIISxpressHTTPRequest_INTERFACE_DEFINED__ */


#ifndef __IIISxpressHTTPNotify_INTERFACE_DEFINED__
#define __IIISxpressHTTPNotify_INTERFACE_DEFINED__

/* interface IIISxpressHTTPNotify */
/* [unique][helpstring][async_uuid][uuid][object] */ 


EXTERN_C const IID IID_IIISxpressHTTPNotify;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("26BEC803-0653-42BE-A202-a4D71DEECE58")
    IIISxpressHTTPNotify : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE OnResponseStart( 
            /* [in] */ IISInfo *pIISInfo,
            /* [in] */ RequestInfo *pRequestInfo,
            /* [in] */ ResponseInfo *pResponseInfo,
            /* [in] */ DWORD dwFilterContext) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnResponseComplete( 
            /* [in] */ DWORD dwFilterContext,
            /* [in] */ BOOL bCompressed,
            /* [in] */ DWORD dwRawSize,
            /* [in] */ DWORD dwCompressedSize) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnResponseEnd( 
            /* [in] */ DWORD dwFilterContext) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnResponseAbort( 
            /* [in] */ DWORD dwFilterContext) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnCacheHit( 
            /* [in] */ IISInfo *pIISInfo,
            /* [in] */ RequestInfo *pRequestInfo,
            /* [in] */ ResponseInfo *pResponseInfo,
            /* [in] */ DWORD dwCompressedSize) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IIISxpressHTTPNotifyVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IIISxpressHTTPNotify * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IIISxpressHTTPNotify * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IIISxpressHTTPNotify * This);
        
        HRESULT ( STDMETHODCALLTYPE *OnResponseStart )( 
            IIISxpressHTTPNotify * This,
            /* [in] */ IISInfo *pIISInfo,
            /* [in] */ RequestInfo *pRequestInfo,
            /* [in] */ ResponseInfo *pResponseInfo,
            /* [in] */ DWORD dwFilterContext);
        
        HRESULT ( STDMETHODCALLTYPE *OnResponseComplete )( 
            IIISxpressHTTPNotify * This,
            /* [in] */ DWORD dwFilterContext,
            /* [in] */ BOOL bCompressed,
            /* [in] */ DWORD dwRawSize,
            /* [in] */ DWORD dwCompressedSize);
        
        HRESULT ( STDMETHODCALLTYPE *OnResponseEnd )( 
            IIISxpressHTTPNotify * This,
            /* [in] */ DWORD dwFilterContext);
        
        HRESULT ( STDMETHODCALLTYPE *OnResponseAbort )( 
            IIISxpressHTTPNotify * This,
            /* [in] */ DWORD dwFilterContext);
        
        HRESULT ( STDMETHODCALLTYPE *OnCacheHit )( 
            IIISxpressHTTPNotify * This,
            /* [in] */ IISInfo *pIISInfo,
            /* [in] */ RequestInfo *pRequestInfo,
            /* [in] */ ResponseInfo *pResponseInfo,
            /* [in] */ DWORD dwCompressedSize);
        
        END_INTERFACE
    } IIISxpressHTTPNotifyVtbl;

    interface IIISxpressHTTPNotify
    {
        CONST_VTBL struct IIISxpressHTTPNotifyVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IIISxpressHTTPNotify_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IIISxpressHTTPNotify_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IIISxpressHTTPNotify_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IIISxpressHTTPNotify_OnResponseStart(This,pIISInfo,pRequestInfo,pResponseInfo,dwFilterContext)	\
    ( (This)->lpVtbl -> OnResponseStart(This,pIISInfo,pRequestInfo,pResponseInfo,dwFilterContext) ) 

#define IIISxpressHTTPNotify_OnResponseComplete(This,dwFilterContext,bCompressed,dwRawSize,dwCompressedSize)	\
    ( (This)->lpVtbl -> OnResponseComplete(This,dwFilterContext,bCompressed,dwRawSize,dwCompressedSize) ) 

#define IIISxpressHTTPNotify_OnResponseEnd(This,dwFilterContext)	\
    ( (This)->lpVtbl -> OnResponseEnd(This,dwFilterContext) ) 

#define IIISxpressHTTPNotify_OnResponseAbort(This,dwFilterContext)	\
    ( (This)->lpVtbl -> OnResponseAbort(This,dwFilterContext) ) 

#define IIISxpressHTTPNotify_OnCacheHit(This,pIISInfo,pRequestInfo,pResponseInfo,dwCompressedSize)	\
    ( (This)->lpVtbl -> OnCacheHit(This,pIISInfo,pRequestInfo,pResponseInfo,dwCompressedSize) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IIISxpressHTTPNotify_INTERFACE_DEFINED__ */


#ifndef __AsyncIIISxpressHTTPNotify_INTERFACE_DEFINED__
#define __AsyncIIISxpressHTTPNotify_INTERFACE_DEFINED__

/* interface AsyncIIISxpressHTTPNotify */
/* [uuid][unique][helpstring][object] */ 


EXTERN_C const IID IID_AsyncIIISxpressHTTPNotify;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("ED80D683-F433-47c6-A6B3-7CFD010DE78F")
    AsyncIIISxpressHTTPNotify : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Begin_OnResponseStart( 
            /* [in] */ IISInfo *pIISInfo,
            /* [in] */ RequestInfo *pRequestInfo,
            /* [in] */ ResponseInfo *pResponseInfo,
            /* [in] */ DWORD dwFilterContext) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Finish_OnResponseStart( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Begin_OnResponseComplete( 
            /* [in] */ DWORD dwFilterContext,
            /* [in] */ BOOL bCompressed,
            /* [in] */ DWORD dwRawSize,
            /* [in] */ DWORD dwCompressedSize) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Finish_OnResponseComplete( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Begin_OnResponseEnd( 
            /* [in] */ DWORD dwFilterContext) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Finish_OnResponseEnd( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Begin_OnResponseAbort( 
            /* [in] */ DWORD dwFilterContext) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Finish_OnResponseAbort( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Begin_OnCacheHit( 
            /* [in] */ IISInfo *pIISInfo,
            /* [in] */ RequestInfo *pRequestInfo,
            /* [in] */ ResponseInfo *pResponseInfo,
            /* [in] */ DWORD dwCompressedSize) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Finish_OnCacheHit( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct AsyncIIISxpressHTTPNotifyVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            AsyncIIISxpressHTTPNotify * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            AsyncIIISxpressHTTPNotify * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            AsyncIIISxpressHTTPNotify * This);
        
        HRESULT ( STDMETHODCALLTYPE *Begin_OnResponseStart )( 
            AsyncIIISxpressHTTPNotify * This,
            /* [in] */ IISInfo *pIISInfo,
            /* [in] */ RequestInfo *pRequestInfo,
            /* [in] */ ResponseInfo *pResponseInfo,
            /* [in] */ DWORD dwFilterContext);
        
        HRESULT ( STDMETHODCALLTYPE *Finish_OnResponseStart )( 
            AsyncIIISxpressHTTPNotify * This);
        
        HRESULT ( STDMETHODCALLTYPE *Begin_OnResponseComplete )( 
            AsyncIIISxpressHTTPNotify * This,
            /* [in] */ DWORD dwFilterContext,
            /* [in] */ BOOL bCompressed,
            /* [in] */ DWORD dwRawSize,
            /* [in] */ DWORD dwCompressedSize);
        
        HRESULT ( STDMETHODCALLTYPE *Finish_OnResponseComplete )( 
            AsyncIIISxpressHTTPNotify * This);
        
        HRESULT ( STDMETHODCALLTYPE *Begin_OnResponseEnd )( 
            AsyncIIISxpressHTTPNotify * This,
            /* [in] */ DWORD dwFilterContext);
        
        HRESULT ( STDMETHODCALLTYPE *Finish_OnResponseEnd )( 
            AsyncIIISxpressHTTPNotify * This);
        
        HRESULT ( STDMETHODCALLTYPE *Begin_OnResponseAbort )( 
            AsyncIIISxpressHTTPNotify * This,
            /* [in] */ DWORD dwFilterContext);
        
        HRESULT ( STDMETHODCALLTYPE *Finish_OnResponseAbort )( 
            AsyncIIISxpressHTTPNotify * This);
        
        HRESULT ( STDMETHODCALLTYPE *Begin_OnCacheHit )( 
            AsyncIIISxpressHTTPNotify * This,
            /* [in] */ IISInfo *pIISInfo,
            /* [in] */ RequestInfo *pRequestInfo,
            /* [in] */ ResponseInfo *pResponseInfo,
            /* [in] */ DWORD dwCompressedSize);
        
        HRESULT ( STDMETHODCALLTYPE *Finish_OnCacheHit )( 
            AsyncIIISxpressHTTPNotify * This);
        
        END_INTERFACE
    } AsyncIIISxpressHTTPNotifyVtbl;

    interface AsyncIIISxpressHTTPNotify
    {
        CONST_VTBL struct AsyncIIISxpressHTTPNotifyVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define AsyncIIISxpressHTTPNotify_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define AsyncIIISxpressHTTPNotify_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define AsyncIIISxpressHTTPNotify_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define AsyncIIISxpressHTTPNotify_Begin_OnResponseStart(This,pIISInfo,pRequestInfo,pResponseInfo,dwFilterContext)	\
    ( (This)->lpVtbl -> Begin_OnResponseStart(This,pIISInfo,pRequestInfo,pResponseInfo,dwFilterContext) ) 

#define AsyncIIISxpressHTTPNotify_Finish_OnResponseStart(This)	\
    ( (This)->lpVtbl -> Finish_OnResponseStart(This) ) 

#define AsyncIIISxpressHTTPNotify_Begin_OnResponseComplete(This,dwFilterContext,bCompressed,dwRawSize,dwCompressedSize)	\
    ( (This)->lpVtbl -> Begin_OnResponseComplete(This,dwFilterContext,bCompressed,dwRawSize,dwCompressedSize) ) 

#define AsyncIIISxpressHTTPNotify_Finish_OnResponseComplete(This)	\
    ( (This)->lpVtbl -> Finish_OnResponseComplete(This) ) 

#define AsyncIIISxpressHTTPNotify_Begin_OnResponseEnd(This,dwFilterContext)	\
    ( (This)->lpVtbl -> Begin_OnResponseEnd(This,dwFilterContext) ) 

#define AsyncIIISxpressHTTPNotify_Finish_OnResponseEnd(This)	\
    ( (This)->lpVtbl -> Finish_OnResponseEnd(This) ) 

#define AsyncIIISxpressHTTPNotify_Begin_OnResponseAbort(This,dwFilterContext)	\
    ( (This)->lpVtbl -> Begin_OnResponseAbort(This,dwFilterContext) ) 

#define AsyncIIISxpressHTTPNotify_Finish_OnResponseAbort(This)	\
    ( (This)->lpVtbl -> Finish_OnResponseAbort(This) ) 

#define AsyncIIISxpressHTTPNotify_Begin_OnCacheHit(This,pIISInfo,pRequestInfo,pResponseInfo,dwCompressedSize)	\
    ( (This)->lpVtbl -> Begin_OnCacheHit(This,pIISInfo,pRequestInfo,pResponseInfo,dwCompressedSize) ) 

#define AsyncIIISxpressHTTPNotify_Finish_OnCacheHit(This)	\
    ( (This)->lpVtbl -> Finish_OnCacheHit(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __AsyncIIISxpressHTTPNotify_INTERFACE_DEFINED__ */



#ifndef __IISxpressServerLib_LIBRARY_DEFINED__
#define __IISxpressServerLib_LIBRARY_DEFINED__

/* library IISxpressServerLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_IISxpressServerLib;

EXTERN_C const CLSID CLSID_IISxpressHTTPRequest;

#ifdef __cplusplus

class DECLSPEC_UUID("E9D82120-347A-41BD-BF61-1BD6BA23ED50")
IISxpressHTTPRequest;
#endif

EXTERN_C const CLSID CLSID_CompressionRuleManager;

#ifdef __cplusplus

class DECLSPEC_UUID("B05C2A2F-E6BE-4769-BDC2-0E0C0686CFA7")
CompressionRuleManager;
#endif

EXTERN_C const CLSID CLSID_EnumStaticFileInfo;

#ifdef __cplusplus

class DECLSPEC_UUID("1A291EBF-8B9C-4986-9389-D08073ACF75C")
EnumStaticFileInfo;
#endif

EXTERN_C const CLSID CLSID_ComIISxpressRegSettings;

#ifdef __cplusplus

class DECLSPEC_UUID("2A22C83C-1E59-4BAB-8F5E-94513D8E91F5")
ComIISxpressRegSettings;
#endif

EXTERN_C const CLSID CLSID_EnumExclusionRule;

#ifdef __cplusplus

class DECLSPEC_UUID("E94AB2C7-B1AB-40BA-B905-94CEC70A1180")
EnumExclusionRule;
#endif

EXTERN_C const CLSID CLSID_ProductLicenseManager;

#ifdef __cplusplus

class DECLSPEC_UUID("51EDAF3B-EF01-41FF-91D6-A4EB5DC34FA7")
ProductLicenseManager;
#endif
#endif /* __IISxpressServerLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

unsigned long             __RPC_USER  VARIANT_UserSize(     unsigned long *, unsigned long            , VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserMarshal(  unsigned long *, unsigned char *, VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserUnmarshal(unsigned long *, unsigned char *, VARIANT * ); 
void                      __RPC_USER  VARIANT_UserFree(     unsigned long *, VARIANT * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


