import os
import math

def adjust_K(listReq, n, m, K):
    
    newReq = []


    #newK = max(math.ceil(n / 3), 3) + 2
    #newK = max(math.ceil(n / 5), 3) + 2
    newK = K
    
    if newK >= K:
        return listReq
    
    newReq.append(str(newK) + "\t" + "5" + "\t" + str(n) + "\t" + str(m) + "\n")
    
    for i in range(1, 2*n + 2*m + 1):
        newReq.append(listReq[i])
    for i in range(2*n + 2*m + 1, 2*n + 2*m + 1 + newK):
        print("Current request: ", listReq[i])
        newReq.append(listReq[i])
        
    
    return newReq

def writeNewFile(filename, listReq):
    with open(filename, 'w') as f:
        for item in listReq:
            f.write("%s" % item)
    print("New file created!")
    
def openFile(filename):
    # Read coordinates from file
    listReq = []
    with open(filename, "r") as file:
        line = file.readline()
        listReq.append(line)
        bits = line.split()
        K = int(bits[0])
        n = int(bits[2])
        m = int(bits[3])
        for line in file:
            listReq.append(line)
    return listReq, n, m, K

list_files = os.listdir('./C92/csarp')

for file in list_files:
    filename = os.path.join('./C72/csarp/', file)
    
    listReq, n, m, K = openFile(filename)
    newReq = adjust_K(listReq, n, m, K)
    
    newPath = './C92/csarp/' + file
    writeNewFile(newPath, newReq)