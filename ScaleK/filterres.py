#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Tue Aug  2 11:19:21 2022

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
            if (len(words) > 1):
                klist.append(words[1])
        
    
    final_instlist = list(dict.fromkeys(instlist))
    
    print('sizes: ', len(final_instlist), ' ', len(klist))
        
    return final_instlist, klist

def write_file(filename, instlist, klist):
    with open(filename, 'w') as f:
        for i in range(len(instlist)-1):
            f.write(instlist[i]+'\t'+klist[i]+'\n')
        f.write(instlist[-1]+'\t'+klist[-1])

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


# instlist, Klist = makeinstList('sfsarpKlist.txt')
instlist, Klist = makeinstList('scaled/SsfsarpKfip.txt')

write_file('SsfsarpKfipN.txt', instlist, Klist)

# makenewfile()

# changeKmulti(instlist, Klist, vnumber)

print(instlist)
print(Klist)