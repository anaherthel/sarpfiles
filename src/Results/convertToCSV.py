import os
import re
import csv

# Função para processar o texto e extrair informações
def process_instance(text):
    info = {}
    # Usando regex para capturar as informações necessárias
    info['Instance Name'] = re.search(r'Instance Name: (\S+)', text).group(1)
    info['Total Time'] = float(re.search(r'Total Time: ([\d.]+)', text).group(1))
    info['UB'] = float(re.search(r'UB: ([\d.]+)', text).group(1))
    info['LB'] = float(re.search(r'LB: ([\d.]+)', text).group(1))
    info['Served Parcels'] = int(re.search(r'Served parcels: (\d+)', text).group(1))
    info['Idle Time'] = float(re.search(r'Total idle time: ([\d.]+)', text).group(1))
    info['Idle Distance'] = float(re.search(r'Total idle distance: ([\d.]+)', text).group(1))
    info['Status'] = re.search(r'Sol status: (\w+)', text).group(1)

    # Formatando os valores flutuantes para 4 casas decimais
    info['Total Time'] = round(info['Total Time'], 4)
    info['UB'] = round(info['UB'], 4)
    info['LB'] = round(info['LB'], 4)
    info['Idle Time'] = round(info['Idle Time'], 4)
    info['Idle Distance'] = round(info['Idle Distance'], 4)
    
    return info

# Função para ler o arquivo de texto e criar os CSVs
def create_csvs_from_txt(input_file, output_folder):
    with open(input_file, 'r') as file:
        content = file.read()
        
    # Separar o conteúdo em blocos por instância
    instances = content.split("NameOfInst")
    
    for instance in instances:
        if "Instance Name:" in instance:
            try:
                info = process_instance(instance)
                # Definir o caminho do arquivo CSV
                csv_filename = os.path.join(output_folder, f"{info['Instance Name']}.csv")
                # Escrever os dados no CSV
                with open(csv_filename, 'w', newline='') as csvfile:
                    fieldnames = ['Instance Name', 'Total Time', 'UB', 'LB', 'Served Parcels', 'Idle Time', 'Idle Distance', 'Status']
                    writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
                    # writer.writeheader()
                    writer.writerow(info)
            except AttributeError:
                # Ignorar instâncias incompletas
                continue
            except Exception as e:
                print(f"Erro ao processar a instância: {e}")

# Exemplo de uso
input_file = 'bundle2results/b2ssfsarp.txt'  # Substitua pelo caminho do seu arquivo de entrada
output_folder = 'bundleResults'  # Substitua pelo caminho da pasta de saída

# Criar a pasta de saída se não existir
os.makedirs(output_folder, exist_ok=True)

create_csvs_from_txt(input_file, output_folder)
