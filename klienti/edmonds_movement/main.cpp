#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>
#include <cassert>
#include <map>
#include <functional>
#include <algorithm>
using namespace std;

#include "common.h"
#include "marshal.h"
#include "update.h"


#define DEBUG

#ifdef DEBUG
    #define dbg true
#elif
    #define dbg false
#endif

mapa _m;
masked_game_state _stav;   // vzdy som hrac cislo 0

int dx[4] = {0,0,0,0};
int dy[4] = {0,0,0,0};

int highest_seen_robot = 0;

string edmonds_in_filename = "";
string edmonds_out_filename = "";
string edmonds_err_filename = "";

vector<pair<int,int> > run_greedy_matching(const int n, const int m, const vector<pair<int,int> > &edges, const vector<int> &costs) {
    vector<pair<int,int> > matching;
    if (m == 0) return matching;

    priority_queue<pair<int, int>,vector<pair<int,int> > , greater<pair<int,int> > > Q;
    for(int i = 0; i < edges.size(); i++) {
        Q.push(make_pair(3 * costs[i] + rand()%3, i));
    }

    set<int> settled_points;
    while (!Q.empty() && settled_points.size() < n-1) {
        const int c = Q.top().first;
        const int q = Q.top().second;

        Q.pop();

        if (settled_points.find(edges[q].first) != settled_points.end() || settled_points.find(edges[q].second) != settled_points.end()){
            continue;
        }
        settled_points.insert(edges[q].first);
        settled_points.insert(edges[q].second);
        matching.push_back(edges[q]);
    }

    return matching;
}

vector<pair<int, int> > run_edmonds(const int n, const int m, const vector<pair<int,int> > &edges, const vector<int> &costs) {
    // zero notation on input and output

    ofstream infile;
    infile.open(edmonds_in_filename);
    infile << n << " " << m << endl;
    for (int i = 0; i < edges.size(); i++) {
        infile << edges[i].first + 1 << " " << edges[i].second + 1 << " " << costs[i] << endl;
    }
    infile.close();

    string command = "python3 edmonds.py < " + edmonds_in_filename + " > " + edmonds_out_filename + " 2> " + edmonds_err_filename;
    //system(command.c_str());
    

    vector<pair<int,int> > matching;
    
    FILE* q = popen(command.c_str(), "r");
    pclose(q);


    FILE *f = fopen(edmonds_out_filename.c_str(), "r");

    int total_cost;
    fscanf(f, "%i", &total_cost);
    cerr << total_cost << endl;

    long long int x, y;
    
    while (fscanf(f, "%lli %lli", &x, &y) > 0) {
        matching.push_back(make_pair(x-1,y-1));
        cerr << x << " " << y << endl;
    }
  
    fclose(f);
    
    return matching;
}

vector<pair<int,int> > run_blossomV(const int n, const int m, const vector<pair<int,int> > &edges, const vector<int> &costs) {
    assert(m == edges.size());

    if (m == 0) {
        return vector<pair<int,int> >(0);
    }


    vector<pair<int,int> > ne;
    vector<int> nc;

    for (int i = 0; i < m; i++) {
        ne.push_back(edges[i]);
        nc.push_back(costs[i]);
    }

    for (int i = 0; i < m; i++) {
        ne.push_back(make_pair(edges[i].first + n, edges[i].second + n));
        nc.push_back(costs[i]);
    }

    for (int i = 0; i < n; i++) {
        ne.push_back(make_pair(i, i + n));
        nc.push_back(0);
    }

    ofstream infile;
    infile.open(edmonds_in_filename);
    infile << 2 * n << " " << 2 * m + n << endl;
    for (int i = 0; i < ne.size(); i++) {
        infile << ne[i].first << " " << ne[i].second << " " << nc[i] << endl;
    }
    infile.close();

    string command = "./blossom5-v2.05.src/blossom5 -e " + edmonds_in_filename + " -w " + edmonds_out_filename + " 2> " + edmonds_err_filename + " >/dev/null < /dev/full";
    FILE* q = popen(command.c_str(), "w");
    pclose(q);
    //system(command.c_str());

    int n_e, total_cost;
    FILE *f = fopen(edmonds_out_filename.c_str(), "r");

    fscanf(f, "%i %i", &n_e, &total_cost);
    cerr << total_cost <<  " " << n_e << endl;

    long long int x, y;
    vector<pair<int,int> > matching;
    
    for (int i = 0; i < n_e; i++) {
        fscanf(f, "%lli %lli", &x, &y);
        if (x < n && y < n) {
            matching.push_back(make_pair(x, y));
        }
    }
  
    fclose(f);
    
    return matching;
}

// main() zavola tuto funkciu, ked nacita mapu
void inicializuj() {
    cerr << "inicializuj()" << endl;
    // (sem patri vas kod)

    // smery
    // vyska/sirka <-> riadok/stlpec
    tie(dx[DOLE], dy[DOLE])     = make_pair(+1, 0);
    tie(dx[VLAVO], dy[VLAVO])   = make_pair(0, -1);
    tie(dx[VPRAVO], dy[VPRAVO]) = make_pair(0, +1);
    tie(dx[HORE], dy[HORE])     = make_pair(-1, 0);

    string ids = to_string(getpid());
    //string ids = "a";

    edmonds_in_filename = "./edm_log/" + ids + "_edm.in";
    edmonds_out_filename = "./edm_log/" + ids + "_edm.out";
    edmonds_err_filename = "./edm_log/" +  ids + "_edm.errlog";
 
    cerr << "inicializuj() end" << endl;
}

vector<Prikaz> generate_movements( const masked_game_state &_state, const mapa &_m, vector<pair<int,int> > robots, vector<vector<pair<int,int> > > edges, vector<vector<int> > costs
    ) {
    // ....
    if (0) {
        cerr << "GENERATE MOVEMENTS:" << endl;
        cerr << "robots: " << endl;
        for(int i = 0; i < robots.size(); i++) {
            cerr << robots[i].first << ", " << robots[i].second << ":\t";
            for (int j = 0; j < edges[i].size(); j++) {
                cerr << "(" << edges[i][j].first << " " << edges[i][j].second << "[" << costs[i][j] << "]" <<  "), ";
            }
            cerr << endl;
        }
    }

    vector<Prikaz> instructions;

    map<pair<int,int> , int> points;

    const int n = robots.size();
    cerr << "n: " << n << endl;
    int m = 0;
    int point_count = 0;

    vector<pair<int,int> > ed_edges;
    vector<int> ed_costs;

    for (int i = 0; i < robots.size(); i++) {
        for (int j = 0; j < edges[i].size(); j++) {
            if (points.find(edges[i][j]) == points.end()) {
                points[edges[i][j]] = point_count;
                point_count++;
            }
            ed_edges.push_back(make_pair(i, points[ edges[i][j] ] + n));
            ed_costs.push_back(costs[i][j]);
            m++;
        }
    }

    vector<pair<int,int> > p2coords(points.size());
    for (auto it = points.begin(); it != points.end(); it++) {
        p2coords[it->second] = it->first;
    }

    //vector<pair<int, int> > matching = run_edmonds(n + points.size(), m, ed_edges, ed_costs);
    vector<pair<int, int> > matching = run_blossomV(n + points.size(), m, ed_edges, ed_costs);
    //vector<pair<int, int> > matching = run_greedy_matching(n + points.size(), m, ed_edges, ed_costs);

    for (int i = 0; i < matching.size(); i++) {
        if (matching[i].first > matching[i].second) {
            swap(matching[i].first, matching[i].second);
        }
        // first contains robot, second contains point
        assert(0 <= matching[i].first && matching[i].first < n);
        assert(n <= matching[i].second && matching[i].second < n + points.size());
        const pair<int,int> robot = robots[matching[i].first];
        assert(_state.mapa[robot.first][robot.second].majitel == 0 && _state.mapa[robot.first][robot.second].sila_robota > 0);
        const pair<int,int> target = p2coords[matching[i].second-n];

        if (robot == target) {
            // do nothing
        }
        else {
            for (int k = 0; k < 4; k++) {
                const int nx = robot.first + dx[k];
                const int ny = robot.second + dy[k];
                if (nx == target.first && ny == target.second && !(_state.mapa[nx][ny].majitel == 0 && _state.mapa[nx][ny].sila_robota > 0)) {
                    Prikaz p;
                    p.pr = POSUN;
                    p.riadok = robot.first; p.stlpec = robot.second;
                    p.instrukcia = static_cast<smer>(k);
                    instructions.push_back(p);
                }
            }
        }
    }
    return instructions;
}

vector<pair<int,int> > get_my_robots(const masked_game_state &state) {
    vector<pair<int, int> > robots;
    for (int i = 0; i < state.vyska; i++) {
        for (int j = 0; j < state.sirka; j++) {
            if (state.mapa[i][j].sila_robota > 0 && state.mapa[i][j].majitel == 0) {
                robots.push_back(make_pair(i, j));
            }
        }
    }
    return robots;
}

vector<pair<int, int> > get_my_labs(const masked_game_state &state, const mapa &m) {
    vector<pair<int, int> > labs;
    for (int i = 0; i < state.vyska; i++) {
        for (int j = 0; j < state.sirka; j++) {
            if (state.mapa[i][j].majitel == 0 && m.squares[i][j] == LAB) {
                labs.push_back(make_pair(i, j));
            }
        }
    }
    return labs;
}

bool is_square_valid(const int x, const int y, const masked_game_state &state, const mapa &m) {
    // it is possible to step in here (at least theoretically)
    return (0 <= x && x < state.vyska && 0 <= y && y < state.sirka && m.squares[x][y] != KAMEN);
}

bool is_weaker_space(const int x, const int y, const int my_power, const masked_game_state &state, const mapa &m) {
    for (int i = 0; i < 4; i++) {
        const int nx = x + dx[i];
        const int ny = y + dy[i];
        if (is_square_valid(nx, ny, state, m) && state.mapa[nx][ny].majitel != 0 && state.mapa[nx][ny].sila_robota >= my_power) {
            return false;
        }
    }
    return true;
}

int next_power = 1;

vector<Prikaz> mass_producing(const masked_game_state &state, const mapa &m) {
    //...
    vector<Prikaz> instructions;

    if (dbg) { cerr << "zelezo: " << state.zelezo << endl;}

    vector<pair<int,int> > my_robots = get_my_robots(state);
    if (dbg) {
        cerr << "MY ROBOTS:" << endl;
        for (int i = 0; i < my_robots.size(); i++) {
            cerr << "[" << my_robots[i].first << ", " << my_robots[i].second << "] ";
        }
        cerr << ";;" << endl;
    }
    vector<pair<int,int> > my_labs = get_my_labs(state, m);
    if (dbg) {
        cerr << "MY LABS:" << endl;
        for (int i = 0; i < my_labs.size(); i++) {
            cerr << "[" << my_labs[i].first << ", " << my_labs[i].second << "] ";
        }
        cerr << ";;" << endl;
    }

    vector<pair<int,int> > forbidden_places;

    int my_resources = state.zelezo;
    for (int i = 0; i < my_labs.size() && my_resources >= next_power; i++) {
        forbidden_places.push_back(my_labs[i]);
        if (state.mapa[my_labs[i].first][my_labs[i].second].sila_robota != 0) continue;
        Prikaz p;
        p.pr = POSTAV;
        p.riadok = my_labs[i].first; p.stlpec = my_labs[i].second;
        p.instrukcia = next_power;
        instructions.push_back(p);
        cerr << "BUILD ROBOT( " << p.instrukcia << " )" << endl;
        
        my_resources -= next_power;
        //next_power++;
    }

    vector<vector<pair<int,int> > > edges(my_robots.size());
    vector<vector<int> > costs(my_robots.size());
    for (int i = 0; i < my_robots.size(); i++) {
        vector<pair<int,int> > ps;
        vector<int> cs;

        const int x = my_robots[i].first;
        const int y = my_robots[i].second;
        const int sila = state.mapa[x][y].sila_robota;

        // ostat na mieste
        {
            const int nx = x;
            const int ny = y;

            int cost = -1;

            if (find(forbidden_places.begin(), forbidden_places.end(), make_pair(x,y)) != forbidden_places.end()) {
                // toto miesto je zakazane, musime padat
                cost += 100;
            }

            if (! is_weaker_space(nx, ny, sila, state, m)) {
                // niekto v okoli je silnejsi, musime padat
                cost += 10; 
            }

            ps.push_back(make_pair(nx,ny));
            cs.push_back(cost);
        }

        // hybat sa
        for (int k = 0; k < 4; k++) {
            const int nx = x + dx[k];
            const int ny = y + dy[k];

            int cost = -50;

            // je policko validne?
            if (!is_square_valid(nx, ny, state, m)) {
                continue;
            }

            // je niekto v okoli silenjsi>
            if (!is_weaker_space(nx, ny, sila, state, m)) {
                cost += 10;
            }

            // je to policko zakazane?
            if (find(forbidden_places.begin(), forbidden_places.end(), make_pair(nx, ny)) != forbidden_places.end()) {
                cost += 100;
            }

            // je na tom policku iny nas robot?
            if (state.mapa[nx][ny].majitel == 0 && state.mapa[nx][ny].sila_robota > 0) {
                cost += 100;
            }

            ps.push_back(make_pair(nx, ny));
            cs.push_back(cost);
        }

        edges[i] = ps;
        costs[i] = cs;
    }

    vector<Prikaz> movements = generate_movements(state, m, my_robots, edges, costs);

    //instructions.reserve(instructions.size() + movements.size());
    for (int i = 0; i < movements.size(); i++) {
        instructions.push_back(movements[i]);
    }

    return instructions;
}

// main() zavola tuto funkciu, ked nacita novy stav hry, a ocakava instrukcie
// tato funkcia ma vratit vector prikazov
vector<Prikaz> zistiTah() {
    //cerr << "kolo " << _stav.kolo << " zistiTah() begin" << endl;
    cerr << "-------------- KOLO: " << _stav.kolo << " -------------" << endl;
    vector<Prikaz> instrukcie;
    //  (sem patri vas kod)
    instrukcie = mass_producing(_stav, _m);
    
    cerr << "'----               KONIEC KOLA                  ------" << endl;
    cerr.flush();
    return instrukcie;
}


int main() {
    // v tejto funkcii su vseobecne veci, nemusite ju menit (ale mozte).
    
    unsigned int seed = time(NULL) * getpid();
    srand(seed);

    nacitaj(cin, _m);
    fprintf(stderr, "START pid=%d, seed=%u\n", getpid(), seed);
    inicializuj();

    while (cin.good()) {
        nacitaj(cin, _stav);
        uloz(cout, zistiTah());
        cout << "2" << endl;   // 2 a flush = koniec odpovede
    }

    return 0;
}
