
import csv

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
            print(f"Header: {header}")
            
            # Iterate over rows and convert each row to a tuple
            for row in csvreader:
                rows_as_tuples.append(tuple(row))

        print(rows_as_tuples)
        return rows_as_tuples, header

    except FileNotFoundError:
        print(f"The file {file_path} does not exist.")
        return None
    except Exception as e:
        print(f"An error occurred: {e}")
        return None


def write_to_csv (file_path, list_instances, header):
    try:
        with open(file_path, mode='w', newline='') as csvfile:
            csvwriter = csv.writer(csvfile)
            
            # Write the header row
            csvwriter.writerow(header)
            
            # Write the data rows
            for instance in list_instances:
                csvwriter.writerow(instance)
        
        print(f"Data written to {file_path}")
    except Exception as e:
        print(f"An error occurred: {e}")

file_path = 'node2MMcsarpM.csv'  # Replace with the path to your CSV file
list_instances, header = read_csv_to_tuples(file_path)

ordered_list = orderResults(list_instances) 

write_to_csv('node2MMcsarpM_ordered.csv', ordered_list, header)
