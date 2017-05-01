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
    system(command.c_str());
    
    FILE *f = fopen(edmonds_out_filename.c_str(), "r");

    int total_cost;
    fscanf(f, "%i", &total_cost);

    long long int x, y;
    vector<pair<int,int> > matching;
    
    while (fscanf(f, "%lli %lli", &x, &y) > 0) {
        matching.push_back(make_pair(x-1,y-1));
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

    edmonds_in_filename = ids + "_edm.in";
    edmonds_out_filename = ids + "_edm.out";
    edmonds_err_filename = ids + "_edm.errlog";
 
    cerr << "inicializuj() end" << endl;
}

vector<Prikaz> generate_movements( const masked_game_state &_state, 
        const mapa &_m, 
        vector<pair<int,int> > robots,
        vector<vector<pair<int,int> > > edges,
        vector<vector<int> > costs
    ) {
    // ....
    vector<Prikaz> instructions;

    map<pair<int,int> , int> points;

    int n = robots.size();
    int m = 0;

    vector<pair<int,int> > ed_edges;
    vector<int> ed_costs;

    for (int i = 0; i < robots.size(); i++) {
        for (int j = 0; j < edges[i].size(); j++) {
            if (points.find(edges[i][j]) == points.end()) {
                points[edges[i][j]] = points.size();
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

    vector<pair<int, int> > matching = run_edmonds(n + points.size(), m, ed_edges, ed_costs);

    for (int i = 0; i < matching.size(); i++) {
        if (matching[i].first > matching[i].second) {
            swap(matching[i].first, matching[i].second);
        }
        // first contains robot, second contains point
        const pair<int,int> robot = robots[matching[i].first];
        const pair<int,int> target = p2coords[matching[i].second];

        if (robot == target) {
            // do nothing
        }
        else {
            for (int k = 0; k < 4; k++) {
                const int nx = robot.first + dx[k];
                const int ny = robot.second + dy[k];
                if (nx == target.first && ny == target.second) {
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

vector<Prikaz> mass_producing(masked_game_state state, mapa m) {
    //...
    vector<Prikaz> instructions;

    vector<pair<int,int> > forbidden_places;



    return instructions;
}

// main() zavola tuto funkciu, ked nacita novy stav hry, a ocakava instrukcie
// tato funkcia ma vratit vector prikazov
vector<Prikaz> zistiTah() {
    cerr << "kolo " << _stav.kolo << " zistiTah() begin" << endl;
    vector<Prikaz> instrukcie;
    //  (sem patri vas kod)
    instrukcie = mass_producing(_stav, _m);
    
    cerr << "zistiTah() end" << endl;
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
