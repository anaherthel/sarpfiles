#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Mon Jun  5 15:44:32 2023

@author: ana
"""
import csv
import math as m
import os

# Automate the creation of lists of solution values to pass as cut off values to bapcode

def read_csv_lists(file_path):
    list_of_files = []
    list_of_values = []
    with open(file_path, 'r') as csv_file:
        reader = csv.reader(csv_file)
        for line in reader:
            list_of_files.append(line[0])
            # if float(line[1]) < 0:
            #     line[1] = 0
            # list_of_values.append(m.ceil(float(line[1])))
            new_val = float(line[1])
            list_of_values.append(m.ceil(new_val))

    return list_of_files, list_of_values

def make_cutoff_file(l_f, l_v, folder_name):
    folder_path = './'+folder_name
    os.mkdir(folder_name)

    for i in range(len(l_f)):
        if l_f[i][-4:] == ".txt":
            l_f[i] = l_f[i][:-4]
        name = l_f[i] + ".txt"
        file_path = os.path.join(folder_path, name)        
        with open(file_path, 'w') as f:
            f.write(str(l_v[i]))

def obtainFolderName(filename):
    
    auxName = filename.split('.')[0]
    auxName = auxName.replace("cutvalues", "", 1)
    auxName = auxName.replace("mcmp", "", 1)

    folderName = 'sol_' + auxName
    
    return folderName


dir_csv = '/home/ana/Documents/PHD/Research/Implementation/sarpfiles/PrepForbaptests/prepMaxRunTime/csvFiles'

csv_list = os.listdir(dir_csv)


for i in csv_list:
    print("i: ", i)
    fullPath = os.path.join(dir_csv, i)
    
    l_f, l_v = read_csv_lists(fullPath)
    
    folder_name = obtainFolderName(i)
    print("FolderName:", folder_name)
    #input()

    make_cutoff_file(l_f, l_v, folder_name)

    print(l_f)
    print(l_v)



