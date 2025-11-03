#include <bits/stdc++.h>
using namespace std;

struct Edge {
    int w, u, v;
};

Edge get_min_edge(const vector<Edge>& all_edges, const unordered_set<int>& added_nodes) {
    const int INF = INT_MAX;
    Edge best{INF, -1, -1};
    for (const auto &e : all_edges) {
        bool in_u = (added_nodes.find(e.u) != added_nodes.end());
        bool in_v = (added_nodes.find(e.v) != added_nodes.end());
        // ровно одна вершина принадлежит added_nodes
        if (in_u ^ in_v) {
            if (e.w < best.w) best = e;
        }
    }
    return best;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int count_nodes = 8;

    vector<Edge> all_edges = {
        {2, 1, 2}, {4, 1, 4}, {6, 1, 7},
        {1, 2, 4}, {8, 2, 8}, {2, 2, 5}, {1, 2, 3},
        {7, 3, 5},
        {1, 4, 7}, {5, 4, 6},
        {2, 5, 7}, {2, 5, 8},
        {1, 6, 7}, {2, 7, 8}
    };

    unordered_set<int> added_nodes;
    added_nodes.insert(1); // начинаем с вершины 1

    vector<Edge> tree_edges;
    int sum_tree_edges = 0;

    auto t1 = chrono::steady_clock::now();

    while ((int)added_nodes.size() < count_nodes) {
        Edge e = get_min_edge(all_edges, added_nodes);
        if (e.w == INT_MAX) { // не найдено подходящего ребра
            break;
        }
        tree_edges.push_back(e);
        sum_tree_edges += e.w;
        added_nodes.insert(e.u);
        added_nodes.insert(e.v);
    }

    auto t2 = chrono::steady_clock::now();
    chrono::duration<double> dur = t2 - t1;

    if ((int)added_nodes.size() < count_nodes) {
        cout << "Остов нельзя построить\n";
    } else {
        cout << "Остов (по Приму):\n";
        int n = 1;
        for (const auto &i : tree_edges) {
            cout << n << ") " << i.u << "-" << i.v << " (вес = " << i.w << ")\n";
            ++n;
        }
        cout << "\n";
        cout << "Вес остова = " << sum_tree_edges << "\n\n";
        cout << "Время работы алгоритма Прима: " << fixed << setprecision(6) << dur.count() << " c.\n";
    }

    return 0;
}
