#include <bits/stdc++.h>
using namespace std;
using ll = long long;
const int oo = 1000000000;

struct Node {
    vector<vector<int>> mat;
    ll cost;             // нижняя оценка (lower bound)
    int depth;           // количество включённых рёбер
    vector<int> next;    // next[u] = v, если ребро u->v включено; -1 иначе

};

struct CompareNode {
    bool operator()(Node const& a, Node const& b) const {
        return a.cost > b.cost; // очередь — минимальная по lower bound
    }
};


void printMatrix(const std::vector<std::vector<int>>& matrix) {
    for (const auto& row : matrix) {
        for (int val : row) {
            if (val == 1000000000)
            {
                std::cout << "oo" << " ";
            }
            else
            {
                std::cout << val << " ";
            }
        }
        std::cout << "\n";
    }
}

// Приведение матрицы: вычитает минимумы по строкам и столбцам.
// Возвращает сумму всех вычитаний. Матрица mat модифицируется.
ll reduce_matrix(vector<vector<int>>& mat) {
    int n = (int)mat.size();
    ll sum = 0;
    // приведение по строкам
    for (int i = 0; i < n; ++i) {
        int mn = oo;
        for (int j = 0; j < n; ++j) if (mat[i][j] < mn) mn = mat[i][j];
        if (mn == oo || mn == 0) continue;
        for (int j = 0; j < n; ++j) if (mat[i][j] < oo) mat[i][j] -= mn;
        sum += mn;
    }
    // приведение по столбцам
    for (int j = 0; j < n; ++j) {
        int mn = oo;
        for (int i = 0; i < n; ++i) if (mat[i][j] < mn) mn = mat[i][j];
        if (mn == oo || mn == 0) continue;
        for (int i = 0; i < n; ++i) if (mat[i][j] < oo) mat[i][j] -= mn;
        sum += mn;
    }
    return sum;
}

// Найти ноль (i,j) с максимальным штрафом (rowMin + colMin)
bool find_max_penalty_zero(const vector<vector<int>>& mat, int &out_i, int &out_j) {
    int n = (int)mat.size();
    ll bestPenalty = -1;
    bool found = false;
    for (int i = 0; i < n; ++i) for (int j = 0; j < n; ++j) {
        if (mat[i][j] != 0) continue;

        // минимальный элемент в строке i, исключая столбец j
        int rowMin = oo;
        for (int jj = 0; jj < n; ++jj) if (jj != j) rowMin = min(rowMin, mat[i][jj]);

        // минимальный элемент в столбце j, исключая строку i
        int colMin = oo;
        for (int ii = 0; ii < n; ++ii) if (ii != i) colMin = min(colMin, mat[ii][j]);

        ll penalty = (ll)((rowMin==oo?oo:rowMin) + (colMin==oo?oo:colMin));

        // если строка или столбец имеют только ∞ (ребро недоступно)
        if (rowMin==oo || colMin==oo) {
            penalty = oo;
        }

        if (!found || penalty > bestPenalty) {
            found = true;
            bestPenalty = penalty;
            out_i = i; out_j = j;
        }
    }
    return found;
}

// Проверка наличия подтура (цикла) длиной < n в массиве next.
// next[u] = v или -1. Возвращает true, если есть подтур.
bool has_subtour(const vector<int>& next) {
    int n = (int)next.size();
    vector<int> used(n, 0);
    for (int i = 0; i < n; ++i) {
        if (used[i]) continue;
        int cur = i;
        unordered_map<int,int> idx;
        int steps = 0;
        while (cur != -1 && !used[cur]) {
            if (idx.count(cur)) { // цикл найден
                int startIdx = idx[cur];
                int cycleLen = steps - startIdx;
                if (cycleLen < n) return true; // подтур найден
                break;
            }
            idx[cur] = steps++;
            used[cur] = 1;
            cur = next[cur];
        }
    }
    return false;
}

// Восстановить тур из next[], если получен полный цикл.
// Возвращает вектор вершин или пустой вектор, если цикл неправильный.
vector<int> reconstruct_tour(const vector<int>& next) {
    int n = (int)next.size();
    int start = -1;
    for (int i = 0; i < n; ++i) if (next[i] != -1) { start = i; break; }
    if (start == -1) return {};
    vector<int> res;
    vector<int> used(n, 0);
    int cur = start;
    for (int k = 0; k < n; ++k) {
        if (cur == -1 || used[cur]) return {}; // некорректный путь
        res.push_back(cur);
        used[cur] = 1;
        cur = next[cur];
    }
    if (cur != start) return {}; // цикл должен замыкаться
    return res;
}

// Алгоритм ветвей и границ (branch and bound) — возвращает пару (стоимость, тур)
pair<ll, vector<int>> branch_and_bound_tsp(const vector<vector<int>>& input) {
    int n = (int)input.size();

    // формируем начальную матрицу
    vector<vector<int>> base = input;



    // заменяем отрицательные веса на oo (отсутствующие рёбра)
    for (int i=0;i<n;++i) for (int j=0;j<n;++j) if (base[i][j] < 0) base[i][j]=oo;

    cout<<"НАЧ---------------------------------------"<<endl;

    printMatrix(base);


    Node root;
    root.mat = base;
    root.next.assign(n, -1);
    root.depth = 0;
    root.cost = reduce_matrix(root.mat);

    priority_queue<Node, vector<Node>, CompareNode> pq;
    pq.push(root);

    ll bestCost = LLONG_MAX;
    vector<int> bestTour;

    size_t nodesProcessed = 0; // ограничение на количество узлов (практическая защита)

    while (!pq.empty()) {
        Node cur = pq.top(); pq.pop();
        nodesProcessed++;

        if (cur.cost >= bestCost) continue; // отсечение по нижней оценке

        // если уже включено n рёбер → пробуем восстановить полный тур
        if (cur.depth == n) {
            auto tour = reconstruct_tour(cur.next);
            if (!tour.empty()) {
                ll total = 0;
                bool ok = true;
                for (int i = 0; i < n; ++i) {
                    int u = tour[i], v = tour[(i+1)%n];
                    if (input[u][v] < 0 || input[u][v] >= oo) { ok=false; break; }
                    total += input[u][v];
                }
                if (ok && total < bestCost) {
                    bestCost = total;
                    bestTour = tour;
                }
            }
            continue;
        }

        // если невозможно найти нуль — ветка завершается
        int zi=-1, zj=-1;
        if (!find_max_penalty_zero(cur.mat, zi, zj)) continue;

        // ВЕТКА 1: ВКЛЮЧИТЬ ребро (zi, zj)
        {
            cout<<"ВКЛ---------------------------------------"<<endl;

            Node child = cur;
            // включение ребра означает: строку zi и столбец zj → oo
            for (int k = 0; k < n; ++k) {
                child.mat[zi][k] = oo;
                child.mat[k][zj] = oo;
            }
            printMatrix(child.mat);


            // запрещаем обратное ребро (zj → zi), чтобы не получить цикл из двух вершин
            child.mat[zj][zi] = oo;

            // обновляем next
            child.next[zi] = zj;
            child.depth = cur.depth + 1;

            // проверка на подтуры
            if (!has_subtour(child.next)) {
                ll add = reduce_matrix(child.mat);
                child.cost = cur.cost + add;
                if (child.cost < bestCost) pq.push(child);
            }
            cout << "(" << zi+1 << ";" << zj+1 <<")"<<endl;
            cout << cur.cost<<endl;
            // for (int t : cur.next) {cout << t << " ";}
            cout<<endl;
            cout<<endl;
            printMatrix(child.mat);
            cout<<endl;
            cout<<"---------------------------------------"<<endl;

        }

        // ВЕТКА 2: ИСКЛЮЧИТЬ ребро (zi, zj)
        {   

            cout<<"ИСКЛ---------------------------------------"<<endl;

            Node child = cur;
            printMatrix(child.mat);

            child.mat[zi][zj] = oo;
            ll add = reduce_matrix(child.mat);
            child.cost = cur.cost + add;
            if (child.cost < bestCost) pq.push(child);
            cout << "*(" << zi+1 << ";" << zj+1 <<")"<<endl;

            cout << cur.cost<<endl;
            // for (int t : cur.next) {cout << t << " ";}
            cout<<endl;
            cout<<endl;
            printMatrix(child.mat);
            cout<<endl;
            cout<<endl;
            cout<<"---------------------------------------"<<endl;


        }

        if (nodesProcessed > 2000000) break; // защитное ограничение
    }

    return { bestCost == LLONG_MAX ? -1 : bestCost, bestTour };
}



int main() {
    vector<vector<int>> mat = {
        {-1, 23, 47, -1, 62, 38},
        {51, -1, 74, 19, 88, 33},
        {67, 42, -1, 56, -1, 91},
        {29, 71, 14, -1, 83, 52},
        {64, -1, 87, 95, -1, 46},
        {92, 57, 26, 73, 68, -1}
    };

    int n = 6;

    auto res2 = branch_and_bound_tsp(mat);
    if (res2.first < 0) cout << "Нет допустимого тура или рёбра отсутствуют\n";
    else {
        cout << "Тур " << res2.first << ": ";
        if (!res2.second.empty()) {
            for (size_t i=0;i<res2.second.size();++i) {
                if (i) cout << " -> ";
                cout << res2.second[i]+1;
            }
            cout << " -> " << res2.second[0]+1 << "\n";
        }
        else {
            cout << "Маршрут получен, но порядок вершин не удалось восстановить\n";
        }
    }
    return 0;
}
