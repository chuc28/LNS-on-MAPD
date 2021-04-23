import matplotlib.pyplot as plt
import numpy as np
from matplotlib.pyplot import MultipleLocator

def plot_makespan(file_list, compare_num):
    makespan_list = []
    runtime_list =[] 
    label =[]
    lines = []
    for file in file_list:
        with open(file, "r") as f: 
            lines.append(f.readlines()[1:]) 
    for line in lines:
        makespan = []
        runtime =[]
        for piece in line:
            makespan.append(int((piece.split(", ")[2]).split(" ")[2]))
            runtime.append(int((piece.split(", ")[4]).split(" ")[2]))
        makespan_list.append(makespan)
        runtime_list.append(runtime)
#     print(makespan_list)
#     print(runtime_list)
    fig, ax = plt.subplots()
    for i in range(compare_num):
        label.append((file_list[i].split("/")[3]).split(".")[0])
        ax.plot(runtime_list[i], makespan_list[i], label=label[i])
    x_major_locator = MultipleLocator(50)
    y_major_locator = MultipleLocator(200)
    ax = plt.gca()
    ax.xaxis.set_major_locator(x_major_locator)
    ax.yaxis.set_major_locator(y_major_locator)
    plt.legend(label, loc='upper right', fontsize=8)
    fig.suptitle('Makespan improvement with different heuristic')
    plt.xlabel('Runtime')
    plt.ylabel('Makespan')
    plt.show()
    fig.savefig('../output/offline/fig.jpg')
    
f1 = "../output/offline/1-0-0.txt"
f2 = "../output/offline/1-1-0.txt"
file_list = [f1, f2]
plot_makespan(file_list, 2)