from mpl_toolkits.mplot3d import Axes3D
import matplotlib.pyplot as plt
import numpy as np
import math

nomethod_filename = "../stable-output.txt"
gradient_filename = "../new_data/gradient-output.txt"
genetic_filename = "../new_data/nm-output.txt"
nelderMead_filename = "../nm-output.txt"

nmd_data = []
with open(nomethod_filename) as f:
    for line in f:
        row = line.split()
        nmd_data.append(row)

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
ax.set_ylabel('h, m')

grad_t = [float(row[0]) for row in grad_data]
grad_r = [float(row[1]) for row in grad_data]
print("grad: {0}".format(len(grad_r) / 10))

ga_t = [float(row[0]) for row in gen_data]
ga_r = [float(row[1]) for row in gen_data]
print("GA: {0}".format(len(ga_r)))

nm_t = [float(row[0]) for row in nm_data]
nm_r = [float(row[1]) for row in nm_data]
print("NM: {0}".format(len(nm_r)))

stable_t = [float(row[0]) for row in nmd_data]
stable_r = [float(row[1]) for row in nmd_data]
print("STABLE: {0}".format(len(stable_r)))

d_grad = []
d_grad_t = []
for i in range(int(len(nm_r)/2)):
    d_grad.append(grad_r[i*2] - stable_r[i])
    d_grad_t.append(grad_t[i*2])

d_ga = []
d_ga_t = []
for i in range(int(len(nm_r)/2)):
    d_ga.append(ga_r[i] - stable_r[i])
    d_ga_t.append(ga_t[i])

d_nm = []
for i in range(int(len(nm_r)/2)):
    d_nm.append(nm_r[i*2] - stable_r[i])

len_size = int(len(nm_r)/2)

print("R(grad): {0}, R(GA): {1}, R(NM): {2}".format(sum(d_grad)/len_size/2, sum(d_ga)/len_size/2, sum(d_nm)/len_size/2))

plt.plot(ga_t, ga_r, 'g-', label='gradient')
plt.plot(nm_t, nm_r, 'r-', label='Genetic-algo')
plt.plot(grad_t, grad_r, 'b-', label='Nelder-Mead')
plt.plot(stable_t, stable_r, 'y-', label='stabilization only')

plt.grid()
plt.legend(loc=2, borderaxespad=0.)
plt.show()

# # angles gradient
# ax = fig.add_subplot(222)
#
# ax.set_xlabel('t, sec')
# ax.set_ylabel('angles, degree')
#
#
# t = [float(row[0]) for row in grad_data]
# alpha = [float(row[2]) for row in grad_data]
# alpha = [(x / (2*math.pi) - int(x / (2*math.pi)) - (
#     0.5 if (x / (2*math.pi) - int(x / (2*math.pi))) > 0 else -0.5
# )) * 360 for x in alpha]
# beta = [float(row[3]) for row in grad_data]
# beta = [(x / (2*math.pi) - int(x / (2*math.pi)) - (
#     0.5 if (x / (2*math.pi) - int(x / (2*math.pi))) > 0 else -0.5
# )) * 360 for x in beta]
# gamma = [float(row[4]) for row in grad_data]
# gamma = [(x / (2*math.pi) - int(x / (2*math.pi)) - (
#     0.5 if (x / (2*math.pi) - int(x / (2*math.pi))) > 0 else -0.5
# )) * 360 for x in gamma]
#
# plt.plot(t, alpha, 'g-', label='alpha')
# plt.plot(t, beta, 'r-', label='beta')
# plt.plot(t, gamma, 'b-', label='gamma')
#
#
# plt.grid()
# plt.legend(loc=2, borderaxespad=0.)
# plt.show()
# # angles Genetic Algo
# ax = fig.add_subplot(222)
#
# ax.set_xlabel('t, sec')
# ax.set_ylabel('angles, degree')
#
#
# t = [float(row[0]) for row in gen_data]
# alpha = [float(row[2]) for row in gen_data]
# alpha = [x / math.pi * 180 for x in alpha]
# beta = [float(row[3]) for row in gen_data]
# beta = [x / math.pi * 180 for x in beta]
# gamma = [float(row[4]) for row in gen_data]
# gamma = [x / math.pi * 180 for x in gamma]
#
#
# plt.plot(t, alpha, 'g-', label='alpha')
# plt.plot(t, beta, 'r-', label='beta')
# plt.plot(t, gamma, 'b-', label='gamma')
#
#
# plt.grid()
# plt.legend(loc=2, borderaxespad=0.)
# plt.show()
# # angles Nelder-Mead
# ax = fig.add_subplot(222)
#
# ax.set_xlabel('t, sec')
# ax.set_ylabel('angles, degree')
#
#
# t = [float(row[0]) for row in nm_data]
# alpha = [float(row[2]) for row in nm_data]
# alpha = [x / math.pi * 180 for x in alpha]
# beta = [float(row[3]) for row in nm_data]
# beta = [x / math.pi * 180 for x in beta]
# gamma = [float(row[4]) for row in nm_data]
# gamma = [x / math.pi * 180 for x in gamma]
#
#
# plt.plot(t, alpha, 'g-', label='alpha')
# plt.plot(t, beta, 'r-', label='beta')
# plt.plot(t, gamma, 'b-', label='gamma')
#
#
# plt.grid()
# plt.legend(loc=2, borderaxespad=0.)
# plt.show()
