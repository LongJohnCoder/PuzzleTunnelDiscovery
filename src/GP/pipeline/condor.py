#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import re
import subprocess
import os
import shutil
from . import util

TEMPLATE_EXCLUDE = [
        re.compile('^Executable\s*=', flags=re.IGNORECASE),
        re.compile('^Error\s*=', flags=re.IGNORECASE),
        re.compile('^Output\s*=', flags=re.IGNORECASE),
        re.compile('^Log\s*=', flags=re.IGNORECASE),
        re.compile('^arguments\s*=', flags=re.IGNORECASE),
        re.compile('^#'),
        re.compile('^$'),
        re.compile('^Queue', flags=re.IGNORECASE),
    ]

def extract_template(fin, fout):
    for line in fin:
        do_write = True
        for p in TEMPLATE_EXCLUDE:
            if p.match(line):
                do_write = False
                break
        if do_write:
            print(line, end='', file=fout)

def local_wait(iodir):
    log_fn = os.path.join(iodir, 'log')
    util.log('[condor] waiting on condor log file {}'.format(log_fn))
    util.shell(['condor_wait' , log_fn])

'''
Side effect:
    iodir will be created if not exist
'''
def local_submit(ws,
                 xfile,
                 iodir_rel,
                 arguments,
                 instances,
                 wait=True):
    if xfile is None or xfile == '':
        msg = "[condor.local_submit] xfile is None or empty, current value {}".format(xfile)
        util.fatal(msg)
        raise RuntimeError(msg)
    SUBMISSION_FILE = 'submission.condor'
    local_scratch = ws.local_ws(iodir_rel)
    os.makedirs(local_scratch, exist_ok=True)
    util.log("[local_submit] using scratch directory {}".format(local_scratch))
    local_sub = os.path.join(local_scratch, SUBMISSION_FILE)
    shutil.copy(ws.condor_template, local_sub)
    with open(local_sub, 'a') as f:
        print('Executable = {}'.format(xfile), file=f)
        print('Output = {}/$(Process).out'.format(local_scratch), file=f)
        print('Error = {}/$(Process).err'.format(local_scratch), file=f)
        print('Log = {}/log'.format(local_scratch), file=f)
        print('arguments =', file=f, end='')
        for a in arguments:
            assert ' ' not in str(a), 'We cannot deal with paths/arguments with spaces'
            print(' {}'.format(a), file=f, end='')
        print('\nQueue {}'.format(instances), file=f)
    util.log("[local_submit] submitting {}".format(local_sub))
    util.shell(['condor_submit', local_sub])
    if wait:
        local_wait(local_scratch)