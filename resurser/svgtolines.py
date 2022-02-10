#!/usr/bin/env python3

import xml.etree.ElementTree as ET
import numpy as np
tree = ET.parse('firefighterbana.svg')

cur_pos = np.array([0,0])
lines = []

outf = open("lines.csv", "w")

for child in tree.iter():
    if "path" in child.tag:
        attrib = child.attrib

        tokens = attrib['d'].split()

        for i in range(len(tokens)):
            print(cur_pos)
            if tokens[i] == 'M':
                s = tokens[i+1]
                comma_idx = s.find(',')
                x = float(s[:comma_idx])
                y = float(s[comma_idx+1:])
                cur_pos = np.array([x,y])        
            elif tokens[i] == 'v':
                d = float(tokens[i+1])
                next_pos = cur_pos+np.array([0,d])
                lines.append((cur_pos, next_pos))
                cur_pos = next_pos
            elif tokens[i] == 'h':
                d = float(tokens[i+1])
                next_pos = cur_pos+np.array([d,0])
                lines.append((cur_pos, next_pos))
                cur_pos = next_pos
            elif tokens[i] == 'V':
                d = float(tokens[i+1])
                next_pos = cur_pos.copy()
                next_pos[1] = d
                lines.append((cur_pos, next_pos))
                cur_pos = next_pos
            elif tokens[i]== 'H':
                d = float(tokens[i+1])
                next_pos = cur_pos.copy()
                next_pos[0] = d
                lines.append((cur_pos, next_pos))
                cur_pos = next_pos


s = ""
for line in lines:
    start, end = line
    start *= 10
    end *= 10
    s += str(start[0])+','+str(start[1])+','+str(end[0])+','+str(end[1])
    s += '\n'
outf.write(s)