#!/usr/bin/env python
#coding: utf-8

import os.path
import shutil, sys


def main(argv):
    
    if len(argv) != 2:
        print("Invalid arguments.")
        sys.exit(1)

    if not os.path.isfile(argv[1]):
        raise Exception('The filename specified is not a valid path of existing file.')

    dy_symbol = "dy.map"
    st_symbol = "st.sym"
    deffile = open(argv[1], "r+")
    file_dy = open(dy_symbol, "w+")
    file_st = open(st_symbol, "w+")
    flag = False

    try:
        file_dy.write("{\n\tglobal:\n")
        for line in deffile:
            line = line.strip()
            if line == "EXPORTS":
                flag = True
                continue

            if flag == True:
                file_st.write(str("%s\n" %line))
                file_dy.write(str("\t\t%s;\n" %line))
        file_dy.write("\tlocal: *;\n};\n")
#    except IOError as ex:
#        print(str(ex))
    finally:
        deffile.close()
        file_st.close()
        file_dy.close()

    sys.exit(0)

if __name__=='__main__':
    main(sys.argv)
