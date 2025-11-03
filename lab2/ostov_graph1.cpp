#include <bits/stdc++.h>
using namespace std;

struct Edge {
    int w, u, v; // вес, вершина1, вершина2
};


int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int count_nodes = 8;

    // список ребер: (вес, вершина1, вершина2)
vector<Edge> all_edges = {
    {2, 1, 2}, {4, 1, 4}, {6, 1, 7},
    {1, 2, 4}, {8, 2, 8}, {2, 2, 5}, {1, 2, 3},
    {7, 3, 5},
    {1, 4, 7}, {5, 4, 6},
    {2, 5, 7}, {2, 5, 8},
    {1, 6, 7}, {2, 7, 8}

};


    // сортировка ребер по весу (возрастание)
    sort(all_edges.begin(), all_edges.end(), [](const Edge &a, const Edge &b){
        return a.w < b.w;
    });

    // adjacent_nodes[i] - список вершин, достижимых из i (сначала только i)
    vector<vector<int>> adjacent_nodes(count_nodes + 1);
    for (int i = 1; i <= count_nodes; ++i) adjacent_nodes[i].push_back(i);

    vector<Edge> tree_edges;
    int sum_tree_edges = 0;

    auto t1 = chrono::steady_clock::now();

    for (const auto &edge : all_edges) {
        int u = edge.u;
        int v = edge.v;
        // проверяем, принадлежит ли v компоненте u
        bool in_same = (find(adjacent_nodes[u].begin(), adjacent_nodes[u].end(), v) != adjacent_nodes[u].end());
        if (!in_same) {
            // объединяем списки достижимости
            vector<int> merged = adjacent_nodes[u];
            merged.insert(merged.end(), adjacent_nodes[v].begin(), adjacent_nodes[v].end());

            // обновляем запись для всех вершин в объединённой компоненте
            for (int node : merged) {
                adjacent_nodes[node] = merged; // копирование вектором
            }

            tree_edges.push_back(edge);
            sum_tree_edges += edge.w;

            if ((int)adjacent_nodes[1].size() == count_nodes) break; // все вершины соединены
        }
    }

    auto t2 = chrono::steady_clock::now();
    chrono::duration<double> dur = t2 - t1;

    if ((int)adjacent_nodes[1].size() < count_nodes) {
        cout << "Остов нельзя построить";
    } else {
        cout << "Остов (по Краскалу): \n";
        int n = 1;
        for (const auto &e : tree_edges) {
            cout << n << ") \n" << e.u << "-" << e.v << " (вес = " << e.w << ")\n";
            ++n;
        }
        cout << " ";
        cout << "Вес остова = " << sum_tree_edges << " ";
        cout << "Время работы алгоритма Краскала: " << fixed << setprecision(6) << dur.count() << " c." << " ";
        cout << string(80, '_') << " ";
    }


    

    return 0;
}
