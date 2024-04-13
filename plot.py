from matplotlib import pyplot as plt

# make the program print the output as a python list [].
# eval() reads it

## INSTRUCTIONS: enter 4 lists directlty in command line
print("Enter the 4 lists")
pack_5 = eval (input())
pack_5_2 = eval(input())
pack_9 = eval(input())
pack_9_2 = eval(input())

plot_labels = ["512^2 elements\nStencil size:5","512^2 elements\nStencil size:9","2048^2 elements\nStencil size:5","2048^2 elements\nStencil size:9",]
plt.xlabel ("Method")
plt.ylabel ("Time (seconds)")
plt.legend()
plt.title("Halo Exchange with varying data size and stencil size")
plt.yscale("log")

plt.boxplot ([pack_5, pack_5_2, pack_9, pack_9_2], labels = plot_labels)
plt.show()
