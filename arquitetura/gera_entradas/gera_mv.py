def int2bin(n, count=24):
	"""returns the binary of integer n, using count number of digits"""
	return "".join([str((n >> y) & 1) for y in range(count-1, -1, -1)])

cont = 0
blk = 0
for i in range(0,136):
	for j in range(0,240):
		print int2bin(blk,2)
		print int2bin(cont,8) + int2bin(cont,8)
		cont = (cont + 1) % 64
		blk = (blk + 1) % 4

