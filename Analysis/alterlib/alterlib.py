#!/usr/bin/env python

import os, sys, io, collections, struct, unix_ar
LuaObject = collections.namedtuple('LuaObject', ['info', 'contents'])

if input("Have you made sure you verified that FileUtils::CFileToLua is compatible with the new lua51.lib file? (y/n) ") != 'y':
    exit()

# Open lua lib in read mode
lua_lib = unix_ar.open(sys.argv[1], mode = "r")

# Get contents of io library
lua_objects = []
for entry in lua_lib.infolist():
    current_index = len(lua_objects)
    lua_objects.append(LuaObject(entry, lua_lib.extract(entry, path=io.BytesIO()).read()))
    if entry.name.strip(b"/").decode('iso-8859-1') == "lib_io.obj":
        io_lib_index = current_index

# Close lua lib
lua_lib.close()

# Get io object file
io_lib = bytearray(lua_objects[io_lib_index].contents)

# Get address of string table
symbolTableAddr, = struct.unpack_from("<I", io_lib, 8)
numberOfSymbols, = struct.unpack_from("<I", io_lib, 12)
stringTableAddr = symbolTableAddr + numberOfSymbols * 18

# Find function name offset relative to the string table
functionStringAddr = io_lib.find(b"_io_std_new", stringTableAddr)
assert functionStringAddr >= 0, 'String "_io_std_new" was not found'
functionStringOffset = functionStringAddr - stringTableAddr
assert functionStringOffset >= 0, 'String "_io_std_new" is outside of the string table'

# Search address of symbol table entry
for symbolIndex in range(numberOfSymbols):
    symbolAddr = symbolTableAddr + symbolIndex * 18
    mustBeZero, = struct.unpack_from("<I", io_lib, symbolAddr)
    symbolNameOffset, = struct.unpack_from("<I", io_lib, symbolAddr + 4)
    if mustBeZero == 0 and symbolNameOffset == functionStringOffset:
        functionSymbolAddr = symbolAddr
        break

else:
    assert False, "Symbol _io_std_new was not found"

# Make symbol global
io_lib[functionSymbolAddr + 16] = 0x2

# Update contents
lua_objects[io_lib_index] = LuaObject(lua_objects[io_lib_index].info, bytes(io_lib))

# Backup old lua lib
os.rename(sys.argv[1], sys.argv[1] + ".old")

# Open lua lib in write mode
lua_lib = unix_ar.open(sys.argv[1], mode = "w")

# Add all files to archive
for entry in lua_objects:
    lua_lib.addfile(entry.info, io.BytesIO(entry.contents))

# Close new lua lib
lua_lib.close()

print("Done!")



