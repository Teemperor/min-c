import os
import filecmp
from os import listdir
from os.path import isfile, join
import subprocess
import sys

args = sys.argv[1:]

def compare_dir(to_compare):
    a_files = set()
    b_files = set()

    for root, dirs, files in os.walk(to_compare):
        for file in files:
            a_files.add(file) 

    for root, dirs, files in os.walk(to_compare + 'Expected'):
        for file in files:
            b_files.add(file) 

    #print(a_files)
    #print(b_files)

    common = a_files.intersection(b_files)
    missing = b_files - a_files
    if len(missing) != 0:
        print("FAIL! Missing files: " + str(missing))
        return

    #print(common)

    different_files = []
    equal_files = []

    for f in common:
        a_f = os.path.join(to_compare, f)
        b_f = os.path.join(to_compare + "Expected", f)
        equal = filecmp.cmp(a_f, b_f)
        if not equal:
            equal_files.append(f)
        else:
            different_files.append(f)

    if len(different_files) != 0:
        print("FAIL! Different: " + str(different_files))
        return
    print("OK!")
    #print("Equal: " + str(equal_files))

onlyfiles = [f for f in listdir(".") if isfile(f)]

for f in onlyfiles:
    if f.endswith(".sh"):
        b = os.path.splitext(os.path.basename(f))[0]

        test_args = args
        test_args.insert(1, os.path.realpath(f))
        
        print(("Running " + b + "... ").ljust(40), end='', flush=True)
        os.chdir(b)
        output = subprocess.Popen(test_args, stdout=subprocess.PIPE).communicate()[0]
        os.chdir("..")
        output_file = open(b + ".log", "w")
        output_file.write(str(output))
        output_file.close()
        compare_dir(b)
