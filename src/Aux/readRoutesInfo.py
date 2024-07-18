import re
import csv
import os

def process_file(input_file, output_csv):
    with open(input_file, 'r') as file:
        lines = file.readlines()
    
    instance_name_pattern = re.compile(r'NameOfInst: (.+)')
    solve_time_pattern = re.compile(r'Solve Time: ([\d.]+)')
    ub_pattern = re.compile(r' UB: ([\d.]+)')
    lb_pattern = re.compile(r' LB: ([\d.]+)')
    served_parcels_pattern = re.compile(r'Served parcels: (\d+)')
    idle_time_pattern = re.compile(r'Total idle time: ([\d.]+)')
    idle_distance_pattern = re.compile(r'Total idle distance: ([\d.]+)')
    sol_status_pattern = re.compile(r'Sol status: (.+)')
    
    instance_name = None
    solve_time = None
    ub = None
    lb = None
    served_parcels = None
    idle_time = None
    idle_distance = None
    sol_status = None
    
    file_exists = os.path.isfile(output_csv)

    with open(output_csv, 'a', newline='') as csvfile:
        csvwriter = csv.writer(csvfile)
        
        if not file_exists:
            csvwriter.writerow([
                "Instance Name", "Solve Time", "UB", "LB", "Served Parcels", 
                "Total Idle Time", "Total Idle Distance", "Sol Status"
            ])
        
        for line in lines:
            instance_match = instance_name_pattern.match(line)
            if instance_match:
                if instance_name:
                    # Escreve os dados da instância anterior no CSV
                    csvwriter.writerow([
                        instance_name,
                        round(solve_time, 4) if solve_time is not None else None,
                        round(ub, 4) if ub is not None else None,
                        round(lb, 4) if lb is not None else None,
                        served_parcels,
                        round(idle_time, 4) if idle_time is not None else None,
                        round(idle_distance, 4) if idle_distance is not None else None,
                        sol_status
                    ])
                
                # Novo arquivo da instância
                instance_path = instance_match.group(1)
                instance_name = instance_path.split('/')[-1]
                solve_time = ub = lb = served_parcels = idle_time = idle_distance = sol_status = None

            if instance_name:
                solve_time_match = solve_time_pattern.match(line)
                if solve_time_match:
                    solve_time = float(solve_time_match.group(1))
                
                ub_match = ub_pattern.match(line)
                if ub_match:
                    ub = float(ub_match.group(1))
                
                lb_match = lb_pattern.match(line)
                if lb_match:
                    lb = float(lb_match.group(1))
                
                served_parcels_match = served_parcels_pattern.match(line)
                if served_parcels_match:
                    served_parcels = int(served_parcels_match.group(1))
                
                idle_time_match = idle_time_pattern.match(line)
                if idle_time_match:
                    idle_time = float(idle_time_match.group(1))
                
                idle_distance_match = idle_distance_pattern.match(line)
                if idle_distance_match:
                    idle_distance = float(idle_distance_match.group(1))
                
                sol_status_match = sol_status_pattern.match(line)
                if sol_status_match:
                    sol_status = sol_status_match.group(1).strip()
        
        # Escreve os dados da última instância no CSV
        if instance_name:
            csvwriter.writerow([
                instance_name,
                round(solve_time, 4) if solve_time is not None else None,
                round(ub, 4) if ub is not None else None,
                round(lb, 4) if lb is not None else None,
                served_parcels,
                round(idle_time, 4) if idle_time is not None else None,
                round(idle_distance, 4) if idle_distance is not None else None,
                sol_status
            ])

# Exemplo de uso:
input_file = 'bundle2results/b2ssfsarp.txt'
output_csv = 'bundleResults/resultsC2.csv'
process_file(input_file, output_csv)
