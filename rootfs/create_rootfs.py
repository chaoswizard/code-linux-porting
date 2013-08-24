#!/usr/bin/env python

i = 1 + 2 * 4
print i,

print 'test raw input'
i = raw_input('pls input:\n')
print int(i)

print 'test while'
count = 0
while count <= 10:
    print count
    count += 1

print 'test for'
for i in range(11):
    print i

print 'test if elif else'
i = int(raw_input('input num\n'))
if i > 0:
    print 'sign is +'
elif i < 0:
    print 'sign is -'
else:
    print 'num is 0'

print 'test list OR array'
numset = [1,2,3,4,5]
total = 0
for i in range(5):
    total += numset[i]
print 'total is', total

for i in range(5):
    numset[i] = int(raw_input('pls input ' + str(i+1) + ' number\n'))
print 'sum is', sum(numset)
print 'average is', float(sum(numset))/5

print 'test x < y < x'
while 1:
    if 1 <= int(raw_input('input a num between 1 - 100\n')) <= 100:
        break
    else:
        print 'error'

print 'test sort'
i = int(raw_input('input num 1\n'))
j = int(raw_input('input num 2\n'))
k = int(raw_input('input num 3\n'))

count = 0
for count in range(2):
    if i > j:
        tmp = i
        i = j
        j = tmp
    if j > k:
        tmp = j
        j = k
        k = tmp
print i, j, k

print 'test Tkinter'
import Tkinter
top = Tkinter.Tk()
hello = Tkinter.Label(top, text='hello world')
hello.pack()

quit = Tkinter.Button(top, text='Quit',
    command=top.quit, bg='red', fg='white')
quit.pack(fill=Tkinter.X, expand=1)

Tkinter.mainloop()
