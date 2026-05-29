import re
import csv
import os

def parse_log(content):

    #with open(logfile, 'r') as file:
    #    content = file.read()

    # Split log into instances
    instance_blocks = re.split(r'Instance Name:\s*(.+)', content)[1:]  # Will be in [name1, block1, name2, block2, ...]
    instances = list(zip(instance_blocks[::2], instance_blocks[1::2]))

    
    #print(instances[0][0], "-", instances[0][1][:4])  # Print first instance name and part of its block
    #input()

    results = []

    for instance_name, block in instances:
        # Root node objective
        
        matches = re.findall(r'^\s*0\s+0\s+([\d\.]+)', block, re.MULTILINE)
        root_obj = float(matches[-1]) if matches else None
        
        # Best bound
        best_bound_matches = re.findall(r'Best Bound\s*=\s*([\d\.]+)', block)
        if not best_bound_matches:
            # Alternatively, find in node lines
            node_lines = re.findall(r'\s+\d+\s+\d+\s+[\d\.]+\s+\d+\s+([\d\.]+)', block)
            if node_lines:
                best_bound = float(node_lines[-1])
            else:
                best_bound = None
        else:
            best_bound = float(best_bound_matches[-1])

        # Best integer solution (via CPLEX)
        best_int_match = re.search(r'\*\s*0\s*0\s*integral\s*\d*\s*([\d\.]+)', block)
        best_int = float(best_int_match.group(1)) if best_int_match else None

        # If no integer found, look for heuristic cutoff
        if best_int is None:
            cutoff_match = re.search(r'Cutoff Value:\s*([\d\.]+)', block)
            best_int = float(cutoff_match.group(1)) if cutoff_match else None

        # Root time
        root_time_match = re.search(r'Root node processing.*?Real time\s*=\s*([\d\.]+)', block, re.DOTALL)
        root_time = float(root_time_match.group(1)) if root_time_match else None

        # Total time
        total_time_match = re.search(r'Total \(root\+branch&cut\)\s*=\s*([\d\.]+)', block)
        total_time = float(total_time_match.group(1)) if total_time_match else None

        # Compute gaps
        root_gap = None
        total_gap = None

        if root_obj is not None and best_int is not None and best_int != 0:
            root_gap = 100.0 * abs(root_obj - best_int) / abs(best_int)

        if best_bound is not None and best_int is not None and best_int != 0:
            total_gap = 100.0 * abs(best_bound - best_int) / abs(best_int)

        results = {
            'Instance': instance_name.strip(),
            'Root Objective': f"{root_obj:.4f}" if root_obj is not None else '',
            'Best Bound': f"{best_bound:.4f}" if best_bound is not None else '',
            'Best Integer': f"{best_int:.4f}" if best_int is not None else '',
            'Root Gap (%)': f"{root_gap:.4f}" if root_gap is not None else '',
            'Root Time (s)': f"{root_time:.2f}" if root_time is not None else '',
            'Total Gap (%)': f"{total_gap:.4f}" if total_gap is not None else '',
            'Total Time (s)': f"{total_time:.2f}" if total_time is not None else ''
        }
        #print("Results for instance: ", results[-1])
        #print("-" * 40)
        #input()
        #print type of results         
        
    return results

def split_instances(full_log, delimiter="Instance Name:"):
    # Keep delimiter as part of each instance by prepending back
    parts = full_log.split(delimiter)
    instances = []
    for part in parts[1:]:  # Skip first (empty) split
        instances.append(delimiter + part)
        
    #print(f"Found {len(instances)} instances.")

    return instances

def process_multiple_instances(log_filename, output_csv):

    with open(log_filename, "r") as f:
        full_log = f.read()

    instances = split_instances(full_log)

    all_metrics = []

    for i, instance_log in enumerate(instances):
        metrics = parse_log(instance_log)
        #print(f"Instance: {metrics['Instance']}")
        #print(f"Root Gap (%): {metrics['Root Gap (%)']}")
        #print(f"Root Time (s): {metrics['Root Time (s)']}")
        #print(f"Total Gap (%): {metrics['Total Gap (%)']}")
        #print(f"Total Time (s): {metrics['Total Time (s)']}")
        #print("-" * 40)
        #input("Press Enter to continue...")
        
        # Extract instance name
        #metrics["Instance"] = i + 1
        all_metrics.append(metrics)

    print(f"Extracted {len(all_metrics)} instances to {output_csv}")

    ## Write to CSV
    with open(output_csv, 'w', newline='') as csvfile:
        fieldnames = ['Instance','Root Objective', 'Best Bound', 'Best Integer', 'Root Gap (%)', 'Root Time (s)', 'Total Gap (%)', 'Total Time (s)']
        writer = csv.DictWriter(csvfile, fieldnames=fieldnames)

        writer.writeheader()
        for row in all_metrics:
            writer.writerow(row)
    
    ## Write to CSV
    #with open(output_csv, mode='w', newline='') as csv_file:
    #    fieldnames = ["Instance", "Root Objective", "Best Bound", "Best Integer", "Root Gap (%)", "Root Gap Time (sec)", "Total Gap (%)", "Total Time (sec)"]
    #    writer = csv.DictWriter(csv_file, fieldnames=fieldnames)
    #    writer.writeheader()
    #    writer.writerows(all_metrics)

    #print(f"Extracted {len(all_metrics)} instances to {output_csv}")

# Example usage
if __name__ == "__main__":
    csv_folder = "/home/ana/Documents/PHD/Research/Implementation/sarpfiles/TreatResults/P1Info/csv_files/"
    log_folder = "/home/ana/Documents/PHD/Research/Implementation/sarpfiles/TreatResults/P1Info/log_files/"
    log_files = [f for f in os.listdir(log_folder) if f.endswith('.txt')]
    for l in log_files:
        print("Current log file: ", l)
        input()
        log_path = os.path.join(log_folder, l)
        l_name = l.split('.txt')[0]
        process_multiple_instances(log_path, output_csv=os.path.join(csv_folder, f"{l_name}_metrics.csv"))
    #process_multiple_instances("cplex_log.txt")
