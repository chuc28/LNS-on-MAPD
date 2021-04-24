import matplotlib.pyplot as plt
import numpy as np
from matplotlib.pyplot import MultipleLocator

# label=["Random Removal", "Shaw Removal", "Worst Removal"]
# label=["Random Insertion", "Greedy Insertion", "Regret Insertion"]
label=["Size 8", "Size 16", "Size 32", "Size 64", "Size 128"]

def plot_makespan(file_list, compare_num):
    makespan_list = []
    runtime_list = [] 
    first_line = []
    lines = []
    for file in file_list:
        with open(file, "r") as f: 
            first_line.append(f.readlines()[0]) 
        with open(file, "r") as f: 
            lines.append(f.readlines()[1:])     
            
    for i, line in enumerate(lines):
        makespan = []
        runtime =[]
        makespan.append(int((first_line[i].split(", ")[0]).split(" ")[3]))
        runtime.append(float(((first_line[i].split(", ")[2]).split(" ")[2]).strip()))
        for piece in line:
            makespan.append(int((piece.split(", ")[2]).split(" ")[2]))
            runtime.append(int((piece.split(", ")[4]).split(" ")[2]))
        makespan_list.append(makespan)
        runtime_list.append(runtime)
#     print(makespan_list)
#     print(runtime_list)
    fig, ax = plt.subplots()
    for i in range(compare_num):
        #label.append((file_list[i].split("/")[4]).split(".")[0])
        ax.plot(runtime_list[i], makespan_list[i], 'o')
    x_major_locator = MultipleLocator(50)
    y_major_locator = MultipleLocator(500)
    ax = plt.gca()
    ax.xaxis.set_major_locator(x_major_locator)
    ax.yaxis.set_major_locator(y_major_locator)
    plt.tick_params(labelsize=12)
    plt.legend(label, loc='upper right', fontsize = 14)
    fig.suptitle('Task Frequency = 1, Agent Number = 50', fontsize=16)
    plt.xlabel('Runtime', fontsize=14)
    plt.ylabel('Makespan', fontsize=14)
    plt.grid(linestyle='-.') 
#     plt.rcParams['savefig.dpi'] = 300 
#     plt.rcParams['figure.dpi'] = 300
    plt.show()
    fig.savefig('../output/offline1/task-1-agent-50/neighbourhood_t1a50.png')
    
f1 = "../output/offline1/task-1-agent-50/size-8.txt"
f2 = "../output/offline1/task-1-agent-50/size-16.txt"
f3 = "../output/offline1/task-1-agent-50/size-32.txt"
f4 = "../output/offline1/task-1-agent-50/size-64.txt"
f5 = "../output/offline1/task-1-agent-50/size-128.txt"
file_list = [f1, f2, f3, f4, f5]
plot_makespan(file_list, 5)