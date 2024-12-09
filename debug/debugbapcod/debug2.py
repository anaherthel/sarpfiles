#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed Nov 22 14:57:22 2023

@author: ana
"""

import pandas as pd
import math

def read_mat(file_path):
    # Reading the CSV file into a DataFrame
    df = pd.read_csv(file_path)
    
    # Converting the DataFrame to a matrix of floating point numbers
    matrix = df.values
    return matrix

def read_info(filename):
    tw = []
    profits = []
    with open(filename, 'r') as f:
        for line in f:
            words = line.split()
            time = words[16].lstrip('[')
            time = time.rstrip(',')
            tw.append(float(time))
            profit = words[21].rstrip('.')
            profits.append(float(profit))
    return tw, profits

def read_infobap(filename):
    tw = []
    services = []
    with open(filename, 'r') as f:
        for line in f:
            words = line.split()
            tw.append(float(words[1]))
            services.append(float(words[3]))
    return tw, services

def read_sol(filename):
    sol = []
    values = []
    durations = []
    with open(filename, 'r') as f:
        for line in f:
            words = line.split()
            print("words: ", words)
            if words[0] == 'Value:': 
                values.append(float(words[1]))
                durations.append(float(words[20]))
            else:
                new_v = []
                for w in words:
                    print('w: ', w)
                    new_v.append(int(w))
                sol.append(new_v)
    return sol, values, durations

def read_solbap(filename, n, m):
    sol = []
    durations = []
    counter = 0
    solVal = 0
    routeval = []
    vehicles = []
    with open(filename, 'r') as f:
        for line in f:
            words = line.split()
            if (len(words) < 2):
                continue
            if words[0] == 'Solution:' and counter == 0: 
                counter += 1
                solVal = float(words[6])
            elif words[0] == 'Solution:' and counter > 0:
                routeval.append(float(words[6]))
            if words[0] == 'Ordered':
                words2 = line.split(':')
                rstring = words2[1].split('->')
                route = []
                tw = [] 
                for i in rstring:
                    temp = i.split('(')
                    route.append(int(temp[0]))
                    a = temp[1].split(')')
                    tw.append(float(a[0]))
                sol.append(route)
                durations.append(tw)
            if(words[1] == 'includes'):
                words2 = line.split('_')
                if len(vehicles) > 0 and vehicles[-1] == int(words2[1]):
                    continue
                vehicles.append(int(words2[1]))
                
    #adjust routes
    print("vec: ", vehicles)
    for i in range(len(sol)):
        sol[i][0] = n + 2*m + vehicles[i]
        sol[i][-1] = n + 2*m + len(vehicles) + vehicles[i]
        for j in range(1, len(sol[i])-1):
            sol[i][j] -= 1
    
    sol.sort()
    
    return sol, durations, routeval, vehicles, solVal

def read_baproute(filename):
    
    solution = []
    
    with open(filename, 'r') as f:
        for line in f:
            route = []
            words = line.split(',')
            for i in range(len(words)):
                route.append(int(words[i]))
            solution.append(route)
    
    return solution

def make_constraints(sol):
    #//IloExpr exp(env);
	#//exp = x[19][9][0];

	#//sprintf (var, "Constraint15");

	#//IloRange cons = (exp == 1);
	#//cons.setName(var);
	#//model.add(cons);

	#//exp = x[9][16][0];

	#//sprintf (var, "Constraint16");

	#//cons = (exp == 1);
	#//cons.setName(var);
	#//model.add(cons);

    index = 16
    filename = "constraints.txt"
    with open(filename, 'w') as f:
        f.write ("IloExpr exp(env);)\n")
        for i in range(len(sol)):
            for j in range(0, len(sol[i])-1):
                f.write("exp = x["+str(sol[i][j])+"]["+str(sol[i][j+1])+"]["+str(i)+"];\n")
                f.write("sprintf (var, \"Constraint"+str(index)+"\");\n")
                if(i == 0 and j == 0):
                    f.write("IloRange cons = (exp == 1);\n")
                else:
                    f.write("cons = (exp == 1);\n")
                f.write("cons.setName(var);\n")
                f.write("model.add(cons);\n")
                index += 1


def print_solution(sol, values, durations):
    for i in range(len(sol)):
        print("Route", i, ":")
        print("Value:", values[i], "- Duration:", durations[i])
        for j in range(len(sol[i])):
            print(sol[i][j], " ", end='')
        print()

def calculate_costs(route, costs):
    cst = []
    
    for i in range(len(route)-1):
        cst.append(costs[i][i+1])
    
    return cst

def calculate_times2(route, times, n, m, tw, s):
    tms = []
    for i in range(len(route)-1):
        tms.append(times[i][i+1])
    # calculated times
    ctimes = []
    # service times
    stimes = []
    passengers = []
    fp = False
    first_passenger = None
    last_passenger = None
    for i in route:
        stimes.append(tw[i])
        ctimes.append(0)
        if i < n:
            passengers.append(1)
            if not fp:
                fp = True
                first_passenger = route.index(i)
        else:
            passengers.append(0)
    for i in reversed(route):
        if i < n:
            last_passenger = route.index(i)
            break
    
    counter = 0
    for i in range(1, len(route)):
        counter = times[route[i-1]][route[i]] + s
        if passengers[i] > 0:
            ctimes[i] = max(counter, tw[route[i]])
        else:
            ctimes[i] = counter
            
    for i in range(1, len(stimes)-1):
        if stimes[i] == 0:
            stimes[i] = stimes[i-1] + ctimes[i]
    stimes[-1] = stimes[-2] + s
    counter = 0
    if first_passenger != None:
        for i in range(first_passenger):
            counter += times[route[i]][route[i+1]]
            counter += s
        stimes[0] = ctimes[first_passenger] - counter
    else:
        stimes[0] = ctimes[0] - counter

    duration = stimes[-1] - stimes[0]
    
    return ctimes, stimes, duration


def printTimes(ctime, stime, cdur):
    print("Calculated Times:\n", ctime)
    print("Service Times:\n", stime)
    print("Duration of route:\n", cdur)

def calcValue(route, cost, profits):
    prof = 0
    currcost = 0
    for i in range(len(route)-1):
        prof += profits[route[i]]
        currcost += cost[route[i]][route[i+1]]
        print("Cost of", route[i], "and", route[i+1],":", cost[route[i]][route[i+1]])
    return round(prof - currcost, 4)

def printTt(route, time):
    for i in range(len(route)-1):
        print("Time of", route[i], "and", route[i+1],":", time[route[i]][route[i+1]])

    
def compareTimes(ctime, stime, cdur, route, n, maxT):
    if cdur > maxT:
        return False
    for i in range(len(ctime)):
        if route[i] < n:
            if ctime[i] != stime[i]:
                return False
    
    return True
    
def compareValues(calcVal, solVal):
    solVal1 = round(solVal, 4)
    calcVal1 = round(calcVal, 4)
    print("Route Value:", solVal)
    print("Calculated Value:", calcVal)
    print(abs(solVal1 - calcVal1))
    if abs(solVal1 - calcVal1) > 0.01:
        return False
    
    return True

def printInfo(tw, profits):
    print("ID\tTW\t\tProfit")
    for i,t in enumerate(tw):
        print(str(i)+" \t"+str(t)+" \t"+str(profits[i]))
    
    
        
        
    
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
