from mdutils.mdutils import MdUtils
from mdutils.tools.TextUtils import TextUtils


mdFile = MdUtils(file_name='reference_funcs.md',title='SMBX 1.3 Function Reference')

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

rows = []
with open("reference_funcs.tsv", "r") as f:
    isHdr = True
    for line in f:
        line = line.strip()
        if len(line) == 0:
            continue
        cols = line.split("\t")
        if cols[0].startswith("0"):
            cols[0] = "0x" + cols[0]
        cols = cols + (5-len(cols))*[""]
        if isHdr:
            rows.extend(cols)
            isHdr = False
            continue
        if cols[1] == "":
            continue
        if cols[0] == "UNUSED":
            cols[0] = "_UNUSED_"
        
        fn = cols[1]
        line = get_line(fn, cols[2], cols[3])
        url = f'https://github.com/smbx/smbx-legacy-source/blob/4a7ff946da8924d2268f6ee8d824034f3a7d7658/{fn}#L{line}'
        filetxt = f'{fn}:{line}'
        cols[1] = TextUtils.text_external_link(text=filetxt, link=url)
        rows.extend(cols)

print(len(rows))

mdFile.new_table(columns=5, rows=len(rows)//5, text=rows)

mdFile.create_md_file()
