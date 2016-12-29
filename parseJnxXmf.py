#!/usr/bin/env python

import xml.etree.ElementTree as ET

f = open("jnx_new_uv_same_index.obj","r")

line = []
v_vt = {}
for i in f.readlines():
	if i.startswith("vt "):
		line.append(i.strip().split("vt ")[-1])
	elif i.startswith("f "):
		triples = i.split()[1:]
		print triples
		v0 = int(triples[0].split("/")[0])-1
		vt0 = int(triples[0].split("/")[1])-1
		print v0
		print vt0
		v_vt[v0]=vt0
		v1 = int(triples[1].split("/")[0])-1
		vt1 = int(triples[1].split("/")[1])-1
		print v1
		print vt1
		v_vt[v1]=vt1
		v2 = int(triples[2].split("/")[0])-1
		vt2 = int(triples[2].split("/")[1])-1
		print v2
		print vt2
		v_vt[v2]=vt2

tree = ET.parse('jnx.xmf')

root = tree.getroot()
i = 0 

for child in root.iter("TEXCOORD"):
	if v_vt[i] < len(line):
		child.text = line[v_vt[i]]
	i+=1
	
tree.write("jnx_update.xmf")