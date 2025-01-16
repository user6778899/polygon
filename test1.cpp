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
        std::cout << "BFS visiting node: " << current->id << " (" << current->lon << ", " << current->lat << ")\n";

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
    std::cout << "DFS visiting node: " << current->id << " (" << current->lon << ", " << current->lat << ")\n";

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

        std::cout << "Dijkstra visiting node: " << current->id << " (" << current->lon << ", " << current->lat << ") with current distance: " << distances[current] << "\n";

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

// --- Printing ---
void print_path(const std::string& algorithm, const std::vector<Node*>& path) {
    std::cout << algorithm << " Path: ";
    for (const auto& node : path) {
        if (node == nullptr) {
            std::cout << "Unknown -> ";
        } else {
            std::cout << node->id << " (" << node->lon << ", " << node->lat << ") -> ";
        }
    }
    std::cout << "END" << std::endl;
    std::cout << algorithm << " Path size: " << path.size() << std::endl;
}

void print_graph(Graph& graph) {
    for (auto& node_pair : graph.nodes) {
        std::cout << "Node " << node_pair.first << " (" << node_pair.second->lon << ", " << node_pair.second->lat << "): ";
        for (auto& neighbor : node_pair.second->neighbors) {
            std::cout << neighbor.first->id << " (" << neighbor.first->lon << ", " << neighbor.first->lat << ") ";
        }
        std::cout << std::endl;
    }
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

    std::cout << "Custom Graph Test:" << std::endl;

    auto bfs_path = bfs(graph, start, goal);
    print_path("BFS", bfs_path);

    auto dfs_path = dfs(graph, start, goal);
    print_path("DFS", dfs_path);

    auto dijkstra_path = dijkstra(graph, start, goal);
    print_path("Dijkstra", dijkstra_path);
}

void run_tests() {
    std::cout << "Running Custom Graph Test..." << std::endl;
    test_custom_graph();
}

int main() {
    // Set console to UTF-8 encoding
    set_console_utf8();

    std::cout << "Running tests on various graphs..." << std::endl;
    run_tests();
    return 0;
}
