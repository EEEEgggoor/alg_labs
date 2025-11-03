#include <iostream>
#include <vector>
#include <limits>

using namespace std;

const int INF = numeric_limits<int>::max();

struct Edge {
    int from;
    int to;
    int weight;
};



int getWeight(int from, int to, const vector<Edge>& edges) {
    for (const auto& edge : edges) {
        if (edge.from == from && edge.to == to) {
            return edge.weight;
        }
    }
    return INF;
}

vector<int> solveIterative(int start, int end, const vector<Edge>& edges, int n) {
    vector<int> f(n + 1, INF);
    vector<int> f_prev(n + 1, INF);
    
    
    // Итерационный процесс
    for (int iteration = 1; iteration <= n; iteration++) {
        
        for (int i = 1; i <= n; i++) {
            if (i == end) {
                f[i] = 0;
                continue;
            }
            
            int min_val = INF;
            vector<string> candidates;
            
            // Вычисляем f_i^(k+1) = min{S_ij + f_j^(k)}
            for (int j = 1; j <= n; j++) {
                if (i != j) {
                    int weight = getWeight(i, j, edges);
                    if (weight != INF && f_prev[j] != INF) {
                        int candidate = weight + f_prev[j];
                        candidates.push_back("S_" + to_string(i) + to_string(j) + 
                                           " + f_" + to_string(j) + "^(" + to_string(iteration-1) + 
                                           ") = " + to_string(weight) + " + " + 
                                           to_string(f_prev[j]) + " = " + to_string(candidate));
                        
                        if (candidate < min_val) {
                            min_val = candidate;
                        }
                    }
                }
            }
            
            if (min_val == INF) {
                f[i] = INF;
            } else {
                f[i] = min_val;

            }
        }
        
        // Проверка сходимости
        bool converged = true;
        for (int i = 1; i <= n; i++) {
            if (f[i] != f_prev[i]) {
                converged = false;
                break;
            }
        }
        
        f_prev = f;
        
        if (converged) {
            break;
        }
    }
    
    return f;
}

vector<int> reconstructPath(int start, int end, const vector<int>& f, const vector<Edge>& edges, int n) {
    vector<int> path;
    int current = start;
    path.push_back(current);
    
    
    while (current != end) {
        
        int next_vertex = -1;
        int min_val = INF;
        vector<string> options;
        
        // Ищем следующую вершину, для которой S_ij + f_j = f_i
        for (int j = 1; j <= n; j++) {
            if (current != j) {
                int weight = getWeight(current, j, edges);
                if (weight != INF && f[j] != INF) {
                    int candidate = weight + f[j];
                    options.push_back("S_" + to_string(current) + to_string(j) + 
                                    " + f_" + to_string(j) + " = " + to_string(weight) + 
                                    " + " + to_string(f[j]) + " = " + to_string(candidate));
                    
                    if (candidate == f[current]) {
                        next_vertex = j;
                        break;
                    }
                    
                    if (candidate < min_val) {
                        min_val = candidate;
                        next_vertex = j;
                    }
                }
            }
        }
        
        if (next_vertex == -1) {
            break;
        }
        
        
        path.push_back(next_vertex);
        current = next_vertex;
    }
    
    return path;
}

int main() {
    const int n = 7;
    
    vector<Edge> Graph = {
        {1, 2, 3}, {1, 3, 8}, {1, 4, 4},
        {2, 3, 4}, {2, 6, 6},
        {3, 5, 7}, {3, 6, 6}, {3, 7, 8},
        {4, 3, 10}, {4, 5, 9},
        {5, 7, 2}, 
        {6, 7, 4}
    };
        
    int start = 1;
    int end = 7;
        
    vector<int> f = solveIterative(start, end, Graph, n);

    
    vector<int> path = reconstructPath(start, end, f, Graph, n);
    
    cout << "Путь: ";
    for (size_t i = 0; i < path.size(); i++) {
        cout << path[i];
        if (i != path.size() - 1) {
            cout << " -> ";
        }
    }
    cout << "\nОбщая длина: " << f[start] << endl;
    
    return 0;
}