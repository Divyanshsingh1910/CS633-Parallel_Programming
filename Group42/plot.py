from matplotlib import pyplot as plt

stencil5 = [0.127308, 1.646402]
stencil9 = [0.436128,  6.040071]

plot_labels = ["Stencil = 5", "Stencil = 9"]
plt.xlabel ("Stencil Value")
plt.ylabel ("Time (seconds)")
plt.legend()
plt.title(f"Halo Exchange on  with varying stencil size")

plt.boxplot ([stencil5, stencil9], labels = plot_labels)
# plt.show() # not showing figures at the moment
plt.savefig (f"boxplot.png")
