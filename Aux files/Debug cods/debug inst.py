
#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Mon Aug 31 09:39:55 2020

@author: ana
"""

from math import sqrt
import plot as p

def read_coordinates(filename):
    coords = []
    with open(filename, 'r') as f:
        for line in f:
            words = line.split()
            x = float(words[0])
            y = float(words[1])
            coords.append((x, y))
    return coords

def euc_dist(p1, p2):
    x1, y1 = p1
    x2, y2 = p2
    return sqrt((x1-x2)**2 + (y1-y2)**2)

def calc_matrix(coordinates):
    matrix = []
    for c1 in coordinates:
        row = []
        for c2 in coordinates:
            dist = euc_dist(c1, c2)
            row.append(dist)
        matrix.append(row)
    return matrix

def write_matrix(matrix, filename):
    with open(filename, 'w') as f:
        for row in matrix:
            for item in row:
                f.write(str(item)+" ")
            f.write("\n")
            

coordinates = read_coordinates('debug_coords3.txt')

p.plot_coordinates(coordinates)

# Now calculate the distance matrix using the euclidean distance.
matrix = calc_matrix(coordinates)

write_matrix(matrix, 'debug_matrix3.txt')
