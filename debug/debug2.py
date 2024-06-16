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

def read_earlier(filename):
    earlier = []
    with open(filename, 'r') as f:
        for line in f:
            words = line.split()
            time = words[1]
            earlier.append(float(time))
    return earlier

def read_profits(filename):
    profits = []
    with open(filename, 'r') as f:
        for line in f:
            words = line.split()
            profit = words[1]
            profits.append(float(profit))
    return profits

def read_service(filename):
    services = []
    with open(filename, 'r') as f:
        for line in f:
            words = line.split()
            service = words[1]
            services.append(float(service))
    return services

def read_info(filename, n, V):
    earlier = []
    profits = []
    services = []
    with open(filename, 'r') as f:
        f.readline()
        for a in range(V): 
            line = f.readline()
            words = line.split()
            profit = words[1]
            profits.append(float(profit))
        f.readline()
        f.readline()
        for a in range(n): 
            line = f.readline()
            words = line.split()
            time = words[1]
            earlier.append(float(time))        
        f.readline()
        f.readline()
        for a in range(V): 
            line = f.readline()
            words = line.split()
            service = words[1]
            services.append(float(service))     

    return earlier, profits, services

# def read_info2(filename): #from heuristic
#     tw = []
#     profits = []
#     with open(filename, 'r') as f:
#         for line in f:
#             words = line.split()
#             time = words[16].lstrip('[')
#             time = time.rstrip(',')
#             tw.append(float(time))
#             profit = words[21].rstrip('.')
#             profits.append(float(profit))
#     return tw, profits

def read_sol(filename):
    sol = []
    values = []
    durations = []
    with open(filename, 'r') as f:
        for line in f:
            words = line.split()
            if words[0] == 'Value:': 
                values.append(float(words[1]))
                durations.append(float(words[20]))
            else:
                new_v = []
                for w in words:
                    new_v.append(int(w))
                sol.append(new_v)
    return sol, values, durations

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


def calc_times(route, n, m, earlier, time, services):
    acctime = 0
    currtime = 0
    for i in range(len(route)-1):
        x = route[i]
        y = route[i+1]
        t = time[x][y]
        if x < n: 
            acctime = earlier[x]        
        print('from', x, 'to', y,':', t)
        acctime += t + services[x]
        if y < n:
            currtime = earlier[y]
        else:
            currtime = acctime
        
        print('Current time:', currtime)
        print('Accumulated time:', acctime)
        

        


def calculate_times(route, times, n, m, tw, s):
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
            passengers.append(2)
            if not fp:
                fp = True
                first_passenger = route.index(i)
        elif i < 2*n:
            passengers.append(1)
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
        # print("Cost of", route[i], "and", route[i+1],":", cost[route[i]][route[i+1]])
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

def read_node_sol(filename, k):
    routes = []
    with open(filename, 'r') as f:
        for i in range(k):
            route = []
            line = f.readline()
            words = line.split('-')
            words[-1] = words[-1].rstrip('\n')
            words[-1] = words[-1].rstrip('')
            for w in words:
                if w == 'S' or w == 'f':
                    route.append(w)
                elif w == '':
                    continue
                else:
                    route.append(int(w))
            routes.append(route)
    
    return routes
    
        
        
    
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        