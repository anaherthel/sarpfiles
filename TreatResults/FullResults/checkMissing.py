import csv
import os

def checkMissing(list_inst, csvlist):
    missing = []
    for inst in list_inst:
        found = False
        for csv in csvlist:
            if inst == csv[0]:
                found = True
                break
        if not found:
            missing.append(inst)
            
    print("Missing instances:")
    if len(missing) == 0:
        print("None")
    else:
        for m in missing:
            print(m, end = ', ')
        print()
    
    #print("End of check")

def list_files_in_folder(folder_path):
    """
    List all files in a specific folder.

    Parameters:
        folder_path (str): The path of the folder to scan.

    Returns:
        list: A list of file names in the folder.
    """
    try:
        # Get a list of all files and folders in the directory
        items = os.listdir(folder_path)

        # Filter out directories, keeping only files
        files = [item.strip('.txt') for item in items if os.path.isfile(os.path.join(folder_path, item))]
        return files
    except FileNotFoundError:
        print(f"Error: The folder '{folder_path}' does not exist.")
        return []
    except Exception as e:
        print(f"An unexpected error occurred: {e}")
        return []

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
    

file_name = 'node2MMsf_dataM'
inst_path = '/home/ana/Documents/PHD/Research/Implementation/sarpfiles/Instances_M2/sf_data'
list_instances = list_files_in_folder(inst_path)

list_instances.sort()
#print(list_instances)

#input()

file_path = file_name + '.csv'  # Replace with the path to your CSV file
list_csv, header = read_csv_to_tuples(file_path)

#print("List of csv:")
#for i in list_csv:
#    print(i[0])

checkMissing(list_instances, list_csv)
#ordered_list = orderResults(list_instances)
#ordered_list = purge_repeated(ordered_list)

#write_to_csv(file_name + '_ordered2.csv', ordered_list, header)
