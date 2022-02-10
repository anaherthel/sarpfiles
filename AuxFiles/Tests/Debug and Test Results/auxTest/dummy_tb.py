import sys
import argparse
import os.path
import re
import math 

# Map the columns of htop
USER = 0
PID = 1
CPU = 2
MEM = 3

cpu_threshold = 90

# Table
table = {
    "brunobruck": 0,
    "leonardo": 0,
    "rubao": 0,
    "joaomarcos": 0,
    "queiroga": 0,
    "walton": 0,
    "bia": 0,
    "root": 0
}

users = {
    "brunobruck": "Bruno",
    "leonardo": "Hugo",
    "rubao": "Rubao",
    "joaomarcos": "Joao",
    "queiroga": "Edu",
    "walton": "Walton",
    "bia": "Bia",
    "root": "Root"
}

# Deal with command line arguments
parser = argparse.ArgumentParser()

parser.add_argument('-l', '--cpu-threshold',
                    action = "store",
                    dest = "cpu_threshold",
                    required = False)
# Parse arguments
args = parser.parse_args()
if args.cpu_threshold is not None:
    cpu_threshold = int(args.cpu_threshold)


# Create the htop txt file
os.system('ps axo user:20,pid,pcpu,pmem,vsz,rss,tty,stat,start,time,comm f > process_temp.txt')

file = tuple(open('process_temp.txt', 'r'))
num_lines = len(file)
index_line = 0

# Find the line with the table header
found = False
while(index_line < num_lines):
    index_tag = file[index_line].find('USER')
    if index_tag != -1:
        found = True
        index_line += 1
        break
    index_line += 1

if not found:
    print('Error: Could not parse the htop file...')
    exit(0)

# Read each line of the table and decides if it should be considered as a process for that user based on the CPU consumption
while(index_line < num_lines):
    line = re.sub(' +', ' ', file[index_line]).strip().split(' ')
    index_line += 1 # Go to the next line

    #  Ignore process that are taking less than cpu_threshold% of the CPU
    if float(line[CPU]) < cpu_threshold:
        continue

    for key in table:
        if key in line[USER]:
            table[key] = table[key] + 1

# Display dummy table
print('Tabela dummy scheduler')
for key, value in table.items():
    print('[{}] {}'.format(value, users[key]))

# Remove the temporary file
os.system('rm process_temp.txt')