#include <iostream>
#include <vector>
#include <unordered_map>
#include <queue>
#include <string>
#include <sstream>
#include <fstream>
#include <cmath>
#include <limits>
#include <chrono>
#include <algorithm>
#include <unordered_set>

struct Node {
    double lon, lat;
    std::vector<std::pair<Node*, double>> neighbors;
};

struct Graph {
    std::unordered_map<std::string, Node*> nodes;

    void add_node(double lon, double lat) {
        std::string key = get_key(lon, lat);
        if (nodes.find(key) == nodes.end()) {
            nodes[key] = new Node{lon, lat};
        }
    }

    void add_edge(double lon1, double lat1, double lon2, double lat2, double weight) {
        add_node(lon1, lat1);
        add_node(lon2, lat2);
        Node* node1 = nodes[get_key(lon1, lat1)];
        Node* node2 = nodes[get_key(lon2, lat2)];
        node1->neighbors.emplace_back(node2, weight);
    }

    std::string get_key(double lon, double lat) {
        return std::to_string(lon) + "," + std::to_string(lat);
    }

    Node* find_closest_node(double lat, double lon) {
        double min_distance = std::numeric_limits<double>::max();
        Node* closest = nullptr;
        for (auto& pair : nodes) {
            Node* node = pair.second;
            double distance = std::sqrt(std::pow(node->lat - lat, 2) + std::pow(node->lon - lon, 2));
            if (distance < min_distance) {
                min_distance = distance;
                closest = node;
            }
        }
        return closest;
    }
};

Graph parse_graph(const std::string& filename) {
    Graph graph;
    std::ifstream file(filename);
    std::string line;

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string parent, neighbors;
        std::getline(ss, parent, ':');
        double lon1, lat1;
        sscanf(parent.c_str(), "%lf,%lf", &lon1, &lat1);

        while (std::getline(ss, neighbors, ';')) {
            double lon2, lat2, weight;
            sscanf(neighbors.c_str(), "%lf,%lf,%lf", &lon2, &lat2, &weight);
            graph.add_edge(lon1, lat1, lon2, lat2, weight);
        }
    }

    return graph;
}

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

void print_path(const std::vector<Node*>& path) {
    std::cout << "Path: ";
    for (const auto& node : path) {
        std::cout << "(" << node->lon << ", " << node->lat << ") -> ";
    }
    std::cout << "END" << std::endl;
}

int main() {
    Graph graph = parse_graph("spb_graph.txt");

    double home_lat = 59.9470649, home_lon = 30.4141326;
    double itmo_lat = 60.010688, itmo_lon = 30.451075;
    // https://2gis.ru/spb/firm/70000001076385536/30.4141326%2C59.9470649?m=30.4141326%2C59.9470649%2F17.28 // Георгиевский сквер
    // https://2gis.ru/spb/geo/5348660212684319/30.451075%2C60.010688?m=30.451992%2C60.010544%2F17.71    // Пискарёвский проспект, 144 лит АЛ

    std::cout << "Searching for node close to home: (" << home_lat << ", " << home_lon << ")" << std::endl;
    std::cout << "Searching for node close to ITMO: (" << itmo_lat << ", " << itmo_lon << ")" << std::endl;

    Node* start = graph.find_closest_node(home_lat, home_lon);
    Node* goal = graph.find_closest_node(itmo_lat, itmo_lon);

    if (!start || !goal) {
        std::cerr << "Error: Could not find start or goal node!" << std::endl;
        return 1;
    }

    std::cout << "Running BFS..." << std::endl;
    auto start_time = std::chrono::high_resolution_clock::now();
    auto bfs_path = bfs(graph, start, goal);
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> bfs_duration = end_time - start_time;
    std::cout << "BFS completed in " << bfs_duration.count() << " seconds." << std::endl;
    print_path(bfs_path);

    std::cout << "Running DFS..." << std::endl;
    start_time = std::chrono::high_resolution_clock::now();
    auto dfs_path = dfs(graph, start, goal);
    end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> dfs_duration = end_time - start_time;
    std::cout << "DFS completed in " << dfs_duration.count() << " seconds." << std::endl;
    print_path(dfs_path);

    std::cout << "Running Dijkstra..." << std::endl;
    start_time = std::chrono::high_resolution_clock::now();
    auto dijkstra_path = dijkstra(graph, start, goal);
    end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> dijkstra_duration = end_time - start_time;
    std::cout << "Dijkstra completed in " << dijkstra_duration.count() << " seconds." << std::endl;
    print_path(dijkstra_path);

    return 0;
}
