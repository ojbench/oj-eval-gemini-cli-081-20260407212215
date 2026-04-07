#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <algorithm>

using namespace std;

const int MAXN = 3005;
const int MAXM = 4505;

struct Edge {
    int to;
    int id;
    int next;
} edges[MAXM * 2];

int head[MAXN], edge_cnt = 0;

void add_edge(int u, int v, int id) {
    edges[edge_cnt] = {v, id, head[u]};
    head[u] = edge_cnt++;
    edges[edge_cnt] = {u, id, head[v]};
    head[v] = edge_cnt++;
}

int tin[MAXN], low[MAXN];
int visited[MAXN];
int is_bridge[MAXM];
int run_id = 0;
int timer = 0;
int disabled_edge = -1;

void dfs(int v, int p_edge) {
    visited[v] = run_id;
    tin[v] = low[v] = timer++;
    for (int i = head[v]; i != -1; i = edges[i].next) {
        int to = edges[i].to;
        int id = edges[i].id;
        if (id == p_edge || id == disabled_edge) continue;
        if (visited[to] == run_id) {
            low[v] = min(low[v], tin[to]);
        } else {
            dfs(to, id);
            low[v] = min(low[v], low[to]);
            if (low[to] > tin[v]) {
                is_bridge[id] = run_id;
            }
        }
    }
}

int comp_visited[MAXN];
int q[MAXN];
uint64_t hash_val[MAXN];

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    int n, m;
    if (!(cin >> n >> m)) return 0;

    for (int i = 1; i <= n; ++i) {
        head[i] = -1;
    }

    for (int i = 0; i < m; ++i) {
        int u, v;
        cin >> u >> v;
        add_edge(u, v, i);
    }

    long long total_sum = 0;

    // 1. Connected components
    run_id++;
    for (int i = 1; i <= n; ++i) {
        if (comp_visited[i] != run_id) {
            int head_q = 0, tail_q = 0;
            q[tail_q++] = i;
            comp_visited[i] = run_id;
            while (head_q < tail_q) {
                int u = q[head_q++];
                for (int j = head[u]; j != -1; j = edges[j].next) {
                    int to = edges[j].to;
                    if (comp_visited[to] != run_id) {
                        comp_visited[to] = run_id;
                        q[tail_q++] = to;
                    }
                }
            }
            long long S = tail_q;
            total_sum += S * (S - 1) / 2;
        }
    }

    // 2. 2-edge-connected components
    run_id++;
    timer = 0;
    disabled_edge = -1;
    for (int i = 1; i <= n; ++i) {
        if (visited[i] != run_id) {
            dfs(i, -1);
        }
    }

    run_id++;
    for (int i = 1; i <= n; ++i) {
        if (comp_visited[i] != run_id) {
            int head_q = 0, tail_q = 0;
            q[tail_q++] = i;
            comp_visited[i] = run_id;
            while (head_q < tail_q) {
                int u = q[head_q++];
                for (int j = head[u]; j != -1; j = edges[j].next) {
                    int to = edges[j].to;
                    int id = edges[j].id;
                    if (is_bridge[id] == run_id - 1) continue;
                    if (comp_visited[to] != run_id) {
                        comp_visited[to] = run_id;
                        q[tail_q++] = to;
                    }
                }
            }
            long long S = tail_q;
            total_sum += S * (S - 1) / 2;
        }
    }

    // 3. 3-edge-connected components
    mt19937_64 rng(1337);
    for (int e = -1; e < m; ++e) {
        disabled_edge = e;
        run_id++;
        timer = 0;
        for (int i = 1; i <= n; ++i) {
            if (visited[i] != run_id) {
                dfs(i, -1);
            }
        }

        run_id++;
        for (int i = 1; i <= n; ++i) {
            if (comp_visited[i] != run_id) {
                uint64_t R = rng();
                int head_q = 0, tail_q = 0;
                q[tail_q++] = i;
                comp_visited[i] = run_id;
                while (head_q < tail_q) {
                    int u = q[head_q++];
                    hash_val[u] ^= R;
                    for (int j = head[u]; j != -1; j = edges[j].next) {
                        int to = edges[j].to;
                        int id = edges[j].id;
                        if (id == disabled_edge || is_bridge[id] == run_id - 1) continue;
                        if (comp_visited[to] != run_id) {
                            comp_visited[to] = run_id;
                            q[tail_q++] = to;
                        }
                    }
                }
            }
        }
    }

    vector<uint64_t> hashes(n);
    for (int i = 1; i <= n; ++i) {
        hashes[i - 1] = hash_val[i];
    }
    sort(hashes.begin(), hashes.end());

    long long current_count = 1;
    for (int i = 1; i < n; ++i) {
        if (hashes[i] == hashes[i - 1]) {
            current_count++;
        } else {
            total_sum += current_count * (current_count - 1) / 2;
            current_count = 1;
        }
    }
    total_sum += current_count * (current_count - 1) / 2;

    cout << total_sum << "\n";

    return 0;
}
