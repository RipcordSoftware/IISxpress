

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0500 */
/* at Tue Dec 14 22:44:05 2010
 */
/* Compiler settings for .\IISxpressTray.idl:
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
#define __REQUIRED_RPCNDR_H_VERSION__ 440
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

#ifndef __IISxpressTray_h_h__
#define __IISxpressTray_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IHTTPNotify_FWD_DEFINED__
#define __IHTTPNotify_FWD_DEFINED__
typedef interface IHTTPNotify IHTTPNotify;
#endif 	/* __IHTTPNotify_FWD_DEFINED__ */


#ifndef __HTTPNotify_FWD_DEFINED__
#define __HTTPNotify_FWD_DEFINED__

#ifdef __cplusplus
typedef class HTTPNotify HTTPNotify;
#else
typedef struct HTTPNotify HTTPNotify;
#endif /* __cplusplus */

#endif 	/* __HTTPNotify_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IHTTPNotify_INTERFACE_DEFINED__
#define __IHTTPNotify_INTERFACE_DEFINED__

/* interface IHTTPNotify */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IHTTPNotify;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("02E738CE-9999-425F-87B1-7DC9BB199D8A")
    IHTTPNotify : public IUnknown
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IHTTPNotifyVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IHTTPNotify * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IHTTPNotify * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IHTTPNotify * This);
        
        END_INTERFACE
    } IHTTPNotifyVtbl;

    interface IHTTPNotify
    {
        CONST_VTBL struct IHTTPNotifyVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IHTTPNotify_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IHTTPNotify_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IHTTPNotify_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IHTTPNotify_INTERFACE_DEFINED__ */



#ifndef __IISxpressTrayLib_LIBRARY_DEFINED__
#define __IISxpressTrayLib_LIBRARY_DEFINED__

/* library IISxpressTrayLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_IISxpressTrayLib;

EXTERN_C const CLSID CLSID_HTTPNotify;

#ifdef __cplusplus

class DECLSPEC_UUID("4D9F3A14-338B-4C20-8CDD-915DE994057F")
HTTPNotify;
#endif
#endif /* __IISxpressTrayLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


