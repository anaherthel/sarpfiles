import matplotlib.pyplot as plt
import os
import random
from math import floor
import statistics

def random_color():
    return (random.random(), random.random(), random.random())  # RGB tuple


def openFile(filename):
    # Read coordinates from file
    n = 0
    m = 0    
    x, y = [], []

    try:
        with open(filename, "r") as file:
            info = file.readline()
            n = int(info.split()[2])
            m = int(info.split()[3])
            for line in file:
                values = line.split()  # Split line into numbers
                xi = float(values[1])
                yi = float(values[2])
                x.append(xi)
                y.append(yi)
    except FileNotFoundError:
        print("Error: Could not open file!")
        exit(1)

    # Check if data is available
    if not x:
        print("Error: No coordinates found in file!")
        exit(1)
    
    return n, m, x, y

def makeOdPairs(n, m, x, y):
    odPairs = []
    
    for i in range(len(x)):
        if i < n:
            odPairs.append(((x[i], y[i]), (x[i+n], y[i+n])))
        elif i >= 2*n and i < 2*n+m+1:
            odPairs.append(((x[i], y[i]), (x[i+m], y[i+m])))
           
    
    return odPairs

def euc_dist(x1, y1, x2, y2):
    dist = ((x1 - x2)**2 + (y1 - y2)**2)**0.5
    return floor(dist*10)/10

import math

def degrees_to_radians(degrees):
    return degrees * math.pi / 180.0

def round_to_decimal_places(value, decimal_places):
    factor = 10 ** decimal_places
    return round(value * factor) / factor

def calc_man_km(Xs, Ys, Xf, Yf, I, J):

    lat1, lon1 = Xf[I], Yf[I]
    lat2, lon2 = Xs[J], Ys[J]

    KM_PER_DEGREE_LAT = 111.0  # Approximate conversion factor

    # Calculate absolute differences in latitude (converted to km)
    delta_lat = abs(lat2 - lat1) * KM_PER_DEGREE_LAT

    # Average latitude in radians
    avg_lat = degrees_to_radians((lat1 + lat2) / 2.0)

    # Calculate km per degree of longitude at the average latitude
    km_per_degree_lon = KM_PER_DEGREE_LAT * math.cos(avg_lat)

    # Calculate absolute difference in longitude (converted to km)
    delta_lon = abs(lon2 - lon1) * km_per_degree_lon

    # Total Manhattan distance in km
    dist = delta_lat + delta_lon

    return round_to_decimal_places(dist, 3)


def tripLengths(n, m, x, y, instType):
    trips = []
    dist_trip = 0
    for i in range(len(x)):
        if i < n:
            if instType != "sfsarp":
                dist_trip = euc_dist(x[i], y[i], x[i+n], y[i+n])
                
                if instType == "ghsarp":
                    dist_trip /= 20
            
            else:
                dist_trip = calc_man_km(x, y, x, y, i, i+n)
            trips.append(dist_trip)

            
        elif i >= 2*n and i < 2*n+m+1:
            if instType != "sfsarp":
                dist_trip = euc_dist(x[i], y[i], x[i+m], y[i+m])
                
                if instType == "ghsarp":
                    dist_trip /= 20
            
            else:
                dist_trip = calc_man_km(x, y, x, y, i, i+m)
                
            trips.append(dist_trip)
    
    meanTrips = statistics.mean(trips)
    
    return trips, meanTrips


l_files = os.listdir("./CoordsFiles")


for i in l_files:
    #print("i: ", i)
    fullPath = os.path.join("./CoordsFiles/", i)
    instName = i.split(".")[0]
    
    #print("Full Path: ", fullPath)
    print("Instance Name: ", instName)
    instType = instName.split("-")[0]

    n, m, x, y = openFile(fullPath)
    od_pairs = makeOdPairs(n, m, x, y)
    #print(x)
    #print(y)
    for i in range(len(od_pairs)):
        #print(od_pairs[i])
        x1, y1 = od_pairs[i][0]
        x2, y2 = od_pairs[i][1]
        if i < n:
            color = (1, 0, 0)
            idx = i
        elif i >= n and i < n+m+1:
            color = (0, 0, 1)
            idx = i + n
        #color = random_color()  # Assign a unique color to each pair
        plt.plot([x1, x2], [y1, y2], color=color, linestyle="-", linewidth=2)  
        mid_x, mid_y = (x1 + x2) / 2, (y1 + y2) / 2

        # Add label with OD pair number
        plt.text(mid_x, mid_y, str(idx), fontsize=12, color=color, fontweight="bold", ha="center", va="center")
    
    t, tM = tripLengths(n, m, x, y, instType)
    print("Trip lengths: ", t)
    print("Mean trip length: ", tM)
        
        
    # Plot the points
    #plt.plot(x, y, "bo-", label="Coordinates")  # 'b' = blue, 'o' = circle marker, '-' = line

    # Labels and title
    plt.xlabel("X-axis")
    plt.ylabel("Y-axis")
    plt.title("OD points from " + instName)
    plt.grid(True)
    #plt.legend()

    plotName = instName + ".png"
    plt.savefig(plotName) 
    # Show the plot
    #plt.show()

    ## Clear the plot
    #plt.clf()
    #plt.cla()
    #plt.close()
    ##input()
