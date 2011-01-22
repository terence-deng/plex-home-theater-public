#!/usr/bin/env python
# encoding: utf-8
"""
bootstrap.py

Created by Jamie Kirkpatrick on 2011-01-15.
Copyright (c) 2011 Plex Inc. All rights reserved.
"""
import getopt
import subprocess
import sys
import os


DEBUG=False


class BootstrapError(RuntimeError):
    '''Internal exception: stores cmd output for debugging'''

    def __init__(self, msg, output):
        super(BootstrapError, self).__init__(msg)
        self.output = output


class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'


def run_cmd(args, message = None, stderr = False):
    '''Run a command and capture the output iteratively'''
    if isinstance(args, str):
        args = [args]
    if message:
        print bcolors.OKBLUE + ("-> %s" % message) + bcolors.ENDC
    env = get_exe_environ()
    cmd = subprocess.Popen(
        args, stdout = subprocess.PIPE, stderr = subprocess.STDOUT, env = env)
    output = ''
    while True:
        out = cmd.stdout.read(1)
        if out == '' and cmd.poll() != None:
            break
        if out != '':
            if DEBUG:
                sys.stdout.write(out)
            else:
                output += out
    if cmd.wait() != 0:
        raise BootstrapError("Command failed: \"%s\"" % " ".join(args), output)


def get_exe_environ():
    env_path = get_env_path()
    env = os.environ
    local_env = {
        'PATH': "%s/bin:/usr/bin:/usr/sbin:/bin:/sbin" % env_path,
        'CFLAGS': "-I%s/include" % env_path,
        'CXXFLAGS': "-I%s/include" % env_path,
        'ACLOCAL': "aclocal -I \"%s/share/aclocal\"" % env_path,
        'LDFLAGS': "-L%s/lib" % env_path
        }
    env.update(local_env)
    return env


def get_env_path():
    '''Returns the path to the build environ'''
    script_path = os.path.abspath(os.path.dirname(__file__))
    return os.path.join(script_path, 'vendor', 'homebrew.git')


def get_brew_path():
    '''Returns the path to the homebrew executable'''
    return os.path.join(get_env_path(), 'bin', 'brew')


def brew(args, message = None):
    '''Run the homebrew command with the given arguments'''
    args.insert(0, get_brew_path())
    run_cmd(args, message)


def brew_install(name, options):
    '''Install a forumula using homebrew'''
    args = ['install', '--32bit', name] + options
    brew(args, "Installing %s" % name)


def brew_link(name):
    '''Create symlinks in the install tree for a given formula'''
    brew(['link', name])


def install_formula(formula):
    '''Install and configure a formula'''
    brew_install(formula.get('name'), formula.get('options', []))
    if formula.get('link', False):
        brew_link(formula.get('name'))


def update_submodules():
    '''Update the registered git submodules'''
    args = ['git', 'submodule', 'update', '--init']
    run_cmd(args, "Updating submodules")


def configure_internal_libs():
    '''Configure the internal vendor libraries'''
    clean = ['find', '.', '-name', '"config.cache"', '-exec', 'rm', '{}', ';']
    run_cmd(clean, "Cleaning caches")
    run_cmd('./bootstrap', "Regenerating configure scripts")
    configure = ['./configure']
    run_cmd(configure, "Configuring internal libs")
    run_cmd(['make', 'clean'])


def build_internal_libs():
    '''Build the internal libraries'''
    run_cmd(['make', 'xcode_depends'], "Building internal libs")


def bootstrap_dependencies():
    requirements = [
        {'name': 'curl', 'link': True},
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
        {'name': 'mad'},
        {'name': 'fribidi'},
        {'name': 'wavpack'},
        {'name': 'sdl'},
        {'name': 'libmpeg2'},
        {'name': 'glew', 'link': True},
        {'name': 'libcdio'},
        {'name': 'openssl', 'link': True},
        {'name': 'libssh2', 'link': True},
        {'name': 'sqlite'},
        {'name': 'libsamplerate'},
        {'name': 'sdl_mixer'},
        {'name': 'sdl_image'},
        {'name': 'ffmpeg'},
        {'name': 'rtmpdump'},
        {'name': 'mysql', 'options': ['--client-only']}
        ]
    for formula in requirements:
        install_formula(formula)


def usage():
    '''Print script usage information'''
    print 'usage: bootstrap.py [--debug]'
    print 'boostrap a PLEX build environment'


def process_args():
    '''Process command line arguments'''
    global DEBUG
    try:
        opts, args = getopt.getopt(sys.argv[1:], "", ["debug"])
    except getopt.GetoptError, e:
        print str(e)
        usage()
        sys.exit(2)
    for o, a in opts:
        if o == '--debug':
            DEBUG=True


def main():
    try:
        process_args()
        update_submodules()
        bootstrap_dependencies()
        configure_internal_libs()
        build_internal_libs()
    except BootstrapError, e:
        print bcolors.FAIL + ("...%s" % e) + bcolors.ENDC
        print "Output: %s" % e.output
        exit(1)
    except KeyboardInterrupt:
        print bcolors.FAIL + ("...interupted") + bcolors.ENDC
        exit(1)


if __name__ == '__main__':
    main()
