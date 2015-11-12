#!/usr/bin/python
length = 6000
with open("data.txt", 'w') as fh:
	for i in range(length):
		fh.write("%d\n" % (255.0/length*i))

