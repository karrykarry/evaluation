import scipy
from scipy.stats import multivariate_normal
import numpy as np
import matplotlib.pyplot as plt 

x,y = np.meshgrid(np.linspace(-10,10,100),np.linspace(-10,10,100))
pos = np.dstack((x,y))
mean = np.array([2.5, 3.3])
cov  = np.array([[5.0,1.0],[1.0,2.0]])



z = scipy.stats.multivariate_normal(mean,cov).pdf(pos)

fig = plt.figure()
ax = fig.add_subplot(111,aspect='equal')
ax.contourf(x,y,z)
ax.set_xlim(-10,10)
ax.set_ylim(-10,10)
ax.set_xlabel('x')
ax.set_ylabel('y')
ax.set_title('pdf')

plt.show()
