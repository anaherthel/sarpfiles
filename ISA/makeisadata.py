import os
import sys
import math
import csv
import random
import numpy as np
from scipy.stats import entropy
from scipy.spatial.distance import pdist
from scipy.spatial import ConvexHull
from sklearn.manifold import MDS


def read_instance(instance_path):
    coord_list = []
    tw_list = []
    with open(instance_path, 'r') as f:
        line = f.readline()
        k = int(line.split()[0])
        n = int(line.split()[2])
        m = int(line.split()[3])
        for line in f:
            words = line.split()
            x = float(words[1])
            y = float(words[2])
            coord_list.append((x, y))
            etw = float(words[4])
            ltw = float(words[5])
            tw_list.append((etw, ltw))
    
    return n, m, k, coord_list, tw_list

def calc_Euc2 (Xs, Ys, Xf, Yf, I, J):
    x1 = Xf[I]
    y1 = Yf[I]
    x2 = Xs[J]
    y2 = Ys[J]
    dist = ((x1 - x2)**2 + (y1 - y2)**2)**0.5
    return math.floor(dist*10)/10

def round_to_decimal_places(value: float, decimal_places: int) -> float:
    factor = 10 ** decimal_places
    return round(value * factor) / factor

def degrees_to_radians(degrees: float) -> float:
    return math.radians(degrees)

def calc_ManKm(Xs, Ys, Xf, Yf, I, J):
    lat1 = Xf[I]
    lon1 = Yf[I]
    lat2 = Xs[J]
    lon2 = Ys[J]

    km_per_degree_lat = 111.0

    delta_lat = abs(lat2 - lat1) * km_per_degree_lat
    
    avg_lat = degrees_to_radians((lat1 + lat2) / 2.0)
    km_per_degree_lon = km_per_degree_lat * math.cos(avg_lat)
    delta_lon = abs(lon2 - lon1) * km_per_degree_lon
    
    dist = delta_lat + delta_lon
    return round_to_decimal_places(dist, 3)
    

#def calc_Matrix(instance, instance_path):
#    inst_class = instance.split('-')[0]
#    n, m, K, coord_list, tw_list = read_instance(instance_path)
#    if inst_class == 'sfsarp' or inst_class == 'ghsarp':
#        k = 1
#    else:
#        k = K
    
#    N = n + 2*m + k
    
#    Xs = []
#    Ys = []
#    Xf = []
#    Yf = []
#    for i in range(n):
#        Xs.append(coord_list[i][0])
#        Ys.append(coord_list[i][1])
#        Xf.append(coord_list[i+n][0])
#        Yf.append(coord_list[i+n][1])
#    for i in range(2*n, 2*n+2*m+k):
#        Xs.append(coord_list[i][0])
#        Ys.append(coord_list[i][1])
#        Xf.append(coord_list[i][0])
#        Yf.append(coord_list[i][1])

#    matrix = []

    
#    scalingFactor = 50
#    for i in range(N):
#        row = []
#        for j in range(N):
#            if i == j:
#                row.append(0)
#                continue
#            if inst_class == 'sarp' or inst_class == 'ghsarp':
#                dist = calc_Euc2(Xs, Ys, Xf, Yf, i, j)
#                if inst_class == 'ghsarp':
#                    dist = dist/scalingFactor
                
#                row.append(dist)
                
#            elif inst_class == 'sfsarp':
#                row.append(calc_ManKm(Xs, Ys, Xf, Yf, i, j))
        
#        if inst_class == 'sarp':
#            for d in range(k):
#                row.append(0)
        
        
#        matrix.append(row)          
    
#    #fixing depots
    
#    if inst_class == 'sfsarp' or inst_class == 'ghsarp':
#        dep_row = []
#        for i in range(N):
#            dep_row.append(matrix[-1][i])
#        for a in range(K-1):
#            matrix.append(dep_row)
#        for i in range(N):            
#            for a in range(K-1):
#                matrix[i].append(matrix[i][-1])
#        for i in range(n + 2*m + 1):            
#            for a in range(K):
#                matrix[i].append(0)
#        #for a in range(K-1):
#        #    matrix[n + 2*m].append(0)
    
#        for i in range(n + 2*m + 1, n + 2*m + K):
#            while len(matrix[i]) < n + 2*m + 2*K:
#                matrix[i].append(0)
#        #for b in range(n + 2*m + 1, n + 2*m + K):
#        #    print("b: ", b)
#        #b = n + 2*m + 2
#        #print("row: ", matrix[b])
#        #matrix[i].pop(-1)
#        #for h in range(K):
#        #    matrix[b].append(0)
#        #    print("row: ", matrix[b])

#        for i in range(K):
#            row = []            
#            for j in range(N + 2*K-1):
#                row.append(0)
#            matrix.append(row)
#    else:
#        for i in range(N):
#            for a in range(k):
#                matrix[i].append(0)
#        for a in range(k):
#            matrix.append([0]*(N+k))
    
#    return matrix

def calc_Matrix_Full(instance, instance_path):
    inst_class = instance.split('-')[0]
    n, m, K, coord_list, tw_list = read_instance(instance_path)
    
    if inst_class == 'sfsarp' or inst_class == 'ghsarp':
        k = 1
    else:
        k = K
    
    N = 2*n + 2*m + k
    fullsize = 2*n + 2*m + 2*K
    
    Xs = []
    Ys = []
    Xf = []
    Yf = []
    for i in range(N):
        Xs.append(coord_list[i][0])
        Ys.append(coord_list[i][1])
        Xf.append(coord_list[i][0])
        Yf.append(coord_list[i][1])

    matrix = []
    
    scalingFactor = 50
    for i in range(N):
        row = []
        for j in range(N):
            if inst_class == 'sarp' or inst_class == 'ghsarp':
                dist = calc_Euc2(Xs, Ys, Xf, Yf, i, j)
                       
                if inst_class == 'ghsarp':
                    dist = dist/scalingFactor
                
                row.append(dist)
                
            elif inst_class == 'sfsarp':
                row.append(calc_ManKm(Xs, Ys, Xf, Yf, i, j))
          
        matrix.append(row)          

    #fixing depots
    
    if inst_class == 'sfsarp' or inst_class == 'ghsarp':
        dep_row = []
        for i in range(N):
            dep_row.append(matrix[-1][i])
        for a in range(K-1):
            matrix.append(dep_row)
        for i in range(N):            
            for a in range(K-1):
                matrix[i].append(matrix[i][-1])
        
        for i in range(2*n + 2*m + k):
            for j in range(K):
                matrix[i].append(0)
    
        for i in range(2*n + 2*m + k + 1, 2*n + 2*m + K):
            while len(matrix[i]) < fullsize:
                matrix[i].append(0)

        
        for i in range(K):
            row = []
            for j in range(fullsize):
                row.append(0)
            matrix.append(row)
                      
    else:
        for i in range(len(matrix)):
            for j in range(K):
                matrix[i].append(0)

        for i in range(K):
            row = []
            for j in range(fullsize):
                row.append(0)
            matrix.append(row)

    return matrix

def calc_PassTrips(matrix, n):
    passTrips = []
    for i in range(n):
        dist = matrix[i][i+n]
        passTrips.append(dist)
    
    return passTrips

def calc_ParcTrips(matrix, n, m):
    parcTrips = []
    for i in range(2*n, m + 2*n):
        dist = matrix[i][i+m]
        parcTrips.append(dist)
    
    return parcTrips

def calc_PassToParcTrips(matrix, n, m, twlist): #delivery of every passenger to pickup of every parcel
    passTimeList = []
    parcTimeList = []

    for i in range(n):
        passTimeList.append((twlist[i][0], twlist[i][1]))
    
    for i in range(2*n, m + 2*n):
        parcTimeList.append((twlist[i][0], twlist[i][1]))
    
    serviceTime = 5/60
    
    passToParcTrips = []
    
    for i in range(n, 2*n):
        for j in range(2*n, 2*n + m):
            if i == j:
                continue
            latestTime = passTimeList[i-n][0] + serviceTime + matrix[i][j]

            if latestTime > passTimeList[j-2*n][1]:
                continue
            dist = matrix[i][j]
            if dist > 0:
                passToParcTrips.append(dist)
                            
    return passToParcTrips

def calc_PassToPassTrips(matrix, n, twlist): #delivery of every passenger to pickup of every passenger, considering time windows
    passTimeList = []
    
    for i in range(n):
        passTimeList.append((twlist[i][0], twlist[i][1]))
        
    serviceTime = 5/60
    
    passToPassTrips = []
    
    for i in range(n, 2*n):
        for j in range(n):
            if i == j or i == j + n:
                continue
            latestTime = passTimeList[i-n][0] + serviceTime + matrix[i][j]
            if latestTime > passTimeList[j][1]:
                continue
            dist = matrix[i][j]
            if dist > 0:
                passToPassTrips.append(dist)
    
    return passToPassTrips

def calc_DepotToPassTrips(matrix, n, m, k): #delivery of every passenger to pickup of every passenger, considering time windows         
   
    depotToPassTrips = []
    depotToParcTrips = []
    
    for i in range(2*n + 2*m, 2*n + 2*m + k):
        for j in range(n):
            dist = matrix[i][j]
            if dist > 0:
                depotToPassTrips.append(dist)
        for j in range(2*n, 2*n + m):
            dist = matrix[i][j]
            if dist > 0:
                depotToParcTrips.append(dist)
    
    return depotToPassTrips, depotToParcTrips

def calc_TWLength(twlist, n, m):
    
    twLength = 0
    for i in range(2*n + 2*m):
        twLength += twlist[i][1] - twlist[i][0]
    
    avgTWLength = twLength / (2*n + 2*m)
    
    return avgTWLength


def calculate_entropy_minutes_verbose(start_times_minutes, start_min=540, end_min=1140, bin_size=15):

    time_range = end_min - start_min
    bins = time_range // bin_size

    # Histogram the data
    filtered_times = [t for t in start_times_minutes if start_min <= t < end_min]
    
    counts, _ = np.histogram(filtered_times, bins=bins, range=(start_min, end_min))

    # Convert to probability distribution
    probabilities = counts / counts.sum()
    probabilities = probabilities[probabilities > 0]

    # Calculate entropy
    entropy_value = entropy(probabilities, base=2)
    max_entropy = np.log2(bins)

    normalized = entropy_value / max_entropy
    
    pct_of_max = normalized *100

    # Display interpretation
    #print(f"Shannon Entropy: {entropy_value:.4f} bits")
    #print(f"Max Entropy for {bins} bins: {max_entropy:.4f} bits")
    #print(f"Entropy is {pct_of_max:.2f}% of the theoretical maximum")

    return entropy_value, max_entropy, normalized, pct_of_max

def calc_fullKM(solution, matrix):
    full_km = 0
    for i in range(len(solution)):
        for j in range(1, len(solution[i])):
            full_km += matrix[solution[i][j-1]][solution[i][j]]
    return full_km


def calculate_2d_entropy(coords, x_range, y_range, grid_size=20):

    coords = np.array(coords)
    x = coords[:, 0]
    y = coords[:, 1]

    ## Auto-range if not provided
    #if x_range is None:
    #    x_range = (x.min(), x.max())
    #if y_range is None:
    #    y_range = (y.min(), y.max())

    # 2D histogram (grid_size x grid_size)
    counts, _, _ = np.histogram2d(x, y, bins=grid_size, range=[x_range, y_range])

    # Flatten and normalize
    flat_counts = counts.flatten()
    total = flat_counts.sum()
    if total == 0:
        print("No data points fell into histogram bins.")
        return 0

    probabilities = flat_counts / total
    probabilities = probabilities[probabilities > 0]

    # Entropy
    entropy_value = entropy(probabilities, base=2)
    max_entropy = np.log2(grid_size ** 2)
    normalized_entropy = entropy_value / max_entropy

    return normalized_entropy

def calculate_spatial_entropy_latlon(coords, lat_range, lon_range, grid_size=40):

    coords = np.array(coords)
    lat = coords[:, 0]
    lon = coords[:, 1]

    ## Auto-range based on data bounds
    #lat_range = (lat.min(), lat.max())
    #lon_range = (lon.min(), lon.max())

    # Create 2D histogram
    counts, _, _ = np.histogram2d(lat, lon, bins=grid_size, range=[lat_range, lon_range])

    flat_counts = counts.flatten()
    total = flat_counts.sum()
    if total == 0:
        print("No data points fell into histogram bins.")
        return 0

    probabilities = flat_counts / total
    probabilities = probabilities[probabilities > 0]

    # Shannon Entropy
    entropy_value = entropy(probabilities, base=2)
    max_entropy = np.log2(grid_size ** 2)
    normalized_entropy = entropy_value / max_entropy


    return normalized_entropy


def make_csvfile(filename):

    if os.path.exists(filename):
        print(f"File {filename} already exists. Please choose a different name.")
        return
    # Create a CSV file with the same name as the input file
    with open(filename, 'w', newline='') as csvfile:
        writer = csv.writer(csvfile)
        
        # Write the header
        header = ['Instances', 'feature_V', 'feature_avgReqPerVehicle', 'feature_avgReqTripLen', 'feature_avgPassTripLen',
                  'feature_avgParcTripLen', 'feature_avgPassToParcTrip', 'feature_avgPassToPassTrip','feature_avgDepotToReqDist',
                  'feature_entropyPass', 'feature_entropyParc', 'feature_entropyReq', 
                  'feature_depotIsolationRatio', 'feature_requestSpreadRadius', 'feature_convexHullArea',
                  'feature_avgTWLength', 'feature_TWEntropy', 'algo_standardMIP', 'algo_FIP', 'algo_BCP']
        writer.writerow(header)
           
def addTo_csvfile(filename, list_features, list_algos):
    
    # sort by instance name
    #list_features.sort(key=lambda x: x[0])
    #list_algos.sort(key=lambda x: x[0])
    ##print("list_algos", list_algos)
    #print("list_features", list_features)
    # Create a CSV file with the same name as the input file
    with open(filename, 'a', newline='') as csvfile:
        writer = csv.writer(csvfile)        
        # Write the data
        for i in range(len(list_features)):
            algo_i = -1
            for j in range(len(list_algos)):
                if list_algos[j][0] == list_features[i][0]:
                    algo_i = j
                    break
            else:
                print("Algo not found for instance: ", list_features[i][0])
                input("Press Enter to continue...")
                
            row = [list_features[i][0], list_features[i][1], list_features[i][2], list_features[i][3], list_features[i][4], 
                   list_features[i][5], list_features[i][6], list_features[i][7], list_features[i][8], 
                   list_features[i][9], list_features[i][10], list_features[i][11], list_features[i][12], list_features[i][13],
                   list_features[i][14], list_features[i][15], list_features[i][16],
                   list_algos[algo_i][1], list_algos[algo_i][2], list_algos[algo_i][3]]
            writer.writerow(row)
            
def calcStdDev(llist):
    mean = sum(llist)/len(llist)
    variance = sum((x - mean) ** 2 for x in llist) / len(llist)
    stddev = math.sqrt(variance)
    return stddev

def calcLoHiReqTripLen(matrix, n, m, avgLen):
    nHigher = 0
    nLower = 0
    for i in range(n):
        dist = matrix[i][i+n]
        if dist < avgLen:
            nLower += 1
        elif dist > avgLen:
            nHigher += 1
    for i in range(m):
        dist = matrix[2*n+i][2*n + i + m]
        if dist < avgLen:
            nLower += 1
        elif dist > avgLen:
            nHigher += 1

    loReqTripLen = nLower / (n + m)
    hiReqTripLen = nHigher / (n + m)


    return loReqTripLen, hiReqTripLen

def compute_spread_radius(dist_matrix):
    # Mask or remove zeros from the diagonal (i.e., distance to self)
    # so we don't consider them when finding the max
    masked = np.copy(dist_matrix)
    np.fill_diagonal(masked, -np.inf)

    spread_radius = np.max(masked)
    return spread_radius

def compute_convex_hull_area(dist_matrix):

    # Ensure it's a proper NumPy array
    dist_matrix = np.array(dist_matrix)

    # Multidimensional scaling to get 2D coordinates from distance matrix
    mds = MDS(n_components=2, dissimilarity='precomputed', random_state=0)
    coords_2d = mds.fit_transform(dist_matrix)

    # Compute the convex hull area
    hull = ConvexHull(coords_2d)
    area = hull.volume  # 2D case: "volume" is actually area

    return area

def read_algos_file(filename):
    if not os.path.exists(filename):
        print(f"File {filename} does not exist.")
        return None

    with open(filename, 'r') as csvfile:
        reader = csv.reader(csvfile)
        header = next(reader)  # Read the header
        data = []
        for row in reader:
            data.append(row)
    
    return data

def make_list_features(directory):
    #make file list from directory
    file_list = os.listdir(directory)
    list_all_features = []
       
    for i in file_list:
        instName = i.split('.')[0]
        instClass = i.split('-')[0]
        
        print("instName: ", instName)
        n, m, K, coord_list, tw_list = read_instance(directory + i)
        
        coords = np.array(coord_list)
        x = coords[:, 0]
        y = coords[:, 1]

        x_range = (x.min(), x.max())
        y_range = (y.min(), y.max())
                
        pass_PU_coords = []
        for j in range(n):
            pass_PU_coords.append(coord_list[j])
            
        pass_DL_coords = []
        for j in range(n, 2*n):
            pass_DL_coords.append(coord_list[j])      
            
        parc_PU_coords = []
        for j in range(2*n, 2*n + m):
            parc_PU_coords.append(coord_list[j])
               
        parc_DL_coords = []
        for j in range(2*n + m, 2*n + 2*m):
            parc_DL_coords.append(coord_list[j])
                
        dist_matrix = calc_Matrix_Full(i, directory + i)
        
        #print("n", n)
        #print("m", m)
        #print("K", K)
        #print("coord_list", coord_list)
        #print("tw_list", tw_list)
        #print("dist_matrix")
        #for r in range(len(dist_matrix)):
        #    if r == 0:
        #        print("\t")
        #    print(str(r), end="\t")
        #print()
        #for r in range(len(dist_matrix)):
        #    print(r, end ="\t")
        #    for e in range(len(dist_matrix[r])):
        #        print(round(dist_matrix[r][e],2), end="\t")
        #    print()
        #input()
        #Create csv for matrix
        #with open('distance_matrix.csv', mode='w', newline='') as file:
        #    writer = csv.writer(file)

        #    # Write header
        #    header = [''] + [f'P{j}' for j in range(len(dist_matrix))]
        #    writer.writerow(header)

        #    # Write rows with row indices
        #    for i in range(len(dist_matrix)):
        #        writer.writerow([f'P{i}'] + dist_matrix[i])     
        
        #print("created distance_matrix.csv")
        #input("Press Enter to continue...")   

        list_features = []
        list_features.append(instName)
        V = n + m
        list_features.append(V)
        
        vehicleRatio = (n + m) / K #avg number of requests per vehicle
        list_features.append(round(vehicleRatio, 5))
        
        passTrips = calc_PassTrips(dist_matrix, n)
        parcTrips = calc_ParcTrips(dist_matrix, n, m)
        
        allTrips = sum(passTrips) + sum(parcTrips)
        #print("allTrips")
        #print(round(allTrips, 2))
        avgTripLen = allTrips / (n + m)
        list_features.append(round(avgTripLen, 5))
        
        full_trip_list = []
        for i in range(n):
            full_trip_list.append(passTrips[i])
        for i in range(m):
            full_trip_list.append(parcTrips[i])
        
        #print(list_features)
        
        #stdDev = calcStdDev(full_trip_list)
        #list_features.append(round(stdDev, 5))
        
        #loReqTrip, hiReqTrip = calcLoHiReqTripLen(dist_matrix, n, m, avgTripLen)
        
        #list_features.append(round(loReqTrip, 5))
        #list_features.append(round(hiReqTrip, 5))
        
        avgPassTripLen = sum(passTrips) / n
        list_features.append(round(avgPassTripLen, 5))
        
        #stdDevPass = calcStdDev(passTrips)
        #list_features.append(round(stdDevPass, 5))
        
        avgParcTripLen = sum(parcTrips) / m
        list_features.append(round(avgParcTripLen, 5))
        
        #stdDevParc = calcStdDev(parcTrips)
        #list_features.append(round(stdDevParc, 5))
        
        passParc = calc_PassToParcTrips(dist_matrix, n, m, tw_list)
        
        avgPassToParcTrip = sum(passParc) / len(passParc)
        list_features.append(round(avgPassToParcTrip, 5))        
        
        passPass = calc_PassToPassTrips(dist_matrix, n, tw_list)
        
        avgPassToPassTrip = sum(passPass) / len(passPass)
        list_features.append(round(avgPassToPassTrip, 5))
        
        #stdDevPassToPass = calcStdDev(passPass)
        #list_features.append(round(stdDevPassToPass, 5))
                
        #stdDevPassToParc = calcStdDev(passParc)
        #list_features.append(round(stdDevPassToParc, 5))
        
        depotPass, depotParc = calc_DepotToPassTrips(dist_matrix, n, m, K)
        
        meanDepotDist = np.mean(depotPass + depotParc)
        
        #avgDepotToPassDist = sum(depotPass) / len(depotPass)
        #list_features.append(round(avgDepotToPassDist, 5))
        
        #stdDevDepotToPassDist = calcStdDev(depotPass)
        #list_features.append(round(stdDevDepotToPassDist, 5))
        
        #avgDepotToParcDist = sum(depotParc) / len(depotParc)
        #list_features.append(round(avgDepotToParcDist, 5))
        
        #stdDevDepotToParcDist = calcStdDev(depotParc)
        #list_features.append(round(stdDevDepotToParcDist, 5))
        
        allreqs = sum(depotPass) + sum(depotParc)
        avgDepotToReqDist = allreqs / (len(depotPass) + len(depotParc))
        list_features.append(round(avgDepotToReqDist, 5))
        
        #stdDevDepotToReqDist = calcStdDev(depotPass + depotParc)
        #list_features.append(round(stdDevDepotToReqDist, 5))
        
        
        #entropy Ratio PU and DL
        if (instClass == 'sarp' or instClass == 'ghsarp'):
            normalizedPassPU = calculate_2d_entropy(np.array(pass_PU_coords), x_range, y_range, grid_size=20)
            normalizedPassDL = calculate_2d_entropy(np.array(pass_DL_coords), x_range, y_range, grid_size=20)
            normalizedParcPU = calculate_2d_entropy(np.array(parc_PU_coords), x_range, y_range, grid_size=20)
            normalizedParcDL = calculate_2d_entropy(np.array(parc_DL_coords), x_range, y_range, grid_size=20)
            normalizedAllPU = calculate_2d_entropy(np.array(pass_PU_coords + parc_PU_coords), x_range, y_range, grid_size=20)
            normalizedAllDL = calculate_2d_entropy(np.array(pass_DL_coords + parc_DL_coords), x_range, y_range, grid_size=20)
        else:
            normalizedPassPU = calculate_spatial_entropy_latlon(np.array(pass_PU_coords), x_range, y_range, grid_size=40)
            normalizedPassDL = calculate_spatial_entropy_latlon(np.array(pass_DL_coords), x_range, y_range, grid_size=40)
            normalizedParcPU = calculate_spatial_entropy_latlon(np.array(parc_PU_coords), x_range, y_range, grid_size=40)
            normalizedParcDL = calculate_spatial_entropy_latlon(np.array(parc_DL_coords), x_range, y_range, grid_size=40)
            normalizedAllPU = calculate_spatial_entropy_latlon(np.array(pass_PU_coords + parc_PU_coords), x_range, y_range, grid_size=40)
            normalizedAllDL = calculate_spatial_entropy_latlon(np.array(pass_DL_coords + parc_DL_coords), x_range, y_range, grid_size=40)
            
        passEntropyRatio = normalizedPassPU / normalizedPassDL
        list_features.append(round(passEntropyRatio, 5))

        parcEntropyRatio = normalizedParcPU / normalizedParcDL
        list_features.append(round(parcEntropyRatio, 5))
        
        allEntropyRatio = normalizedAllPU / normalizedAllDL
        list_features.append(round(allEntropyRatio, 5))
        
        #print("normalizedPassRatio", passEntropyRatio)
        
        #print("normalizedParcRatio", parcEntropyRatio)
        #print("normalizedAllRatio", allEntropyRatio)
        #input("Press Enter to continue...")

        #Depot Isolation/Centrality
        
        pickups = pass_PU_coords + parc_PU_coords
        #print("pickups", pickups)
        
        dist_PUs = []
            
        PUIndices = []
        for i in range(n):
            PUIndices.append(i)
        for i in range(2*n, 2*n + m):
            PUIndices.append(i)        
            
        for i in range(len(PUIndices)):
            row = []
            for j in range(len(PUIndices)):
                dist = dist_matrix[i][j]
                row.append(dist)
            dist_PUs.append(row)
        
        #print("dist_PUs")
        #for r in range(len(dist_PUs)):
        #    for e in range(len(dist_PUs[r])):
        #        print(round(dist_PUs[r][e],2), end="\t")
        #    print()
            
        #input()

        mean_intra_dist = np.mean(dist_PUs)

        
        isolationRatio = meanDepotDist / mean_intra_dist
        
        #print("isolationRatio", isolationRatio)
        list_features.append(round(isolationRatio, 5))
        
        #Request Spread Radius
        spread_radius = compute_spread_radius(dist_matrix)
        #print(f"Spread Radius: {spread_radius:.4f} units")
        
        list_features.append(round(spread_radius, 5))
                
        #Convex Hull Area/Bounding Box
        area_hull = compute_convex_hull_area(dist_matrix)
        #print(f"Convex Hull Area: {area_hull:.4f} units^2")
        
        list_features.append(round(area_hull, 5))

        avgTWlength = calc_TWLength(tw_list, n, m)
        
        
        list_features.append(round(avgTWlength, 5))
        
        # Example usage:
        start_times_minutes = []
        
        for i in range(2*n + 2*m):
            start_times_minutes.append(tw_list[i][0])
            
        entropy_value, max_entropy, normalized, pct_of_max = calculate_entropy_minutes_verbose(start_times_minutes, bin_size=15)
        #print(f"Shannon Entropy (15-min bins):", entropy_value)
        #print(f"Shannon Entropy Normalized(15-min bins):", normalized)
        
        list_features.append(round(normalized, 5))

        list_all_features.append(list_features)

        
    return list_all_features

make_csvfile("SARPmetadata.csv")

dir_csv = '/home/ana/Documents/PHD/Research/Implementation/sarpfiles/ISA/MetaData/'
dir_inst = '/home/ana/Documents/PHD/Research/Implementation/sarpfiles/ISA/AllInst/'

list_algos = read_algos_file('algorithms2.csv')
#print()

#print("list_algos", list_algos)
list_features = make_list_features(dir_inst)

addTo_csvfile("SARPmetadata.csv", list_features, list_algos)

