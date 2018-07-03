#!/usr/bin/env python

"""Parse string from the device to numbers and vice versa.

"""


def str2num(in):
    return [int(s) for s in str.split() if s.isdigit()]
