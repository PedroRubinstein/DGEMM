import matplotlib.pyplot as plt
import numpy as np

figsize = (19.20, 10.80) # Isso é 1/100 da resolução
fontsize_x = 30
fontsize_y = 20
fontsize_legend= 20
barwidth = 0.15

# Figura dos tempos de execução
fig, ax = plt.subplots(figsize=figsize, layout='constrained')
plt.xticks(fontsize=fontsize_x)
plt.yticks(fontsize=fontsize_y)

categories = ['Python', 'C', 'SIMD', 'Unrolling']

# Ler dados (no C, peguei o -O3)

data256 = 3.69, 0.00682, 0.00218, 0.00129
data512 = 30.2, 0.131, 0.034, 0.0127
data1024 = 248, 2.2, 0.55, 0.134
data2048 = 2350, 80, 7, 2.41
data4096 = 19710, 870, 175.5, 49.31

# Determinar posições das barras de cada tamanho de matriz
br1 = np.arange(len(data256))
br2 = [x + barwidth for x in br1]
br3 = [x + barwidth for x in br2]
br4 = [x + barwidth for x in br3]
br5 = [x + barwidth for x in br4]

plt.bar(br1, data256, barwidth, label='256x256')
plt.bar(br2, data512, barwidth, label='512x512')
plt.bar(br3, data1024, barwidth, label='1024x1024')
plt.bar(br4, data2048, barwidth, label='2048x2048')
plt.bar(br5, data4096, barwidth, label='4096x4096')

plt.xticks([r+2*barwidth for r in range(len(data256))], categories)

plt.title('Tempo de execução', fontsize=fontsize_x)
plt.legend(fontsize=fontsize_legend)
plt.savefig('Tempo')
plt.close()

# Figura da performance
fig, ax = plt.subplots(figsize=figsize, layout='constrained')
plt.xticks(fontsize=fontsize_x)
plt.yticks(fontsize=fontsize_y)

data256 = [data256[0]/data256[i] for i in range(len(data256))]
data512 = [data512[0]/data512[i] for i in range(len(data512))]
data1024 = [data1024[0]/data1024[i] for i in range(len(data1024))]
data2048 = [data2048[0]/data2048[i] for i in range(len(data2048))]
data4096 = [data4096[0]/data4096[i] for i in range(len(data4096))]

plt.bar(br1, data256, barwidth, label='256x256')
plt.bar(br2, data512, barwidth, label='512x512')
plt.bar(br3, data1024, barwidth, label='1024x1024')
plt.bar(br4, data2048, barwidth, label='2048x2048')
plt.bar(br5, data4096, barwidth, label='4096x4096')

plt.xticks([r+2*barwidth for r in range(len(data256))], categories)

plt.title('Performance (relativa ao Python)', fontsize=fontsize_x)
plt.legend(fontsize=fontsize_legend)
plt.savefig('Performance')
plt.close()