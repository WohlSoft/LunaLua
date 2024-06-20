#!/usr/bin/env python3
import sys
from pprint import pprint
from dataclasses import dataclass

import antlr4
from VisualBasic6Lexer import VisualBasic6Lexer
from VisualBasic6Parser import VisualBasic6Parser
from VisualBasic6ParserListener import VisualBasic6ParserListener
from make_converted_func import VBTranslator

@dataclass(frozen=True)
class TypeInfo:
    size: int
    align: int
    vbName: str
    cName: str
    memName: str

    def displayAddress(self, field):
        return field.address

basicTypes = dict((x.vbName, x) for x in (
    TypeInfo(size=1, align=1, vbName="Byte",
             cName="uint8_t",  memName="FIELD_BYTE"),
    TypeInfo(size=2, align=2, vbName="Integer",
             cName="int16_t",  memName="FIELD_WORD"),
    TypeInfo(size=4, align=4, vbName="Long",
             cName="int32_t",  memName="FIELD_DWORD"),
    TypeInfo(size=2, align=2, vbName="Boolean",
             cName="VB6Bool",  memName="FIELD_BOOL"),
    TypeInfo(size=4, align=4, vbName="Single",
             cName="float",    memName="FIELD_FLOAT"),
    TypeInfo(size=8, align=4, vbName="Double",
             cName="double",   memName="FIELD_DFLOAT"),
    TypeInfo(size=4, align=4, vbName="String",
             cName="VB6StrPtr", memName="FIELD_STRING"),
))

def typeToC(type_):
    if type_ in basicTypes:
        return basicTypes[type_].cName
    else:
        return f'SMBX13::Types::{type_}_t'

def parseArgList(argStr, forceByValBasicTypes=False, includeDefaults=False, nameOnly=False, fullType=True):
    inputStream = antlr4.InputStream(
        "Function Foobar" + argStr + "\nEnd Function"
        )
    lexer = VisualBasic6Lexer(inputStream)
    tokens = antlr4.CommonTokenStream(lexer)
    parser = VisualBasic6Parser(tokens)
    functionStmt = parser.functionStmt()
    argList = functionStmt.argList()
    asTypeClause = functionStmt.asTypeClause()
    
    translator = VBTranslator(parser)

    returnTypeC = "void"
    if asTypeClause:
        type_ = asTypeClause.type_().getText()
        returnTypeC = typeToC(type_)

    argsC = []
    for arg in argList.arg():
        # Get by val or reference
        byVal = arg.BYVAL()
        if byVal != None:
            byVal = True
        else:
            byVal = False
        byRef = not byVal

        if arg.asTypeClause():
            varType = typeToC(arg.asTypeClause().type_().getText())
            if forceByValBasicTypes and not varType.startswith("SMBX13::"):
                byRef = False
                byVal = True
            if byRef:
                varType += "&"
        else:
            varType = "void*"
        varName = arg.ambiguousIdentifier().getText()
        if varName == "bool":
            varName += "_"

        defValStr = ""
        if includeDefaults:
            argDefault = arg.argDefaultValue()
            if argDefault:
                defValStr = "=" + translator.translateValueStmt(argDefault.valueStmt())

        if nameOnly:
            argsC.append(varName)
        else:
            argsC.append(f'{varType} {varName}{defValStr}')
    argListC = ", ".join(argsC)
    
    if fullType:
        return f'{returnTypeC}(__stdcall *)({argListC})'
    else:
        return argListC, returnTypeC

def main(argv):
    print("#if !defined(SMBXINTERNAL_FUNCTIONS_H)")
    print("#define SMBXINTERNAL_FUNCTIONS_H")
    print("")

    print('#include <cstdint>')
    print('#include "../Misc/VB6StrPtr.h"')
    print('#include "../Misc/VB6Bool.h"')
    print('')

    print("namespace SMBX13 {")
    print("    namespace Types {")
    print("        struct Location_t;")
    print("    }\n")

    with open("reference_funcs.tsv", "r") as f:
        isHdr = True
        currentNamespace = None
        namespaces = []
        for line in f:
            line = line.strip()
            if len(line) == 0:
                continue
            cols = line.split("\t")
            cols = cols + (5-len(cols))*[""]
            if isHdr:
                isHdr = False
                continue
            if cols[1] == "":
                continue
            if cols[0] == "UNUSED":
                continue
            
            addr = int(cols[0], 16)
            modName = cols[1].split(".", 1)[0]
            funcType = cols[2]
            funcName = cols[3]
            argStr = cols[4]
            isPublic = ("private" not in funcType.lower())
            #if not isPublic:
            #    continue
            #if "Function" not in funcType:
            #    continue
            args = parseArgList(argStr)
            argsByVal, retTypeC = parseArgList(argStr, forceByValBasicTypes=True, includeDefaults=True, fullType=False)
            argNames, retTypeC = parseArgList(argStr, nameOnly=True, fullType=False)

            namespace = modName
            if not isPublic:
                namespace += "_Private"
            elif namespace not in namespaces:
                namespaces.append(namespace)
            ptrLine = f'static const auto _{funcName}_ptr = reinterpret_cast<{args}>(0x{addr:08X});'
            retStr = ""
            if retTypeC != "void":
                retStr = "return "
            funcLine = f'inline {retTypeC} {funcName}({argsByVal}) {{ {retStr}_{funcName}_ptr({argNames}); }}'

            if namespace != currentNamespace:
                if currentNamespace != None:
                    print('    }\n')
                currentNamespace = namespace
                print(f'    namespace {namespace} {{')
            print(f'        {ptrLine}')
            print(f'        {funcLine}')
        print('    }\n')
        print('    // All public functions')
        print('    namespace Functions {')
        for namespace in namespaces:
            print(f"        using namespace {namespace};")
        print('    }')
        print('}\n')
        print("#endif // !defined(SMBXINTERNAL_FUNCTIONS_H)")
        print("")

if __name__ == '__main__':
    main(sys.argv)
