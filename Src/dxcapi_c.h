///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// dxcapi_c.h                                                                //
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
// This file is distributed under the University of Illinois Open Source     //
// License. See LICENSE.TXT for details.                                     //
//                                                                           //
// Provides C-style declarations for the DirectX Compiler API based on       //
// the C++ dxcapi.h header, using the COM vtable pattern.                    //
//                                                                           //
// Generated based on dxcapi.h                                               //
// Updated to remove STDMETHODIMP macros and IUnknown definitions.           //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#ifndef __DXC_API_C_H__
#define __DXC_API_C_H__

#include <stddef.h> // For size_t
#include <stdint.h> // For uint32_t, int32_t, uint8_t
#include <Unknwnbase.h>

// ---------------------------------------------------------------------------
// Basic COM types and Macros (Minimal definitions)
//
// It's recommended to include actual Windows headers like <windows.h>,
// <unknwn.h>, <objidl.h>, <oleauto.h>, <guiddef.h> for full definitions
// if compiling in a Windows environment.
//
// NOTE: IUnknown, IUnknownVtbl, IID_IUnknown, IMalloc, IStream are assumed
//       to be defined by included system headers (e.g., <unknwn.h>, <objidl.h>).
// ---------------------------------------------------------------------------

#ifndef __cplusplus
#ifndef bool
#define bool  _Bool
#define true  1
#define false 0
#endif
#endif

#ifndef _MSC_VER
#ifndef __stdcall
#ifdef _WIN32
#define __stdcall __attribute__((stdcall))
#else
#define __stdcall
#endif
#endif // __stdcall
#endif // _MSC_VER


#ifndef STDMETHODCALLTYPE
#ifdef _WIN32
#define STDMETHODCALLTYPE       __stdcall
#else
#define STDMETHODCALLTYPE
#endif
#endif // STDMETHODCALLTYPE

// STDMETHODIMP and STDMETHODIMP_ are removed as requested.
// Use HRESULT STDMETHODCALLTYPE and ReturnType STDMETHODCALLTYPE directly.

#ifndef DECLSPEC_UUID
#if defined(_MSC_VER)
#define DECLSPEC_UUID(x)    __declspec(uuid(x))
#else
#define DECLSPEC_UUID(x)
#endif
#endif // DECLSPEC_UUID

#ifndef DECLSPEC_NOVTABLE
#if defined(_MSC_VER)
#define DECLSPEC_NOVTABLE   __declspec(novtable)
#else
#define DECLSPEC_NOVTABLE
#endif
#endif // DECLSPEC_NOVTABLE

#ifndef __RPC_FAR
#define __RPC_FAR
#endif // __RPC_FAR

#ifndef FAR
#define FAR
#endif // FAR

#ifndef EXTERN_C
#ifdef __cplusplus
#define EXTERN_C    extern "C"
#else
#define EXTERN_C    extern
#endif
#endif // EXTERN_C

// SAL Annotations - Define as empty if not available
#ifndef _In_
#define _In_
#endif
#ifndef _In_opt_
#define _In_opt_
#endif
#ifndef _In_z_
#define _In_z_
#endif
#ifndef _In_opt_z_
#define _In_opt_z_
#endif
#ifndef _In_bytecount_
#define _In_bytecount_(x)
#endif
#ifndef _In_count_
#define _In_count_(x)
#endif
#ifndef _In_opt_count_
#define _In_opt_count_(x)
#endif
#ifndef _Out_
#define _Out_
#endif
#ifndef _Out_opt_
#define _Out_opt_
#endif
#ifndef _Outptr_
#define _Outptr_
#endif
#ifndef _Outptr_opt_
#define _Outptr_opt_
#endif
#ifndef _Outptr_result_maybenull_
#define _Outptr_result_maybenull_
#endif
#ifndef _COM_Outptr_
#define _COM_Outptr_
#endif
#ifndef _COM_Outptr_opt_
#define _COM_Outptr_opt_
#endif
#ifndef _COM_Outptr_result_maybenull_
#define _COM_Outptr_result_maybenull_
#endif
#ifndef _Outptr_opt_result_z_
#define _Outptr_opt_result_z_
#endif
#ifndef _Outptr_result_z_
#define _Outptr_result_z_
#endif
#ifndef _Out_UINT32_
#define _Out_UINT32_(x)
#endif
#ifndef _Maybenull_
#define _Maybenull_
#endif

// Basic Types (Provide minimal definitions if not using system headers)
#ifndef BASETYPES
#define BASETYPES
typedef int32_t                 BOOL;
typedef uint8_t                 BYTE;
typedef int32_t                 HRESULT;
typedef uint32_t                UINT32;
typedef uint64_t                UINT64;
typedef unsigned long           ULONG; // Commonly used by AddRef/Release
typedef size_t                  SIZE_T;
typedef void * LPVOID;
typedef const void * LPCVOID;
typedef char                    CHAR;
typedef const char * LPCSTR;
typedef wchar_t                 WCHAR;
typedef const WCHAR * LPCWSTR;
typedef WCHAR * LPWSTR;
typedef WCHAR * BSTR; // Note: BSTR requires special allocation/deallocation (SysAllocString/SysFreeString)
#endif // BASETYPES

// GUID and Interface Definitions
#ifndef GUID_DEFINED
#define GUID_DEFINED
typedef struct _GUID {
    uint32_t Data1;
    uint16_t Data2;
    uint16_t Data3;
    uint8_t  Data4[8];
} GUID;
#endif // GUID_DEFINED

#ifndef _REFGUID_DEFINED
#define _REFGUID_DEFINED
typedef const GUID *REFGUID;
#endif // _REFGUID_DEFINED

#ifndef _REFIID_DEFINED
#define _REFIID_DEFINED
typedef const GUID *REFIID;
#endif // _REFIID_DEFINED

#ifndef _REFCLSID_DEFINED
#define _REFCLSID_DEFINED
typedef const GUID *REFCLSID;
#endif // _REFCLSID_DEFINED

// Define DEFINE_GUID if not already defined (e.g., by guiddef.h)
#ifndef DEFINE_GUID
#ifdef __cplusplus
#define DEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) EXTERN_C const GUID DECLSPEC_SELECTANY name = { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }
#else
#define DEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) const GUID name = { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }
#endif // __cplusplus
#endif // DEFINE_GUID

// Forward declarations for interfaces
// Note: IUnknown, IMalloc, IStream are assumed to be defined externally.
typedef struct IDxcBlob IDxcBlob;
typedef struct IDxcBlobEncoding IDxcBlobEncoding;
typedef struct IDxcBlobUtf16 IDxcBlobUtf16;
typedef struct IDxcBlobUtf8 IDxcBlobUtf8;
typedef struct IDxcIncludeHandler IDxcIncludeHandler;
typedef struct IDxcCompilerArgs IDxcCompilerArgs;
typedef struct IDxcLibrary IDxcLibrary; // Legacy
typedef struct IDxcOperationResult IDxcOperationResult; // Legacy
typedef struct IDxcCompiler IDxcCompiler; // Legacy
typedef struct IDxcCompiler2 IDxcCompiler2; // Legacy
typedef struct IDxcLinker IDxcLinker;
typedef struct IDxcUtils IDxcUtils;
typedef struct IDxcResult IDxcResult;
typedef struct IDxcExtraOutputs IDxcExtraOutputs;
typedef struct IDxcCompiler3 IDxcCompiler3;
typedef struct IDxcValidator IDxcValidator;
typedef struct IDxcValidator2 IDxcValidator2;
typedef struct IDxcContainerBuilder IDxcContainerBuilder;
typedef struct IDxcAssembler IDxcAssembler;
typedef struct IDxcContainerReflection IDxcContainerReflection;
typedef struct IDxcOptimizerPass IDxcOptimizerPass;
typedef struct IDxcOptimizer IDxcOptimizer;
typedef struct IDxcVersionInfo IDxcVersionInfo;
typedef struct IDxcVersionInfo2 IDxcVersionInfo2;
typedef struct IDxcVersionInfo3 IDxcVersionInfo3;
typedef struct IDxcPdbUtils IDxcPdbUtils;

// Assume IUnknown, IUnknownVtbl are defined externally (e.g., in <unknwn.h>)
// Assume IMalloc is defined externally (e.g., in <objidl.h>)
// Assume IStream is defined externally (e.g., in <objidl.h>)

// ---------------------------------------------------------------------------
// DXC Specific Definitions
// ---------------------------------------------------------------------------

// Code Pages
#define DXC_CP_UTF8 65001
#define DXC_CP_UTF16 1200
#define DXC_CP_ACP 0 // Use for: Binary; ANSI Text; Autodetect UTF with BOM

// Hash Flags
#define DXC_HASHFLAG_INCLUDES_SOURCE  1

// Hash Digest Structure
typedef struct DxcShaderHash {
  UINT32 Flags; // DXC_HASHFLAG_*
  BYTE HashDigest[16];
} DxcShaderHash;

// DXIL Container Part FourCCs
#define DXC_FOURCC(ch0, ch1, ch2, ch3) ( \
  (UINT32)(BYTE)(ch0)        | (UINT32)(BYTE)(ch1) << 8  | \
  (UINT32)(BYTE)(ch2) << 16  | (UINT32)(BYTE)(ch3) << 24   \
  )
#define DXC_PART_PDB                      DXC_FOURCC('I', 'L', 'D', 'B')
#define DXC_PART_PDB_NAME                 DXC_FOURCC('I', 'L', 'D', 'N')
#define DXC_PART_PRIVATE_DATA             DXC_FOURCC('P', 'R', 'I', 'V')
#define DXC_PART_ROOT_SIGNATURE           DXC_FOURCC('R', 'T', 'S', '0')
#define DXC_PART_DXIL                     DXC_FOURCC('D', 'X', 'I', 'L')
#define DXC_PART_REFLECTION_DATA          DXC_FOURCC('S', 'T', 'A', 'T')
#define DXC_PART_SHADER_HASH              DXC_FOURCC('H', 'A', 'S', 'H')
#define DXC_PART_INPUT_SIGNATURE          DXC_FOURCC('I', 'S', 'G', '1')
#define DXC_PART_OUTPUT_SIGNATURE         DXC_FOURCC('O', 'S', 'G', '1')
#define DXC_PART_PATCH_CONSTANT_SIGNATURE DXC_FOURCC('P', 'S', 'G', '1')

// Common Compiler Arguments
#define DXC_ARG_DEBUG L"-Zi"
#define DXC_ARG_SKIP_VALIDATION L"-Vd"
#define DXC_ARG_SKIP_OPTIMIZATIONS L"-Od"
#define DXC_ARG_PACK_MATRIX_ROW_MAJOR L"-Zpr"
#define DXC_ARG_PACK_MATRIX_COLUMN_MAJOR L"-Zpc"
#define DXC_ARG_AVOID_FLOW_CONTROL L"-Gfa"
#define DXC_ARG_PREFER_FLOW_CONTROL L"-Gfp"
#define DXC_ARG_ENABLE_STRICTNESS L"-Ges"
#define DXC_ARG_ENABLE_BACKWARDS_COMPATIBILITY L"-Gec"
#define DXC_ARG_IEEE_STRICTNESS L"-Gis"
#define DXC_ARG_OPTIMIZATION_LEVEL0 L"-O0"
#define DXC_ARG_OPTIMIZATION_LEVEL1 L"-O1"
#define DXC_ARG_OPTIMIZATION_LEVEL2 L"-O2"
#define DXC_ARG_OPTIMIZATION_LEVEL3 L"-O3"
#define DXC_ARG_WARNINGS_ARE_ERRORS L"-WX"
#define DXC_ARG_RESOURCES_MAY_ALIAS L"-res_may_alias"
#define DXC_ARG_ALL_RESOURCES_BOUND L"-all_resources_bound"
#define DXC_ARG_DEBUG_NAME_FOR_SOURCE L"-Zss"
#define DXC_ARG_DEBUG_NAME_FOR_BINARY L"-Zsb"

// Input Buffer Structure
typedef struct DxcBuffer {
  LPCVOID Ptr;
  SIZE_T Size;
  UINT32 Encoding; // Use DXC_CP_* values
} DxcBuffer;

// Define Structure
typedef struct DxcDefine {
  LPCWSTR Name;
  _Maybenull_ LPCWSTR Value;
} DxcDefine;

// Validator Flags
static const UINT32 DxcValidatorFlags_Default = 0;
static const UINT32 DxcValidatorFlags_InPlaceEdit = 1;
static const UINT32 DxcValidatorFlags_RootSignatureOnly = 2;
static const UINT32 DxcValidatorFlags_ModuleOnly = 4;
static const UINT32 DxcValidatorFlags_ValidMask = 0x7;

// Output Kinds
typedef enum DXC_OUT_KIND {
  DXC_OUT_NONE = 0,
  DXC_OUT_OBJECT = 1,
  DXC_OUT_ERRORS = 2,
  DXC_OUT_PDB = 3,
  DXC_OUT_SHADER_HASH = 4,
  DXC_OUT_DISASSEMBLY = 5,
  DXC_OUT_HLSL = 6,
  DXC_OUT_TEXT = 7,
  DXC_OUT_REFLECTION = 8,
  DXC_OUT_ROOT_SIGNATURE = 9,
  DXC_OUT_EXTRA_OUTPUTS  = 10,
  DXC_OUT_FORCE_DWORD = 0xFFFFFFFF
} DXC_OUT_KIND;

// Extra Output Names
#define DXC_EXTRA_OUTPUT_NAME_STDOUT L"*stdout*"
#define DXC_EXTRA_OUTPUT_NAME_STDERR L"*stderr*"

// Version Info Flags
static const UINT32 DxcVersionInfoFlags_None = 0;
static const UINT32 DxcVersionInfoFlags_Debug = 1;
static const UINT32 DxcVersionInfoFlags_Internal = 2;

// Argument Pair Structure
typedef struct DxcArgPair {
  const WCHAR *pName;
  const WCHAR *pValue;
} DxcArgPair;

// ---------------------------------------------------------------------------
// DXC Interfaces
// ---------------------------------------------------------------------------

// --- IDxcBlob ---
DEFINE_GUID(IID_IDxcBlob, 0x8BA5FB08, 0x5195, 0x40e2, 0xAC, 0x58, 0x0D, 0x98, 0x9C, 0x3A, 0x01, 0x02);

typedef struct IDxcBlobVtbl {
    // IUnknown methods (assuming IUnknownVtbl is defined externally)
    IUnknownVtbl IUnknown_vtbl;

    // IDxcBlob methods
    LPVOID (STDMETHODCALLTYPE *GetBufferPointer)(
        _In_ IDxcBlob *This);

    SIZE_T (STDMETHODCALLTYPE *GetBufferSize)(
        _In_ IDxcBlob *This);
} IDxcBlobVtbl;

struct IDxcBlob {
    const IDxcBlobVtbl *lpVtbl;
};

// --- IDxcBlobEncoding ---
DEFINE_GUID(IID_IDxcBlobEncoding, 0x7241d424, 0x2646, 0x4191, 0x97, 0xc0, 0x98, 0xe9, 0x6e, 0x42, 0xfc, 0x68);

typedef struct IDxcBlobEncodingVtbl {
    // IDxcBlob methods
    IDxcBlobVtbl IDxcBlob_vtbl;

    // IDxcBlobEncoding methods
    HRESULT (STDMETHODCALLTYPE *GetEncoding)(
        _In_ IDxcBlobEncoding *This,
        _Out_ BOOL *pKnown,
        _Out_ UINT32 *pCodePage);
} IDxcBlobEncodingVtbl;

struct IDxcBlobEncoding {
    const IDxcBlobEncodingVtbl *lpVtbl;
};

// --- IDxcBlobUtf16 ---
DEFINE_GUID(IID_IDxcBlobUtf16, 0xA3F84EAB, 0x0FAA, 0x497E, 0xA3, 0x9C, 0xEE, 0x6E, 0xD6, 0x0B, 0x2D, 0x84);

typedef struct IDxcBlobUtf16Vtbl {
    // IDxcBlobEncoding methods
    IDxcBlobEncodingVtbl IDxcBlobEncoding_vtbl;

    // IDxcBlobUtf16 methods
    LPCWSTR (STDMETHODCALLTYPE *GetStringPointer)(
        _In_ IDxcBlobUtf16 *This);

    SIZE_T (STDMETHODCALLTYPE *GetStringLength)(
        _In_ IDxcBlobUtf16 *This);
} IDxcBlobUtf16Vtbl;

struct IDxcBlobUtf16 {
    const IDxcBlobUtf16Vtbl *lpVtbl;
};

// --- IDxcBlobUtf8 ---
DEFINE_GUID(IID_IDxcBlobUtf8, 0x3DA636C9, 0xBA71, 0x4024, 0xA3, 0x01, 0x30, 0xCB, 0xF1, 0x25, 0x30, 0x5B);

typedef struct IDxcBlobUtf8Vtbl {
    // IDxcBlobEncoding methods
    IDxcBlobEncodingVtbl IDxcBlobEncoding_vtbl;

    // IDxcBlobUtf8 methods
    LPCSTR (STDMETHODCALLTYPE *GetStringPointer)(
        _In_ IDxcBlobUtf8 *This);

    SIZE_T (STDMETHODCALLTYPE *GetStringLength)(
        _In_ IDxcBlobUtf8 *This);
} IDxcBlobUtf8Vtbl;

struct IDxcBlobUtf8 {
    const IDxcBlobUtf8Vtbl *lpVtbl;
};

// --- IDxcIncludeHandler ---
DEFINE_GUID(IID_IDxcIncludeHandler, 0x7f61fc7d, 0x950d, 0x467f, 0xb3, 0xe3, 0x3c, 0x02, 0xfb, 0x49, 0x18, 0x7c);

typedef struct IDxcIncludeHandlerVtbl {
    // IUnknown methods
    IUnknownVtbl IUnknown_vtbl;

    // IDxcIncludeHandler methods
    HRESULT (STDMETHODCALLTYPE *LoadSource)(
        _In_ IDxcIncludeHandler *This,
        _In_z_ LPCWSTR pFilename,
        _COM_Outptr_result_maybenull_ IDxcBlob **ppIncludeSource);
} IDxcIncludeHandlerVtbl;

struct IDxcIncludeHandler {
    const IDxcIncludeHandlerVtbl *lpVtbl;
};

// --- IDxcCompilerArgs ---
DEFINE_GUID(IID_IDxcCompilerArgs, 0x73EFFE2A, 0x70DC, 0x45F8, 0x96, 0x90, 0xEF, 0xF6, 0x4C, 0x02, 0x42, 0x9D);

typedef struct IDxcCompilerArgsVtbl {
    // IUnknown methods
    IUnknownVtbl IUnknown_vtbl;

    // IDxcCompilerArgs methods
    LPCWSTR* (STDMETHODCALLTYPE *GetArguments)(
        _In_ IDxcCompilerArgs *This);

    UINT32 (STDMETHODCALLTYPE *GetCount)(
        _In_ IDxcCompilerArgs *This);

    HRESULT (STDMETHODCALLTYPE *AddArguments)(
        _In_ IDxcCompilerArgs *This,
        _In_opt_count_(argCount) LPCWSTR *pArguments,
        _In_ UINT32 argCount);

    HRESULT (STDMETHODCALLTYPE *AddArgumentsUTF8)(
        _In_ IDxcCompilerArgs *This,
        _In_opt_count_(argCount) LPCSTR *pArguments,
        _In_ UINT32 argCount);

    HRESULT (STDMETHODCALLTYPE *AddDefines)(
        _In_ IDxcCompilerArgs *This,
        _In_count_(defineCount) const DxcDefine *pDefines,
        _In_ UINT32 defineCount);
} IDxcCompilerArgsVtbl;

struct IDxcCompilerArgs {
    const IDxcCompilerArgsVtbl *lpVtbl;
};

// --- IDxcLibrary (Legacy) ---
DEFINE_GUID(IID_IDxcLibrary, 0xe5204dc7, 0xd18c, 0x4c3c, 0xbd, 0xfb, 0x85, 0x16, 0x73, 0x98, 0x0f, 0xe7);

typedef struct IDxcLibraryVtbl {
    // IUnknown methods
    IUnknownVtbl IUnknown_vtbl;

    // IDxcLibrary methods
    HRESULT (STDMETHODCALLTYPE *SetMalloc)(
        _In_ IDxcLibrary *This,
        _In_opt_ IMalloc *pMalloc);

    HRESULT (STDMETHODCALLTYPE *CreateBlobFromBlob)(
        _In_ IDxcLibrary *This,
        _In_ IDxcBlob *pBlob,
        UINT32 offset,
        UINT32 length,
        _COM_Outptr_ IDxcBlob **ppResult);

    HRESULT (STDMETHODCALLTYPE *CreateBlobFromFile)(
        _In_ IDxcLibrary *This,
        _In_z_ LPCWSTR pFileName,
        _In_opt_ UINT32* codePage,
        _COM_Outptr_ IDxcBlobEncoding **pBlobEncoding);

    HRESULT (STDMETHODCALLTYPE *CreateBlobWithEncodingFromPinned)(
        _In_ IDxcLibrary *This,
        _In_bytecount_(size) LPCVOID pText,
        UINT32 size,
        UINT32 codePage,
        _COM_Outptr_ IDxcBlobEncoding **pBlobEncoding);

    HRESULT (STDMETHODCALLTYPE *CreateBlobWithEncodingOnHeapCopy)(
        _In_ IDxcLibrary *This,
        _In_bytecount_(size) LPCVOID pText,
        UINT32 size,
        UINT32 codePage,
        _COM_Outptr_ IDxcBlobEncoding **pBlobEncoding);

    HRESULT (STDMETHODCALLTYPE *CreateBlobWithEncodingOnMalloc)(
        _In_ IDxcLibrary *This,
        _In_bytecount_(size) LPCVOID pText,
        IMalloc *pIMalloc,
        UINT32 size,
        UINT32 codePage,
        _COM_Outptr_ IDxcBlobEncoding **pBlobEncoding);

    HRESULT (STDMETHODCALLTYPE *CreateIncludeHandler)(
        _In_ IDxcLibrary *This,
        _COM_Outptr_ IDxcIncludeHandler **ppResult);

    HRESULT (STDMETHODCALLTYPE *CreateStreamFromBlobReadOnly)(
        _In_ IDxcLibrary *This,
        _In_ IDxcBlob *pBlob,
        _COM_Outptr_ IStream **ppStream);

    HRESULT (STDMETHODCALLTYPE *GetBlobAsUtf8)(
        _In_ IDxcLibrary *This,
        _In_ IDxcBlob *pBlob,
        _COM_Outptr_ IDxcBlobEncoding **pBlobEncoding); // Note: Original header returns IDxcBlobEncoding, but likely meant IDxcBlobUtf8

    HRESULT (STDMETHODCALLTYPE *GetBlobAsUtf16)(
        _In_ IDxcLibrary *This,
        _In_ IDxcBlob *pBlob,
        _COM_Outptr_ IDxcBlobEncoding **pBlobEncoding); // Note: Original header returns IDxcBlobEncoding, but likely meant IDxcBlobUtf16
} IDxcLibraryVtbl;

struct IDxcLibrary {
    const IDxcLibraryVtbl *lpVtbl;
};

// --- IDxcOperationResult (Legacy) ---
DEFINE_GUID(IID_IDxcOperationResult, 0xCEDB484A, 0xD4E9, 0x445A, 0xB9, 0x91, 0xCA, 0x21, 0xCA, 0x15, 0x7D, 0xC2);

typedef struct IDxcOperationResultVtbl {
    // IUnknown methods
    IUnknownVtbl IUnknown_vtbl;

    // IDxcOperationResult methods
    HRESULT (STDMETHODCALLTYPE *GetStatus)(
        _In_ IDxcOperationResult *This,
        _Out_ HRESULT *pStatus);

    HRESULT (STDMETHODCALLTYPE *GetResult)(
        _In_ IDxcOperationResult *This,
        _COM_Outptr_result_maybenull_ IDxcBlob **ppResult);

    HRESULT (STDMETHODCALLTYPE *GetErrorBuffer)(
        _In_ IDxcOperationResult *This,
        _COM_Outptr_result_maybenull_ IDxcBlobEncoding **ppErrors);
} IDxcOperationResultVtbl;

struct IDxcOperationResult {
    const IDxcOperationResultVtbl *lpVtbl;
};

// --- IDxcCompiler (Legacy) ---
DEFINE_GUID(IID_IDxcCompiler, 0x8c210bf3, 0x011f, 0x4422, 0x8d, 0x70, 0x6f, 0x9a, 0xcb, 0x8d, 0xb6, 0x17);

typedef struct IDxcCompilerVtbl {
    // IUnknown methods
    IUnknownVtbl IUnknown_vtbl;

    // IDxcCompiler methods
    HRESULT (STDMETHODCALLTYPE *Compile)(
        _In_ IDxcCompiler *This,
        _In_ IDxcBlob *pSource,
        _In_opt_z_ LPCWSTR pSourceName,
        _In_opt_z_ LPCWSTR pEntryPoint,
        _In_z_ LPCWSTR pTargetProfile,
        _In_opt_count_(argCount) LPCWSTR *pArguments,
        _In_ UINT32 argCount,
        _In_count_(defineCount) const DxcDefine *pDefines,
        _In_ UINT32 defineCount,
        _In_opt_ IDxcIncludeHandler *pIncludeHandler,
        _COM_Outptr_ IDxcOperationResult **ppResult);

    HRESULT (STDMETHODCALLTYPE *Preprocess)(
        _In_ IDxcCompiler *This,
        _In_ IDxcBlob *pSource,
        _In_opt_z_ LPCWSTR pSourceName,
        _In_opt_count_(argCount) LPCWSTR *pArguments,
        _In_ UINT32 argCount,
        _In_count_(defineCount) const DxcDefine *pDefines,
        _In_ UINT32 defineCount,
        _In_opt_ IDxcIncludeHandler *pIncludeHandler,
        _COM_Outptr_ IDxcOperationResult **ppResult);

    HRESULT (STDMETHODCALLTYPE *Disassemble)(
        _In_ IDxcCompiler *This,
        _In_ IDxcBlob *pSource,
        _COM_Outptr_ IDxcBlobEncoding **ppDisassembly);
} IDxcCompilerVtbl;

struct IDxcCompiler {
    const IDxcCompilerVtbl *lpVtbl;
};

// --- IDxcCompiler2 (Legacy) ---
DEFINE_GUID(IID_IDxcCompiler2, 0xA005A9D9, 0xB8BB, 0x4594, 0xB5, 0xC9, 0x0E, 0x63, 0x3B, 0xEC, 0x4D, 0x37);

typedef struct IDxcCompiler2Vtbl {
    // IDxcCompiler methods
    IDxcCompilerVtbl IDxcCompiler_vtbl;

    // IDxcCompiler2 methods
    HRESULT (STDMETHODCALLTYPE *CompileWithDebug)(
        _In_ IDxcCompiler2 *This,
        _In_ IDxcBlob *pSource,
        _In_opt_z_ LPCWSTR pSourceName,
        _In_opt_z_ LPCWSTR pEntryPoint,
        _In_z_ LPCWSTR pTargetProfile,
        _In_opt_count_(argCount) LPCWSTR *pArguments,
        _In_ UINT32 argCount,
        _In_count_(defineCount) const DxcDefine *pDefines,
        _In_ UINT32 defineCount,
        _In_opt_ IDxcIncludeHandler *pIncludeHandler,
        _COM_Outptr_ IDxcOperationResult **ppResult,
        _Outptr_opt_result_z_ LPWSTR *ppDebugBlobName,
        _COM_Outptr_opt_ IDxcBlob **ppDebugBlob);
} IDxcCompiler2Vtbl;

struct IDxcCompiler2 {
    const IDxcCompiler2Vtbl *lpVtbl;
};

// --- IDxcLinker ---
DEFINE_GUID(IID_IDxcLinker, 0xF1B5BE2A, 0x62DD, 0x4327, 0xA1, 0xC2, 0x42, 0xAC, 0x1E, 0x1E, 0x78, 0xE6);

typedef struct IDxcLinkerVtbl {
    // IUnknown methods
    IUnknownVtbl IUnknown_vtbl;

    // IDxcLinker methods
    HRESULT (STDMETHODCALLTYPE *RegisterLibrary)(
        _In_ IDxcLinker *This,
        _In_opt_ LPCWSTR pLibName,
        _In_ IDxcBlob *pLib);

    HRESULT (STDMETHODCALLTYPE *Link)(
        _In_ IDxcLinker *This,
        _In_opt_ LPCWSTR pEntryName,
        _In_ LPCWSTR pTargetProfile,
        _In_count_(libCount) const LPCWSTR *pLibNames,
        _In_ UINT32 libCount,
        _In_opt_count_(argCount) const LPCWSTR *pArguments,
        _In_ UINT32 argCount,
        _COM_Outptr_ IDxcOperationResult **ppResult);
} IDxcLinkerVtbl;

struct IDxcLinker {
    const IDxcLinkerVtbl *lpVtbl;
};

// --- IDxcUtils ---
DEFINE_GUID(IID_IDxcUtils, 0x4605C4CB, 0x2019, 0x492A, 0xAD, 0xA4, 0x65, 0xF2, 0x0B, 0xB7, 0xD6, 0x7F);

typedef struct IDxcUtilsVtbl {
    // IUnknown methods
    IUnknownVtbl IUnknown_vtbl;

    // IDxcUtils methods
    HRESULT (STDMETHODCALLTYPE *CreateBlobFromBlob)(
        _In_ IDxcUtils *This,
        _In_ IDxcBlob *pBlob,
        UINT32 offset,
        UINT32 length,
        _COM_Outptr_ IDxcBlob **ppResult);

    HRESULT (STDMETHODCALLTYPE *CreateBlobFromPinned)(
        _In_ IDxcUtils *This,
        _In_bytecount_(size) LPCVOID pData,
        UINT32 size,
        UINT32 codePage,
        _COM_Outptr_ IDxcBlobEncoding **pBlobEncoding);

    HRESULT (STDMETHODCALLTYPE *MoveToBlob)(
        _In_ IDxcUtils *This,
        _In_bytecount_(size) LPCVOID pData,
        IMalloc *pIMalloc,
        UINT32 size,
        UINT32 codePage,
        _COM_Outptr_ IDxcBlobEncoding **pBlobEncoding);

    HRESULT (STDMETHODCALLTYPE *CreateBlob)(
        _In_ IDxcUtils *This,
        _In_bytecount_(size) LPCVOID pData,
        UINT32 size,
        UINT32 codePage,
        _COM_Outptr_ IDxcBlobEncoding **pBlobEncoding);

    HRESULT (STDMETHODCALLTYPE *LoadFile)(
        _In_ IDxcUtils *This,
        _In_z_ LPCWSTR pFileName,
        _In_opt_ UINT32* pCodePage,
        _COM_Outptr_ IDxcBlobEncoding **pBlobEncoding);

    HRESULT (STDMETHODCALLTYPE *CreateReadOnlyStreamFromBlob)(
        _In_ IDxcUtils *This,
        _In_ IDxcBlob *pBlob,
        _COM_Outptr_ IStream **ppStream);

    HRESULT (STDMETHODCALLTYPE *CreateDefaultIncludeHandler)(
        _In_ IDxcUtils *This,
        _COM_Outptr_ IDxcIncludeHandler **ppResult);

    HRESULT (STDMETHODCALLTYPE *GetBlobAsUtf8)(
        _In_ IDxcUtils *This,
        _In_ IDxcBlob *pBlob,
        _COM_Outptr_ IDxcBlobUtf8 **pBlobEncoding);

    HRESULT (STDMETHODCALLTYPE *GetBlobAsUtf16)(
        _In_ IDxcUtils *This,
        _In_ IDxcBlob *pBlob,
        _COM_Outptr_ IDxcBlobUtf16 **pBlobEncoding);

    HRESULT (STDMETHODCALLTYPE *GetDxilContainerPart)(
        _In_ IDxcUtils *This,
        _In_ const DxcBuffer *pShader,
        _In_ UINT32 DxcPart,
        _Outptr_result_nullonfailure_ void **ppPartData,
        _Out_ UINT32 *pPartSizeInBytes);

    HRESULT (STDMETHODCALLTYPE *CreateReflection)(
        _In_ IDxcUtils *This,
        _In_ const DxcBuffer *pData,
        REFIID iid,
        void **ppvReflection);

    HRESULT (STDMETHODCALLTYPE *BuildArguments)(
        _In_ IDxcUtils *This,
        _In_opt_z_ LPCWSTR pSourceName,
        _In_opt_z_ LPCWSTR pEntryPoint,
        _In_z_ LPCWSTR pTargetProfile,
        _In_opt_count_(argCount) LPCWSTR *pArguments,
        _In_ UINT32 argCount,
        _In_count_(defineCount) const DxcDefine *pDefines,
        _In_ UINT32 defineCount,
        _COM_Outptr_ IDxcCompilerArgs **ppArgs);

    HRESULT (STDMETHODCALLTYPE *GetPDBContents)(
        _In_ IDxcUtils *This,
        _In_ IDxcBlob *pPDBBlob,
        _COM_Outptr_ IDxcBlob **ppHash,
        _COM_Outptr_ IDxcBlob **ppContainer);
} IDxcUtilsVtbl;

struct IDxcUtils {
    const IDxcUtilsVtbl *lpVtbl;
};

// --- IDxcResult ---
DEFINE_GUID(IID_IDxcResult, 0x58346CDA, 0xDDE7, 0x4497, 0x94, 0x61, 0x6F, 0x87, 0xAF, 0x5E, 0x06, 0x59);

typedef struct IDxcResultVtbl {
    // IDxcOperationResult methods
    IDxcOperationResultVtbl IDxcOperationResult_vtbl;

    // IDxcResult methods
    BOOL (STDMETHODCALLTYPE *HasOutput)(
        _In_ IDxcResult *This,
        _In_ DXC_OUT_KIND dxcOutKind);

    HRESULT (STDMETHODCALLTYPE *GetOutput)(
        _In_ IDxcResult *This,
        _In_ DXC_OUT_KIND dxcOutKind,
        _In_ REFIID iid,
        _COM_Outptr_opt_result_maybenull_ void **ppvObject,
        _COM_Outptr_ IDxcBlobUtf16 **ppOutputName); // Note: Original header has ppOutputName as optional, adjusted based on likely usage

    UINT32 (STDMETHODCALLTYPE *GetNumOutputs)(
        _In_ IDxcResult *This);

    DXC_OUT_KIND (STDMETHODCALLTYPE *GetOutputByIndex)(
        _In_ IDxcResult *This,
        UINT32 Index);

    DXC_OUT_KIND (STDMETHODCALLTYPE *PrimaryOutput)(
        _In_ IDxcResult *This);
} IDxcResultVtbl;

struct IDxcResult {
    const IDxcResultVtbl *lpVtbl;
};

// --- IDxcExtraOutputs ---
DEFINE_GUID(IID_IDxcExtraOutputs, 0x319b37a2, 0xa5c2, 0x494a, 0xa5, 0xde, 0x48, 0x01, 0xb2, 0xfa, 0xf9, 0x89);

typedef struct IDxcExtraOutputsVtbl {
    // IUnknown methods
    IUnknownVtbl IUnknown_vtbl;

    // IDxcExtraOutputs methods
    UINT32 (STDMETHODCALLTYPE *GetOutputCount)(
        _In_ IDxcExtraOutputs *This);

    HRESULT (STDMETHODCALLTYPE *GetOutput)(
        _In_ IDxcExtraOutputs *This,
        _In_ UINT32 uIndex,
        _In_ REFIID iid,
        _COM_Outptr_opt_result_maybenull_ void **ppvObject,
        _COM_Outptr_opt_result_maybenull_ IDxcBlobUtf16 **ppOutputType,
        _COM_Outptr_opt_result_maybenull_ IDxcBlobUtf16 **ppOutputName);
} IDxcExtraOutputsVtbl;

struct IDxcExtraOutputs {
    const IDxcExtraOutputsVtbl *lpVtbl;
};

// --- IDxcCompiler3 ---
DEFINE_GUID(IID_IDxcCompiler3, 0x228B4687, 0x5A6A, 0x4730, 0x90, 0x0C, 0x97, 0x02, 0xB2, 0x20, 0x3F, 0x54);

typedef struct IDxcCompiler3Vtbl {
    // IUnknown methods
    IUnknownVtbl IUnknown_vtbl;

    // IDxcCompiler3 methods
    HRESULT (STDMETHODCALLTYPE *Compile)(
        _In_ IDxcCompiler3 *This,
        _In_ const DxcBuffer *pSource,
        _In_opt_count_(argCount) LPCWSTR *pArguments,
        _In_ UINT32 argCount,
        _In_opt_ IDxcIncludeHandler *pIncludeHandler,
        _In_ REFIID riid,
        _Out_ LPVOID *ppResult); // Should be IDxcResult**

    HRESULT (STDMETHODCALLTYPE *Disassemble)(
        _In_ IDxcCompiler3 *This,
        _In_ const DxcBuffer *pObject,
        _In_ REFIID riid,
        _Out_ LPVOID *ppResult); // Should be IDxcResult**
} IDxcCompiler3Vtbl;

struct IDxcCompiler3 {
    const IDxcCompiler3Vtbl *lpVtbl;
};

// --- IDxcValidator ---
DEFINE_GUID(IID_IDxcValidator, 0xA6E82BD2, 0x1FD7, 0x4826, 0x98, 0x11, 0x28, 0x57, 0xE7, 0x97, 0xF4, 0x9A);

typedef struct IDxcValidatorVtbl {
    // IUnknown methods
    IUnknownVtbl IUnknown_vtbl;

    // IDxcValidator methods
    HRESULT (STDMETHODCALLTYPE *Validate)(
        _In_ IDxcValidator *This,
        _In_ IDxcBlob *pShader,
        _In_ UINT32 Flags,
        _COM_Outptr_ IDxcOperationResult **ppResult);
} IDxcValidatorVtbl;

struct IDxcValidator {
    const IDxcValidatorVtbl *lpVtbl;
};

// --- IDxcValidator2 ---
DEFINE_GUID(IID_IDxcValidator2, 0x458e1fd1, 0xb1b2, 0x4750, 0xa6, 0xe1, 0x9c, 0x10, 0xf0, 0x3b, 0xed, 0x92);

typedef struct IDxcValidator2Vtbl {
    // IDxcValidator methods
    IDxcValidatorVtbl IDxcValidator_vtbl;

    // IDxcValidator2 methods
    HRESULT (STDMETHODCALLTYPE *ValidateWithDebug)(
        _In_ IDxcValidator2 *This,
        _In_ IDxcBlob *pShader,
        _In_ UINT32 Flags,
        _In_opt_ DxcBuffer *pOptDebugBitcode,
        _COM_Outptr_ IDxcOperationResult **ppResult);
} IDxcValidator2Vtbl;

struct IDxcValidator2 {
    const IDxcValidator2Vtbl *lpVtbl;
};

// --- IDxcContainerBuilder ---
DEFINE_GUID(IID_IDxcContainerBuilder, 0x334b1f50, 0x2292, 0x4b35, 0x99, 0xa1, 0x25, 0x58, 0x8d, 0x8c, 0x17, 0xfe);

typedef struct IDxcContainerBuilderVtbl {
    // IUnknown methods
    IUnknownVtbl IUnknown_vtbl;

    // IDxcContainerBuilder methods
    HRESULT (STDMETHODCALLTYPE *Load)(
        _In_ IDxcContainerBuilder *This,
        _In_ IDxcBlob *pDxilContainerHeader);

    HRESULT (STDMETHODCALLTYPE *AddPart)(
        _In_ IDxcContainerBuilder *This,
        _In_ UINT32 fourCC,
        _In_ IDxcBlob *pSource);

    HRESULT (STDMETHODCALLTYPE *RemovePart)(
        _In_ IDxcContainerBuilder *This,
        _In_ UINT32 fourCC);

    HRESULT (STDMETHODCALLTYPE *SerializeContainer)(
        _In_ IDxcContainerBuilder *This,
        _Out_ IDxcOperationResult **ppResult);
} IDxcContainerBuilderVtbl;

struct IDxcContainerBuilder {
    const IDxcContainerBuilderVtbl *lpVtbl;
};

// --- IDxcAssembler ---
DEFINE_GUID(IID_IDxcAssembler, 0x091f7a26, 0x1c1f, 0x4948, 0x90, 0x4b, 0xe6, 0xe3, 0xa8, 0xa7, 0x71, 0xd5);

typedef struct IDxcAssemblerVtbl {
    // IUnknown methods
    IUnknownVtbl IUnknown_vtbl;

    // IDxcAssembler methods
    HRESULT (STDMETHODCALLTYPE *AssembleToContainer)(
        _In_ IDxcAssembler *This,
        _In_ IDxcBlob *pShader,
        _COM_Outptr_ IDxcOperationResult **ppResult);
} IDxcAssemblerVtbl;

struct IDxcAssembler {
    const IDxcAssemblerVtbl *lpVtbl;
};

// --- IDxcContainerReflection ---
DEFINE_GUID(IID_IDxcContainerReflection, 0xd2c21b26, 0x8350, 0x4bdc, 0x97, 0x6a, 0x33, 0x1c, 0xe6, 0xf4, 0xc5, 0x4c);

typedef struct IDxcContainerReflectionVtbl {
    // IUnknown methods
    IUnknownVtbl IUnknown_vtbl;

    // IDxcContainerReflection methods
    HRESULT (STDMETHODCALLTYPE *Load)(
        _In_ IDxcContainerReflection *This,
        _In_ IDxcBlob *pContainer);

    HRESULT (STDMETHODCALLTYPE *GetPartCount)(
        _In_ IDxcContainerReflection *This,
        _Out_ UINT32 *pResult);

    HRESULT (STDMETHODCALLTYPE *GetPartKind)(
        _In_ IDxcContainerReflection *This,
        UINT32 idx,
        _Out_ UINT32 *pResult);

    HRESULT (STDMETHODCALLTYPE *GetPartContent)(
        _In_ IDxcContainerReflection *This,
        UINT32 idx,
        _COM_Outptr_ IDxcBlob **ppResult);

    HRESULT (STDMETHODCALLTYPE *FindFirstPartKind)(
        _In_ IDxcContainerReflection *This,
        UINT32 kind,
        _Out_ UINT32 *pResult);

    HRESULT (STDMETHODCALLTYPE *GetPartReflection)(
        _In_ IDxcContainerReflection *This,
        UINT32 idx,
        REFIID iid,
        void **ppvObject);
} IDxcContainerReflectionVtbl;

struct IDxcContainerReflection {
    const IDxcContainerReflectionVtbl *lpVtbl;
};

// --- IDxcOptimizerPass ---
DEFINE_GUID(IID_IDxcOptimizerPass, 0xAE2CD79F, 0xCC22, 0x453F, 0x9B, 0x6B, 0xB1, 0x24, 0xE7, 0xA5, 0x20, 0x4C);

typedef struct IDxcOptimizerPassVtbl {
    // IUnknown methods
    IUnknownVtbl IUnknown_vtbl;

    // IDxcOptimizerPass methods
    HRESULT (STDMETHODCALLTYPE *GetOptionName)(
        _In_ IDxcOptimizerPass *This,
        _COM_Outptr_ LPWSTR *ppResult);

    HRESULT (STDMETHODCALLTYPE *GetDescription)(
        _In_ IDxcOptimizerPass *This,
        _COM_Outptr_ LPWSTR *ppResult);

    HRESULT (STDMETHODCALLTYPE *GetOptionArgCount)(
        _In_ IDxcOptimizerPass *This,
        _Out_ UINT32 *pCount);

    HRESULT (STDMETHODCALLTYPE *GetOptionArgName)(
        _In_ IDxcOptimizerPass *This,
        UINT32 argIndex,
        _COM_Outptr_ LPWSTR *ppResult);

    HRESULT (STDMETHODCALLTYPE *GetOptionArgDescription)(
        _In_ IDxcOptimizerPass *This,
        UINT32 argIndex,
        _COM_Outptr_ LPWSTR *ppResult);
} IDxcOptimizerPassVtbl;

struct IDxcOptimizerPass {
    const IDxcOptimizerPassVtbl *lpVtbl;
};

// --- IDxcOptimizer ---
DEFINE_GUID(IID_IDxcOptimizer, 0x25740E2E, 0x9CBA, 0x401B, 0x91, 0x19, 0x4F, 0xB4, 0x2F, 0x39, 0xF2, 0x70);

typedef struct IDxcOptimizerVtbl {
    // IUnknown methods
    IUnknownVtbl IUnknown_vtbl;

    // IDxcOptimizer methods
    HRESULT (STDMETHODCALLTYPE *GetAvailablePassCount)(
        _In_ IDxcOptimizer *This,
        _Out_ UINT32 *pCount);

    HRESULT (STDMETHODCALLTYPE *GetAvailablePass)(
        _In_ IDxcOptimizer *This,
        UINT32 index,
        _COM_Outptr_ IDxcOptimizerPass** ppResult);

    HRESULT (STDMETHODCALLTYPE *RunOptimizer)(
        _In_ IDxcOptimizer *This,
        IDxcBlob *pBlob,
        _In_count_(optionCount) LPCWSTR *ppOptions,
        UINT32 optionCount,
        _COM_Outptr_ IDxcBlob **pOutputModule,
        _COM_Outptr_opt_ IDxcBlobEncoding **ppOutputText);
} IDxcOptimizerVtbl;

struct IDxcOptimizer {
    const IDxcOptimizerVtbl *lpVtbl;
};

// --- IDxcVersionInfo ---
DEFINE_GUID(IID_IDxcVersionInfo, 0xb04f5b50, 0x2059, 0x4f12, 0xa8, 0xff, 0xa1, 0xe0, 0xcd, 0xe1, 0xcc, 0x7e);

typedef struct IDxcVersionInfoVtbl {
    // IUnknown methods
    IUnknownVtbl IUnknown_vtbl;

    // IDxcVersionInfo methods
    HRESULT (STDMETHODCALLTYPE *GetVersion)(
        _In_ IDxcVersionInfo *This,
        _Out_ UINT32 *pMajor,
        _Out_ UINT32 *pMinor);

    HRESULT (STDMETHODCALLTYPE *GetFlags)(
        _In_ IDxcVersionInfo *This,
        _Out_ UINT32 *pFlags);
} IDxcVersionInfoVtbl;

struct IDxcVersionInfo {
    const IDxcVersionInfoVtbl *lpVtbl;
};

// --- IDxcVersionInfo2 ---
DEFINE_GUID(IID_IDxcVersionInfo2, 0xfb6904c4, 0x42f0, 0x4b62, 0x9c, 0x46, 0x98, 0x3a, 0xf7, 0xda, 0x7c, 0x83);

typedef struct IDxcVersionInfo2Vtbl {
    // IDxcVersionInfo methods
    IDxcVersionInfoVtbl IDxcVersionInfo_vtbl;

    // IDxcVersionInfo2 methods
    HRESULT (STDMETHODCALLTYPE *GetCommitInfo)(
        _In_ IDxcVersionInfo2 *This,
        _Out_ UINT32 *pCommitCount,
        _Outptr_result_z_ char **pCommitHash);
} IDxcVersionInfo2Vtbl;

struct IDxcVersionInfo2 {
    const IDxcVersionInfo2Vtbl *lpVtbl;
};

// --- IDxcVersionInfo3 ---
DEFINE_GUID(IID_IDxcVersionInfo3, 0x5e13e843, 0x9d25, 0x473c, 0x9a, 0xd2, 0x03, 0xb2, 0xd0, 0xb4, 0x4b, 0x1e);

typedef struct IDxcVersionInfo3Vtbl {
    // IUnknown methods
    IUnknownVtbl IUnknown_vtbl; // Note: Original header doesn't show inheritance from IDxcVersionInfo2

    // IDxcVersionInfo3 methods
    HRESULT (STDMETHODCALLTYPE *GetCustomVersionString)(
        _In_ IDxcVersionInfo3 *This,
        _Outptr_result_z_ char **pVersionString);
} IDxcVersionInfo3Vtbl;

struct IDxcVersionInfo3 {
    const IDxcVersionInfo3Vtbl *lpVtbl;
};

// --- IDxcPdbUtils ---
DEFINE_GUID(IID_IDxcPdbUtils, 0xE6C9647E, 0x9D6A, 0x4C3B, 0xB9, 0x4C, 0x52, 0x4B, 0x5A, 0x6C, 0x34, 0x3D);

typedef struct IDxcPdbUtilsVtbl {
    // IUnknown methods
    IUnknownVtbl IUnknown_vtbl;

    // IDxcPdbUtils methods
    HRESULT (STDMETHODCALLTYPE *Load)(
        _In_ IDxcPdbUtils *This,
        _In_ IDxcBlob *pPdbOrDxil);

    HRESULT (STDMETHODCALLTYPE *GetSourceCount)(
        _In_ IDxcPdbUtils *This,
        _Out_ UINT32 *pCount);

    HRESULT (STDMETHODCALLTYPE *GetSource)(
        _In_ IDxcPdbUtils *This,
        _In_ UINT32 uIndex,
        _COM_Outptr_ IDxcBlobEncoding **ppResult);

    HRESULT (STDMETHODCALLTYPE *GetSourceName)(
        _In_ IDxcPdbUtils *This,
        _In_ UINT32 uIndex,
        _Outptr_result_z_ BSTR *pResult);

    HRESULT (STDMETHODCALLTYPE *GetFlagCount)(
        _In_ IDxcPdbUtils *This,
        _Out_ UINT32 *pCount);

    HRESULT (STDMETHODCALLTYPE *GetFlag)(
        _In_ IDxcPdbUtils *This,
        _In_ UINT32 uIndex,
        _Outptr_result_z_ BSTR *pResult);

    HRESULT (STDMETHODCALLTYPE *GetArgCount)(
        _In_ IDxcPdbUtils *This,
        _Out_ UINT32 *pCount);

    HRESULT (STDMETHODCALLTYPE *GetArg)(
        _In_ IDxcPdbUtils *This,
        _In_ UINT32 uIndex,
        _Outptr_result_z_ BSTR *pResult);

    HRESULT (STDMETHODCALLTYPE *GetArgPairCount)(
        _In_ IDxcPdbUtils *This,
        _Out_ UINT32 *pCount);

    HRESULT (STDMETHODCALLTYPE *GetArgPair)(
        _In_ IDxcPdbUtils *This,
        _In_ UINT32 uIndex,
        _Outptr_result_z_ BSTR *pName,
        _Outptr_result_z_ BSTR *pValue);

    HRESULT (STDMETHODCALLTYPE *GetDefineCount)(
        _In_ IDxcPdbUtils *This,
        _Out_ UINT32 *pCount);

    HRESULT (STDMETHODCALLTYPE *GetDefine)(
        _In_ IDxcPdbUtils *This,
        _In_ UINT32 uIndex,
        _Outptr_result_z_ BSTR *pResult);

    HRESULT (STDMETHODCALLTYPE *GetTargetProfile)(
        _In_ IDxcPdbUtils *This,
        _Outptr_result_z_ BSTR *pResult);

    HRESULT (STDMETHODCALLTYPE *GetEntryPoint)(
        _In_ IDxcPdbUtils *This,
        _Outptr_result_z_ BSTR *pResult);

    HRESULT (STDMETHODCALLTYPE *GetMainFileName)(
        _In_ IDxcPdbUtils *This,
        _Outptr_result_z_ BSTR *pResult);

    HRESULT (STDMETHODCALLTYPE *GetHash)(
        _In_ IDxcPdbUtils *This,
        _COM_Outptr_ IDxcBlob **ppResult);

    HRESULT (STDMETHODCALLTYPE *GetName)(
        _In_ IDxcPdbUtils *This,
        _Outptr_result_z_ BSTR *pResult);

    BOOL (STDMETHODCALLTYPE *IsFullPDB)(
        _In_ IDxcPdbUtils *This);

    HRESULT (STDMETHODCALLTYPE *GetFullPDB)(
        _In_ IDxcPdbUtils *This,
        _COM_Outptr_ IDxcBlob **ppFullPDB);

    HRESULT (STDMETHODCALLTYPE *GetVersionInfo)(
        _In_ IDxcPdbUtils *This,
        _COM_Outptr_ IDxcVersionInfo **ppVersionInfo);

    HRESULT (STDMETHODCALLTYPE *SetCompiler)(
        _In_ IDxcPdbUtils *This,
        _In_ IDxcCompiler3 *pCompiler);

    HRESULT (STDMETHODCALLTYPE *CompileForFullPDB)(
        _In_ IDxcPdbUtils *This,
        _COM_Outptr_ IDxcResult **ppResult);

    HRESULT (STDMETHODCALLTYPE *OverrideArgs)(
        _In_ IDxcPdbUtils *This,
        _In_ DxcArgPair *pArgPairs,
        UINT32 uNumArgPairs);

    HRESULT (STDMETHODCALLTYPE *OverrideRootSignature)(
        _In_ IDxcPdbUtils *This,
        _In_ const WCHAR *pRootSignature);
} IDxcPdbUtilsVtbl;

struct IDxcPdbUtils {
    const IDxcPdbUtilsVtbl *lpVtbl;
};

// ---------------------------------------------------------------------------
// DXC CLSIDs (Class Identifiers)
// ---------------------------------------------------------------------------
DEFINE_GUID(CLSID_DxcCompiler,           0x73e22d93, 0xe6ce, 0x47f3, 0xb5, 0xbf, 0xf0, 0x66, 0x4f, 0x39, 0xc1, 0xb0);
DEFINE_GUID(CLSID_DxcLinker,             0xef6a8087, 0xb0ea, 0x4d56, 0x9e, 0x45, 0xd0, 0x7e, 0x1a, 0x8b, 0x78, 0x06);
DEFINE_GUID(CLSID_DxcDiaDataSource,      0xcd1f6b73, 0x2ab0, 0x484d, 0x8e, 0xdc, 0xeb, 0xe7, 0xa4, 0x3c, 0xa0, 0x9f);
DEFINE_GUID(CLSID_DxcCompilerArgs,       0x3e56ae82, 0x224d, 0x470f, 0xa1, 0xa1, 0xfe, 0x30, 0x16, 0xee, 0x9f, 0x9d);
DEFINE_GUID(CLSID_DxcLibrary,            0x6245d6af, 0x66e0, 0x48fd, 0x80, 0xb4, 0x4d, 0x27, 0x17, 0x96, 0x74, 0x8c);
DEFINE_GUID(CLSID_DxcUtils,              0x6245d6af, 0x66e0, 0x48fd, 0x80, 0xb4, 0x4d, 0x27, 0x17, 0x96, 0x74, 0x8c); // Same as DxcLibrary
DEFINE_GUID(CLSID_DxcValidator,          0x8ca3e215, 0xf728, 0x4cf3, 0x8c, 0xdd, 0x88, 0xaf, 0x91, 0x75, 0x87, 0xa1);
DEFINE_GUID(CLSID_DxcAssembler,          0xd728db68, 0xf903, 0x4f80, 0x94, 0xcd, 0xdc, 0xcf, 0x76, 0xec, 0x71, 0x51);
DEFINE_GUID(CLSID_DxcContainerReflection,0xb9f54489, 0x55b8, 0x400c, 0xba, 0x3a, 0x16, 0x75, 0xe4, 0x72, 0x8b, 0x91);
DEFINE_GUID(CLSID_DxcOptimizer,          0xae2cd79f, 0xcc22, 0x453f, 0x9b, 0x6b, 0xb1, 0x24, 0xe7, 0xa5, 0x20, 0x4c);
DEFINE_GUID(CLSID_DxcContainerBuilder,   0x94134294, 0x411f, 0x4574, 0xb4, 0xd0, 0x87, 0x41, 0xe2, 0x52, 0x40, 0xd2);
DEFINE_GUID(CLSID_DxcPdbUtils,           0x54621dfb, 0xf2ce, 0x457e, 0xae, 0x8c, 0xec, 0x35, 0x5f, 0xae, 0xec, 0x7c);

// ---------------------------------------------------------------------------
// DXC API Entry Points (C Function Declarations)
// ---------------------------------------------------------------------------

// Define DXC_API_IMPORT appropriately for C
#ifndef DXC_API_IMPORT
#ifdef _WIN32
#define DXC_API_IMPORT __declspec(dllimport)
#else
#define DXC_API_IMPORT __attribute__ ((visibility ("default")))
#endif
#endif // DXC_API_IMPORT

// DxcCreateInstanceProc typedef
typedef HRESULT (STDMETHODCALLTYPE *DxcCreateInstanceProc)(
    _In_ REFCLSID   rclsid,
    _In_ REFIID     riid,
    _Out_ LPVOID* ppv
);

// DxcCreateInstance2Proc typedef
typedef HRESULT (STDMETHODCALLTYPE *DxcCreateInstance2Proc)(
  _In_ IMalloc    *pMalloc,
  _In_ REFCLSID   rclsid,
  _In_ REFIID     riid,
  _Out_ LPVOID* ppv
);

// DxcCreateInstance function declaration
EXTERN_C DXC_API_IMPORT HRESULT STDMETHODCALLTYPE DxcCreateInstance(
  _In_ REFCLSID   rclsid,
  _In_ REFIID     riid,
  _Out_ LPVOID* ppv
);

// DxcCreateInstance2 function declaration
EXTERN_C DXC_API_IMPORT HRESULT STDMETHODCALLTYPE DxcCreateInstance2(
  _In_ IMalloc    *pMalloc,
  _In_ REFCLSID   rclsid,
  _In_ REFIID     riid,
  _Out_ LPVOID* ppv
);


#endif // __DXC_API_C_H__

