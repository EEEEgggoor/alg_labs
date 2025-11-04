#include <iostream>
#include <algorithm>
#include <limits>
#include <vector>
#include <chrono>

using namespace std;
using namespace std::chrono;

typedef struct Edge{
    int in, out, weight;
} edge;

void deykstra(vector<edge> Graph, int count_versh, int start_versh = 1){
    const int INF = numeric_limits<int>::max();
    
    vector<vector<int>> adj_matrix(count_versh + 1, vector<int>(count_versh + 1, INF));

    for (int i = 1; i <= count_versh; i++) {
        adj_matrix[i][i] = 0;
    }


    for (const auto& e : Graph) {
        adj_matrix[e.in][e.out] = e.weight;
        adj_matrix[e.out][e.in] = e.weight;
    }
    
    vector<int> dist(count_versh + 1, INF);
    vector<bool> visited(count_versh + 1, false);
    vector<int> prev(count_versh + 1, -1);

    dist[start_versh] = 0;

    for (int i = 1; i <= count_versh; i++){
        int min_dist = INF;
        int curr_versh = -1;

        for (int j = 1; j <= count_versh; j++){
            if(!visited[j] && dist[j] < min_dist){
                min_dist = dist[j];
                curr_versh = j;
            }
        }


        if (curr_versh == -1) break;
        
        visited[curr_versh] = true;

        //neigh - соседние вершины
        for (int neigh = 1; neigh <= count_versh; neigh++){
            if(!visited[neigh] && adj_matrix[curr_versh][neigh]!=INF){
                int new_dist = dist[curr_versh] + adj_matrix[curr_versh][neigh];
                if (new_dist < dist[neigh]){
                    dist[neigh] = new_dist;
                    prev[neigh] = curr_versh;
                }
            }
        }
    }

    cout << "Кратчайшие расстояния от вершины " << start_versh << ":\n";
    cout << "Вершина\tРасстояние\tПуть\n";
    
    for (int i = 1; i<=count_versh; i++){
        cout << i << "\t";
        if (dist[i] == INF) cout<< "--" << "\t";
        else{
            cout << dist[i] << "\t\t";
        }
        vector<int> path;
        for (int v = i; v != -1; v = prev[v]) {
            path.push_back(v);
        }
        reverse(path.begin(), path.end());
        for (size_t j = 0; j < path.size(); j++) {
            cout << path[j];
            if (j < path.size() - 1) cout << " - ";
        }
        cout << endl;
    }

}

int main(){
        vector<edge> Graph = {{1, 2, 6},
                            {1, 4, 1},
                            {1, 6, 10},
                            {2, 4, 1},
                            {2, 5, 1}, 
                            {2, 3, 12}, 
                            {3, 5, 1}, 
                            {3, 8, 1},
                            {4, 5, 4}, 
                            {4, 6, 6}, 
                            {4, 7, 7},
                            {5, 7, 5},
                            {5, 8, 8}, 
                            {6, 7, 0}, 
                            {7, 8, 2} };


    auto start_timer = high_resolution_clock::now();

    deykstra(Graph, 8);
    auto end_timer = high_resolution_clock::now();



    auto duration = duration_cast<microseconds>(end_timer - start_timer);


    cout << "\n Время работы: " << duration.count() << endl;

    return 0;
}