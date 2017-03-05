#!/usr/bin/env python2
import sys
import os
import stat

filename, src, dest = sys.argv[1:4]
filename_stage = filename + '.esy_rewrite'
filestat = os.stat(filename)
# TODO: we probably should handle symlinks too in a special way,
# to modify their location to a rewritten path
with open(filename, 'r') as input_file, open(filename_stage, 'w') as output_file:
    for line in input_file:
        output_file.write(line.replace(src, dest))
os.rename(filename_stage, filename)
os.chmod(filename, stat.S_IMODE(filestat.st_mode))