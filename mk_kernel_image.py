#!/usr/bin/env python

"create kernel image"

import sys
import os
import getopt
import time
#import subprocess

#line switch char
linesep = os.linesep
dirsep = os.sep


#rootfs class, base is object
class CKerImage(object):
    """
    kernel image class
    """
    def __init__(self, name, imgtype):
        global linesep
        #time stamp
        self.stamp = time.strftime("%Y%m%d%H%M%S")
        self.name = imgtype
        self.path = name + self.stamp + '.' + self.name
        self.tools_dir = 'my-utils'
        self.ker = 'arch' + dirsep + 'arm' + dirsep + 'boot' +\
                       dirsep + 'zImage'
        self.ver = 'Elvon-3.9'
        self.load_addr = '0x30008000'
        self.entry_addr = '0x30008040'
        mydebug('Init Kernel Image')

    def set_kerdir(self, path):
        "set kernel build top dircetory"
        self.src = path + dirsep + self.ker
        if (not os.path.isfile(self.src)) or (not os.path.exists(self.src)):
           return False
        
        return True

    def proc_cmd(self, cmd, args):
        self.proc_str = cmd + ' ' + args
        print 'proc:', self.proc_str
        #subprocess.Popen(self.porc_str)
        os.system(self.proc_str)
        
    def info(self):
        print 'src path\t:',  self.src
        print 'dest path\t:', self.path

#yaffs class
class CUImage(CKerImage):
    """
    kernel image for uboot 
    """
    def __init__(self, name):
        super(CUImage, self).__init__(name, 'uImage')
        mydebug('Init image for uboot')
    def get_cmd(self):
        return self.tools_dir + dirsep + 'mk_uImage'
    def get_arg(self):
        return  '-n ' + self.ver + ' ' + \
                '-A arm -O linux -T kernel -C none' + ' ' + \
                '-a ' + self.load_addr + ' ' +\
                '-e ' + self.entry_addr + ' ' +\
                '-d ' + self.src + ' ' + self.path

#ramfs class
class CZImage(CKerImage):
    """
    kernel image for zimage 
    """
    def __init__(self, name):
        super(CZImage, self).__init__(name, 'zImage')
        mydebug('Init image for normal boot')
    def get_cmd(self):
        return 'cp -a -f'
    def get_arg(self):
        return self.src + ' ' + self.path    
        

#global variables define


support_img_tbl = {
            "uImage":CUImage,
            "zImage":CZImage,
           }



#option table
#if option has param,must follow char':' or '=' when long opt
opt_short_tbl = 'ht:vd:'
opt_long_tbl = ["help", "type=", "dir="]

#usage string for tips
usage_str = '[options] -t type -d dir' + linesep +\
            '\t-t, --type=name\timage type' + linesep +\
            '\t     support list:' + str(support_img_tbl.keys()) +linesep +\
            '\t-d, --dir=directory\tkernel build top directory' + linesep +\
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
class KerImage(object):
    """
    kernel Image
    """
    def __init__(self, key, name):
        global support_img_tbl
        self.key = key
        self.cls_tab = support_img_tbl
        self.cls_name = self.cls_tab[key];
        self.instance = self.cls_name(name)

    def setup_kerdir(self, path):
        return super(self.cls_name, self.instance).set_kerdir(path)

    def make_image(self):
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
        imgtype = "unsupport"
        kerdir = ""
        for o, a in opts:
            if o == "-v":
                debug = True
            elif o in ("-h", "--help"):
                usage()
                sys.exit()
            elif o in ("-t", "--type"):
                imgtype = a
                mydebug('arg:type=', a)
            elif o in ("-d", "--dir"):
                kerdir = a
                mydebug('arg:directory=', a)
            else:
                pass
            
        if imgtype not in support_img_tbl.keys():
            print 'unsupport kernel image type:%s.' % (str(imgtype))
            usage()
            return 0
        else:
            mykerimage = KerImage(imgtype, "ker")
            if not mykerimage.setup_kerdir(kerdir):
                print 'invalid kernel output path:%s.' % (str(kerdir))
                usage()
                return 0
        
            mykerimage.dump("elvon dump:")
            mykerimage.make_image()
            
        
if __name__ == '__main__':
    main()
