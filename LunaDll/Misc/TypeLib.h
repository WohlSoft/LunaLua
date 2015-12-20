#ifndef TYPE_LIB_H__
#define TYPE_LIB_H__

#include <windows.h>

#include <string>
#include <iostream>

#include "win32_Unicode.h"

class TypeLib {
  public:
    TypeLib();

    bool Open(ITypeLib* toRead);
    bool Open(std::string const& type_lib_file);

    std::string LibDocumentation();

    int NofTypeInfos();

    bool CurTypeInfo(int);

    std::string TypeDocumentation();

    // within current type info
    int   NofFunctions();
    // within current function
    short NofParameters();
    short NofOptionalParameters();

    // within current type info
    int NofVariables();

    std::string FunctionName();
    std::string VariableName();
    std::string ParameterName();

    enum INVOKEKIND {
      func   = DISPATCH_METHOD,
      get    = DISPATCH_PROPERTYGET,
      put    = DISPATCH_PROPERTYPUT,
      putref = DISPATCH_PROPERTYPUTREF
    };

    INVOKEKIND InvokeKind();

    // See <oaidl.h>
    enum VARIABLEKIND {
      instance  = VAR_PERINSTANCE,
      static_   = VAR_STATIC,
      const_    = VAR_CONST,
      dispatch  = VAR_DISPATCH
    };
    VARIABLEKIND VariableKind();

    // See <oaidl.h>
    enum TYPEFLAG {
      FDEFAULT       = IMPLTYPEFLAG_FDEFAULT,
      FSOURCE        = IMPLTYPEFLAG_FSOURCE,
      FRESTRICTED    = IMPLTYPEFLAG_FRESTRICTED,
      FDEFAULTVTABLE = IMPLTYPEFLAG_FDEFAULTVTABLE,
    };

    // TODO? How to be used correctly
    bool HasFunctionTypeFlag(TYPEFLAG);

    // see here
    bool IsTypeEnum     ();
    bool IsTypeRecord   ();
    bool IsTypeModule   ();
    bool IsTypeInterface();
    bool IsTypeDispatch ();
    bool IsTypeCoClass  ();
    bool IsTypeAlias    ();
    bool IsTypeUnion    ();
    bool IsTypeMax      ();

    std::string ReturnType();
    std::string ParameterType();

    std::string VariableType();

    // Only makes sense if VariableKind == VARIABLEKIND.const_
    std::string ConstValue();

    bool NextTypeInfo();

    bool NextFunction();
    bool NextVariable();

    bool NextParameter();

    bool ParameterIsIn();
    bool ParameterIsOut();
    bool ParameterIsFLCID();
    bool ParameterIsReturnValue();
    bool ParameterIsOptional();
    bool ParameterHasDefault();
    bool ParameterHasCustumData();

  private:
    ITypeLib*     typeLib_;
    int           nofTypeInfos_;
    int           curTypeInfo_;

    ITypeInfo*    curITypeInfo_;
    TYPEATTR*     curTypeAttr_;

    int           curTypeIdx_;

    int           curFunc_;
    int           curVar_;
    int           curFuncParam_;

    int           curImplTypeFlags_;

    FUNCDESC*     curFuncDesc_;

    VARDESC*      curVarDesc_;

    BSTR*         funcNames_;
    unsigned int  nofFuncNames_;

    void          ReleaseFuncNames();
    void          GetFuncNames();

    bool          ParameterIsHasX(int);

    bool          IsTypeKind(int);

    std::string   TypeDocumentation_(ITypeInfo*);

    std::string   UserdefinedType(HREFTYPE);
    std::string   Type(ELEMDESC const&);
};

#endif