
# Requires 
from clang.cindex import Index, CursorKind
from optparse import OptionParser
import lunaparse
import lunagen
import sys, os
import ccsyspath


def main():
    parser = OptionParser("usage: %prog filename struct-name [options]")
    parser.add_option("-o", "--output-dir", dest="output",
                   help="The output directory for the generated files", type="string")
    parser.disable_interspersed_args()
    (options_result, args) = parser.parse_args()
    
    # Check for args
    # 1. Input file
    if len(args) <= 0:
        parser.error("No target file specified")
    # 2. Struct name --> for C++ Struct name
    if len(args) <= 1:
        parser.error("No struct name specified")
    
    # Fetch args
    input_file = args[0]
    struct_name = args[1]
    output_dir = options_result.output
    if output_dir is None:
        output_dir = os.path.dirname(os.path.realpath(sys.argv[0]))

    lunalua_index = Index.create()
    args = '-x c++ --std=c++1z -D__LUNA_CODE_GENERATOR__'.split()
    syspath = ccsyspath.system_include_paths('clang++')
    incargs = [b'-I' + inc for inc in syspath]
    args = args + incargs
    print args
    example_file = lunalua_index.parse(input_file, args)
    
    if not example_file:
        print("Failed to load example file!")
        return
    
    my_cur = example_file.cursor
    parsed_class = lunaparse.find_and_parse_class(my_cur, CursorKind.STRUCT_DECL, struct_name)
    if parsed_class is None:
        print("Failed to find class to parse!")
        return
    
    lunagen.generate_binding_class(parsed_class, output_dir, struct_name)
    
    
if __name__ == '__main__':
    main()


