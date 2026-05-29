#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Thu Nov 13 2025

@author: ana
"""
import csv
import re
import pandas as pd


def csv_header(file_path, row_data):
    with open(file_path, 'w', newline='') as csvfile:
        writer = csv.writer(csvfile)
        writer.writerow(row_data)

def add_row_to_csv(file_path, row_data):
    with open(file_path, 'a', newline='') as csvfile:
        writer = csv.writer(csvfile)
        writer.writerow(row_data)
        
def get_instance(filename):
    
    pattern_params = {
        "bcCountCut3rowPackR1C": re.compile(r"bcCountCut3rowPackR1C\s*([\d\.]+)"),
        "bcCountCutInMaster": re.compile(r"bcCountCutInMaster\s*([\d\.]+)"),
        "bcCountCutR1C": re.compile(r"bcCountCutR1C\s*([\d\.]+)"),
        "bcCountNodeProc": re.compile(r"bcCountNodeProc\s*([\d\.]+)")
    }   
    
    total_time = None
    instance = None
    name = filename.rstrip('.txt')
    output = 'Res'+name+'.csv'
    row_data_head = ('Instance', 'LB', 'UB', 'Time', 'bcCountNodeProc', 'bcCountCutR1C', 'bcCountCutInMaster', 'bcCountCut3rowPackR1C')
    row_data = [None] * len(row_data_head)
    for i in range(4, len(row_data_head)):
        row_data[i] = 0    
    csv_header(output, row_data_head)
    new_inst = False
    counter = 0
    with open(filename, 'r') as f:
        for line in f:
            instance = None
            for key, pattern in pattern_params.items():
                match = pattern.search(line)
                if match:
                    value = int(match.group(1))
                    if key == "bcCountNodeProc":
                        row_data[4] = value
                    elif key == "bcCountCutR1C":
                        row_data[5] = value
                    elif key == "bcCountCutInMaster":
                        row_data[6] = value
                    elif key == "bcCountCut3rowPackR1C":
                        row_data[7] = value
                    print(f"{key}: {value}")
            if line[:12] == 'command line':
                counter += 1
                LB = 'N'
                UB = 'N'
                total_time = 'N'
                # print(row_data)
                if new_inst != False and counter > 0:
                    # print("Instance without info")
                    add_row_to_csv(output, row_data_head)                
                arguments = line.split()
                path = arguments[7].split('/')
                instance = path[3]
                row_data[0] = instance
                row_data[1] = LB
                row_data[2] = UB
                row_data[3] = total_time
                new_inst = True
                print(instance)
                # input()
            elif line[:18] == 'Search is finished':
                strings = line.split()
                LB = float(strings[7])
                UB = float(strings[9])
                row_data[1] = LB
                row_data[2] = UB
                print(LB, UB)
                # input()
            elif line[:4] == 'TIME':
                strings = line.split('=')
                total_time = int(strings[2])
                row_data[3] = total_time
                print(total_time)
                # input()
                new_inst = False
                add_row_to_csv(output, row_data)
                row_data = [None] * len(row_data_head)
                for i in range(4, len(row_data_head)):
                    row_data[i] = 0

            

            # row_data = (instance, LB, UB, total_time)


                
                
# LB, UB, instance, total_time = get_instance('output.txt') 
log_path = '/home/ana/Documents/PHD/Research/Implementation/sarpfiles/BapResults/logs'
#list the files in the directory
import os
os.chdir(log_path)
files = os.listdir(log_path)
for file in files:
    if file.endswith('.txt'):
        get_instance(file)
        



