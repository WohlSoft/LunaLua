from lunaparse import LunaClass, SpecialLunaFieldType
import shutil
import os


# TODO: Generate the helper classes if not exist

def generate_binding_class(class_obj, generation_path, struct_name):
    """
    Generates a binding file
    
    @type class_obj: LunaClass
    @type generation_path: str
    
    @param class_obj: The LunaClass instance which represents a LunaDll struct
    @param generation_path: A directory where the generated files are placed.
    
    """

    # Local constants
    lunagen_helper_filename = "LunaGenHelperUtils.h"
    lunagen_name_prefix = "LunaGen"

    # Get the file path to the execution folder (the location of THIS file)
    # and add the subfolder include to the path string
    file_path_to_include = os.path.dirname(os.path.realpath(__file__)) + "/include/"
    
    # Now copy the helper headers to the generation folder:
    # {thisfile}/include/LunaGenHelperUtils.h --> {generation_folder}/LunaGenHelperUtils.h
    # DO COPY ONLY WHEN FINISHED!!!!
    # shutil.copy2(file_path_to_include + lunagen_helper_filename, generation_path + "/" + lunagen_helper_filename)
    

    # Now we need the file of the original file to include this to the original one
    class_file_to_include = os.path.relpath(class_obj.file_path, generation_path).replace("\\", "/")

    # Generation name (i.e Class name is "Block" --> LunaGenBlock)
    generator_name = lunagen_name_prefix + class_obj.name

    # Open file and write include base
    generator_writer = open(generation_path + "/" + generator_name + ".h", "w")
    generator_header_lock = generator_name + "_hhhhh";
    generator_writer.write("#ifndef " + generator_header_lock + "\n"
                           "#define " + generator_header_lock + "\n"
                           "\n"
                           "#include \"" + class_file_to_include + "\"\n"
                           "#include \"" + lunagen_helper_filename + "\"\n"
                           "\n"
                           "#include <luabind/luabind.hpp>\n"
                           "\n"
                           "template<typename T>\n"
                           "inline auto GenerateBinding" + class_obj.name.capitalize() + "()\n"
                           "{\n"
                           "    typedef typename T::internal_class internal_class;\n"
                           "    return LunaGen::LunaGenHelper<T>::defClass()\n")

    for nextField in class_obj.fields:
        if nextField.hidden:
            continue
        for nextName in [nextField.name] + nextField.alt_names:
            if nextField.special_type == SpecialLunaFieldType.Momentum:  # TODO: Prevent regestration with custom name
                def get_momentum_line(varName):
                    return ("        .property(\"" + varName + "\", "
                                                                    "&T::MomentumGetter<&internal_class::" + nextField.name + ", &Momentum::" + varName + ">, "
                                                                    "&T::MomentumSetter<&internal_class::" + nextField.name + ", &Momentum::" + varName + ">)\n")

                generator_writer.write(
                    get_momentum_line("x") +
                    get_momentum_line("y") +
                    get_momentum_line("width") +
                    get_momentum_line("height") +
                    get_momentum_line("speedX") +
                    get_momentum_line("speedY")
                )
            else:
                generator_writer.write("        .property(\"" + nextName + "\", "
                                                                           "&T::Getter<decltype(internal_class::" + nextField.name + "), &internal_class::" + nextField.name + ">, "
                                                                           "&T::Setter<decltype(internal_class::" + nextField.name + "), &internal_class::" + nextField.name + ">)\n")

    generator_writer.write(";"
                           "}\n"
                           "\n")

    generator_writer.write("#endif")
    generator_writer.close()
