#!/usr/bin/env python

import xml.etree.ElementTree as ET
tree = ET.parse('jnx.xmf')

root = tree.getroot()

for child in root.iter("POS"):
	print "v %s" % child.text

for child in root.iter("NORM"):
	print "vn %s" % child.text
	
for child in root.iter("TEXCOORD"):
	print "vt %s" % child.text
	
for child in root.iter("FACE"):
	#split = child.find("VERTEXID").split()
	split= child.attrib["VERTEXID"].split()
	a = int(split[0]) + 1
	b = int(split[1]) + 1
	c = int(split[2]) + 1
	
	print "f %s/%s/%s %s/%s/%s %s/%s/%s" % (a,a,a,b,b,b,c,c,c)	