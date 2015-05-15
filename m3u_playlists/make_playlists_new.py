# User defined data
directories_to_ignore = { "iTunes" }

# Rest

import os
import subprocess
import itertools
import re
import math
from collections import namedtuple
OggInfo = namedtuple("OggInfo", ["title", "artist", "album", "tracknumber", "length"])

from mutagen.oggvorbis import OggVorbis

def run_ogg_info(filename):
    """Uses `Mutagen` library instead"""
    # This will capture, as a dictionary, the title, album etc.
    # However, each member will be a list, so grab 1st entry.
    data = OggVorbis(filename)
    # Can extract the length from data.info member
    length = math.ceil(data.info.length)
    # Return
    return OggInfo(data["title"][0], data["artist"][0], data["album"][0],
                   data["tracknumber"][0], length)
                   
def get_sorted_filenames(directory):
    walker = os.walk(directory)
    _, dirs, files = next(walker)
    if len(dirs) > 0:
        raise Exception("{} contains subdirectories, which we don't expect!".format(directory))
    files.sort()
    return files
    
def make_m3u(directory, output_filename):
    # This is a slight hack...  We assemble the output in a single string
    # and then write to a file in one go.  This means the chance of a Ctrl-C
    # interrupt leaving a half-formed file is much smaller.
    m3u = ""
    m3u += "#EXTM3U\n"
    for file in get_sorted_filenames(directory):
        data = run_ogg_info(os.path.join(directory, file))
        if data.length == None or data.artist == None or data.title == None:
            raise Exception("{} doesn't contain enough data: {}".format(file, data))
        m3u += "#EXTINF:{},{} - {}\n".format(data.length, data.artist, data.title)
        fullfilename = os.path.join(directory, file)
        base, filename = os.path.split(fullfilename)
        _, firstdir = os.path.split(base)
        m3u += os.path.join(firstdir, filename) + "\n"
    # Now write to file
    with open(output_filename, "w") as f:
        print(m3u, file=f, end="")


# Main script

# Parse arguments
import argparse

parser = argparse.ArgumentParser(description="Generate m3u files.")
parser.add_argument("task", default="new", choices=["new", "all"],
    help = "new: Generate only new m3u files; all: Generate all files.")
parser.add_argument("--ignore", nargs="*",
    help = "List of directories to ignore.")
args = parser.parse_args()

if args.ignore:
    for d in args.ignore:
        directories_to_ignore.add(d)

# Walk current directory

walker = os.walk(".")
root, dirs, files = next(walker)
for directory in dirs:
    if directory in directories_to_ignore:
        continue
    filename = directory+".m3u"
    if args.task == "new" and filename in files:
        continue
    print(directory)
    make_m3u(directory, filename)
