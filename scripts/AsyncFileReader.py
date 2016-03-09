import threading, queue, time, BuildUtils

class AsyncFileReader(threading.Thread):
    def __init__(self, fd, tailMode = False):
        assert callable(fd.readline)

        threading.Thread.__init__(self)
        self.daemon = True
        self._fd = fd
        self.queue = queue.Queue()
        self._stopEvent = threading.Event()
        self._tailMode = tailMode
        self.readLines = ""
        self.start()

    @classmethod
    def fromFileName(cls, fileName):
        open(fileName, 'a').close()
        return cls(open(fileName), True)

    # TODO: refactor
    def run(self):
        while not self._stopEvent.isSet():
            if not self._tailMode:
                try:
                    for line in iter(self._fd.readline, b''):
                        normalizedLine = BuildUtils.NormalizeNewlines(line.decode('ascii', 'ignore'))
                        self.readLines += normalizedLine
                        self.queue.put(normalizedLine)
                except ValueError:
                    time.sleep(0.1)
            else:
                where = self._fd.tell()
                line = self._fd.readline()
                if not line:
                    time.sleep(0.1)
                    self._fd.seek(where)
                else:
                    normalizedLine = BuildUtils.NormalizeNewlines(line.encode('utf-8', 'ignore').decode('ascii', 'ignore'))
                    self.readLines += normalizedLine
                    self.queue.put(normalizedLine)

    def stop(self):
        self._stopEvent.set()
        self.join()
        self._fd.close()