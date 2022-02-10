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

        for i in range(len(tokens)):
            if tokens[i] == 'M':
                s = tokens[i+1]
                comma_idx = s.find(',')
                x = float(s[:comma_idx])
                y = float(s[comma_idx+1:])
                cur_pos = np.array([x,y])        
            elif tokens[i] == 'v':
                d = float(tokens[i+1])
                next_pos = cur_pos+np.array([0,d])
                lines.append((cur_pos.copy(), next_pos.copy()))
                cur_pos = next_pos
            elif tokens[i] == 'h':
                d = float(tokens[i+1])
                next_pos = cur_pos+np.array([d,0])
                lines.append((cur_pos.copy(), next_pos.copy()))
                cur_pos = next_pos
            elif tokens[i] == 'V':
                d = float(tokens[i+1])
                next_pos = cur_pos.copy()
                next_pos[1] = d
                lines.append((cur_pos.copy(), next_pos.copy()))
                cur_pos = next_pos
            elif tokens[i]== 'H':
                d = float(tokens[i+1])
                next_pos = cur_pos.copy()
                next_pos[0] = d
                lines.append((cur_pos.copy(), next_pos.copy()))
                cur_pos = next_pos


s = ""
for line in lines:
    start, end = line
    start *= 10
    end *= 10
    s += '{{'+str(int(start[0]))+','+str(int(start[1]))+'},{'+str(int(end[0]))+','+str(int(end[1]))+'}},'
    s += '\n'
print(s)
outf.write(s)
