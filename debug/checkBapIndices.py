#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Fri May 24 11:56:14 2024

@author: ana
"""

n = 20
m = 30
k = 6

# in a route:
print("Bap indices:")
for i in range(n + 2*m + 1):
    if i == 0:
        print("S:\t"+str(i))

    elif i < n + 1:
        if i == 1:
            print("C: ", end = '\t')        
        print(i, end = '\t')
    elif i < n + m + 1: 
        if i == n + 1:
            print("\nP: ", end = '\t')        
        print(i, end = '\t')
    else:
        if i == n + m + 1:
            print("\nD: ", end = '')        
        print(i, end = '\t')
        
print("\nBundle indices:")       
for i in range(n + 2*m + k):
    if i < n:
        if i == 0:
            print("C: ", end = '\t') 
        print(i, end = '\t')        
    elif i < n + m:
        if i == n:
            print("\nP: ", end = '\t')        
        print(i, end = '\t')
    elif i < n + m: 
        if i == n + m:
            print("\nD: ", end = '\t')        
        print(i, end = '\t')
    else:
        if i == n + 2*m:         
            print("\nS:", end = '\t')
        print(i, end = '\t')
            
print("\nBap solution with bundle indices:") 
routes = [
    '0(0,0,0,0) -> 32(0.00103659,1,0,0) -> 62(0.0850626,1,0,1) -> 15(14.25,0,1,0) -> 14(16.6833,-1,2,-1) -> 48(16.8503,0,0,-1) -> 78(16.9338,0,0,0) -> 81(17.0171,0,0,0)',
    '0(0,0,0,0) -> 41(0.000804878,1,0,0) -> 71(0.0848967,1,0,1) -> 12(12.8667,0,1,0) -> 18(14.25,-1,2,-1) -> 49(14.4228,0,0,-1) -> 79(14.5066,0,0,0) -> 81(14.5899,0,0,0)',
    '0(0,0,0,0) -> 35(0.000804878,1,0,0) -> 65(0.0848967,1,0,1) -> 1(14.1833,0,1,0) -> 17(14.4167,-1,2,-1) -> 40(14.5847,0,0,-1) -> 70(14.6687,0,0,0) -> 16(15.3167,-1,1,-1) -> 38(15.4837,0,0,-1) -> 68(15.5672,0,0,0) -> 81(15.6505,0,0,0)',
    '0(0,0,0,0) -> 31(0.000704878,1,0,0) -> 61(0.0844797,1,0,1) -> 20(9.41667,0,1,0) -> 10(14.25,-1,2,-1) -> 30(14.4226,0,0,-1) -> 60(14.5066,0,0,0) -> 81(14.59,0,0,0)',
    '0(0,0,0,0) -> 50(0.000617073,1,0,0) -> 80(0.0844407,1,0,1) -> 9(9.9,0,1,0) -> 3(12.4333,-1,2,-1) -> 45(12.6008,0,0,-1) -> 75(12.6847,0,0,0) -> 25(12.7681,1,0,0) -> 6(14.3167,0,20,-1) -> 55(14.4838,0,0,0) -> 39(14.5673,1,0,0) -> 69(14.651,1,0,1) -> 19(14.7667,0,1,0) -> 43(14.9338,1,0,0) -> 8(16.0333,0,20,-1) -> 73(16.2004,0,0,0) -> 44(16.2838,1,0,0) -> 74(16.3672,1,0,1) -> 5(16.6833,0,1,0) -> 81(16.8501,0,1,0)',
    '0(0,0,0,0) -> 21(0.000590244,1,0,0) -> 2(12.4667,0,20,-1) -> 51(12.6337,0,0,0) -> 13(12.8667,-1,1,-1) -> 37(13.0337,0,0,-1) -> 67(13.1172,0,0,0) -> 11(13.4667,-1,1,-1) -> 47(13.6338,0,0,-1) -> 77(13.7174,0,0,0) -> 29(13.8008,1,0,0) -> 59(13.8842,1,0,1) -> 4(14.2,0,1,0) -> 46(14.3675,1,0,0) -> 76(14.4509,1,0,1) -> 7(14.5833,0,1,0) -> 81(14.7508,0,1,0)'    
    ]
idroutes = []
for route in routes:
    route = route.split(' -> ')
    idroute = []
    for i in route:
        words = i.split('(')
        idroute.append(int(words[0]))
    idroutes.append(idroute)

routesnewid = []
routesrepr = []
for route in idroutes:
    newr = []
    rrep = []
    for i in route:
        if i == 0:
            newr.append('S')
            rrep.append('S')
            continue
        elif i < n + 1: 
            rrep.append('c')
        elif i < n + m + 1:
            rrep.append('P')
        elif i < n + 2*m + 1:
            rrep.append('D')
        else:
            newr.append('f')
            rrep.append('f')
            continue
        newr.append(i - 1)
    routesnewid.append(newr)
    routesrepr.append(rrep)

print('\n')
for a,newr in enumerate(routesnewid):
    rrep = routesrepr[a]
    for i in newr:
        print(i, end='-')
    print('\n')
    for i in rrep:
        print(i, end='-')
    print('\n')            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            

