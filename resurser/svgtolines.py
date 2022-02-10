#!/usr/bin/env python3

import xml.etree.ElementTree as ET
import numpy as np
tree = ET.parse('firefighterbana.svg')

cur_pos = np.array([0,0])
lines = []

outf = open("lines.txt", "w")

for child in tree.iter():
    if "path" in child.tag:
        attrib = child.attrib

        tokens = attrib['d'].split()
        print(attrib['d'])
        sp = cur_pos.copy()
        cur_command = ""
        for i in range(len(tokens)):
            if not tokens[i][-1].isdigit():
                cur_command = tokens[i]
                continue

            if cur_command == 'M':
                s = tokens[i]
                comma_idx = s.find(',')
                x = float(s[:comma_idx])
                y = float(s[comma_idx+1:])
                cur_pos = np.array([x,y])        
            elif cur_command == 'v':
                d = float(tokens[i])
                next_pos = cur_pos+np.array([0,d])
                lines.append((cur_pos.copy(), next_pos.copy()))
                cur_pos = next_pos
            elif cur_command == 'h':
                d = float(tokens[i])
                next_pos = cur_pos+np.array([d,0])
                lines.append((cur_pos.copy(), next_pos.copy()))
                cur_pos = next_pos
            elif cur_command == 'V':
                d = float(tokens[i])
                next_pos = cur_pos.copy()
                next_pos[1] = d
                lines.append((cur_pos.copy(), next_pos.copy()))
                cur_pos = next_pos
            elif cur_command== 'H':
                d = float(tokens[i])
                next_pos = cur_pos.copy()
                next_pos[0] = d
                lines.append((cur_pos.copy(), next_pos.copy()))
                cur_pos = next_pos
            elif cur_command == 'L':
                s = tokens[i]
                comma_idx = s.find(',')
                x = float(s[:comma_idx])
                y = float(s[comma_idx+1:])
                next_pos = np.array([x,y])

                lines.append((cur_pos.copy(), next_pos.copy()))
            elif not tokens[i][-1].isdigit():
                print(tokens[i])
                print("ERROR"*50)


s = ""
for line in lines:
    start, end = line
    start *= 10
    end *= 10
    s += '{{'+str(int(start[0]))+','+str(int(start[1]))+'},{'+str(int(end[0]))+','+str(int(end[1]))+'}},'
    s += '\n'
print(s)
outf.write(s)
