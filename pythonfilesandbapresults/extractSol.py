#Code to extract:
#1. Solution
#2. Served Parcels
#3. Deadheading amount in km

import sys
import os
import csv

def read_solution(filename):
    solution = []
    arcs_solution = []
    solvalues = []
    instanceNames = []
    ublist = []
    lblist = []
    newinst = False
    times = []
    single = False
    with open(filename, 'r') as f:
        for line in f:
            if line.startswith("command line:"):
                #print(line)
                words = line.split()
                instPath = words[7]
                words2 = instPath.split('/')
                instName = words2[-1]
                instName = instName.split('.')[0]
                instanceNames.append(instName)
                n = int(instName.split('-')[1])
                m = int(instName.split('-')[2])
                if instName.split('-')[0] != 'sarp':
                    single = True
                #input()
                newinst = False

            if line.startswith("Search is finished"):
                newinst = True
                words = line.split()
                lb = words[7]
                ub = words[9]
                lblist.append(lb)
                ublist.append(ub)
                counter = 0
                sol = []        
                vehicles = []
                arcs = []
            if newinst and line.startswith("Solution:"):
                words = line.split()
                if counter == 0:
                    counter += 1
                    solVal = float(words[6])
                    solvalues.append(solVal)
                    
            if line.startswith("   Ordered solution"):
                words2 = line.split(':')
                rstring = words2[1].split('->')
                route = []
                for i in rstring:
                    temp = i.split('(')
                    route.append(int(temp[0]))
                sol.append(route)
            if line.startswith("Solution includes"):
                #obtain costs
                words2 = line.split()
                arc_i = None
                arc_j = None
                costArc = 0
                start_index = words2[2].find('i') + 1  
                end_index = words2[2].find('j', start_index)  
                if start_index > 0 and end_index > 0:  
                    arc_i = int(words2[2][start_index:end_index])
                start_index = words2[2].find('j') + 1 
                end_index = words2[2].find('_', start_index)               
                if start_index > 0 and end_index > 0: 
                    arc_j = int(words2[2][start_index:end_index])
                start_index = words2[2].find('(') + 1  
                end_index = words2[2].find(')', start_index)
                if start_index > 0 and end_index > 0:  
                    costArc = float(words2[2][start_index:end_index])
                
                #print("arc_i: ", arc_i, "arc_j: ", arc_j, "costArc: ", costArc)
                #input()

                
                words2 = line.split('_')              
                arcs.append([arc_i, arc_j, costArc, int(words2[1])])
                
                if len(vehicles) > 0 and vehicles[-1] == int(words2[1]):
                    continue
                
                vehicles.append(int(words2[1])) 
                #print("arcs: ", arcs)    
                              
            
            if newinst and line.startswith("undefined"):
                solution.append([])
                arcs_solution.append([])                
                solvalues.append(-1)
            
            if line.startswith("TIME"):
               
                words = line.split()                
                time = float(words[4])
                
                if len(sol) < 1:                    
                    times.append(time)        
                                
                else:
                    if not single:                    
                        for i in range(max(vehicles)):
                            if i not in vehicles:
                                vehicles.append(i)
                            
                        for i in range(len(sol)):
                            sol[i][0] = n + 2*m + vehicles[i]
                            sol[i][-1] = n + 2*m + len(vehicles) + vehicles[i]
                            for j in range(1, len(sol[i])-1):
                                sol[i][j] -= 1                    
                        sol.sort()                                
                        solution.append(sol)
                        arcs_solution.append(arcs)                  
                        times.append(time)
                        
                    else:
                        for i in range(len(sol)):
                            sol[i][0] = n + 2*m
                            sol[i][-1] = n + 2*m + len(sol)
                            for j in range(1, len(sol[i])-1):
                                sol[i][j] -= 1 
                                                   
                        sol.sort()                                
                        solution.append(sol)
                        arcs_solution.append(arcs)
                        times.append(time)
                

                                  
                #print("sol: ", sol)
                #input()
                #print("instanceNames: ", instanceNames)
                #print("solvalues: ", solvalues)
                #print("ublist: ", ublist)
                #print("lblist: ", lblist)
                #print("arcs_solution: ", arcs_solution)
                #input()
                

    
    return solution, instanceNames, solvalues, ublist, lblist, arcs_solution, times

def check_served_parcels(solution, instanceNames):
    n = int(instanceNames.split('-')[1])
    m = int(instanceNames.split('-')[2])
    
    served_parcels = 0
    for i in range(len(solution)):#for each vehicle
        if(solution[i] == -1):
            return served_parcels
        for j in range(1, len(solution[i])): #for each stop
            if(solution[i][j] >= n and solution[i][j] < n + m):
                served_parcels += 1
    return served_parcels

def fixArcs(arcs_solution, instanceNames, solution):
    n = int(instanceNames.split('-')[1])
    m = int(instanceNames.split('-')[2])
    
    arcs_solution.sort(key = lambda x: x[3])

    arcs_vehicle = []
    lastVehicle = arcs_solution[-1][3]
    for i in range(lastVehicle + 1):
        arcs_vehicle.append([])
    
    for i in range(len(arcs_solution)):
        pos = arcs_solution[i][3]
        arcs_vehicle[pos].append(arcs_solution[i])

    ordered_arcs = []
    
    for i in range(len(arcs_vehicle)):
        aux = []   
        next = i
        
        if len(arcs_vehicle[i]) == 0:
            insertion = [i, i, 0, i]
            arcs_vehicle[i].append(insertion)
            ordered_arcs.append([insertion])
            solution.insert(i, [i, i])
            continue
        
        while len(aux) < len(arcs_vehicle[i]):
            for j in range(len(arcs_vehicle[i])):
                if arcs_vehicle[i][j][0] == next:
                    aux.append(arcs_vehicle[i][j])
                    next = arcs_vehicle[i][j][1]
                    break
                
        ordered_arcs.append(aux)
    

    #Fix indices on arcs
    for i in range(len(ordered_arcs)):
        for j in range(len(ordered_arcs[i])):
            if ordered_arcs[i][j][0] < len(ordered_arcs):
                currentK = ordered_arcs[i][j][0]
                ordered_arcs[i][j][0] = n + 2*m + currentK
            else:
                ordered_arcs[i][j][0] -= len(ordered_arcs)
                
            if ordered_arcs[i][j][1] < len(ordered_arcs):
                currentK = ordered_arcs[i][j][1]
                ordered_arcs[i][j][1] = n + 2*m + len(ordered_arcs) + currentK
            else:
                ordered_arcs[i][j][1] -= len(ordered_arcs)
            if len(ordered_arcs[i]) == 1:
                solution[i][0] = ordered_arcs[i][0][0]
                solution[i][1] = ordered_arcs[i][0][1]
    

                
    return ordered_arcs

def fixArcsSingle(arcs_solution, instanceNames, solution):
    #Check solution first to know where to cut arcs solution list
    n = int(instanceNames.split('-')[1])
    m = int(instanceNames.split('-')[2])
    
    arcs_solution.sort(key = lambda x: x[3])

    arcs_vehicle = []
   
   
    for i in range(len(arcs_solution)):
        #for j in range(len(arcs_solution[i])):
        if arcs_solution[i][0] == 0:
            arcs_solution[i][0] = n + 2*m
        else:
            arcs_solution[i][0] -= 1
            
        if arcs_solution[i][1] == 0:
            arcs_solution[i][1] = n + 2*m + len(solution)
        else:
            arcs_solution[i][1] -= 1
                   
    ordered_arcs = []
    
    for i in range(len(solution)):
        aux = []
        
        for j in range(len(solution[i]) - 1):
            a = solution[i][j]
            b = solution[i][j+1]
            
            for k in range(len(arcs_solution)):
                if arcs_solution[k][0] == a and arcs_solution[k][1] == b:
                    aux.append(arcs_solution[k])
                    break
        ordered_arcs.append(aux)

                
    return ordered_arcs

def check_deadheading(solution, instanceNames, ordered_arcs):
    n = int(instanceNames.split('-')[1])
    m = int(instanceNames.split('-')[2])

    e_hours = 0
    e_kms = 0
    for i in range(len(solution)):#for each vehicle
        load = 0
        e_kms += (ordered_arcs[i][0][2]/0.46)
        e_hours += (ordered_arcs[i][0][2]/0.46)/41
        for j in range(1, len(solution[i])-1): #for each stop
            if(solution[i][j] >= n and solution[i][j] < n + m):
                load += 1
            elif(solution[i][j] >= n + m):
                load -= 1
            if(load == 0):
                e_hours += (ordered_arcs[i][j][2]/0.46)/41
                e_kms += (ordered_arcs[i][j][2]/0.46)

    return e_hours, e_kms          

def csv_header(file_path, row_data):
    with open(file_path, 'w', newline='') as csvfile:
        writer = csv.writer(csvfile)
        writer.writerow(row_data)

def add_row_to_csv(file_path, row_data):
    with open(file_path, 'a', newline='') as csvfile:
        writer = csv.writer(csvfile)
        writer.writerow(row_data)

def make_csvfile(filename, instanceNames, solvalues, ublist, lblist, servedparcels, hoursList, kmsList, times):
    name = filename.rstrip('.txt')
    output = 'Res'+name+'.csv'
    row_data = ('Instance', 'Served', 'Sol Value', 'LB', 'UB', 'Time', 'Empty hours', 'Empty kms')
    csv_header(output, row_data)
    
    for i in range(len(instanceNames)):
        row_data = (instanceNames[i], servedparcels[i], solvalues[i], lblist[i], ublist[i], times[i], hoursList[i], kmsList[i])
        add_row_to_csv(output, row_data)
        

def read_files(directory):
    #make file list from directory
    file_list = os.listdir(directory)
    for i in file_list:
        print(i)
        solution, instanceNames, solvalues, ublist, lblist, arcs_solution, times = read_solution(directory + i)

        servedparcels = []
        hoursList = []
        kmsList = []

        for j in range(len(instanceNames)):
            print("instance: ", instanceNames[j])
            if len(solution[j]) < 1:
                hoursList.append(-1)
                kmsList.append(-1)
                servedparcels.append(-1)
                continue
            print("solution: ", solution[j])
            ordered_arcs = []
            print(instanceNames[j])
            served = check_served_parcels(solution[j], instanceNames[j])
            servedparcels.append(served)
            if instanceNames[j].split('-')[0] != 'sarp':
                ordered_arcs = fixArcsSingle(arcs_solution[j], instanceNames[j], solution[j])
            else:
                ordered_arcs = fixArcs(arcs_solution[j], instanceNames[j], solution[j])
            print("ordered_arcs: ", ordered_arcs)
            e_hours, e_kms = check_deadheading(solution[j], instanceNames[j], ordered_arcs)
            hoursList.append(e_hours)
            kmsList.append(e_kms)
            #input()
        
        print("before csv")
        make_csvfile(i, instanceNames, solvalues, ublist, lblist, servedparcels, hoursList, kmsList, times)
        print("after csv")
            
            
read_files("/home/ana/Documents/PHD/Research/Implementation/sarpfiles/pythonfilesandbapresults/logs/")

