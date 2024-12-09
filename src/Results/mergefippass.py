import os
import pandas as pd

# Directory containing the CSV files
directory = '/home/ana/Documents/PHD/Research/Implementation/sarpfiles/src/Results/fippassResults'

# Create an empty DataFrame to store all rows
combined_df = pd.DataFrame()

# Initialize a dictionary to categorize files by class
file_classes = {
    'ghsarp': [],
    'csarp': [],
    'sfsarp': []
}
file_list = os.listdir(directory)

file_list.sort()

# Iterate over the files and categorize them based on the prefix
for filename in file_list:
    if filename.endswith('.csv'):
        if filename.startswith('ghsarp'):
            file_classes['ghsarp'].append(filename)
        elif filename.startswith('sarp'):
            file_classes['csarp'].append(filename)
        elif filename.startswith('sfsarp'):
            file_classes['sfsarp'].append(filename)

print(file_classes)  # Debugging
# Function to add rows from files of a specific class to the combined DataFrame
def append_class_rows(file_list, class_name):
    global combined_df
    for file in file_list:
        file_path = os.path.join(directory, file)
        df = pd.read_csv(file_path, header=None)  # Assuming no header in the individual CSV files
        combined_df = pd.concat([combined_df, df])
    # Add a separator row after processing each class
    if not combined_df.empty:
        separator_row = pd.DataFrame([["--- Separator for class: " + class_name]])
        combined_df = pd.concat([combined_df, separator_row])

# Append rows for each class and add separators
for class_name, files in file_classes.items():
    append_class_rows(files, class_name)

# Write the final DataFrame to a new CSV file
combined_df.to_csv('fippassmerge.csv', index=False, header=False)
