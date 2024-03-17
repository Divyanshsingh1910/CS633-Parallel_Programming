from matplotlib import pyplot as plt

for size in [512, 2048]:
	for stencil in [5,9]:
		# make the c program print the output as a python list [].
		# eval() reads it
		pack = eval (input())
		srecv = eval(input())
	
		plot_labels = ["Using Send/Recv", "Using Pack-Unpack"]
		plt.xlabel ("Method")
		plt.ylabel ("Time (microseconds)")
		plt.legend()
		plt.title(f"Halo Exchange on {size*size} elements with stencil size {stencil}")

		plt.boxplot ([srecv, pack], labels = plot_labels)
		# plt.show() # not showing figures at the moment
		plt.savefig (f"boxplot_{size}_stencil{stencil}.png")

