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
k = 1
V = n + 2*m + 2*k

nBundles = n * 3 * m + 2*k + n - 1

# Replace 'your_file.csv' with the path to your CSV file
file_path = 'matrix.csv'

matrix = d.read_mat(file_path)
# print(matrix[0])
cost = []
time = []
for i in range(len(matrix)):
    r1 = []
    r2 = []
    for j in range(len(matrix[i])):
        r1.append(round(matrix[i][j]*c1, 4))
        r2.append(round(matrix[i][j]/vmed, 4))
    cost.append(r1)
    time.append(r2)
    
# print("9-3:",time[9][3])

# print("11-1:",time[11][1])
# print("1-8:",time[1][8])

# earlier = d.read_earlier('earlier.txt')
# servicetimes = d.read_service('service.txt')
# profits = d.read_profits('profits.txt')

#
# earlier, profits, services = d.read_info('all_infonode.txt', n, V)

#if bap info to translate to node
earlier, services = d.read_infobap('infobap.txt')

print(earlier)
#print(profits)
print(services)


# d.printInfo(tw, profits)

#sol = d.read_node_sol('routetest.txt', k)
sol = d.read_baproute('baproute.txt')
# sol, values, durations = d.read_sol('solution.txt')

# d.print_solution(sol, values, durations)
ctimes, stimes, durations = d.calculate_times2(sol, time, n, m, earlier, services)  

print("solution:",sol)

print("ctimes:",ctimes)
print("stimes:",stimes)
print("durations:",durations)

totalVal = 0
# print("="*50)




# for i in sol:
#     print("Route", sol.index(i))
# #     # d.printTt(i, time)
# #     print("-"*50)
#     value = d.calcValue(i, cost, profits)
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




    
