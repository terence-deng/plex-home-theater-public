#!/usr/bin/env python
# encoding: utf-8
"""
bootstrap.py

Created by Jamie Kirkpatrick on 2011-01-15.
Copyright (c) 2011 Plex Inc. All rights reserved.
"""

from subprocess import Popen, PIPE
from time import sleep
import sys
import os


def run_cmd(args):
    '''Run a command and capture the output iteratively'''
    cmd = Popen(args, stdout = PIPE)
    while True:
        out = cmd.stdout.read(1)
        if out == '' and cmd.poll() != None:
            break
        if out != '':
            sys.stdout.write(out)
            sys.stdout.flush()
    return cmd.wait() == 0


def get_brew_path():
    '''Returns the path to the homebrew executable'''
    script_path = os.path.abspath(os.path.dirname(__file__))
    return os.path.join(script_path, 'vendor', 'homebrew.git', 'bin', 'brew')


def brew(args):
    '''Run the homebrew command with the given arguments'''
    args.insert(0, get_brew_path())
    return run_cmd(args)


def brew_install(name, options):
    '''Install a forumula using homebrew'''
    args = ['install', '--32bit', name] + options
    return brew(args)


def brew_link(name):
    '''Create symlinks in the install tree for a given formula'''
    args = ['link', name]
    return brew(args)


def install_formula(formula):
    '''Install and configure a formula'''
    if not brew_install(formula.get('name'), formula.get('options', [])):
        return False
    return brew_link(formula.get('name')) if formula.get('link', False) else True


def update_submodules():
    '''Update the registered git submodules'''
    args = ['git', 'submodule', 'update', '--init']
    return run_cmd(args)


def main():
    if not update_submodules():
        exit(1)
    requirements = [
        {'name': 'curl'},
        {'name': 'pcre'},
        {'name': 'expat'},
        {'name': 'libiconv', 'link': True},
        {'name': 'gettext', 'link': True},
        {'name': 'cmake'},
        {'name': 'pkgconfig'},
        {'name': 'lzo'},
        {'name': 'lzo1'},
        {'name': 'boost'},
        {'name': 'fontconfig'},
        {'name': 'jpeg'},
        {'name': 'libtiff'},
        {'name': 'libpng', 'link': True},
        {'name': 'libogg'},
        {'name': 'libvorbis'},
        {'name': 'mad'},
        {'name': 'fribidi'},
        {'name': 'wavpack'},
        {'name': 'sdl'},
        {'name': 'libmpeg2'},
        {'name': 'glew', 'link': True},
        {'name': 'libcdio'},
        {'name': 'openssl', 'link': True},
        {'name': 'libssh2', 'link': True},
        {'name': 'mysql', 'options': ['--client-only']},
        {'name': 'sqlite'},
        {'name': 'libsamplerate'},
        {'name': 'sdl_mixer'},
        {'name': 'sdl_image'}
    ]
    for formula in requirements:
        if not install_formula(formula):
            exit(1)


if __name__ == '__main__':
    main()
