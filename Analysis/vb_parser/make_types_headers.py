from smbx_vb_tools import *

def main(argv):

    # Read from stdin or specified file
    parser, moduleName = getParserForArgv(argv)

    # Run parsing
    module = parser.module()

    # Test enumerating type information
    typeDb = {}
    types = enumerateTypeInformation(module, typeDb=typeDb)
    moduleVars = (enumerateTopLevelVariables(module, moduleName=moduleName, typeDb=typeDb))
    
    # Dump block structure info
    #dumpTypeInformation(makeFlatStruct(typeDb["Player"], iterateArrays=True))
    #dumpSafeRanges(typeDb["Player"])

    #dumpTypeInformation(moduleVars)
    #dumpSafeRanges(moduleVars)
    #dumpStringAddresses(moduleVars)
    #dumpAllowableArrayRefs(moduleVars)
    #dumpTypeInformation(typeDb["NPC"])

    print("#if !defined(SMBXINTERNAL_TYPES_H)")
    print("#define SMBXINTERNAL_TYPES_H")
    print("")

    print('#include <cstdint>')
    print('#include "../Misc/VB6StrPtr.h"')
    print('#include "../Misc/VB6Bool.h"')
    print('#include "../Misc/VB6Array.h"')
    print('')
    print("#pragma pack(push, 1)")
    print("namespace SMBX13 {")
    print("    namespace Types {")

    print("        struct StdPicture_t;\n")
    for type_ in types:
        for line in dumpTypeInformationAsCStruct(type_):
            print("        "+line)
    for line in dumpTypeInformationAsCStruct(moduleVars):
        print("        "+line)
    print("    }\n")
    print('}')
    print("#pragma pack(pop)")
    print("")
    print("#endif // !defined(SMBXINTERNAL_TYPES_H)")
    print("")


    print("#if !defined(SMBXINTERNAL_VARIABLES_H)")
    print("#define SMBXINTERNAL_VARIABLES_H")
    print("")

    print('#include "Types.h"')
    print('')
    print("namespace SMBX13 {")
    print(f"    static {moduleVars.cName}& vars = *reinterpret_cast<{moduleVars.cName}*>(0x{moduleVars.fields[0].address:X});")
    print('')
    print('    // Present structure as namespace as well')
    print('    namespace Vars {')
    for field in moduleVars.fields:
        if field.name.startswith("_padding"):
            continue
        print(f'    static auto& {field.name} = SMBX13::vars.{field.name};')
    print('    }')
    print('}')
    print("")
    print("#endif // !defined(SMBXINTERNAL_VARIABLES_H)")
    print("")

if __name__ == '__main__':
    main(sys.argv)