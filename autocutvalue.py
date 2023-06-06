#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Mon Jun  5 15:44:32 2023

@author: ana
"""
import csv
import math as m
import os



def read_csv_lists(file_path):
    list_of_files = []
    list_of_values = []
    with open(file_path, 'r') as csv_file:
        reader = csv.reader(csv_file)
        for line in reader:
            list_of_files.append(line[0])
            if float(line[1]) < 0:
                line[1] = 0
            list_of_values.append(m.ceil(float(line[1])))

    return list_of_files, list_of_values

def make_cutoff_file(l_f, l_v):
    folder_name = 'sol_sfsarp'
    folder_path = './sol_sfsarp'
    os.mkdir(folder_name)

    for i in range(len(l_f)):
        name = l_f[i] + ".txt"
        file_path = os.path.join(folder_path, name)        
        with open(file_path, 'w') as f:
            f.write(str(l_v[i]))
        
    
file = "cutvaluessfsarpS.csv"

l_f, l_v = read_csv_lists(file)

make_cutoff_file(l_f, l_v)

print(l_f)
print(l_v)
