#!/usr/bin/env python
"""Holds static x-axes for different ranges, as well as all the accumulated temperature data ever."""

FNAME_LOG = "temperature_measurements.log"


class Logger(object):
    """Records measured data on the disc for later retrieval.

    Format is one line per second, appended at a large file.
    The file is periodically compressed and backed up, with
    previous archives being deleted.
    """
    def __init__(self, workdir, backupdir=None, backup_interval=None):
        self.logfile = open('/'.join((workdir, FNAME_LOG)), 'a+')

    def add_line(self, string):
        self.logfile.write(string)

    def read(self, interval_seconds, step_seconds):
        # TODO
        # NOTE: do not read all in once, parse the data on the way
        # NOTE: guard against absurdly large requests not ot cause memory overflow
        # NOTE: guard against end of file
        measurements = []
        for i in range(0, interval_seconds, step_seconds):
            measurements.append(0)  # TODO
        return measurements


def main():
    from os.path import expanduser  # find $HOME on any OS
    l = Logger(workdir=expanduser('~'))
    for i in range(10):
        l.add_line("Hqlqlq\n")

if __name__ == "__main__":
    main()
