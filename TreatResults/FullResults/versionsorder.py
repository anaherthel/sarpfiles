
import csv
import os
import pandas as pd

def orderResults(resultslist):
    orderedresultslist = resultslist[:]
    
    orderedresultslist.sort(key = lambda x: x[0])
    
    print("Results list sorted:")
    print(orderedresultslist)
    
    return orderedresultslist

def read_csv_to_tuples(file_path):
    rows_as_tuples = []
    try:
        with open(file_path, mode='r') as csvfile:
            csvreader = csv.reader(csvfile)
            
            # Skip the header row (optional, depending on your CSV structure)
            header = next(csvreader)
            #print(f"Header: {header}")
            
            # Iterate over rows and convert each row to a tuple
            for row in csvreader:
                rows_as_tuples.append(tuple(row))

        #print(rows_as_tuples)
        return rows_as_tuples, header
    except FileNotFoundError:
        print(f"The file {file_path} does not exist.")
        return None
    except Exception as e:
        print(f"An error occurred: {e}")
        return None
    
    
def read_csv_to_lists(file_path):
    rows_as_lists = []
    try:
        with open(file_path, mode='r') as csvfile:
            csvreader = csv.reader(csvfile)
            
            # Skip the header row (optional, depending on your CSV structure)
            header = next(csvreader)
            #print(f"Header: {header}")
            
            # Iterate over rows and convert each row to a tuple
            for row in csvreader:
                rows_as_lists.append(list(row))

        #print(rows_as_tuples)
        return rows_as_lists, header
    
    except FileNotFoundError:
        print(f"The file {file_path} does not exist.")
        return None
    except Exception as e:
        print(f"An error occurred: {e}")
        return None
    
def purge_repeated(orderedresultslist):
    
    currentInstance = orderedresultslist[0][0]
    selectedInstances = []
    selectItem = None

    for i in range(1, len(orderedresultslist)):
        if orderedresultslist[i][0] == currentInstance: 
            selectItem = orderedresultslist[i]
        else:
            selectedInstances.append(selectItem)
            currentInstance = orderedresultslist[i][0]
            selectItem = orderedresultslist[i]
    
    selectedInstances.append(selectItem)
    
    return selectedInstances

def makeEmptycsv (filename, instList, header, currType):
    with open(filename, mode='w', newline='') as csvfile:
        csvwriter = csv.writer(csvfile)
        rows = []
        if len(instList) > 1:
            for i in range(len(instList)):
                instListType = instList[i][0].split("-")[0]

                if instListType == "sarp":
                    instListType = "csarp"
                if instListType == currType:
                    csvwriter.writerow(header)
                    for instance in instList[i]:
                        row = [instance]
                        for a in range(len(header) - 1):
                            row.append("")
                        rows.append(row)
        else:
            instListType = instList[0][0].split("-")[0]
            if instListType == "sarp":
                instListType = "csarp"
        
            csvwriter.writerow(header)
            for instance in instList[0]:
                row = [instance]
                for a in range(len(header) - 1):
                    row.append("")
                rows.append(row) 
        
        rows.sort()
        changed = False
        if currType == "csarp":
            if (int(rows[0][0].split("-")[4]) > 9 and int(rows[1][0].split("-")[4]) < 10) or (int(rows[0][0].split("-")[4]) < 10 and int(rows[1][0].split("-")[4]) > 9):
                for i in range(len(rows)):
                    if rows[i][0].split("-")[4] == "10":
                        rows[i][0] = rows[i][0].replace("10", "91")
                        #print("rows[i][0]: ", rows[i][0])
                        #input()
                changed = True
        
        rows.sort()
        
        if changed:
            for i in range(len(rows)):
                rows[i][0] = rows[i][0].replace("91", "10")
                #print("rows[i][0]: ", rows[i][0])
                #input()

        
        for i in rows:
            csvwriter.writerow(i)

    #print(f"{file_path} created.")


def write_to_csvNew (file_path, list_instances, header, exists):
    if exists == 1:
        mode = 'a'
        try:
            with open(file_path, mode, newline='') as csvfile:
                csvwriter = csv.writer(csvfile)
                
                # Write the data rows
                for instance in list_instances:
                    csvwriter.writerow(instance)
            
            print(f"Data written to {file_path}")
        except Exception as e:
            print(f"An error occurred: {e}")        
    else:
        mode = 'w'
        try:
            with open(file_path, mode, newline='') as csvfile:
                csvwriter = csv.writer(csvfile)
                
                # Write the header row
                csvwriter.writerow(header)
                
                # Write the data rows
                for instance in list_instances:
                    csvwriter.writerow(instance)
            
            print(f"Data written to {file_path}")
        except Exception as e:
            print(f"An error occurred: {e}")

list_sets = ['C6', 'C9', 'C10']
basePath = '/home/ana/Documents/PHD/Research/Implementation/sarpfiles'

list_instances = []

for s in list_sets:
    setList = []
    list_types = []
    for a in range(1, 3):
        partialsetList = []
        foldername = basePath + '/' + s + str(a)
        
        current_types = os.listdir(foldername)
        
        for t in current_types:
            if t not in list_types:
                list_types.append(t)
            

            current_list = os.listdir(foldername + '/' + t)
            auxList = []
            for f in current_list:
                instance = f.split('.')[0]
                
                auxList.append(instance)

            if partialsetList == []:
                partialsetList.append(auxList)
            else:
                current_type = auxList[0].split('-')[0]
                for i in range(len(partialsetList)):
                    if current_type == partialsetList[i][0].split('-')[0]:
                        for nfile in auxList:
                            partialsetList[i].append(nfile)
                        break
                    elif i == len(partialsetList) - 1:
                            partialsetList.append(auxList)
                            break
        
        #look for correct type and add instance names to the pertaining list
        
        if setList == []:
            for i in range(len(partialsetList)):
                setList.append(partialsetList[i])
        else:
            for i in range(len(partialsetList)):
                currType = partialsetList[i][0].split("-")[0]
                for j in range(len(setList)):
                    #print("trying type: ", setList[j][0].split("-")[0])
                    if currType == setList[j][0].split("-")[0]:
                        for nfile in partialsetList[i]:
                            setList[j].append(nfile)
                        break
        #print("partialsetList: ", partialsetList)
        #print("setList: ", setList)
        #input()
        
    list_instances.append(setList)
    #print("list_instances: ", list_instances)

print(list_instances)

listModes = ["node2MM", "nodefip2MM"]

list_final_files = []

for i in range(len(list_sets)):
    for j in range(len(listModes)):
        if list_sets[i] == 'C6':
            auxfile = "res_" + list_sets[i] + listModes[j] + "sfsarp.csv"
            list_final_files.append(auxfile)
            
        auxfile = "res_" + list_sets[i] + listModes[j] + "csarp.csv"
        list_final_files.append(auxfile)

#Read csv files M and L, divide instances names based on the list of instances, generate new csv files with the ordered instances

csv_folder = '/home/ana/Documents/PHD/Research/Implementation/sarpfiles/TreatResults/FullResults/CSVs'

output_folder = '/home/ana/Documents/PHD/Research/Implementation/sarpfiles/TreatResults/FullResults/CSVs/Ordered'

output_folder2 = '/home/ana/Documents/PHD/Research/Implementation/sarpfiles/TreatResults/FullResults/CSVs/Final'

l_files = os.listdir(csv_folder)

final_files = []

for i in range(len(list_final_files)):
    final_files.append([])
    header = ['Instance Name ', ' n', ' m', ' served', ' prof Customer', ' prof Parcel', ' costs', ' K', ' Sol Time', ' Sol Val', ' sol Stat', ' LB', ' UB', ' GAP', ' vrps', ' e(h)', ' et(%)', ' e(km)', ' ed(%) ']    
    currentFileName = list_final_files[i]
    currentFileName = currentFileName.split('.')[0]
    currentFileName = currentFileName.split('res_')[1]
    currSet = currentFileName.split('node')[0]
    currType = currentFileName.split('2MM')[1]
    setID = list_sets.index(currSet)
    instList = list_instances[setID]
    
    filename = output_folder2 + '/' + list_final_files[i]
    
    makeEmptycsv(filename, instList, header, currType)


for l in l_files:
    if len(l.split('.')) < 2: continue 

    if l.split('.')[1] == 'csv':
        file_path = csv_folder + '/' + l
        current_name = l.split('.')[0]
        current_name = current_name[:-6]
        if current_name not in final_files:
            final_files.append(current_name)
        
        
        list_inst_file, header = read_csv_to_tuples(file_path)


        ordered_list = orderResults(list_inst_file)

        outputFile = "res_" + current_name + ".csv"
        filename = output_folder + "/" + outputFile
        # Check if the file exists
        exists = 0
        if not os.path.exists(filename):
            # Create an empty CSV file
            exists = 0
            #with open(filename, "w") as file:
            #    file.write("column1,column2,column3\n")  # Optional header
            #print(f"{filename} created.")
            
        else:
            exists = 1
            print(f"{filename} already exists.")


        
        #file_name = l.split('.')[0]
        #file_path = csv_folder + '/' + l
        #list_instances, header = read_csv_to_tuples(file_path)
        
        #ordered_list = purge_repeated(ordered_list)
        
        write_to_csvNew(filename, ordered_list, header, exists)


l_files1 =  os.listdir(output_folder2)

for l in l_files1:
    #file1 = pd.read_csv(output_folder2 + "/" + l)  # Assume it has one column: "index"
    file1, header = read_csv_to_lists(output_folder2 + "/" + l)

    filename = l.split('.')[0]
    
    fName = l.split('.')[0]
    fName = fName.split('res_')[1]
    currType = fName.split('2MM')[1]
    if currType == "csarp":
        typeAdd = ""
    elif currType == "sfsarp":
        typeAdd = "sf"
    
    currMethod = fName.split('node')[1]
    currMethod = currMethod.split('2MM')[0]

    if currMethod == "":
        methodAdd = "node2MM"
    elif currMethod == "fip":
        methodAdd = "nodefip2MM"
    
    #file2 = pd.read_csv(output_folder + "/" + "res_" + methodAdd + typeAdd + ".csv")  # Assume it has one column: "index"
    file2, header = read_csv_to_lists(output_folder + "/" + "res_" + methodAdd + typeAdd + ".csv")
    
    print("File1: ", l)
    print("File2: ", "res_" + methodAdd + typeAdd + ".csv")
    for a in range(len(file1)):
        for b in range(len(file2)):
            if file1[a][0] == file2[b][0]:
                for i in range(1, len(file1[a])):
                    file1[a][i] = file2[b][i]
                break
        #print("file1 so far:", file1)
        #input()        
        write_to_csvNew(output_folder2 + "/" + "update_" + l, file1, header, 0)
    
    


    print("File updated successfully!")
    input()
