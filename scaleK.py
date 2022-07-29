#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Thu Jul 28 15:52:42 2022

@author: ana
"""

# Take instances names

def makeinstList(filename):
    instlist = []
    klist = []
    with open(filename, 'r') as f:
        for line in f:
            words = line.split()
            instlist.append(words[0]+'.txt')

            # knumber = words[1].strip('\n')
            klist.append(words[1])
    
        
    return instlist, klist

def changeK(instlist, Klist):
    for i in range(len(instlist)):
        filename = instlist[i]
        with open('Instances_M/sf_datasolved/'+filename, 'r') as f:
            newf = f.readlines()
            words = newf[0].split()
            words[0] = Klist[i]
        
        newline = words[0] + "\t"
        for w in range(1,len(words) - 1):
            newline += words[w]+'\t'
        
        newline += words[-1] + '\n'
        
        print(newline)
        newf.pop(0)
        newf.insert(0, newline)
        with open('Instances_M/sf_datasolved/'+filename, 'w') as f:
            for line in newf:
                f.write(line)


instlist, Klist = makeinstList('sfsarpKlist.txt')
changeK(instlist, Klist)

print(instlist)
print(Klist)