# User defined data
ogginfo_path = "ogginfo.exe"
directories_to_ignore = { "iTunes" }

# Rest

import os
import subprocess
import itertools
import re
import math
from collections import namedtuple
OggInfo = namedtuple("OggInfo", ["title", "artist", "album", "tracknumber", "length"])

def run_ogg_info(cmdpath, filename):
    """Run ogginfo.exe from `cmdpath` on `filename`.
    Returns OggInfo object."""
    proc = subprocess.Popen([cmdpath, filename], stdout=subprocess.PIPE, universal_newlines=True)
    data = list(proc.stdout)
    metadata = { "title": None, "artist": None, "album": None, "tracknumber": None, "length": None }
    # Find User comments
    userdata = itertools.dropwhile(lambda s : not s.startswith("User comments section follows"), data)
    for line in userdata:
        for st in ["title", "artist", "album", "tracknumber"]:
            m = re.search("{}\s*=\s*(.*)".format(st), line)
            if m != None:
                metadata[st] = m.group(1)
    # Find stream information
    streamdata = itertools.dropwhile(lambda s : not s.startswith("Vorbis stream"), data)
    for line in streamdata:
        m = re.search("Playback length:\s*(.*)", line)
        if m != None:
            # Expect format "{int}m:{float}s"
            t = re.search("(\d+)m:([\d\.]+)s", m.group(1))
            if t == None:
                raise Exception("Unknown time code: '{}'".format(m.group(1)))
            else:
                metadata["length"] = int(t.group(1)) * 60 + math.ceil(float(t.group(2)))
    # Return
    return OggInfo(metadata["title"], metadata["artist"], metadata["album"],
                   metadata["tracknumber"], metadata["length"])
                   
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
        data = run_ogg_info(ogginfo_path, os.path.join(directory, file))
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
parser.add_argument("--ogginfo",
    help = "Path to ogginfo.exe, searches by default in current directory.")
args = parser.parse_args()

if args.ignore:
    for d in args.ignore:
        directories_to_ignore.add(d)

if args.ogginfo:
    ogginfo_path = os.path.join(args.ogginfo, ogginfo_path)
    print("Path:", ogginfo_path)
    
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
