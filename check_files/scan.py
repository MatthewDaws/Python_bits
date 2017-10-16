import hashlib
import os
import json
import logging
import sys

def hash_file(filename):
    with open(filename, "rb") as file:
        hasher = hashlib.sha256()
        while True:
            data = file.read(65536)
            hasher.update(data)
            if data is None or len(data) == 0:
                return hasher.hexdigest()


class Scan():
    def __init__(self, basedir, logger):
        self._logger = logger
        self._base = basedir
        self._scan()
        
    def _scan(self):
        self._logger.debug("Scanning '%s' for all files and sub-directories", self._base)
        self._all_files = []
        for dirname, subdirs, files in os.walk(self._base):
            for file in files:
                if file.startswith("__scan__"):
                    continue
                fullfile = os.path.join(dirname, file)
                self._all_files.append(fullfile)
        self._logger.debug("Found %s files", len(self._all_files))

    @property
    def all_files(self):
        return list(self._all_files)
    
    def generate_hashes(self):
        for filename in self.all_files:
            self._logger.debug("Hashing '%s'", filename)
            yield filename, hash_file(filename)
            
    def all_hashes(self):
        filenames, hashes = [], []
        for filename, hexhash in self.generate_hashes():
            filenames.append(filename)
            hashes.append(hexhash)
        return filenames, hashes

    def diff(self, old_files):
        old_files = set(old_files)
        new_files = set(self._all_files)
        self._logger.info("Differences in files:")
        for x in old_files:
            if x not in new_files:
                self._logger.info("DELETED: '%s'", x)
        for x in new_files:
            if x not in old_files:
                self._logger.info("NEW FILE: '%s'", x)
        self._logger.info(" ------ End of differences")
    

class StatePersist():
    def __init__(self):
        self._files = None
        self._hashes = None
        
    def dump(self):
        data = {"files" : self._files,
                "hashes" : self._hashes,
                }
        return json.dumps(data)
    
    @staticmethod
    def from_json_string(data):
        data = json.loads(data)
        assert set(data) == {"files", "hashes"}
        state = StatePersist()
        state.files = data["files"]
        state.hashes = data["hashes"]
        assert len(state.files) == len(state.hashes)
        return state
    
    def __eq__(self, other):
        return set(self.files) == set(other.files)
    
    @property
    def files(self):
        return self._files
    
    @files.setter
    def files(self, v):
        self._files = list(v)
    
    @property
    def hashes(self):
        return self._hashes
    
    @hashes.setter
    def hashes(self, v):
        self._hashes = list(v)


def make_logger(filename = "scan.log"):
    logger = logging.getLogger("scan")
    logger.setLevel(logging.DEBUG)
    fmt = logging.Formatter("{asctime} {levelname} {name} - {message}", style="{")
    
    ch = logging.StreamHandler()
    ch.setFormatter(fmt)
    logger.addHandler(ch)
    
    ch = logging.FileHandler(filename)
    ch.setFormatter(fmt)
    logger.addHandler(ch)
    return logger


class Setting():
    def __init__(self, basedir, logger):
        self._logger = logger
        self._base = basedir
        if os.path.exists(self._filename(0)):
            self._state = self._reload()
        else:
            self._state = None
        
    def save(self):
        for i in range(3):
            with open(self._filename(i), "wt", encoding="utf8") as file:
                file.write( self._state.dump() )
        
    @property
    def state(self):
        return self._state
    
    @state.setter
    def state(self, v):
        self._state = v
        
    def _filename(self, i):
        filename = "__scan__{}.json".format(i)
        return os.path.join(self._base, "__scan__" + filename)

    def _reload(self):
        states = []
        for i in range(3):
            try:
                with open(self._filename(i), "rt", encoding="utf8") as file:
                    states.append( StatePersist.from_json_string(file.read()) )
            except:
                pass
        if len(states) < 2:
            self._logger.error("Could not load 2 good state files")
            raise Exception("Not enough good state files.")
        for i, state in enumerate(states):
            for other in states[i+1:]:
                if state == other:
                    return state
        self._logger.error("No two states agree.")
        raise Exception("Not enough good state files.")

def run(basedir):
    logger = make_logger()
    logger.info("Working with directory '%s'", basedir)
    logger.info("Reloading old settings")
    settings = Setting(basedir, logger)
    scanner = Scan(basedir, logger)
    if settings.state is None:
        logger.info("No settings file found, so performing initial scan")
        filenames, hashes = scanner.all_hashes()
        logger.info("Saving results...")
    else:
        scanner.diff(settings.state.files)
        filenames, hashes = scanner.all_hashes()
        new_hashes = {f : h for f, h in zip(filenames, hashes)}
        logger.info("Comparing hashes...")
        for filename, old_hash in zip(settings.state.files, settings.state.hashes):
            if filename in new_hashes and new_hashes[filename] != old_hash:
                logger.error("Hash for '%s' has changed", filename)
                logger.info("Was %s", old_hash)
                logger.info("Now %s", new_hashes[filename])
    state = StatePersist()
    state.files = filenames
    state.hashes = hashes
    settings.state = state
    settings.save()
    logger.info("All done")
    
if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: `python {} basedir`".format(sys.argv[0]))
        sys.exit(0)
    run(sys.argv[1])
    