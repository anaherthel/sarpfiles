import pandas as pd

def process_csv_files(csv_files):
    # Dicionário para armazenar os dados agrupados
    data = {
        'csarp': {},
        'ghsarp': {},
        'sfsarp': {}
    }

    # Função para inicializar as listas de valores para cada chave
    def initialize_keys(data, current_group, key, num_files):
        if key not in data[current_group]:
            data[current_group][key] = {}
            for i in range(1, num_files + 1):
                data[current_group][key][f'col2_file{i}'] = []
                data[current_group][key][f'col4_file{i}'] = []
                data[current_group][key][f'col5_file{i}'] = []
                data[current_group][key][f'col7_file{i}'] = []

    # Função para processar cada arquivo CSV
    def process_csv(file, file_index, num_files):
        df = pd.read_csv(file)
        
        current_group = None
        
        for index, row in df.iterrows():
            if row.iloc[0] in data.keys():
                current_group = row.iloc[0]
            elif current_group:
                instance_name = row.iloc[0]
                parts = instance_name.split('-')
                num_customers = parts[1]
                num_parcels = parts[2]
                key = f"{num_customers}-{num_parcels}"
                
                initialize_keys(data, current_group, key, num_files)
                
                data[current_group][key][f'col2_file{file_index}'].append(float(row.iloc[1]))
                data[current_group][key][f'col4_file{file_index}'].append(float(row.iloc[3]))
                data[current_group][key][f'col5_file{file_index}'].append(float(row.iloc[4]))
                data[current_group][key][f'col7_file{file_index}'].append(float(row.iloc[6]))
    
    num_files = len(csv_files)
    
    # Processar todos os arquivos CSV
    for file_index, csv_file in enumerate(csv_files, start=1):
        process_csv(csv_file, file_index, num_files)
    
    # Função para calcular a média e salvar os resultados
    def save_results(group, results):
        with open(f'{group}_results.txt', 'w') as file:
            sorted_keys = sorted(results.keys(), key=lambda x: (int(x.split('-')[0]), int(x.split('-')[1])))
            for key in sorted_keys:
                values = results[key]
                avg_values = []
                for i in range(1, num_files + 1):
                    avg_col2 = sum(values[f'col2_file{i}']) / len(values[f'col2_file{i}']) if values[f'col2_file{i}'] else 0
                    avg_col4 = sum(values[f'col4_file{i}']) / len(values[f'col4_file{i}']) if values[f'col4_file{i}'] else 0
                    avg_col5 = sum(values[f'col5_file{i}']) / len(values[f'col5_file{i}']) if values[f'col5_file{i}'] else 0
                    avg_col7 = sum(values[f'col7_file{i}']) / len(values[f'col7_file{i}']) if values[f'col7_file{i}'] else 0
                    avg_values.extend([avg_col2, avg_col4, avg_col5, avg_col7])
                num_customers, num_parcels = key.split('-')
                # if int(num_customers) >= 30 and int(num_parcels) >= 20:
                #     continue
                avg_values_str = " & ".join(f"{val:.2f}" for val in avg_values)
                file.write(f"{num_customers} & {num_parcels} & {avg_values_str} \\\\ \n")
    
    # Salvar os resultados para cada grupo
    for group, results in data.items():
        save_results(group, results)

# Exemplos de arquivos CSV a serem processados
csv_files = ['C2/fip/multiParcelLoad.csv', 'C2/bundlefip/multiInsertion.csv']

process_csv_files(csv_files)
