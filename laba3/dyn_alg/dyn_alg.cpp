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

vector<vector<int>> abjMatrix(const vector<Edge>& edges, int n) {
    vector<vector<int>> adjMatrix(n + 1, vector<int>(n + 1, INF));
    
    for (int i = 1; i <= n; i++) {
        adjMatrix[i][i] = 0;
    }
    
    for (const auto& edge : edges) {
        adjMatrix[edge.from][edge.to] = edge.weight;
    }
    
    return adjMatrix;
}

vector<int> solveIterative(int start, int end, const vector<Edge>& Graph, int n) {


    vector<vector<int>> adjMatrix = abjMatrix(Graph, n);



    vector<int> f(n + 1, INF);
    vector<int> f_prev(n + 1, INF);
    
    // Базовый случай: путь до самой себя равен 0
    f_prev[end] = 0;
    f[end] = 0;
    
    // Итерационный процесс
    for (int iteration = 1; iteration <= n; iteration++) {
        for (int i = 1; i <= n; i++) {
            if (i == end) {
                f[i] = 0;
                continue;
            }
            
            int min_val = INF;
            
            // Вычисляем f_i^(k+1) = min{S_ij + f_j^(k)} для всех j
            for (int j = 1; j <= n; j++) {
                if (i != j && adjMatrix[i][j] != INF && f_prev[j] != INF) {
                    int candidate = adjMatrix[i][j] + f_prev[j];
                    if (candidate < min_val) {
                        min_val = candidate;
                    }
                }
            }
            
            f[i] = min_val;
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

vector<int> reconstructPath(int start, int end, const vector<int>& f, const vector<Edge>& Graph, int n) {
    vector<int> path;
    int current = start;
    path.push_back(current);

    vector<vector<int>> adjMatrix = abjMatrix(Graph, n);

    
    while (current != end) {
        int next_vertex = -1;
        
        // Ищем следующую вершину, для которой S_current,j + f_j = f_current
        for (int j = 1; j <= n; j++) {
            if (current != j && adjMatrix[current][j] != INF && f[j] != INF) {
                int candidate = adjMatrix[current][j] + f[j];
                if (candidate == f[current]) {
                    next_vertex = j;
                    break;
                }
            }
        }
        
        if (next_vertex == -1) {
            // Если не нашли точного соответствия, ищем минимальный вариант
            int min_val = INF;
            for (int j = 1; j <= n; j++) {
                if (current != j && adjMatrix[current][j] != INF && f[j] != INF) {
                    int candidate = adjMatrix[current][j] + f[j];
                    if (candidate < min_val) {
                        min_val = candidate;
                        next_vertex = j;
                    }
                }
            }
        }
        
        if (next_vertex == -1 || next_vertex == current) {
            break; // Нет пути или зацикливание
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
    
    
    cout << "Кратчайшие расстояния от вершин до " << end << ":\n";
    for (int i = 1; i <= n; i++) {
        cout << "f[" << i << "] = ";
        if (f[i] == INF) {
            cout << "INF";
        } else {
            cout << f[i];
        }
        cout << endl;
    }
    cout << endl;
    
    cout << "Кратчайший путь от " << start << " до " << end << ": ";
    for (size_t i = 0; i < path.size(); i++) {
        cout << path[i];
        if (i != path.size() - 1) {
            cout << " -> ";
        }
    }
    cout << "\n Общая длина: " << f[start] << endl;
    
    return 0;
}