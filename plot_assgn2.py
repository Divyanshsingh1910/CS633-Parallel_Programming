from matplotlib import pyplot as plt

# make the program print the output as a python list [].
# eval() reads it
# variables are poorly named because we've used assignment 1's script

## INSTRUCTIONS: enter 4 lists directlty in command line
print("Enter the 8 lists:")
pack_5 = eval (input	("np = 8, 4096^2 with leaders: "))
pack_5_2 = eval(input	("np = 8, 4096^2 without leaders: "))
pack_9 = eval(input		("np = 8, 8192^2 with leaders: "))
pack_9_2 = eval(input	("np = 8, 8192^2 without leaders: "))
pack_12proc_5 = eval (input	("np = 12, 4096^2 with leaders: "))
pack_12proc_5_2 = eval(input	("np = 12, 4096^2 without leaders: "))
pack_12proc_9 = eval(input		("np = 12, 8192^2 with leaders: "))
pack_12proc_9_2 = eval(input	("np = 12, 8192^2 without leaders: "))

plot_labels = ["N = 4096^2\nWith Leaders\n8 processes","N = 4096^2\nWithout Leaders\n8 processes","N = 8192^2\nWith Leaders\n8 processes","N = 8192^2\nWithout Leaders\n8 processes",]
plot_labels += ["N = 4096^2\nWith Leaders\n12 processes","N = 4096^2\nWithout Leaders\n12 processes","N = 8192^2\nWith Leaders\n12 processes","N = 8192^2\nWithout Leaders\n12 processes",]

plt.xlabel ("Method")
plt.ylabel ("Time (seconds)")
plt.legend()
plt.title("Halo Exchange with varying data size and with/without heirarchical decomposition")

plt.boxplot ([pack_5, pack_5_2, pack_9, pack_9_2, pack_12proc_5, pack_12proc_5_2, pack_12proc_9, pack_12proc_9_2], labels = plot_labels)
plt.show()
