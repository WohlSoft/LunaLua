
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
        #: :type nextChild: clang.cindex.Cursor
        for nextChild in cursor.get_children():
            if nextChild.kind == clang.cindex.CursorKind.ANNOTATE_ATTR:
                #: :type attrSpelling: str
                attrSpelling = nextChild.spelling
                #: :type attrVals: list
                attrVals = attrSpelling.split(":")
                # Annotation with args
                if len(attrVals) >= 2:
                    attrKey = attrVals[0]
                    if attrKey == "lunagen_alt_name":
                        self.alt_names.append(attrVals[1])
                
        
        
        
class LunaClass:
    def __init__(self, cursor):
        """
        Creates a new LunaClass which represents a C-Struct
        @type cursor: Cursor
        """
        self.name = cursor.spelling # type: str
        self.fields = []
        self.file_path = str(cursor.location.file)

        for subChild in cursor.get_children():
            if subChild.kind == clang.cindex.CursorKind.FIELD_DECL:
                self.fields.append(LunaField(subChild))