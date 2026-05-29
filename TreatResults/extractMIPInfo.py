import re
import csv
import os

def parse_cplex_instance(log_text):
    root_gap = None
    root_gap_time = None
    total_gap = None
    total_time = None

    # Root gap from Best Bound and Best Integer
    root_bound_match = re.search(r"\b0\s+0\s+([\d\.]+)\s+\d+\s+Best Integer\s+Best Bound.*", log_text)
    best_int_match = re.search(r"\*\s*0\s*0\s*integral\s*\d*\s*([\d\.]+)", log_text)
    bound_match = re.search(r"\b0\s+0\s+([\d\.]+)\s+\d+\s+.*\s+([\d\.]+)", log_text)

    if best_int_match and bound_match:
        best_int = float(best_int_match.group(1))
        best_bound = float(bound_match.group(2))
        if best_bound != 0:
            root_gap = round(100 * (best_bound - best_int) / best_bound, 4)

    # Root node time
    root_time_match = re.search(r"Root node processing.*?=\s*([\d\.]+)\s*sec", log_text)
    if root_time_match:
        root_gap_time = float(root_time_match.group(1))

    # Total time
    total_time_match = re.search(r"Total \(root\+branch&cut\)\s*=\s*([\d\.]+)\s*sec", log_text)
    if total_time_match:
        total_time = float(total_time_match.group(1))

    # Total gap
    total_gap_match = re.search(r"\*\s*0\s*0\s*integral\s*\d*\s*([\d\.]+)\s*([\d\.]+).*?(\d+\.\d+)%", log_text)
    if total_gap_match:
        total_gap = float(total_gap_match.group(3))
    else:
        if "0.00%" in log_text:
            total_gap = 0.0

    return {
        "Root Gap (%)": root_gap if root_gap is not None else "",
        "Root Gap Time (sec)": root_gap_time if root_gap_time is not None else "",
        "Total Gap (%)": total_gap if total_gap is not None else "",
        "Total Time (sec)": total_time if total_time is not None else ""
    }

def split_instances(full_log, delimiter="Instance Name:"):
    # Keep delimiter as part of each instance by prepending back
    parts = full_log.split(delimiter)
    instances = []
    for part in parts[1:]:  # Skip first (empty) split
        instances.append(delimiter + part)
    print(f"Found {len(instances)} instances in the log.")
    print("Instances:", instances)
    return instances

def process_multiple_instances(log_filename, output_csv="cplex_all_metrics.csv"):
    with open(log_filename, "r") as f:
        full_log = f.read()

    instances = split_instances(full_log)
    all_metrics = []

    for i, instance_log in enumerate(instances):
        metrics = parse_cplex_instance(instance_log)
        metrics["Instance"] = i + 1
        all_metrics.append(metrics)

    # Write to CSV
    with open(output_csv, mode='w', newline='') as csv_file:
        fieldnames = ["Instance", "Root Gap (%)", "Root Gap Time (sec)", "Total Gap (%)", "Total Time (sec)"]
        writer = csv.DictWriter(csv_file, fieldnames=fieldnames)
        writer.writeheader()
        writer.writerows(all_metrics)

    print(f"Extracted {len(all_metrics)} instances to {output_csv}")

# Example usage
if __name__ == "__main__":
    log_folder = "/home/ana/Documents/PHD/Research/Implementation/sarpfiles/TreatResults/P1Info/log_files/"
    log_files = [f for f in os.listdir(log_folder)]
    for l in log_files:
        print("Processing file:", l)
        #process_multiple_instances("cplex_log.txt")
