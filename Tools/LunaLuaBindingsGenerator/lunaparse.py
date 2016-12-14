
import clang.cindex
import os
from clang.cindex import Cursor, CursorKind

class SpecialLunaFieldType:
    NoSpecial, Momentum = range(2)

class LunaField:
    def __init__(self, cursor):
        """
        Creates a new LunaField which represents a C-Field in a C-Struct
        @type cursor: Cursor 
        """
        self.name = cursor.spelling
        self.alt_names = []
        self.hidden = False
        self.special_type = SpecialLunaFieldType.NoSpecial

        type_name = cursor.type.spelling
        if type_name == "Momentum":
            self.special_type = SpecialLunaFieldType.Momentum

        for next_child in cursor.get_children(): # type: Cursor
            if next_child.kind == clang.cindex.CursorKind.ANNOTATE_ATTR:
                attr_spelling = next_child.spelling # type: str
                # If element is empty, then skip it
                if attr_spelling == "":
                    continue
                attr_vals = attr_spelling.split(":") # type: list[str]
                # Annotation with args
                attr_key = attr_vals[0]
                if len(attr_vals) >= 2:
                    if attr_key == "lunagen_alt_name":
                        self.alt_names.append(attr_vals[1])
                else:
                    if attr_key == "lunagen_hidden":
                        self.hidden = True


class LunaClass:
    def __init__(self, cursor):
        """
        Creates a new LunaClass which represents a C-Struct
        @type cursor: Cursor
        """

        def check_base_class(class_cursor):
            """
            Checks manually (by opening the file), if it has the base class.
            @param class_cursor: The cursor of the class
            @type class_cursor: Cursor
            @return: The base-class name or None
            @rtype: str
            """
            source_file = open(class_cursor.extent.start.file.name, "r")
            source_line = class_cursor.extent.start.line  # type: int
            source_file.seek(0, os.SEEK_SET)
            for lines in range(1, source_line):
                source_file.readline()  # Skip till to the line with the class decl
            line_class_decl = source_file.readline()
            while True:
                next_line = source_file.readline()
                line_class_decl = line_class_decl + " " + next_line
                if next_line.find('{') >= 0:
                    break
                if not next_line:
                    raise Exception("Did not find '{'!")

            normalized_line = ' '.join(line_class_decl.split())
            normalized_line = normalized_line[0:len(normalized_line) - 1].strip()
            base_class_decl_index = normalized_line.find(':')
            if base_class_decl_index == -1:  # No base class declartion --> No base class, return empty string
                return None

            # Remove the derived class name
            base_class_line = normalized_line[base_class_decl_index + 1:len(normalized_line)].strip()  # type: str

            # Now check if it is template, if it is, then remove all template arguments
            possible_angle_bracket_start = base_class_line.find('<')
            possible_angle_bracket_end = base_class_line.find('>')
            if possible_angle_bracket_start >= 0 and possible_angle_bracket_end == -1:
                raise Exception("Did found start angle bracket, but not end angle bracket!")
            if possible_angle_bracket_start == -1 and possible_angle_bracket_end >= 0:
                raise Exception("Did not found start angle bracket, but did found end angle bracket!")
            if possible_angle_bracket_start >= 0 and possible_angle_bracket_end >= 0:
                base_class_line = base_class_line[0:possible_angle_bracket_start].strip()

            base_class_split = base_class_line.split()
            if len(base_class_split) == 1:
                base_class_line = base_class_split[0]
            elif len(base_class_split) == 2:  # if it is 2
                base_class_line = base_class_split[1]
            else:
                raise Exception("Base class decl has too many parts.")

            # print("Line with decl: " + normalized_line)
            # print("Base class decl: " + base_class_line)
            # print("Looking for source file: " + class_cursor.extent.start.file.name)
            return base_class_line

        self.name = cursor.spelling  # type: str
        self.fields = [] # type: list[LunaField]
        self.file_path = str(cursor.location.file)  # type: str
        self.base_class = check_base_class(cursor)  # type: str

        for sub_child in cursor.get_children():
            if sub_child.kind == clang.cindex.CursorKind.FIELD_DECL:
                self.fields.append(LunaField(sub_child))


def find_and_parse_class(cursor, class_type, class_name):
    """
    Goes through all cursors and tries to fetch the class
    @param cursor: The top-most cursor
    @param class_type: The type of the class to parse, must be CursorKind.STRUCT_DECL or CursorKind.CLASS_DECL
    @param class_name: The name of the class to find
    @type cursor: Cursor
    @type class_type: CursorKind
    @type class_name: str
    @return: The parsed class
    @rtype: LunaClass
    """

    def get_info(node, max_depth, depth=0):
        if max_depth is not None and depth >= max_depth:
            children = None
        else:
            children = [get_info(c, max_depth, depth + 1)
                        for c in node.get_children()]
        return {'kind': node.kind,
                'start': node.extent.start,
                'end': node.extent.end,
                'spelling': node.spelling,
                'type': node.type.kind,
                'is_definition': node.is_definition(),
                'children': children}

    def dump_cursor(cursor_to_dump):
        print(str(get_info(cursor_to_dump, 10)))

    def has_fields(class_cursor):
        """

        @param class_cursor: The class cursor
        @type class_cursor: Cursor
        @return: True, if the class cursor has fields
        @rtype: bool
        """
        for nextPossibleField in class_cursor.get_children(): # type: Cursor
            if nextPossibleField.kind == CursorKind.FIELD_DECL:
                return True
        return False

    if cursor.spelling == class_name and cursor.kind == class_type:
        if has_fields(cursor):  # Ensure that the this cursor is the definition
            dump_cursor(cursor)
            return LunaClass(cursor)

    for nextChild in cursor.get_children():  # type: Cursor
        call_result = find_and_parse_class(nextChild, class_type, class_name)
        if call_result is not None:
            return call_result

