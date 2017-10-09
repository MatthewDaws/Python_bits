import time

def f(x):
	time.sleep(1)
	print("Completed {}".format(x))

try:
	for x in range(1000):
		f(x)
except KeyboardInterrupt:
	print("Exiting, but we were on x={}".format(x))
