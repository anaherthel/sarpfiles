#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Thu Jul 28 15:52:42 2022

@author: ana
"""

# Take instances names

# def makeinstList(filename):
#     instlist = []
#     klist = []
#     with open(filename, 'r') as f:
#         for line in f:
#             words = line.split()
#             instlist.append(words[0]+'.txt')

#             # knumber = words[1].strip('\n')
#             klist.append(words[1])
    
        
#     return instlist, klist

def makeinstList(filename):
    instlist = []
    klist = []
    with open(filename, 'r') as f:
        for line in f:
            words = line.split()
            instlist.append(words[0])

            # knumber = words[1].strip('\n')
            klist.append(words[1])
    
        
    return instlist, klist

# def makeinstListMulti(filename):
#     instlist = []
#     klist = []
#     vnumber = []
#     with open(filename, 'r') as f:
#         for line in f:
#             words = line.split()
#             instlist.append(words[0]+'.txt')

#             # knumber = words[1].strip('\n')
#             klist.append(words[1])
#             vline = []
#             for a in range(2, len(words)):
#                 vline.append(int(words[a]))
#             vnumber.append(vline)
        
#     return instlist, klist, vnumber

def changeK(instlist, Klist):
    for i in range(len(instlist)):
        filename = instlist[i]
        with open('Instances/sf_data/'+filename, 'r') as f:
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
        with open('Instances/sf_data/'+filename, 'w') as f:
            for line in newf:
                f.write(line)
                
# Older version to select non consecutive vehicle depots from instances
# def changeKmulti(instlist, Klist, vnumber):
#     for i in range(len(instlist)):
#         filename = instlist[i]
#         with open('Instances/csarp/'+filename, 'r') as f:
#             newf = f.readlines()
#             words = newf[0].split()
#             words[0] = Klist[i]
#             n = int(words[2])
#             m = int(words[3])
            
#         newline = words[0] + "\t"
#         for w in range(1,len(words) - 1):
#             newline += words[w]+'\t'
        
#         newline += words[-1] + '\n'
        
#         print(newline)
#         newf.pop(0)
#         newf.insert(0, newline)
#         counter = 0
#         counter2 = 2*n+2*m
#         print('counter2: ', counter2)
               
#         with open('Instances_M/csarp/'+filename, 'w') as f:
#             for line in newf:
#                 words = line.split('\t')
#                 if words[3] == '0':
#                     if counter in vnumber[i]:
#                         words[0] = str(counter2)
#                         newline = words[0] + "\t"        
#                         for w in range(1,len(words) - 1):
#                             newline += words[w]+'\t'
                        
#                         newline += words[-1]
#                         print(newline)
#                         f.write(newline)
#                         counter2 += 1
#                     counter += 1

#                 else:
#                     f.write(line)

def changeKmulti(instlist, Klist):
    for i in range(len(instlist)):
        filename = instlist[i]
        with open('Instances/csarp/'+filename, 'r') as f:
            newf = f.readlines()
            words = newf[0].split()
            words[0] = Klist[i]
            n = int(words[2])
            m = int(words[3])
            
        newline = words[0] + "\t"
        for w in range(1,len(words) - 1):
            newline += words[w]+'\t'
        
        newline += words[-1] + '\n'
        
        # print(newline)
        newf.pop(0)
        newf.insert(0, newline)
        nlines = 2*n + 2*m + int(Klist[i]) + 1
        
        # print(nlines)
        # print(newf)
                     
        with open('Instances/csarp/'+filename, 'w') as f:
            counter = 0
            while counter < nlines:
                line = newf[counter]
                f.write(line)
                counter += 1

# instlist, Klist = makeinstList('sfsarpKlist.txt')
instlist, Klist = makeinstList('SsfsarpKfipN.txt')

# changeKmulti(instlist, Klist)
changeK(instlist, Klist)

# print(instlist)
# print(Klist)