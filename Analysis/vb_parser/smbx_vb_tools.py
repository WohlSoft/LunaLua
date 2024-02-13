#!/usr/bin/env python3
from pprint import pprint
from dataclasses import dataclass
from typing import Optional
import itertools
import sys
import os.path
import antlr4
from VisualBasic6Lexer import VisualBasic6Lexer
from VisualBasic6Parser import VisualBasic6Parser
from VisualBasic6ParserListener import VisualBasic6ParserListener

knownAddresses = {
    "myBackBuffer": 0x00B25028,
    "Coins": 0x00B2C5A8,
    "MessageText": 0x00B250E4,
    "MenuMouseX": 0x00B2D6BC,
    "Cheater": 0x00B2C8C4,
    "Physics": 0x00B2C6DC,
    "ShadowMode": 0x00B2C8AA,
    "MultiHop": 0x00B2C8AC,
    "GodMode": 0x00B2C8C0,
    "BlocksSorted": 0x00B2C894,
    "newEventNum": 0x00B2D710,
    "GFXBlock": 0xB2C95C-16,
    "GFXBlockMask": 0xB2C978-16,
    "GFXBackground2": 0xB2CA00-16,
    "GFXBackground2Height": 0xB2CA44-16,
    "GFXBackground2Width": 0xB2CA60-16,
    "GFXBackgroundMask": 0xB2CC6C-16,
    "GFXNPC": 0xB2CA98-16,
    "noSound": 0x00B2D734,
    "LevelName": 0x00B2D764,
    "BattleIntro": 0x00B2D760,
    "tempTime": 0xB2D738,
}


@dataclass(frozen=True)
class TypeInfo:
    size: int
    align: int
    vbName: str
    cName: str
    cInit: str
    memName: str

    def displayAddress(self, field):
        return field.address

    def yieldCFieldInfo(self, field):
        cNamePart = self.cName
        fieldNamePart = field.name+";"
        addrPart = f'0x{self.displayAddress(field):X}'
        yield cNamePart, fieldNamePart, addrPart


@dataclass(frozen=True)
class FieldInfo:
    address: Optional[int]
    name: str
    typeInfo: TypeInfo

    def __str__(self):
        # Get address string
        if self.typeInfo != None:
            dispAddr = self.typeInfo.displayAddress(self)
        else:
            dispAddr = self.address
        if dispAddr != None:
            addrStr = f'0x{dispAddr:X}'
        else:
            addrStr = '?'

        # Get name
        name = self.name

        # Get type string
        if self.typeInfo != None:
            typeStr = self.typeInfo.vbName
        else:
            typeStr = '?'

        def noneBlank(x): return x if x != None else ""

        return f'{addrStr}\t{noneBlank(self.typeInfo.memName)}\t{name}\t{typeStr}'


def getParentModuleContext(ctx: antlr4.ParserRuleContext):
    while not isinstance(ctx, VisualBasic6Parser.ModuleContext):
        ctx = ctx.parentCtx
    return ctx


def evalConstant(ctx: antlr4.ParserRuleContext, name: str):
    module = getParentModuleContext(ctx)
    # Proccess each module body element
    moduleBody = module.moduleBody()
    for moduleBodyElement in moduleBody.moduleBodyElement():
        # If it is a module block, process each block statement
        moduleBlock = moduleBodyElement.moduleBlock()
        if moduleBlock:
            block = moduleBlock.block()
            for stmt in block.blockStmt():
                # If it's a variable statement, process it
                constStmt = stmt.constStmt()
                if constStmt:
                    # Iterate sub-statements
                    for constSubStmt in constStmt.constSubStmt():
                        if name != constSubStmt.ambiguousIdentifier().getText():
                            continue
                        # We have a match for name
                        return evalValueStmt(constSubStmt.valueStmt())


def evalValueStmt(valueStmt: VisualBasic6Parser.ValueStmtContext):
    if isinstance(valueStmt, VisualBasic6Parser.VsLiteralContext):
        literal = valueStmt.literal()
        integerLiteral = literal.integerLiteral()
        if integerLiteral:
            return int(integerLiteral.getText())
    elif isinstance(valueStmt, VisualBasic6Parser.VsICSContext):
        implicitCallStmt_InStmt = valueStmt.implicitCallStmt_InStmt()
        iCS_S_VariableOrProcedureCall = implicitCallStmt_InStmt.iCS_S_VariableOrProcedureCall()
        if iCS_S_VariableOrProcedureCall:
            constName = iCS_S_VariableOrProcedureCall.ambiguousIdentifier().getText()
            value = evalConstant(valueStmt, constName)
            return value
    elif isinstance(valueStmt, VisualBasic6Parser.VsPlusMinusContext):
        if valueStmt.PLUS():
            return evalValueStmt(valueStmt.valueStmt())
        elif valueStmt.MINUS():
            return -evalValueStmt(valueStmt.valueStmt())
    raise NotImplementedError(
        f"Unknown Statement \"{valueStmt.getText()}\"\n\t{valueStmt.toStringTree(recog=_parser)}")


@dataclass(frozen=True)
class ArraySubscript:
    base: int
    length: int

    @staticmethod
    def fromSubscriptContext(subscript: [VisualBasic6Parser.Subscript_Context]):
        subscript = tuple(evalValueStmt(x) for x in subscript.valueStmt())
        if len(subscript) == 1:
            minIdx = 0
            maxIdx = subscript[0]
        else:
            minIdx = min(*subscript)
            maxIdx = max(*subscript)
        return ArraySubscript(base = minIdx, length = maxIdx - minIdx + 1)

    def asVbRangeString(self):
        return f'{self.base} To {self.base + self.length - 1}'


@dataclass(frozen=True)
class ArrayTypeInfo(TypeInfo):
    subscripts: tuple[ArraySubscript, ...]
    baseType: TypeInfo

    # def yieldCFieldInfo(self, field):
    #     cNamePart = self.baseType.cName
    #     subscriptStr = "".join(f'[{x.length}]' for x in self.subscripts)
    #     fieldNamePart = field.name+subscriptStr+';'
    #     addrPart = f'0x{field.address:X}'
    #     yield cNamePart, fieldNamePart, addrPart


@dataclass(frozen=True)
class ArrayRefTypeInfo(TypeInfo):
    subscripts: tuple[ArraySubscript, ...]
    baseType: TypeInfo

    # Array references store the pointer at offset 16
    def displayAddress(self, field):
        if field.address != None:
            pointerOffset = 16
            if self.baseType.vbName == "StdPicture":
                pointerOffset = 28  # References to arrays of StdPicture are sized bigger for some reason
            return field.address + pointerOffset
        return None

    # def yieldCFieldInfo(self, field):
    #     cNamePart = self.baseType.cName
    #     subscriptStr = "".join(f'[{x.length}]' for x in self.subscripts)
    #     fieldNamePart = f'(&{field.name}){subscriptStr};'
    #     pointerOffset = 16
    #     if self.baseType.vbName == "StdPicture":
    #         pointerOffset = 28  # References to arrays of StdPicture are sized bigger for some reason
    #     yield 'uint8_t', f'_reserved_before_{field.name}[{pointerOffset}];', f'0x{field.address:X}'
    #     yield cNamePart, fieldNamePart, f'0x{field.address+pointerOffset:X}'
    #     yield 'uint8_t', f'_reserved_after_{field.name}[{self.size-pointerOffset-4}];', f'0x{field.address+pointerOffset+4:X}'


@dataclass(frozen=True)
class StructInfo(TypeInfo):
    fields: tuple[FieldInfo, ...]


basicTypes = dict((x.vbName, x) for x in (
    TypeInfo(size=1, align=1, vbName="Byte",
             cName="uint8_t",   cInit="0",     memName="FIELD_BYTE"),
    TypeInfo(size=2, align=2, vbName="Integer",
             cName="int16_t",   cInit="0",     memName="FIELD_WORD"),
    TypeInfo(size=4, align=4, vbName="Long",
             cName="int32_t",   cInit="0",     memName="FIELD_DWORD"),
    TypeInfo(size=2, align=2, vbName="Boolean",
             cName="VB6Bool",   cInit="false", memName="FIELD_BOOL"),
    TypeInfo(size=4, align=4, vbName="Single",
             cName="float",     cInit="0.0f",  memName="FIELD_FLOAT"),
    TypeInfo(size=8, align=4, vbName="Double",
             cName="double",    cInit="0.0",   memName="FIELD_DFLOAT"),
    TypeInfo(size=4, align=4, vbName="String",
             cName="VB6StrPtr", cInit="L\"\"", memName="FIELD_STRING"),
))
paddingTypes = {
    1: basicTypes["Byte"],
    2: basicTypes["Integer"],
    3: basicTypes["Long"],
}


def getArrVbTypeName(baseType, subscripts, byReference):
    if baseType == None:
        return '?'
    subscriptStr = ", ".join(x.asVbRangeString() for x in subscripts)
    name = f'{baseType.vbName}({subscriptStr})'
    if byReference:
        name += ' (BY REFERENCE)'
    return name


def getArrCTypeName(baseType, subscripts, byReference):
    subscriptStr = ", ".join(f'{x.base}, {x.base + x.length - 1}' for x in reversed(subscripts))
    if baseType == None:
        return '?'
    elif byReference:
        return f'VB6ArrayRef<{baseType.cName}, {subscriptStr}>'
    elif len(subscripts) == 2:
        return f'VB6Array2D<{baseType.cName}, {subscriptStr}>'    
    else:
        return f'VB6Array<{baseType.cName}, {subscriptStr}>'


def getTypeInfo(subscripts, varType, inStructure, typeDb=None):
    if subscripts and not inStructure:
        baseType = getTypeInfo(None, varType, inStructure, typeDb)
        arrRefSize = 28
        if baseType.vbName == "StdPicture":
            arrRefSize = 40  # References to arrays of StdPicture are sized bigger for some reason
        return ArrayRefTypeInfo(
            size=arrRefSize,
            align=4,
            vbName=getArrVbTypeName(baseType, subscripts, True),
            cName=getArrCTypeName(baseType, subscripts, True),
            memName=None,
            subscripts=subscripts,
            baseType=baseType
        )
    elif subscripts:
        baseType = getTypeInfo(None, varType, inStructure, typeDb)
        arrSize = baseType.size
        if arrSize != None or True:
            for subscript in subscripts:
                arrSize *= subscript.length
        return ArrayTypeInfo(
            size=arrSize,
            align=baseType.align,
            vbName=getArrVbTypeName(baseType, subscripts, False),
            cName=getArrCTypeName(baseType, subscripts, False),
            memName=None,
            subscripts=subscripts,
            baseType=baseType
        )
    elif varType in basicTypes:
        return basicTypes[varType]
    elif (typeDb != None) and (varType in typeDb):
        return typeDb[varType]
    else:
        return TypeInfo(
            size=None,
            align=None,
            vbName=varType,
            #cName=f"SMBX13::Types::{varType}_t",
            cName=f"{varType}_t",
            memName=None
        )


def iterateFieldInformation(items, initialAddr=0, inStructure=True, typeDb=None, knownAddresses=None, printMatches=False):
    nextAddr = initialAddr
    structAlign = 4
    padCount = 0
    for ctx in items:
        # Get name
        varName = ctx.ambiguousIdentifier().getText()

        # Get subscripts
        varSubscripts = ctx.subscripts()
        if varSubscripts:
            varSubscripts = tuple(ArraySubscript.fromSubscriptContext(x)
                                  for x in varSubscripts.subscript_())

        # Get type
        varType = ctx.asTypeClause().type_()
        varBaseType = varType.baseType()
        if varBaseType:
            varType = varBaseType.getText()
        else:
            varComplexType = varType.complexType()
            if varComplexType:
                varType = varComplexType.getText()

        # Attempt to get var size
        typeInfo = getTypeInfo(varSubscripts, varType, inStructure, typeDb)

        # Padding
        varAddr = nextAddr
        if (varAddr != None) and (typeInfo != None):
            mod = varAddr % typeInfo.align
            if mod != 0:
                padCount += 1
                padLen = typeInfo.align - mod
                yield FieldInfo(
                    address=varAddr,
                    name=f"_padding{padCount}_",
                    typeInfo=paddingTypes[padLen]
                )
                varAddr += padLen

        # Check variable against reference of known addresses
        if (knownAddresses != None):
            refAddr = knownAddresses.get(varName, None)
            if refAddr:
                if (varAddr != None) and (varAddr != refAddr):
                    print(
                        f"MISMATCH! {varName} at {refAddr:X} not {varAddr:X}")
                if printMatches and (varAddr != None) and (varAddr == refAddr):
                    print(
                        f"MATCH! {varName} at {refAddr:X}")
                varAddr = refAddr

        # Update text address
        nextAddr = None
        if (varAddr != None) and (typeInfo != None):
            nextAddr = varAddr + typeInfo.size

        yield FieldInfo(
            address=varAddr,
            name=varName,
            typeInfo=typeInfo
        )

    # Final padding
    if inStructure:
        varAddr = nextAddr
        mod = varAddr % structAlign
        if mod != 0:
            padCount += 1
            padLen = structAlign - mod
            yield FieldInfo(
                address=varAddr,
                name=f"_padding{padCount}_",
                typeInfo=paddingTypes[padLen]
            )


def iterateTopLevelTypes(module: VisualBasic6Parser.ModuleContext):
    # Proccess each module body element
    moduleBody = module.moduleBody()
    for moduleBodyElement in moduleBody.moduleBodyElement():
        # If it is a type statement, process it
        typeStmt = moduleBodyElement.typeStmt()
        if typeStmt:
            # Yield type statment
            yield typeStmt


def iterateTypeFields(typeStmt: VisualBasic6Parser.TypeStmtContext):
    for typeStmt_Element in typeStmt.typeStmt_Element():
        yield typeStmt_Element


def enumerateTypeInformation(module: VisualBasic6Parser.ModuleContext, typeDb={}):
    types = []
    for typeStmt in iterateTopLevelTypes(module):
        # Get visibility
        visibility = typeStmt.visibility()
        if visibility:
            visibility = visibility.getText()

        # Get name
        typeName = typeStmt.ambiguousIdentifier().getText()

        # Parse fields from the type
        fields = tuple(iterateFieldInformation(
            typeStmt.typeStmt_Element(),
            inStructure=True,
            typeDb=typeDb,
        ))

        # Get structure size if possible
        typeSize = None
        if (len(fields) > 0):
            lastField = fields[len(fields)-1]
            if (lastField.address != None) and (lastField.typeInfo.size != None):
                typeSize = lastField.address + lastField.typeInfo.size

        # Take the alignment to be the max of types in the structure
        typeAlign = 0
        for field in fields:
            typeAlign = max(typeAlign, field.typeInfo.align)

        # Add type to typeDb
        typeInfo = StructInfo(
            size=typeSize,
            align=typeAlign,
            vbName=typeName,
            #cName=f'SMBX13::Types::{typeName}_t',
            cName=f'{typeName}_t',
            memName=None,
            fields=fields
        )
        typeDb[typeName] = typeInfo
        types.append(typeInfo)
    return types


def dumpTypeInformation(typeInfo: StructInfo):
    print(f"Type: {typeInfo.vbName}")
    for field in typeInfo.fields:
        print(f"\t{field}")
    print(f"\tType Size: 0x{typeInfo.size:X}")


def dumpTypes(types: [StructInfo], makeFlat=True, iterateArrays=True):
    for typeInfo in types:
        if makeFlat:
            typeInfo = makeFlatStruct(typeInfo, iterateArrays=iterateArrays)
        dumpTypeInformation(typeInfo)

def dumpTypeInformationAsCStruct(typeInfo: StructInfo):
    yield f'struct {typeInfo.cName.split("::")[-1]} {{'
    maxCNameLen = 0
    maxFieldLen = 0
    fieldParts = []
    for field in typeInfo.fields:
        for cNamePart, fieldNamePart, addrPart in field.typeInfo.yieldCFieldInfo(field):
            fieldParts.append((cNamePart, fieldNamePart, addrPart))
            maxCNameLen = max(maxCNameLen, len(cNamePart))
            maxFieldLen = max(maxFieldLen, len(fieldNamePart))
    for cNamePart, fieldNamePart, addrPart in fieldParts:
        yield f'    {cNamePart:<{maxCNameLen}} {fieldNamePart:<{maxFieldLen}} // {addrPart}'
    yield f'}};'
    yield f'#if !defined(__INTELLISENSE__)'
    yield f'    static_assert(sizeof({typeInfo.cName}) == 0x{typeInfo.size:X}, "sizeof({typeInfo.cName}) must be 0x{typeInfo.size:X}");'
    yield f'#endif'
    yield f''

def iterateTopLevelVariables(module: VisualBasic6Parser.ModuleContext, publicOnly: bool):
    # Proccess each module body element
    moduleBody = module.moduleBody()
    for moduleBodyElement in moduleBody.moduleBodyElement():
        # If it is a module block, process each block statement
        moduleBlock = moduleBodyElement.moduleBlock()
        if moduleBlock:
            block = moduleBlock.block()
            for stmt in block.blockStmt():
                # If it's a variable statement, process it
                variableStmt = stmt.variableStmt()
                if variableStmt:
                    # Get visibility
                    visibility = variableStmt.visibility()
                    if visibility:
                        visibility = visibility.getText()
                    if publicOnly and ((visibility == None) or (visibility.lower() != "public")):
                        continue
                    # Iterate sub-statements
                    for variableSubStmt in variableStmt.variableListStmt().variableSubStmt():
                        yield variableSubStmt


def enumerateTopLevelVariables(module: VisualBasic6Parser.ModuleContext, moduleName="module", typeDb=None):
    # Get fields from top level of module
    fields = tuple(iterateFieldInformation(
        iterateTopLevelVariables(module, publicOnly=True),
        initialAddr=None,
        inStructure=False,
        typeDb=typeDb,
        knownAddresses=knownAddresses,
    ))

    # Get structure size if possible
    typeSize = None
    if (len(fields) > 0):
        lastField = fields[len(fields)-1]
        if (lastField.address != None) and (lastField.typeInfo.size != None):
            typeSize = lastField.address + \
                lastField.typeInfo.size - fields[0].address

    # Take the alignment to be the max of types in the structure
    typeAlign = 0
    for field in fields:
        typeAlign = max(typeAlign, field.typeInfo.align)

    # Placeholder module name
    modName = "module"

    # Return top level variables as a structure
    return StructInfo(
        size=typeSize,
        align=typeAlign,
        vbName=moduleName,
        cName=f'SMBX13::Types::{moduleName.split(".", 1)[0]}_t',
        memName=None,
        fields=fields
    )


def iterateFlattenField(field: FieldInfo, iterateArrays=False):
    """
    If this field is of StructInfo type, yield for each sub-field within it.
    Optionally, if it's an ArrayTypeInfo field, yield for each array element.
    Otherwise just yield itself back out.
    """
    if isinstance(field.typeInfo, StructInfo):
        for subField in field.typeInfo.fields:
            for finalField in iterateFlattenField(subField, iterateArrays=iterateArrays):
                yield FieldInfo(
                    address=field.address + finalField.address,
                    name=f'{field.name}.{finalField.name}',
                    typeInfo=finalField.typeInfo
                )
    elif isinstance(field.typeInfo, ArrayTypeInfo) and iterateArrays:
        baseType = field.typeInfo.baseType
        subscripts = field.typeInfo.subscripts
        ranges = tuple(range(x.length) for x in reversed(subscripts))
        for indices in itertools.product(*ranges):
            flatIdx = 0
            idxStr = []
            for subscript, idx in zip(reversed(subscripts), reversed(indices)):
                flatIdx *= subscript.length
                flatIdx += idx
                idxStr.append(f'{idx+subscript.base}')
            idxStr = ",".join(reversed(idxStr))
            for subField in iterateFlattenField(FieldInfo(
                    address=field.address + flatIdx * baseType.size,
                    name=f'{field.name}({idxStr})',
                    typeInfo=baseType), iterateArrays=iterateArrays):
                yield subField
    else:
        yield field


def makeFlatStruct(struct: StructInfo, iterateArrays=False):
    """
    Converts a struct to a flattened version of itself.
    """
    def iterNewFields():
        for field in struct.fields:
            for subField in iterateFlattenField(field, iterateArrays=iterateArrays):
                yield subField
    return StructInfo(
        size=struct.size,
        align=struct.align,
        vbName=struct.vbName,
        cName=struct.cName,
        memName=struct.memName,
        fields=tuple(iterNewFields())
    )


safeTypes = {
    basicTypes["Byte"],
    basicTypes["Integer"],
    basicTypes["Long"],
    basicTypes["Boolean"],
    basicTypes["Single"],
    basicTypes["Double"]
}
blacklistVars = {
    "myBackBuffer",
	"myBufferBMP",
    "GFXBlock",
    "GFXBlockMask",
    "GFXBlockBMP",
    "GFXBlockMaskBMP",
    "GFXBackground2",
    "GFXBackground2BMP",
    "GFXNPC",
    "GFXNPCMask",
    "GFXNPCBMP",
    "GFXNPCMaskBMP",
    "GFXEffect",
    "GFXEffectMask",
    "GFXEffectBMP",
    "GFXEffectMaskBMP",
    "GFXBackground",
    "GFXBackgroundMask",
    "GFXBackgroundBMP",
    "GFXBackgroundMaskBMP",
    "GFXMario",
    "GFXMarioMask",
    "GFXMarioBMP",
    "GFXMarioMaskBMP",
    "GFXLuigi",
    "GFXLuigiMask",
    "GFXLuigiBMP",
    "GFXLuigiMaskBMP",
    "GFXPeach",
    "GFXPeachMask",
    "GFXPeachBMP",
    "GFXPeachMaskBMP",
    "GFXToad",
    "GFXToadMask",
    "GFXToadBMP",
    "GFXToadMaskBMP",
    "GFXLink",
    "GFXLinkMask",
    "GFXLinkBMP",
    "GFXLinkMaskBMP",
    "GFXYoshiB",
    "GFXYoshiBMask",
    "GFXYoshiBBMP",
    "GFXYoshiBMaskBMP",
    "GFXYoshiT",
    "GFXYoshiTMask",
    "GFXYoshiTBMP",
    "GFXYoshiTMaskBMP",
    "GFXTileCustom",
    "GFXTile",
    "GFXTileBMP",
    "GFXLevelCustom",
    "GFXLevel",
    "GFXLevelMask",
    "GFXLevelBMP",
    "GFXLevelMaskBMP",
    "GFXSceneCustom",
    "GFXScene",
    "GFXSceneMask",
    "GFXSceneBMP",
    "GFXSceneMaskBMP",
    "GFXPathCustom",
    "GFXPath",
    "GFXPathMask",
    "GFXPathBMP",
    "GFXPathMaskBMP",
    "GFXPlayerCustom",
    "GFXPlayer",
    "GFXPlayerMask",
    "GFXPlayerBMP",
    "GFXPlayerMaskBMP",
}


def getSafeRanges(struct: StructInfo):
    """
    Yields the safe ranges within a structure
    """
    startAddr = None
    endAddr = None
    for field in makeFlatStruct(struct, iterateArrays=True).fields:
        if (field.typeInfo in safeTypes) and (field.name not in blacklistVars):
            # This is a safe type, update the addresses
            if startAddr == None:
                startAddr = field.address
            endAddr = field.address + field.typeInfo.size - 1
        elif (startAddr != None):
            # This is not a safe type, and we have a pending range to flush
            yield (startAddr, endAddr)
            # Clear accumulated range
            startAddr, endAddr = None, None
    if (startAddr != None):
        yield (startAddr, endAddr)

def getStringAddresses(struct: StructInfo):
    """
    Yield addresses of strings within a structure
    """
    for field in makeFlatStruct(struct, iterateArrays=True).fields:
        if (field.typeInfo == basicTypes["String"]):
            yield field.address

def dumpSafeRanges(struct: StructInfo):
    print(f"Safe ranges for {struct.vbName}")
    for start, end in getSafeRanges(struct):
        print(f"\t0x{start:X} to 0x{end:X}")

def dumpStringAddresses(struct: StructInfo):
    print(f"String addresses in {struct.vbName}")
    for address in getStringAddresses(struct):
        print(f"\t0x{address:X}")

def getAllowableArrayRefs(struct: StructInfo):
    for field in makeFlatStruct(struct, iterateArrays=True).fields:
        if isinstance(field.typeInfo, ArrayRefTypeInfo) and (field.name not in blacklistVars):
            yield field

def dumpAllowableArrayRefs(struct: StructInfo):
    print(f"Array bases in {struct.vbName}")
    for field in getAllowableArrayRefs(struct):
        print(f"\t{field}")

def getParserForArgv(argv):
    if len(argv) > 1:
        fn = argv[1]
        moduleName = os.path.basename(fn)
        inputStream = antlr4.FileStream(fn)
    else:
        moduleName = "stdin"
        inputStream = antlr4.InputStream(sys.stdin.readline())

    # Init lexer and pareser
    lexer = VisualBasic6Lexer(inputStream)
    tokens = antlr4.CommonTokenStream(lexer)
    parser = VisualBasic6Parser(tokens)
    return parser, moduleName

def getFuncList():
    funcs = set()
    funcArgStrs = {}
    with open("reference_funcs.tsv", "r") as f:
        isHdr = True
        currentNamespace = None
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
            funcName = cols[3]
            argStr = cols[4]
            funcs.add(funcName)
            funcArgStrs[funcName] = argStr
    return funcs, funcArgStrs

def main(argv):
    print("This is just a shared library")

if __name__ == '__main__':
    main(sys.argv)
