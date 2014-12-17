import numpy as np

x = np.arange(40).reshape(20,2)
x = np.random.rand(10,2)*10
print("Will save this array:")
print(x)
np.save("int32.npy", x.astype(np.int32))
np.save("int64.npy", x.astype(np.int64))
np.save("float32.npy", x.astype(np.float32))
np.save("double64.npy", x.astype(np.float64))

dt = np.dtype( "i4, f4" )
x = np.empty(20, dtype = dt)
np.save("mixed.npy", x)