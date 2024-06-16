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
n = 8
m = 7
k = 2
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

earlier, profits, services = d.read_info('all_infonode.txt', n, V)

print(earlier)
print(profits)
print(services)

# print("2-7:",time[2][7])
# print("calc:", (tw[7] + service) - (tw[2] - time[21][2]))
# input()

# d.printInfo(tw, profits)

sol = d.read_node_sol('routetest.txt', k)
# sol, values, durations = d.read_sol('solution.txt')

# d.print_solution(sol, values, durations)

# sol = [[20, 4, 9, 3, 8, 1, 6, 20], [21, 2, 7], [22, 0, 5, 22]]
# sol = [[28, 5, 12, 3, 10, 28], 
#  [29, 17, 1, 8, 24, 15, 6, 13, 22, 20, 0, 7, 27, 29], 
#  [30, 19, 4, 11, 26, 18, 0, 7, 25, 30]]

# d.print_solution(sol, values, durations)
print("solution:",sol)
# sol[1] = [29, 17, 1, 8, 24, 20, 6, 13, 27, 18, 0, 7, 25, 29]
# sol[2] = [30, 2, 9, 4, 11, 30]

# sol = [[28, 5, 12, 3, 10, 28], [29, 17, 1, 8, 24, 20, 6, 13, 27, 18, 0, 7, 25, 29], [30, 2, 9, 4, 11, 30]]
totalVal = 0
# print("="*50)

print(earlier[4])
t = earlier[4] + services[4] + time[4][17] + services[17]
print('end time of bundle 95:', t)
print('end time of 95 again:', 10.93473+ 0.58431) 

print('trip from 95 to 66:', time[17][3] )
t += time[17][3]
print('arrival time at 3:',t)
t2 = earlier[3]
print('start time of bundle 66:', t2) 

if (t <= t2):
    print('ok')
else:
    print('invalid', t2-t)


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




    
