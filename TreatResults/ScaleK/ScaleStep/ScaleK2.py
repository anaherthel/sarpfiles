#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Thu Jul 28 15:52:42 2022

@author: ana
"""
import sys
import csv
import os

def changeK(instFolder, data): #changes K in all files of the instance folder (sf_data and ghsarp)
    
    for i in range(len(data)):
        filename = data[i][0]+".txt"
        with open(instFolder + filename, 'r') as f:
            newf = f.readlines()
            words = newf[0].split()
            # Klist[i] = str((int(Klist[i])+1))
            #print("instance: ", data[i][0], "- K: ", words[0], "- new K: ", data[i][1])
            
            if (int(words[0]) > data[i][1]):
                words[0] = str(data[i][1])
        
        newline = words[0] + "\t"
        for w in range(1,len(words) - 1):
            newline += words[w]+'\t'
        
        newline += words[-1] + '\n'
        
        #print(newline)
        newf.pop(0)
        newf.insert(0, newline)
        with open(instFolder + filename, 'w') as f:
            for line in newf:
                f.write(line)

def changeKmulti(instFolder, data): #changes K in all files of the instance folder (csarp)
    for i in range(len(data)):
        filename = data[i][0] + ".txt"
        with open(instFolder + filename, 'r') as f:
            newf = f.readlines()
            words = newf[0].split()
            print("instance: ", data[i][0], "- K: ", words[0], "- new K: ", data[i][1])
            if (int(words[0]) <= data[i][1]):
                continue
            else:
                words[0] = str(data[i][1])
            n = int(words[2])
            m = int(words[3])
            
        newline = words[0] + "\t"
        for w in range(1,len(words) - 1):
            newline += words[w]+'\t'
        
        newline += words[-1] + '\n'
        
        # print(newline)
        newf.pop(0)
        newf.insert(0, newline)
        nlines = 2*n + 2*m + data[i][1] + 1
        
        # print(nlines)
        # print(newf)
        
        with open(instFolder + filename, 'w') as f:
            counter = 0
            while counter < nlines:
                line = newf[counter]
                f.write(line)
                counter += 1
        input()

def read_csv(file_name):
    data = []
    try:
        # Open the CSV file
        with open(file_name, mode='r') as file:
            # Create a CSV reader
            csv_reader = csv.reader(file)
            
            next(csv_reader)
            # Iterate through each row
            for row in csv_reader:
                # Convert the first column to int and the second to float
                name_inst = row[0]
                K = int(row[1])
                
                # Append the pair to the list
                data.append((name_inst, K))
    except Exception as e:
        print(f"Error: {e}")
    return data

def makeMissingList(data, inst_list, instFolder):
    #check which instances are missing
    print("The sizes of lists are the same: ", len(data) == len(inst_list))
    missing = []
    for a in inst_list:
        if a == 'missing.txt':
            continue
        aux_name = a.split('.')[0]
        #print("aux_name: ", aux_name)
        found = False
        for b in data:
            aux_name2 = b[0]
            #print("aux_name2: ", aux_name2)
            if aux_name == aux_name2:
                #print("found: ", aux_name)
                found = True
                break
        if not found:         
            missing.append(aux_name)
    
    print("\n\nMissing instances: ", missing)
    
    missing = sorted(missing)   
    
    filename = instFolder + 'missing.txt'

    if len(missing) != 0:
        with open(filename, 'w') as f:
            f.write("Missing instances:\n")
            for i in missing:
                f.write(i + '\n')
        


def read_files_scale(directory_csv, directory_inst):
    #make file list from directory
    csv_file_list = os.listdir(directory_csv)
    
    for i in csv_file_list:
        print("i: ", i)
        name_file = i.split('.')[0]
        
        print("did not continue")
        data = read_csv(directory_csv + i)
        
        name_file = i.split('.')[0]
        print("name_file: ", name_file)
        name_file = name_file[5:]
        inst_category = name_file[-1]
        name_file = name_file[:-1]
        instFolder = directory_inst + 'Instances_' + inst_category + '/' + name_file + '/'
        
        inst_list = os.listdir(instFolder)
        
        print(inst_list)
        
        makeMissingList(data, inst_list, instFolder)
        
        if name_file != 'csarp':    
            changeK(instFolder, data)                 
                        
        else:
            changeKmulti(instFolder, data)
        
        input()
        


# missing = checkInstances(instlist, 'file_list.txt')

############
dir_csv = '/home/ana/Documents/PHD/Research/Implementation/sarpfiles/TreatResults/ScaleK/ScaleStep/CSVScale/'
dir_inst = '/home/ana/Documents/PHD/Research/Implementation/sarpfiles/TreatResults/ScaleK/ScaleStep/'
read_files_scale(dir_csv, dir_inst)

#instlist, Klist = makeinstList('Scscale1.txt')

#changeKmulti(instlist, Klist)
# changeK(instlist, Klist)
##################
# print(instlist)
# print(Klist)