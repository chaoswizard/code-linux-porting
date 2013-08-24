#!/usr/bin/env python

"create rootfs"

import sys
import os
import getopt
import time

#line swith char
linesep = os.linesep


#rootfs class, base is object
class CRootFs(object):
    """
    rootfs base class
    """
    def __init__(self, name, fstype):
        global linesep
        #time stamp
        self.stamp = time.strftime("%Y%m%d%H%M%S")
        self.name = fstype
        self.path = name + self.stamp + '.' + self.name
        mydebug('Init rootfs')
    def info(self):
        print 'path is: %s%s' % (self.path, linesep) 

#yaffs class
class CYaffsFs(CRootFs):
    """
    yaffs 
    """
    def __init__(self, name):
        super(CYaffsFs, self).__init__(name, 'yaffs')
        mydebug('Init yaffs')

#ramfs class
class CRamFs(CRootFs):
    """
    ramfs 
    """
    def __init__(self, name):
        super(CRamFs, self).__init__(name, 'ramfs')
        mydebug('Init ramfs')
        
        
#cramfs class
class CCramFs(CRootFs):
    """
    cramfs
    """
    def __init__(self, name):
        super(CCramFs, self).__init__(name, 'cramfs')
        mydebug('Init cramfs')


#global variables define


support_fs_tbl = {
            "yaffs":CYaffsFs,
            "ramfs":CRamFs,
            "cramfs":CCramFs,
        }



#option table
#if option has param,must follow char':' or '=' when long opt
opt_short_tbl = 'hf:v'
opt_long_tbl = ["help", "fstype="]

#usage string for tips
usage_str = '[options] -f fsname' + linesep +\
            '\t-f, --fstype=name\tfilesystem types name' + linesep +\
            '\t     support list:' + str(support_fs_tbl.keys()) +linesep +\
            '\t-v\t\t\tverbose mode' + linesep +\
            '\t-h, --help\t\tprint this message'





#is verbose mode
debug = False

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


#virtual rootfs class
class RootFs(object):
    """
    rootfs
    """
    def __init__(self, key, name):
        global support_fs_tbl
        self.key = key
        self.cls_tab = support_fs_tbl
        self.cls_name = self.cls_tab[key];
        self.instance = self.cls_name(name)
            
    def dump(self, dump_name):
        print dump_name
        super(self.cls_name, self.instance).info()

    
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
        global debug
        fstype = "unsupport"
        for o, a in opts:
            if o == "-v":
                debug = True
            elif o in ("-h", "--help"):
                usage()
                sys.exit()
            elif o in ("-f", "--fstype"):
                fstype = a
                mydebug('input fstype=', a)
            else:
                pass
            
        if fstype not in support_fs_tbl.keys():
            print 'unsupport fs type:%s.' % (fstype)
            usage()
            return 0
        else:
            myrootfs = RootFs(fstype, "img")
            myrootfs.dump("elvon dump:")
            
        
if __name__ == '__main__':
    main()
