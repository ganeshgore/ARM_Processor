import numpy as np
import os
import subprocess
import os

update = False   
cmd = [ 'FASMARM.EXE' , 'program.asm', 'program.bin']
process = p = subprocess.Popen(cmd,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT)

for line in iter(p.stdout.readline, b''):
    print line
    if "passes" in line:
        update = True



if (update):
    f = open("program.bin", "rb")  # reopen the file
    f.seek(0, os.SEEK_SET)  # seek
    # x = np.fromfile(f, dtype=np.uint8)  # read the data into numpy
    x = np.fromfile(f, dtype=np.uint32)  # read the data into numpy
    fp = open("program.mif","w+")
    fp.write("@000"+ "\n")
    count=0
    for inst in x:
        count = count+1
        print (hex(inst)[2:-1])
        fp.write(hex(inst)[2:-1] + "\n")
    while count<33:
        fp.write("00000000" + "\n")
        count = count +1
        
    fp.close()
    print "==========================="
    print "Program.mif Created"
    print "==========================="
else:
    print "Error In Compiling Code"