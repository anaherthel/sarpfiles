import os
import csv

def list_files(folder_path):
    files = os.listdir(folder_path)
    files = [file for file in files if os.path.isfile(os.path.join(folder_path, file))]

    for i in range(len(files)):
        file = files[i].strip('.txt')
        files[i] = file
    
    return files

def order_files(files):
    list_tuples = []
    for i in files:
        names = i.split('-')
        n1 = int(names[1])
        n2 = int(names[2])
        tuple = (n1, n2, i)
        list_tuples.append(tuple)    
        
    list_tuples.sort()

    new_files = []
    for i in list_tuples:
        new_files.append(i[2])
        
    return new_files



def merge_csv_files(input_folder, output_file, filelist):
    # List to store rows from all CSV files
    merged_data = []

    # Loop through all files in the specified folder
    for f in filelist:
        file_name = input_folder + f + ".csv" 
                 
        # Open and read the CSV file
        with open(file_name, mode='r', newline='') as file:
            reader = csv.reader(file)
            
            # Read the first row from the CSV file
            row = next(reader, None)
            
            if row:  # Ensure the row is not empty
                merged_data.append(row)

    # Write all rows to the output CSV file
    with open(output_file, mode='w', newline='') as file:
        writer = csv.writer(file)
        
        # Write each row to the output CSV file
        writer.writerows(merged_data)

    print(f"Data from all CSV files has been merged into {output_file}.")
    

folder_path = "/home/ana/Documents/PHD/Research/Implementation/sarpfiles/C3/csarp" # folder path

files_csarp = list_files(folder_path)

files_csarp = order_files(files_csarp)

# Example usage
rootpath = "/home/ana/Documents/PHD/Research/Implementation/sarpfiles/"  # Replace with your input folder path
results1 = "src/Results/newBundlefip2/" #partial bundles (bundlep2)
results2 = "src/Results/mnodefipresults/" #fip multi
results3 = "src/Results/fipResults/" #fip single
results4 = "src/Results/bundleResults/" #bundle2
results5 = "src/Results/mbundlefipResults/" #bundle4

filename = rootpath + results1

output_file = "csarppartialbundlresults.csv"   # Replace with your desired output file path

merge_csv_files(filename, output_file, files_csarp)


print(files_csarp)

