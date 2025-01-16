#include <iostream>
#include <vector>
#include <unordered_map>
#include <queue>
#include <unordered_set>
#include <limits>
#include <cmath>
#include <algorithm>
#include <windows.h>  // For working with Windows API

// Function to set console to UTF-8 encoding
void set_console_utf8() {
    SetConsoleOutputCP(CP_UTF8);  // Set output encoding to UTF-8
    SetConsoleCP(CP_UTF8);         // Set input encoding to UTF-8
}

struct Node {
    double lon, lat;
    std::vector<std::pair<Node*, double>> neighbors;
    int id;  // ID or label for the node (A, B, C, etc.)
};

struct Graph {
    std::unordered_map<int, Node*> nodes;

    void add_node(int id, double lon, double lat) {
        if (nodes.find(id) == nodes.end()) {
            nodes[id] = new Node{lon, lat, {}, id};
        }
    }

    void add_edge(int id1, int id2, double weight) {
        if (nodes.find(id1) != nodes.end() && nodes.find(id2) != nodes.end()) {
            nodes[id1]->neighbors.emplace_back(nodes[id2], weight);
        }
    }

    Node* get_node(int id) {
        return nodes.find(id) != nodes.end() ? nodes[id] : nullptr;
    }
};

// --- Pathfinding algorithms ---
std::vector<Node*> bfs(Graph& graph, Node* start, Node* goal) {
    std::queue<std::vector<Node*>> queue;
    std::unordered_map<Node*, bool> visited;
    queue.push({start});

    while (!queue.empty()) {
        auto path = queue.front();
        queue.pop();

        Node* current = path.back();

        if (current == goal) {
            return path;
        }

        if (visited[current]) continue;
        visited[current] = true;

        for (auto& neighbor : current->neighbors) {
            auto new_path = path;
            new_path.push_back(neighbor.first);
            queue.push(new_path);
        }
    }

    return {};
}

bool dfs_recursive(Node* current, Node* goal, std::unordered_set<Node*>& visited, std::vector<Node*>& path) {
    path.push_back(current);

    if (current == goal) {
        return true;
    }
    visited.insert(current);

    for (auto& neighbor : current->neighbors) {
        if (visited.find(neighbor.first) == visited.end()) {
            if (dfs_recursive(neighbor.first, goal, visited, path)) {
                return true;
            }
        }
    }

    path.pop_back();
    return false;
}

std::vector<Node*> dfs(Graph& graph, Node* start, Node* goal) {
    std::unordered_set<Node*> visited;
    std::vector<Node*> path;
    dfs_recursive(start, goal, visited, path);
    return path;
}

std::vector<Node*> dijkstra(Graph& graph, Node* start, Node* goal) {
    std::unordered_map<Node*, double> distances;
    std::unordered_map<Node*, Node*> previous;
    auto compare = [](std::pair<Node*, double> a, std::pair<Node*, double> b) {
        return a.second > b.second;
    };
    std::priority_queue<std::pair<Node*, double>, std::vector<std::pair<Node*, double>>, decltype(compare)> queue(compare);

    for (auto& pair : graph.nodes) {
        distances[pair.second] = std::numeric_limits<double>::infinity();
    }
    distances[start] = 0.0;

    queue.push({start, 0.0});

    while (!queue.empty()) {
        Node* current = queue.top().first;
        queue.pop();

        if (current == goal) {
            break;
        }

        for (auto& neighbor : current->neighbors) {
            double new_distance = distances[current] + neighbor.second;
            if (new_distance < distances[neighbor.first]) {
                distances[neighbor.first] = new_distance;
                previous[neighbor.first] = current;
                queue.push({neighbor.first, new_distance});
            }
        }
    }

    std::vector<Node*> path;
    for (Node* at = goal; at != nullptr; at = previous[at]) {
        path.push_back(at);
    }
    std::reverse(path.begin(), path.end());
    return path;
}

// --- Path Comparison ---
bool compare_paths(const std::vector<Node*>& path1, const std::vector<Node*>& path2) {
    if (path1.size() != path2.size()) {
        return false;
    }
    for (size_t i = 0; i < path1.size(); ++i) {
        if (path1[i] != path2[i]) {
            return false;
        }
    }
    return true;
}

// --- Tests ---
void test_custom_graph() {
    Graph graph;

    // Nodes
    graph.add_node(1, 0.0, 0.0);  // A
    graph.add_node(2, 1.0, 0.0);  // B
    graph.add_node(3, 2.0, 0.0);  // C
    graph.add_node(4, 0.0, 1.0);  // D
    graph.add_node(5, 2.0, 1.0);  // E

    // Edges
    graph.add_edge(1, 2, 1.0);  // A -> B
    graph.add_edge(1, 3, 2.0);  // A -> C
    graph.add_edge(2, 4, 4.0);  // B -> D
    graph.add_edge(3, 5, 1.0);  // C -> E
    graph.add_edge(4, 5, 5.0);  // D -> E
    graph.add_edge(2, 3, 3.0);  // B -> C

    // Start and goal nodes
    Node* start = graph.get_node(1);  // A
    Node* goal = graph.get_node(5);   // E

    auto bfs_path = bfs(graph, start, goal);
    auto dfs_path = dfs(graph, start, goal);
    auto dijkstra_path = dijkstra(graph, start, goal);

    // Check if paths are correct
    if (compare_paths(bfs_path, dijkstra_path)) {
        std::cout << "Test passed: BFS and Dijkstra found the same correct path." << std::endl;
    } else {
        std::cout << "Test failed: BFS and Dijkstra found different paths." << std::endl;
    }

    if (compare_paths(dfs_path, dijkstra_path)) {
        std::cout << "Test passed: DFS and Dijkstra found the same correct path." << std::endl;
    } else {
        std::cout << "Test failed: DFS and Dijkstra found different paths." << std::endl;
    }
}

void test_random_graph_5_nodes() {
    Graph graph;
    graph.add_node(1, 0.0, 0.0);
    graph.add_node(2, 1.0, 0.0);
    graph.add_node(3, 2.0, 0.0);
    graph.add_node(4, 0.0, 1.0);
    graph.add_node(5, 2.0, 1.0);

    graph.add_edge(1, 2, 1.0);
    graph.add_edge(1, 3, 2.0);
    graph.add_edge(2, 4, 1.5);
    graph.add_edge(3, 5, 2.5);

    Node* start = graph.get_node(1);
    Node* goal = graph.get_node(5);

    auto bfs_path = bfs(graph, start, goal);
    auto dfs_path = dfs(graph, start, goal);
    auto dijkstra_path = dijkstra(graph, start, goal);

    if (compare_paths(bfs_path, dijkstra_path)) {
        std::cout << "Random Graph Test (5 nodes): Test passed." << std::endl;
    } else {
        std::cout << "Random Graph Test (5 nodes): Test failed." << std::endl;
    }
}

void test_random_graph_10_nodes() {
    Graph graph;
    graph.add_node(1, 0.0, 0.0);
    graph.add_node(2, 1.0, 0.0);
    graph.add_node(3, 2.0, 0.0);
    graph.add_node(4, 0.0, 1.0);
    graph.add_node(5, 2.0, 1.0);
    graph.add_node(6, 1.5, 1.0);
    graph.add_node(7, 1.0, 2.0);
    graph.add_node(8, 2.0, 2.0);
    graph.add_node(9, 2.5, 1.5);
    graph.add_node(10, 3.0, 0.0);

    graph.add_edge(1, 2, 1.0);
    graph.add_edge(2, 3, 2.0);
    graph.add_edge(3, 4, 3.0);
    graph.add_edge(4, 5, 1.0);
    graph.add_edge(5, 6, 1.0);
    graph.add_edge(6, 7, 1.0);
    graph.add_edge(7, 8, 1.0);
    graph.add_edge(8, 9, 1.0);
    graph.add_edge(9, 10, 1.0);

    Node* start = graph.get_node(1);
    Node* goal = graph.get_node(10);

    auto bfs_path = bfs(graph, start, goal);
    auto dfs_path = dfs(graph, start, goal);
    auto dijkstra_path = dijkstra(graph, start, goal);

    if (compare_paths(bfs_path, dijkstra_path)) {
        std::cout << "Random Graph Test (10 nodes): Test passed." << std::endl;
    } else {
        std::cout << "Random Graph Test (10 nodes): Test failed." << std::endl;
    }
}

int main() {
    set_console_utf8();
    test_custom_graph();
    test_random_graph_5_nodes();
    test_random_graph_10_nodes();

    return 0;
}
