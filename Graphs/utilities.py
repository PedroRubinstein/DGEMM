import matplotlib.pyplot as plt
import numpy as np

class Data:
    def __init__(self, values, label):
        'Cada objeto Data representa todos os dados para um tamanho específico de matriz'
        self.values = values
        self.label = label

def make_figure(data_list, categories, figsize, barwidth, fontsize_x, fontsize_y, fontsize_legend, title, filename):
    # data_list é uma lista de objetos Data
    assert len(data_list) > 0

    # Figura dos tempos de execução
    fig, ax = plt.subplots(figsize=figsize, layout='constrained')
    plt.xticks(fontsize=fontsize_x)
    plt.yticks(fontsize=fontsize_y)

    # Determinar posições das barras de cada tamanho de matriz
    first_data = data_list[0]
    assert type(first_data) == Data
    bars = []
    bars.append(np.arange(len(first_data.values)))

    for i in range(1, len(data_list)):
        bars.append([x + barwidth for x in bars[i - 1]])

    for i in range(len(data_list)):
        data = data_list[i]
        assert type(data) == Data

        plt.bar(bars[i], data.values, barwidth, label=data.label)

    n_bars = len(data_list)
    tick_offset = ((n_bars - 1) / 2) * barwidth
    plt.xticks([r+tick_offset for r in range(len(first_data.values))], categories)

    plt.title(title, fontsize=fontsize_x)
    plt.legend(fontsize=fontsize_legend)
    plt.savefig(filename)
    plt.close()