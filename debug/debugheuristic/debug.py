#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed Nov 22 11:07:26 2023

@author: ana
"""

import debug2 as d

vmed = 41
a1 = 3.24
a2 = 1.03
b1 = 2.74
b2 = 0.83
c1 = 0.46
maxT = 10
# service = 0.083
service = 5/60
service = round(service, 4)
n = 5
m = 5
k = 2
V = n + 2*m + 2*k

nBundles = n * 3 * m + 2*k + n - 1

# Replace 'your_file.csv' with the path to your CSV file
file_path = 'matrix.csv'

matrix = d.read_mat(file_path)
#print(matrix)
costMat = []
timeMat = []
for i in range(len(matrix)):
    r1 = []
    r2 = []
    for j in range(len(matrix[i])):
        r1.append(round(matrix[i][j]*c1, 4))
        r2.append(round(matrix[i][j]/vmed, 4))
    costMat.append(r1)
    timeMat.append(r2)
    
# print("9-3:",time[9][3])

# print("11-1:",time[11][1])
# print("1-8:",time[1][8])

# earlier = d.read_earlier('earlier.txt')
# servicetimes = d.read_service('service.txt')
# profits = d.read_profits('profits.txt')

#
earlier, profits = d.read_info('info.txt')

servtime = round(5/60, 4)

#if bap info to translate to node
#earlier, services = d.read_infobap('info.txt')

print(matrix)

#print(earlier)
#print(profits)
#print(services)


d.printInfo(earlier, profits)

sol, values, durations = d.read_sol('solution.txt')

# d.print_solution(sol, values, durations)
stimes, durations_list, violations = d.calculate_times_of_Heu(sol, timeMat, n, m, earlier, servtime)  

print("solution:",sol)

print("stimes:",stimes)

for i in range(len(durations_list)):
    print("D1:",durations_list[i], " - D2:",durations[i])
    if abs(durations_list[i] - durations[i]) > 0.001:
        print("Difference in durations")  
        
for i in range(len(violations)):
    for j in range(len(violations[i])):
        if violations[i][j]:
            print("Violation in route", i, "in node", sol[i][j])

totalVal = 0

mipsol = d.convertHeuToMIP(sol, n)

print("MIP SOLUTION:", mipsol)



# print("="*50)

# for i in sol:
#     print("Route", sol.index(i))
# #     # d.printTt(i, time)
# #     print("-"*50)
#     value = d.calcValue(i, costMat, profits)
#     print(value)
#     totalVal += value
    
# #     cVal = d.compareValues(value, values[sol.index(i)])
# #     print("FEASIBLE VALUE\n" if cVal else "INFEASIBLE VALUE\n")
#     # for j in range(len(i)-1):
#     #     print('j:',j, 'j+1:', j+1)
#     #     print()
#     ctimes = d.calc_times(i, n, m, earlier, time, services)

#     # d.printTimes(ctimes, stimes, cdur)
    
# #     print("="*50)
    
# #     feasRoute = d.compareTimes(ctimes, stimes, cdur, i, n, maxT)
# #     print("FEASIBLE TIME" if feasRoute else "INFEASIBLE TIME")
# #     print("="*50)

# print("TOTAL VALUE:", totalVal)
# # print("="*50)




    
