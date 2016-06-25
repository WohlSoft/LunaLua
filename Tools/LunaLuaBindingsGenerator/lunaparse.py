
import clang.cindex
from clang.cindex import Cursor
import os


class LunaField:
    def __init__(self, cursor):
        """
        Creates a new LunaField which represents a C-Field in a C-Struct
        @type cursor: Cursor 
        """
        self.name = cursor.spelling
        self.alt_names = []
        self.hidden = False
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
        self.name = cursor.spelling # type: str
        self.fields = [] # type: list[LunaField]
        self.file_path = str(cursor.location.file) # type: str

        for sub_child in cursor.get_children():
            if sub_child.kind == clang.cindex.CursorKind.FIELD_DECL:
                self.fields.append(LunaField(sub_child))