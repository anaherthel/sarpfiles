#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Thu Jun 22 10:33:58 2023

@author: ana
"""
import csv

def csv_header(file_path, row_data):
    with open(file_path, 'w', newline='') as csvfile:
        writer = csv.writer(csvfile)
        writer.writerow(row_data)

def add_row_to_csv(file_path, row_data):
    with open(file_path, 'a', newline='') as csvfile:
        writer = csv.writer(csvfile)
        writer.writerow(row_data)
        
def get_instance(filename):
    total_time = None
    instance = None
    name = filename.rstrip('.txt')
    output = 'Res'+name+'.csv'
    row_data = ('Instance', 'LB', 'UB', 'Time')
    csv_header(output, row_data)
    new_inst = False
    counter = 0
    with open(filename, 'r') as f:
        for line in f:
            instance = None
            if line[:12] == 'command line':
                counter += 1
                LB = 'N'
                UB = 'N'
                total_time = 'N'
                # print(row_data)
                if new_inst != False and counter > 0:
                    # print("Instance without info")
                    add_row_to_csv(output, row_data)                
                arguments = line.split()
                path = arguments[7].split('/')
                instance = path[3]
                row_data = [instance, LB, UB, total_time]
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

            # row_data = (instance, LB, UB, total_time)


                
                
# LB, UB, instance, total_time = get_instance('output.txt') 
get_instance('bap31ssfsarp.txt') 

