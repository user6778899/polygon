import matplotlib.pyplot as plt
import networkx as nx

def draw_custom_graph():
    # Создаем граф
    G = nx.Graph()

    # Добавляем узлы (идентификаторы узлов: A=1, B=2, C=3, D=4, E=5)
    G.add_node(1, pos=(0.0, 0.0))  # A
    G.add_node(2, pos=(1.0, 0.5))  # B
    G.add_node(3, pos=(2.0, 0.0))  # C
    G.add_node(4, pos=(0.0, 1.0))  # D
    G.add_node(5, pos=(2.0, 1.0))  # E

    # Добавляем рёбра с весами
    G.add_edge(1, 2, weight=1.0)  # A -> B
    G.add_edge(1, 3, weight=2.0)  # A -> C
    G.add_edge(2, 4, weight=4.0)  # B -> C
    G.add_edge(3, 5, weight=1.0)  # B -> D
    G.add_edge(4, 5, weight=5.0)  # C -> E
    G.add_edge(2, 3, weight=3.0)  # D -> E

    # Получаем позиции для узлов
    pos = nx.get_node_attributes(G, 'pos')

    # Рисуем граф с рёбрами и весами
    plt.figure(figsize=(8, 6))
    nx.draw(G, pos, with_labels=True, node_size=1000, node_color='skyblue', font_size=12, font_weight='bold')

    # Рисуем веса рёбер
    edge_labels = nx.get_edge_attributes(G, 'weight')
    nx.draw_networkx_edge_labels(G, pos, edge_labels=edge_labels, font_size=10)

    # Заголовок графика
    plt.title("Custom Graph: A to E")
    plt.show()

# Вызов функции для рисования графа
draw_custom_graph()
