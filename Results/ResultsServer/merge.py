import csv
import re
import os

def get_class(instance_name):
    match = re.search(r'-(\d+)-(\d+)-', instance_name)
    if match:
        n = int(match.group(1))
        m = int(match.group(2))
        if n + m > 15:
            return 'C2'
        else:
            return 'C1'
    return None

def merge_csvs_from_folders(folder1, folder2, output_path, target_class):
    output_rows = []
    header_written = False

    for filename in os.listdir(folder1):
        if filename.endswith(".csv"):
            instance_name = os.path.splitext(filename)[0]

            if get_class(instance_name) == target_class:
                file1_path = os.path.join(folder1, filename)
                file2_path = os.path.join(folder2, filename)

                if os.path.exists(file2_path):
                    with open(file1_path, mode='r') as file1, open(file2_path, mode='r') as file2:
                        csv1_reader = csv.reader(file1)
                        csv2_reader = csv.reader(file2)
                        
                        row1 = next(csv1_reader)
                        row2 = next(csv2_reader)
                        
                        if not header_written:
                            header = ["Instance", "SumCol2", "SumCol3", "SumCol4", "Col5_from_csv2", "Col6_from_csv2", "Col7_from_csv2", "Status"]
                            output_rows.append(header)
                            header_written = True

                        new_row = [instance_name]
                        new_row.extend([round(float(row1[i]) + float(row2[i]), 4) for i in range(1, 4)])  # sum columns 2, 3, and 4 and round to 4 decimal places
                        new_row.extend([round(float(value), 4) for value in row2[4:7]])  # copy columns 5, 6, and 7 from csv2 and round to 4 decimal places
                        if row2[7] == "Optimal":
                            new_row.append("Optimal")
                        else:
                            new_row.append("Feasible")

                        output_rows.append(new_row)

    # Sort rows by the instance name (first column)
    output_rows = sorted(output_rows, key=lambda x: x[0])

    with open(output_path, mode='w', newline='') as output_file:
        csv_writer = csv.writer(output_file)
        csv_writer.writerows(output_rows)

# Example usage:
merge_csvs_from_folders('bundleResults', 'mbundlefipResults', 'C2/bundlefip/multiInsertion.csv', 'C2')  # Change 'C1' to 'C2' for C2 class
