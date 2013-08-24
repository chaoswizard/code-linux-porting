#!/usr/bin/env python

"create rootfs"

import sys
import os
import getopt

support_fs_tbl = ["yaffs", "cramfs", "ramfs"] 

#line swith char
linesep = os.linesep

#option table
#if option has param,must follow char':' or '=' when long opt
opt_short_tbl = 'hf:v'
opt_long_tbl = ["help", "fstype="]

#usage string for tips
usage_str = '[options] -f fsname' + linesep +\
            '\t-f, --fstype=name\tfilesystem types name' + linesep +\
            '\t     support list:' + str(support_fs_tbl) +linesep +\
            '\t-v\t\t\tverbose mode' + linesep +\
            '\t-h, --help\t\tprint this message'





#is verbose mode
debug = False

#parse type
fstype = "unsupport"

#my debug fucntion
def mydebug(*arglist, **argdict):
    global debug
    if not debug:
        return 0
    for i in arglist:
        print i,
    print
    for i in argdict:
        print i, argdict[i],

def yaffs_fs_create():
    mydebug('create yaffs')


def ramfs_fs_create():
    mydebug('create ramfs')

def cramfs_fs_create():
    mydebug('create cramfs')

    

def usage():
    global usage_str
    print 'usage:%s %s' % (sys.argv[0], usage_str)

  
def main():
    "main function for rootfs create dispatch"
    #print sys.argv
    #get argv count
    if len(sys.argv) < 2:
        print 'no options input.'
        usage()
        return 2
        
    try:
        #parse command line options
        opts, args = getopt.getopt(sys.argv[1:],
                                   opt_short_tbl,
                                   opt_long_tbl)
    except getopt.GetoptError:
        print 'get options error.'
        usage()
        return 2
    else:
        global fstype, debug
        for o, a in opts:
            if o == "-v":
                debug = True
            if o in ("-h", "--help"):
                usage()
                sys.exit()
            if o in ("-f", "--fstype"):
                fstype = a
                mydebug('input fstype=', a)
                break
            
        if fstype == support_fs_tbl[0]:
            yaffs_fs_create()
        elif fstype == support_fs_tbl[1]:
            cramfs_fs_create()
        elif fstype == support_fs_tbl[2]:
            ramfs_fs_create()
        else:
            print 'unsupport fs type:%s.' % (fstype)
            usage()
            return 0
        
if __name__ == '__main__':
    main()
