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
            if words[0] == 'Value:': 
                values.append(float(words[1]))
                durations.append(float(words[20]))
            else:
                new_v = []
                for w in words:
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

def calculate_times_of_Heu(solution, timesMat, n, m, tw, s):
    tms_list = []
    for i in range(len(solution)):
        tms = []
        for j in range(len(solution[i])-1):
            tms.append(timesMat[solution[i][j]][solution[i][j+1]])
        tms_list.append(tms)
    # calculated times
    ctimes = []
    # service times
    stimes = []
    durations_list = []
    violations = []
      
    for i in range(len(solution)):
        auxtm = []
        auxsv = []
        passengers = []
        twviol = []
        
        fp = False
        #first_passenger = None
        #last_passenger = None
    
        for j in range(len(solution[i])):
            twviol.append(False)
            auxtm.append(0)
            auxsv.append(tw[solution[i][j]])
            if solution[i][j] < n:
                passengers.append(2)
                if not fp:
                    fp = True
                    #first_passenger = solution[i].index(j)
            elif solution[i][j] < 2*n:
                passengers.append(1)
            else:
                passengers.append(0)
        for j in reversed(solution[i]):
            if j < n:
                #last_passenger = solution[i].index(j)
                break
        counter = 0
        
        for j in range(1, len(solution[i])):
            counter = timesMat[solution[i][j-1]][solution[i][j]] + s #s is a constant
            if passengers[j] > 0:
                #auxtm[j] = max(counter, tw[route[i][j]])
                auxtm[j] = counter
                auxsv[j] = max((auxsv[j-1] + counter), tw[solution[i][j]])
                if auxsv[j] > tw[solution[i][j]]:
                    twviol[j] = True
            else:
                auxtm[j] = counter           
                auxsv[j] = auxsv[j-1] + counter
        #auxsv[0] = 9
        
        duration = auxsv[-1] - auxsv[0]
        durations_list.append(duration)
        ctimes.append(auxtm)
        stimes.append(auxsv)
        violations.append(twviol)
    
    return stimes, durations_list, violations


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
        
def convertHeuToMIP(solution, n):
    newsol = []
    for i in range(len(solution)):
        newV = []
        for j in range(len(solution[i])):
            if (solution[i][j] >= n):
                newV.append(solution[i][j] - n)
        newsol.append(newV)

    return newsol




    
    
        
        
    
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        