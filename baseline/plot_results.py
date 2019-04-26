import sys
from pprint import pprint
import numpy as np

input_file_path = sys.argv[1]

list_100_200_0 = []
list_100_0_200 = []
list_200_100_0 = []
list_200_0_100 = []
list_300_0_0 = []

with open(input_file_path, 'r') as f:
    line = f.readline()
    while line:
        line = f.readline()
        splitted_line = line.split(',')
        if len(splitted_line) < 3:
            continue

        nvme_support = int(splitted_line[0].strip())
        memory_cap = int(splitted_line[1].strip())
        latency = int(splitted_line[2].strip())
        if nvme_support:
            if memory_cap == 100:
                list_100_0_200.append(latency)
            elif memory_cap == 200:
                list_200_0_100.append(latency)
        else:
            if memory_cap == 100:
                list_100_200_0.append(latency)
            elif memory_cap == 200:
                list_200_100_0.append(latency)
            elif memory_cap == 300:
                list_300_0_0.append(latency)

mean_list = []
mean_list.append(np.mean(list_100_200_0))
mean_list.append(np.mean(list_200_100_0))
mean_list.append(np.mean(list_100_0_200))
mean_list.append(np.mean(list_200_0_100))
mean_list.append(np.mean(list_300_0_0))

std_list = []
std_list.append(np.std(list_100_200_0))
std_list.append(np.std(list_200_100_0))
std_list.append(np.std(list_100_0_200))
std_list.append(np.std(list_200_0_100))
std_list.append(np.std(list_300_0_0))

import matplotlib.pyplot as plt

x = np.array([1, 2, 3, 4, 5])
y = mean_list
e = std_list

fig, sub_plot = plt.subplots()

plt.xlabel("MEM|HDD|NVMe" , fontsize='x-large')
sub_plot.set_xticklabels(["", "100|200|0", "200|100|0", "100|0|200", "200|0|100", "300|0|0"])
plt.ylabel("Latency (ns)", fontsize='x-large')

plt.bar(x, y, color=(0, 1, 0, 0), linewidth=1, alpha=0.5, \
        hatch="//", width=0.5)

plt.errorbar(x, y, e, linestyle='None', marker='None', color='red')

plt.savefig('plot.png', dpi=300, bbox_inches='tight')
