

def read_inst(filename):
    with open(filename) as f:
        lines = []
        for line in f:
            line = line.split('\t')
            for i in range(len(line)):
                line[i] = line[i].strip()
            lines.append(line)
            lines[-1][-1] = lines[-1][-1].strip('\n')   
            #print(lines)
            #input() 
    print(lines)
    return lines

def change_deliveryTW(lines):
    for i in range(len(lines)):
        if i == 0:
            continue
        if lines[i][3] == '-1':
            tw1 = int(lines[i][4])
            print(tw1)
            #tw2 = tw1 + 10
            tw2 = tw1 + 60
            print(tw2)
            lines[i][5] = str(tw2)
            print(lines[i][5])
            print(lines[i])
        #elif lines[i][3] == '1':
        #    tw1 = int(lines[i][4])
        #    print(tw1)
        #    tw2 = tw1 + 10
        #    print(tw2)
        #    lines[i][5] = str(tw2)
        #    print(lines[i][5])
        #    print(lines[i])            
            #input()
    print(lines)
    return lines

def write_inst(filename, lines):
    with open(filename, 'w') as f:
        for line in lines:
            for i in range(len(line)):
                print(line[i])
                if i == len(line) - 1:
                    f.write(line[i] + '\n')
                else:
                    f.write(line[i] + '\t')
            #f.write(line)


def change_deliveryTW2(lines):
    for i in range(len(lines)):
        if i == 0:
            continue
        if lines[i][3] == '-1':
            tw1 = int(lines[i][4])
            print(tw1)
            tw1 -= 30
            tw2 = tw1 + 30
            print(tw2)
            lines[i][4] = str(tw1)
            lines[i][5] = str(tw2)
            #print(lines[i][5])
            #print(lines[i])
        elif lines[i][3] == '1':
            tw1 = int(lines[i][4])
            #print(tw1)
            tw2 = tw1 + 30
            #print(tw2)
            lines[i][5] = str(tw2)
            #print(lines[i][5])
            #print(lines[i])            
            #input()
    print(lines)
    return lines

def write_inst(filename, lines):
    with open(filename, 'w') as f:
        for line in lines:
            for i in range(len(line)):
                print(line[i])
                if i == len(line) - 1:
                    f.write(line[i] + '\n')
                else:
                    f.write(line[i] + '\t')
            #f.write(line)


filename = 'sarp-30-30-B-3.txt'
outputName = 'sarp-30-30-B-5.txt'
lines = read_inst(filename)
#lines = change_deliveryTW2(lines)
lines = change_deliveryTW(lines)
write_inst(outputName, lines)
    
