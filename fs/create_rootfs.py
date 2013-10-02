#!/usr/bin/env python

"create rootfs"

import sys
import os
import getopt
import time
#import subprocess

#line switch char
linesep = os.linesep
dirsep = os.sep


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
        self.image_path = self.stamp + name + '.' + self.name
        self.fs_path = ""
        self.tool_search_dir = 'fs_utils'
        self.fs_search_dir = 'rootfs'
        mydebug('Init rootfs')

    def set_fs_path(self, path):
        "set root fs top dircetory"
        self.fs_path = self.fs_search_dir + dirsep + path
        if (not os.path.isdir(self.fs_path)) or (not os.path.exists(self.fs_path)):
            return False
        
        return True

    def get_fs_path(self):
        return self.fs_path

    def proc_cmd(self, cmd, args):
        self.proc_str ='fakeroot ' + self.tool_search_dir + dirsep + cmd + ' ' + args + '2>&1 | tee ' + self.stamp + '.log' 
        print 'proc:', self.proc_str
        #subprocess.Popen(self.porc_str)
        os.system(self.proc_str)
        
    def info(self):
        print 'src path\t:',  self.get_fs_path()
        print 'dest path\t:', self.image_path

#yaffs class
class CYaffsFs(CRootFs):
    """
    yaffs 
    """
    def __init__(self, name):
        super(CYaffsFs, self).__init__(name, 'yaffs')
        mydebug('Init yaffs')
    def get_cmd(self):
        return 'mkyaffs2image'
    def get_arg(self):
        return self.get_fs_path() +' ' + self.image_path

#ramfs class
class CRamFs(CRootFs):
    """
    ramfs 
    """
    def __init__(self, name):
        super(CRamFs, self).__init__(name, 'ramfs')
        mydebug('Init ramfs')
    def get_cmd(self):
        return 'test_mk_ramfs'
    def get_arg(self):
        return self.get_fs_path() +' ' + self.image_path     
        
#cramfs class
class CCramFs(CRootFs):
    """
    cramfs
    """
    def __init__(self, name):
        super(CCramFs, self).__init__(name, 'cramfs')
        mydebug('Init cramfs')
    def get_cmd(self):
        return 'test_mk_cramfs'
    def get_arg(self):
        return self.get_fs_path() +' ' + self.image_path  

#global variables define


support_fs_tbl = {
            "yaffs":CYaffsFs,
            "ramfs":CRamFs,
            "cramfs":CCramFs,
        }



#option table
#if option has param,must follow char':' or '=' when long opt
opt_short_tbl = 'ht:vd:'
opt_long_tbl = ["help", "type=", "dir=", "prefix="]

#usage string for tips
usage_str = '[options] -t type -d dir' + linesep +\
            '\t-t, --type=name\tfilesystem type' + linesep +\
            '\t     support list:' + str(support_fs_tbl.keys()) +linesep +\
            '\t-d, --dir=directory\.rootfs directory' + linesep +\
            '\t-v\t\t\tverbose mode' + linesep +\
            '\t--prefix\t\toutput prefix name' + linesep +\
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

    def setup_rootfs_dir(self, path):
        return super(self.cls_name, self.instance).set_fs_path(path)

    def make_fs(self):
        super(self.cls_name, self.instance).proc_cmd(self.instance.get_cmd(), self.instance.get_arg())
        
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
    if len(sys.argv) <= 2:
        print 'param is 2 at least.'
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
        prefix_name = "_fsimg"
        fsdir = ""
        for o, a in opts:
            if o == "-v":
                debug = True
            elif o in ("-h", "--help"):
                usage()
                sys.exit()
            elif o in ("-t", "--type"):
                fstype = a
                mydebug('arg:type=', a)
            elif o in ("-d", "--dir"):
                fsdir = a
                mydebug('arg:directory=', a)
            elif o == "--prefix":
                prefix_name = prefix_name + str(a)
            else:
                pass
            
        if fstype not in support_fs_tbl.keys():
            print 'unsupport fs type:%s.' % (str(fstype))
            usage()
            return 0
        else:
            myrootfs = RootFs(fstype, prefix_name)
            if not myrootfs.setup_rootfs_dir(fsdir):
                print 'invalid rootfs path:%s.' % (str(fsdir))
                usage()
                return 0
        
            myrootfs.dump("elvon dump:")
            myrootfs.make_fs()
            
        
if __name__ == '__main__':
    main()
