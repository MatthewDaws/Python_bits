m3u Playlists
=============

A simple script to produce a list of `.m3u` files from subdirectories which contain OGG Vorbis files.

   - Uses `argparse` to parse a command line options
   - Uses `subprocess` to run the command-line programme `ogginfo` which does the actual reading of metadata from the `.ogg` files.
   
The script `make_playlist_new.py` uses instead the [Mutagen](https://bitbucket.org/lazka/mutagen/overview) project (a pure Python audio metadata reader/writer) to read the metadata.  Requires installing the Mutagen package though (but this is easy as it's pure Python).
