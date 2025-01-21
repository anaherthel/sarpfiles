import os

def rearrange_rows(file_path):
    with open(file_path, 'r') as file:
        lines = file.readlines()
    
    # Header remains unchanged
    header = lines[0].strip()
    data_lines = [line.strip() for line in lines[1:]]
    
    # Parse rows into groups based on the fourth column (index 3)
    groups = {3: [], -3: [], 1: [], -1: [], 0: [], -2: []}
    order = [3, -3, 1, -1, 0, -2]
    
    for line in data_lines:
        parts = line.split()
        group_key = int(parts[4])  # Column index 4 is the fifth column
        
        if group_key in groups:
        
            groups[group_key].append(parts)         
            
        else:
            print("not found")  # Default group if unexpected value

    # Create new row order: +3, -3, +1, -1, 0
    new_groups = {3: [], -3: [], 1: [], -1: [], 0: [], -2: []}
    counter = 0
    for group in order:
        if len(groups[group]) == 0:
            continue
        for listline in groups[group]:
            #listline = []
            #listline.append(str(counter))
            #for i in range(1, len(auxlist)):
            #    listline.append(i)
            #print(listline)
            newline = str(counter) + "\t"
            for i in range(1, len(listline)-1):
                if i == 3:
                    continue
                else:
                    newline += listline[i] + "\t"
            newline += listline[-1]
            #print(newline)
            new_groups[group].append(newline)
            #print(groups)
            #input()
            counter += 1
    
    #print(new_groups)
    #input()
    
    reordered_rows = (
        new_groups[3] + new_groups[-3] + new_groups[1] + new_groups[-1] + new_groups[0]
    )

    # Combine header and reordered rows
    reordered_content = [header] + reordered_rows
    return "\n".join(reordered_content)

def process_folder(folder_path1, folder_path2):
    output_folder = os.path.join(folder_path2, "processed_files")
    os.makedirs(output_folder, exist_ok=True)

    for filename in os.listdir(folder_path1):
        input_path = os.path.join(folder_path1, filename)
        if os.path.isfile(input_path) and filename.endswith(".txt"):
            # Rearrange rows and save to the new file
            new_content = rearrange_rows(input_path)
            #print(new_content)
            #input()
            output_path = os.path.join(output_folder, filename)
            with open(output_path, 'w') as output_file:
                output_file.write(new_content)
            print(f"Processed: {filename}")

# Example usage
folder_path = "/home/ana/Documents/PHD/Research/Implementation/sarpfiles/Instances_M/sf_data"  # Replace with your folder path
folder_path2 = "/home/ana/Documents/PHD/Research/Implementation/sarpfiles/Instances_M/sf_data2"  # Replace with your folder path
process_folder(folder_path, folder_path2)