from mpl_toolkits.mplot3d import Axes3D
import matplotlib.pyplot as plt
import numpy as np
import math

gradient_filename = "gradient-output.txt"
genetic_filename = "../genetic-output.txt"
nelderMead_filename = "../output.txt"

grad_data = []
with open(gradient_filename) as f:
    for line in f:
        row = line.split()
        grad_data.append(row)

gen_data = []
with open(genetic_filename) as f:
    for line in f:
        row = line.split()
        gen_data.append(row)


nm_data = []
with open(nelderMead_filename) as f:
    for line in f:
        row = line.split()
        nm_data.append(row)

fig = plt.figure()
ax = fig.add_subplot(111)

ax.set_xlabel('t, sec')
ax.set_ylabel('r, m')

t = [float(row[0]) for row in grad_data]
r = [float(row[1]) for row in grad_data]
plt.plot(t, r, 'g-', label='gradient')
t = [float(row[0]) for row in gen_data]
r = [float(row[1]) for row in gen_data]
plt.plot(t, r, 'r-', label='GA')
t = [float(row[0]) for row in nm_data]
r = [float(row[1]) for row in nm_data]
plt.plot(t, r, 'b-', label='Nelder-Mead')

plt.grid()
plt.legend(loc=2, borderaxespad=0.)
plt.show()

# angles gradient
ax = fig.add_subplot(222)

ax.set_xlabel('t, sec')
ax.set_ylabel('angles, degree')


t = [float(row[0]) for row in grad_data]
alpha = [float(row[2]) for row in grad_data]
alpha = [x / math.pi * 180 for x in alpha]
beta = [float(row[3]) for row in grad_data]
beta = [x / math.pi * 180 for x in beta]
gamma = [float(row[4]) for row in grad_data]
gamma = [x / math.pi * 180 for x in gamma]

plt.plot(t, alpha, 'g-', label='alpha')
plt.plot(t, beta, 'r-', label='beta')
plt.plot(t, gamma, 'b-', label='gamma')


plt.grid()
plt.legend(loc=2, borderaxespad=0.)
plt.show()
# angles Genetic Algo
ax = fig.add_subplot(222)

ax.set_xlabel('t, sec')
ax.set_ylabel('angles, degree')


t = [float(row[0]) for row in gen_data]
alpha = [float(row[2]) for row in gen_data]
alpha = [x / math.pi * 180 for x in alpha]
beta = [float(row[3]) for row in gen_data]
beta = [x / math.pi * 180 for x in beta]
gamma = [float(row[4]) for row in gen_data]
gamma = [x / math.pi * 180 for x in gamma]


plt.plot(t, alpha, 'g-', label='alpha')
plt.plot(t, beta, 'r-', label='beta')
plt.plot(t, gamma, 'b-', label='gamma')


plt.grid()
plt.legend(loc=2, borderaxespad=0.)
plt.show()
# angles Nelder-Mead
ax = fig.add_subplot(222)

ax.set_xlabel('t, sec')
ax.set_ylabel('angles, degree')


t = [float(row[0]) for row in nm_data]
alpha = [float(row[2]) for row in nm_data]
alpha = [x / math.pi * 180 for x in alpha]
beta = [float(row[3]) for row in nm_data]
beta = [x / math.pi * 180 for x in beta]
gamma = [float(row[4]) for row in nm_data]
gamma = [x / math.pi * 180 for x in gamma]


plt.plot(t, alpha, 'g-', label='alpha')
plt.plot(t, beta, 'r-', label='beta')
plt.plot(t, gamma, 'b-', label='gamma')


plt.grid()
plt.legend(loc=2, borderaxespad=0.)
plt.show()