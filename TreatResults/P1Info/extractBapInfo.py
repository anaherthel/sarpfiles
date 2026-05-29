import re
import csv
import os

def parse_log(content):

    #with open(logfile, 'r') as file:
    #    content = file.read()

    # Split log into instances
    instance_blocks = re.split(r'command line:\s*(.+)', content)[1:]  # Will be in [name1, block1, name2, block2, ...]
    
    instNames = instance_blocks[::2]
    instancesBlocks = instance_blocks[1::2]
    
    for i in range(len(instNames)):
        instanceString = instNames[i]
        words = instanceString.split()
        instpath = words[6]
        instparts = instpath.split('/')
        instname = instparts[-1]
        instNames[i] = instname

    instances = list(zip(instNames, instancesBlocks))
    #print(instances[0][0], "-", instances[0][1][:4])  # Print first instance name and part of its block
    #input()

    results = []

    for instance_name, block in instances:
        #print("Instance name: ", instance_name)
        # Root node objective
        
        pattern = re.compile(r'bcRecRootLpVal\s+([-+]?\d+(?:\.\d+)?(?:[eE][-+]?\d+)?)')

        bcRecRootLpVal_values = pattern.findall(block)
        bcRecRootLpVal_values = [float(v) for v in bcRecRootLpVal_values]
            
        root_obj = bcRecRootLpVal_values[-1] if bcRecRootLpVal_values else None
                    
        # Best integer solution 
        pattern = r"Search is finished, global bounds\s*:\s*\[\s*([-+]?\d+(?:\.\d+)?(?:[eE][-+]?\d+)?)[\s,]+([-+]?\d+(?:\.\d+)?(?:[eE][-+]?\d+)?)\s*\].*?TIME\s*=\s*(\dh\d+m\d+s\d+t\s*=\s*\d+)"

        match = re.search(pattern, block)
        if match:
            lower_bound = float(match.group(1))
            upper_bound = float(match.group(2))
            time_string = match.group(3)
            
            #print("Lower Bound:", lower_bound)
            #print("Upper Bound:", upper_bound)
            #print("Time:", time_string)
        else:
            print("Pattern not found.")
            
        best_int = float(upper_bound) if match else None
        best_bound = float(lower_bound) if match else None
        
        # If no integer found, look for heuristic cutoff
        if best_int is None:
            cutoff_match = re.search(r'cutOffValue\s*=\s*([\d\.]+)', block)
            best_int = float(cutoff_match.group(1)) if cutoff_match else None
            
        #if best_bound is None or best_bound < best_int:
        #    db_values = [float(match) for match in re.findall(r"<DB=\s*(-?[\d.]+)>", block)]

        #    if not db_values:
        #        print("No dual bound values found.")
        #    else:
        #        last_db = db_values[-1]  # Typically the most relevant/final value
        #        #best_bound = max(db_values)  # For minimization
        #        best_bound = last_db
        
        time_pattern = r"(\d+)h(\d+)m(\d+)s(\d+)t"
        time_match = re.search(time_pattern, time_string)
        if time_match:
            hours = int(time_match.group(1))
            minutes = int(time_match.group(2))
            seconds = int(time_match.group(3))
            t_value = int(time_match.group(4))  # usually same as seconds
            total_seconds = hours * 3600 + minutes * 60 + seconds + (t_value/100)
        
        total_time = total_seconds if time_match else None
            
        # Root time
        root_time_match = re.search(r'bcTimeRoot.*?&\s*(\dh\d+m\d+s\d+t)', block)
        root_time_string = root_time_match.group(1) if root_time_match else None

        if root_time_string is not None:
            time_pattern = r"(\d+)h(\d+)m(\d+)s(\d+)t"
            time_match = re.search(time_pattern, root_time_string)
            
            if time_match:
                hours = int(time_match.group(1))
                minutes = int(time_match.group(2))
                seconds = int(time_match.group(3))
                t_value = int(time_match.group(4))  # usually same as seconds
                total_seconds = hours * 3600 + minutes * 60 + seconds + (t_value/100)
                
            root_time = total_seconds     
        else:
            root_time = None

        # Compute gaps
        root_gap = None
        total_gap = None

        if root_obj is not None and best_int is not None and best_int != 0:

            root_gap = 100.0 * abs(root_obj - best_int) / (max(1.0, abs(best_int)))

        if best_bound is not None and best_int is not None and best_int != 0:
            total_gap = 100.0 * abs(best_bound - best_int) / (max(1.0, abs(best_int)))

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
        
        #print("Results for instance: ", results)
        #print("-" * 40)
        #input()
        #print type of results         
        
    return results

def split_instances(full_log, delimiter="command line:"):
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
    csv_folder = "/home/ana/Documents/PHD/Research/Implementation/sarpfiles/TreatResults/P1Info/csv_filesBAP/"
    log_folder = "/home/ana/Documents/PHD/Research/Implementation/sarpfiles/TreatResults/P1Info/log_filesBAP/"
    log_files = [f for f in os.listdir(log_folder) if f.endswith('.txt')]
    for l in log_files:
        print("Current log file: ", l)
        #input()
        log_path = os.path.join(log_folder, l)
        l_name = l.split('.txt')[0]
        process_multiple_instances(log_path, output_csv=os.path.join(csv_folder, f"{l_name}_metricsBAP.csv"))
