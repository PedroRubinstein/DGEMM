from utilities import *

figsize = (19.20, 10.80) # Isso é 1/100 da resolução
fontsize_x = 30
fontsize_y = 20
fontsize_legend= 20
barwidth = 0.15

categories = ['Python', 'C', 'SIMD', 'Unrolling', 'Blocking']

# Ler dados (no C, peguei o -O3)

categories = ['Python', 'C', 'SIMD', 'Unrolling', 'Blocking']

# Ler dados (no C, peguei o -O3)

data_list = [
    Data((3.69, 0.00682, 0.00218, 0.00129, 0.00108), '256x256'),
    Data((30.2, 0.131, 0.034, 0.0127, 0.0093), '512x512'),
    Data((248, 2.2, 0.55, 0.134, 0.076), '1024x1024'),
    Data((2350, 80, 7, 2.41, 0.646), '2048x2048'),
    Data((19710, 870, 175.5, 49.31, 6.725), '4096x4096'),
    Data((173900, 7250, 1757, 431.6, 56.63), '8192x8192')
]

# Figura da performance
relative_data_list = []
for data in data_list:
    values = [1] # Tempo do C dividido pelo tempo do tamanho de matriz em questão
    for i in range(1, len(data.values)): values.append(data.values[i-1]/data.values[i])

    relative_data_list.append(Data(values, data.label))
    
make_figure(relative_data_list, categories, figsize, barwidth, fontsize_x, fontsize_y, fontsize_legend, 'Aumento de performance em relação à otimização anterior', 'PerformanceRelativaAnteriorLog', log_scale=True)