from mdutils.mdutils import MdUtils
from mdutils.tools.TextUtils import TextUtils
from dataclasses import dataclass
import os.path

def get_line(fn, fn_type, fn_name):
    with open(f'../smbx-legacy-source/{fn}', 'r') as fn_srch:
        ln = 1
        lines = []
        lines_text = []
        for line in fn_srch:
            if (fn_type in line) and (" "+fn_name+"(" in line):
                lines.append(ln)
                lines_text.append(line)
            ln += 1
        if len(lines) == 1:
            return lines[0]
        else:
            print(fn, fn_name)
            print("\n".join(f'\t{x}' for x in lines_text))

@dataclass
class FuncRange:
    start : int
    label : str

funcs = []
with open("reference_funcs.tsv", "r") as f:
    isHdr = True
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
        fn = cols[1]
        line = get_line(fn, cols[2], cols[3])
        url = f'https://github.com/smbx/smbx-legacy-source/blob/4a7ff946da8924d2268f6ee8d824034f3a7d7658/{fn}#L{line}'
        filetxt = f'{fn}:{line}'
        label = TextUtils.text_external_link(text=fn.split('.',1)[0]+'.'+cols[3], link=url)
        funcs.append(FuncRange(start = addr, label = label))

cells = ["#", "Address", "Size", "VB6 Func", "LunaLua Source"]
i = 1
with open("patches.tsv", "r") as f:
    isHdr = True
    for line in f:
        line = line.strip()
        if len(line) == 0:
            continue
        cols = line.split("\t")
        
        addr, size, src = cols
        fn, line = src.split(":")
        fn = fn.replace("\\", "/")
        url = f'https://github.com/WohlSoft/LunaLua/blob/e79f8c830e44aba9444bb799879af9575ee9968d/LunaDll/{fn}#L{line}'
        fn = os.path.basename(fn)
        link = TextUtils.text_external_link(text=fn+":"+line, link=url)

        addr = int(addr[2:], 16)
        vbLabel = ""
        for func in funcs:
            if func.start <= addr:
                vbLabel = func.label
            else:
                break
        
        cells.extend([f'{i}', f'0x{addr:08X}', size, vbLabel, link])
        i += 1

print(len(cells))


mdFile = MdUtils(file_name='reference_lunalua_patches.md',title='LunaLua Patch Reference')

mdFile.new_table(columns=5, rows=len(cells)//5, text=cells,  text_align='left')

mdFile.create_md_file()
