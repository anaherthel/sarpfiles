import re

def process_file(input_file):
    with open(input_file, 'r') as file:
        lines = file.readlines()
    
    instance_name_pattern = re.compile(r'NameOfInst: (.+)')
    solution_by_nodes_pattern = re.compile(r'Solution by nodes:')
    vehicle_pattern = re.compile(r'Vehicle \d+: ([\d -]+)')
    obj_val_pattern = re.compile(r'Obj Val: ([\d.]+)')

    instance_name = None
    routes = []
    obj_val = None
    write_routes = False
    
    for line in lines:
        instance_match = instance_name_pattern.match(line)
        if instance_match:
            if instance_name:
                # Salva as rotas no arquivo da instância anterior
                instance_name2 = "bundleSol/" + instance_name
                save_routes_to_file(instance_name2, routes, obj_val)
            
            # Novo arquivo da instância
            instance_path = instance_match.group(1)
            instance_name = instance_path.split('/')[-1]
            routes = []
            obj_val = None
            write_routes = False

        obj_val_match = obj_val_pattern.match(line)
        if obj_val_match:
            obj_val = obj_val_match.group(1)

        if solution_by_nodes_pattern.match(line):
            write_routes = True
            continue
        
        if write_routes:
            vehicle_match = vehicle_pattern.match(line)
            if vehicle_match:
                route_string = vehicle_match.group(1).strip(' -')
                route_elements = list(map(int, route_string.split(' - ')))
                routes.append(route_elements)
    
    # Salva a última instância
    if instance_name:
        instance_name2 = "bundleSol/" + instance_name
        save_routes_to_file(instance_name2, routes, obj_val)

def save_routes_to_file(filename, routes, obj_val):
    with open(filename, 'w') as file:
        file.write(f"{obj_val}\n")
        file.write(f"{len(routes)}\n")
        for route in routes:
            file.write(f"{len(route)} {' '.join(map(str, route))}\n")

# Exemplo de uso:
input_file = 'bundle2results/b2scsarp.txt'
process_file(input_file)