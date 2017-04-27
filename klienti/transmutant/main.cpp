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

bool is_square_valid(const int x, const int y, const masked_game_state &state, const mapa &m);

vector<pair<int,int> > run_greedy_matching(const int n, const int m, const vector<pair<int,int> > &edges, const vector<int> &costs) {
    vector<pair<int,int> > matching; matching.reserve(n);
    if (m == 0) return matching;

    //priority_queue<pair<int, int>,vector<pair<int,int> > , greater<pair<int,int> > > Q;
    vector<pair<int,int> > Q;

    for(int i = 0; i < edges.size(); i++) {
        Q.push_back(make_pair(3 * costs[i] + rand()%3, i));
    }
    sort(Q.begin(), Q.end());

    if (0) {
        cerr << "greedy Q:" << endl;
        for (int i = 0; i < Q.size(); i++) {
            cerr << Q[i].first << " " << Q[i].second << " | "; 
        }
        cerr << endl;
    }

    vector<bool> is_settled(n, false);
    int settled_count = 0;

    for (int k = 0; k < Q.size() && settled_count < n-1; k++){
    //while (!Q.empty() && settled_points.size() < n-1) {

        //const int c = Q.top().first;
        //const int q = Q.top().second;

        //Q.pop();

        const int c = Q[k].first;
        const int q = Q[k].second;



        //if (settled_points.find(edges[q].first) != settled_points.end() || settled_points.find(edges[q].second) != settled_points.end()){
        //    continue;
        //}
        if (is_settled[edges[q].first] || is_settled[edges[q].second]) {
            continue;
        }

        //settled_points.insert(edges[q].first);
        //settled_points.insert(edges[q].second);
        is_settled[edges[q].first] = true;
        is_settled[edges[q].second] = true;
        settled_count += 2;
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
    

    vector<pair<int,int> > matching; matching.reserve(n);
    
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


    vector<pair<int,int> > ne; ne.reserve(5 * n);
    vector<int> nc; nc.reserve(5 * n);

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
    vector<pair<int,int> > matching; matching.reserve(n);
    
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
vector<vector<double> > basic_landscape;
vector<vector<bool> > is_visited;
queue<tuple<int,int,int,smer> > Q;
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
 
    basic_landscape = vector<vector<double> > (_m.height, vector<double>(_m.width, 0));
    is_visited = vector<vector<bool> > (_m.height, vector<bool>(_m.width, false));

    cerr << "inicializuj() end" << endl;
}

vector<pair<int,int> > ed_edges;
vector<int> ed_costs;
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

    vector<Prikaz> instructions; instructions.reserve(robots.size());

    map<pair<int,int> , int> points;

    const int n = robots.size();
    cerr << "n: " << n << endl;
    int m = 0;
    int point_count = 0;

    ed_edges.clear(); ed_edges.reserve(5 * robots.size());
    ed_costs.clear(); ed_costs.reserve(5 * robots.size());

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

    if (dbg) {
        cerr << "GRAPH REMAPPING" << endl;
        for (int i = 0; i < p2coords.size(); i++) {
            cerr << i << " " << p2coords[i].first << "  " << p2coords[i].second << endl;
        }
    }


    vector<pair<int,int> > matching;
    //matching = run_edmonds(n + points.size(), m, ed_edges, ed_costs);
    //matching = run_blossomV(n + points.size(), m, ed_edges, ed_costs);
    //matching = run_greedy_matching(n + points.size(), m, ed_edges, ed_costs);

    const int TOO_MANY_ROBOTS = 50;
    if (n > TOO_MANY_ROBOTS) {
        matching = run_greedy_matching(n + points.size(), m, ed_edges, ed_costs);
    }
    else {
        matching = run_blossomV(n + points.size(), m, ed_edges, ed_costs);
    }

    if(dbg) {
        cerr << "MATCHING" << endl;
        for (int i = 0; i < matching.size(); i++) {
            cerr << "(" << matching[i].first << " " << matching[i].second << "), ";
        }
        cerr << endl;
    }


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
                
                if (is_square_valid(nx,ny,_state,_m) && nx == target.first && ny == target.second && !(_state.mapa[nx][ny].majitel == 0 && _state.mapa[nx][ny].sila_robota > 0)) {
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
    vector<pair<int, int> > robots; robots.reserve(30);
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
    vector<pair<int, int> > labs; labs.reserve(5);
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
    return (0 <= x && x < state.vyska-1 && 0 <= y && y < state.sirka && m.squares[x][y] != KAMEN);
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

double l_our_land (const int dist) {
    return 0;
}

double l_our_lab (const int dist) {
    return 0;
}

double l_our_city (const int dist) {
    return 0;
}

double l_foreign_land(const int dist) {
    return 100.0/(10 + dist);
}

double l_foreing_city(const int dist) {
    return 500.0/(10 + dist);
}

double l_foreign_lab(const int dist) {
    return 1000.0/(10 + dist);
}

double l_enemy(const int dist) {
    if (dist < 2) return 0;
    return 200.0/(10 + (dist-2));
}


vector<Prikaz> get_commands_fields(masked_game_state state, mapa m) {
    vector<Prikaz> instructions; instructions.reserve(40);

    
    vector<pair<int, int> > my_robots = get_my_robots(state);
    if (dbg) {
        cerr << "MY ROBOTS:" << endl;
        for (int i = 0; i < my_robots.size(); i++) {
            cerr << "[" << my_robots[i].first << ", " << my_robots[i].second << "] ";
        }
        cerr << ";;" << endl;
    }

    vector<pair<int, int> > my_labs = get_my_labs(state, m);
    if (dbg) {
        cerr << "MY LABS:" << endl;
        for (int i = 0; i < my_labs.size(); i++) {
            cerr << "[" << my_labs[i].first << ", " << my_labs[i].second << "] ";
        }
        cerr << ";;" << endl;
    }

    // decide whether to build new robots (of course yes!)
    vector<pair<int,int> > forbidden_places; forbidden_places.reserve(my_labs.size());

    if (my_labs.size() > 0 && state.zelezo > 0) {
        for (auto it = my_labs.begin(); it != my_labs.end(); it++) {
            forbidden_places.push_back(*it);
        }

        random_shuffle(my_labs.begin(), my_labs.end());

        // vector<int> portions; portions.reserve(my_labs.size());
        // int total = 0;
        // for (int t = 0; t < my_labs.size(); t++) if(state.mapa[my_labs[t].first][my_labs[t].second].sila_robota == 0) {
        //     // maybe try to distribute resources unevenly (based on threat?)
            
        //     if (portions.size() == 0) {
        //         portions.push_back(1);
        //     }
        //     else  if (portions.size() == 1) {
        //         portions.push_back(portions[portions.size() - 1] + 10);
        //     }
        //     else {
        //         portions.push_back(0);
        //     }
        //     total += portions[portions.size() - 1];
            
        // }
        // for (int t = 0; t < portions.size(); t++) {
        //     portions[t] = static_cast<int>(1.00 * portions[t]/total * state.zelezo);
        // }


        int valid_lab_count = 0;
        for (int i = 0; i < my_labs.size(); i++) {
            if (state.mapa[my_labs[i].first][my_labs[i].second].sila_robota == 0) {
                valid_lab_count += 1;
            }
        }

        vector<int> portions(my_labs.size(), 0);
        bool gonna_build_big_stuff = (rand()%7 != 0);
        for (int i = 0; i < my_labs.size(); i++) {
            if (gonna_build_big_stuff) {
                portions[i] = state.zelezo - valid_lab_count + 1;
                gonna_build_big_stuff = false;
            }
            else {
                portions[i] = 1;
            }
        }


        int cc = 0;
        for (int t = 0; t < my_labs.size(); t++) if(state.mapa[my_labs[t].first][my_labs[t].second].sila_robota == 0 && portions[cc] > 0) {
            Prikaz p;
            p.pr = POSTAV;
            p.riadok = my_labs[t].first; p.stlpec = my_labs[t].second;
            p.instrukcia = portions[cc];
            instructions.push_back(p);
            cc++;
        }

    }
    
    // for every left (maybe handpicked) robot estimate the nearest interesting point,
    // pair robots to points and find the best moves to achieve that
    // use Edmonds later
    

    if (state.kolo % 1 == 0){
        for (int i = 0; i < state.vyska; i++) {
            for(int j = 0; j < state.sirka; j++) {
                basic_landscape[i][j] = 0;
            }
        }
        
        while (!Q.empty()) Q.pop();

        for (int a = 0; a < state.vyska; a++) {
            for (int b = 0; b < state.sirka; b++) {
                is_visited[a][b] = false;
            }
        }
        
        for (int i = 0; i < state.vyska; i++) {
            for (int j = 0; j < state.sirka; j++) {
                if (!is_square_valid(i,j,state, m)) continue;

                if (m.squares[i][j] == TRAVA) {
                    if (rand() % 5 != 0) continue; 
                }
                
                for (int a = 0; a < state.vyska; a++) {
                    for (int b = 0; b < state.sirka; b++) {
                        is_visited[a][b] = false;
                    }
                }
                
                Q.push(make_tuple(i,j,0,TU));

                int max_dist = 100;

                while(!Q.empty()) {
                    const int x = get<0>(Q.front());
                    const int y = get<1>(Q.front());
                    const int dist = get<2>(Q.front());
                    const int from = get<3>(Q.front());
                    Q.pop();
                    if(is_visited[x][y]) continue;

                    if(dist > max_dist) continue;

                    is_visited[x][y] = true;
                    
                    //basic_landscape[x][y] += dist;
                    double light = 0;
                    if (state.mapa[i][j].majitel == 0) {
                        light += l_our_land(dist);
                    }
                    else if (state.mapa[i][j].majitel != 0) {
                        light += l_foreign_land(dist);
                        if (m.squares[i][j] == LAB) {
                            light += l_foreign_lab(dist);
                        }
                        else if (m.squares[i][j] == MESTO) {
                            light += l_foreing_city(dist);
                        }

                        if (state.mapa[i][j].sila_robota > 0) {
                            light += l_enemy(dist);
                        }
                    }

                    basic_landscape[x][y] += light;


                    for (int k = 0; k < 4; k++) {
                        const int nx = x + dx[k];
                        const int ny = y + dy[k];
                        if (!is_square_valid(nx, ny, state, m)) continue;
                        if (is_visited[nx][ny]) continue;
                        Q.push(make_tuple(nx,ny,dist+1, static_cast<smer>(k)));
                    }
                    
                }
                
            }
        }
        
        if (dbg) {
            cerr << "BASIC LANDSCAPE:" << endl;
            for (int i = 0; i < state.vyska; i++) {
                for (int j = 0; j < state.sirka; j++) {
                    cerr << basic_landscape[i][j] << "\t";
                }
                cerr << endl;
            }
        }
    }
    
    
    vector<vector<pair<int,int> > > edges(my_robots.size());
    vector<vector<int> > costs(my_robots.size());

    random_shuffle(my_robots.begin(), my_robots.end());

    vector<bool> is_hunter(my_robots.size(), false);

    vector<pair<int,int> > robots_by_power(my_robots.size());
    for (int i = 0; i < my_robots.size(); i++) {
        robots_by_power[i] = make_pair(state.mapa[my_robots[i].first][my_robots[i].second].sila_robota, i);
    }
    sort(robots_by_power.begin(), robots_by_power.end());

    for (int i = static_cast<int>(my_robots.size() * (1 - 0.15)); i < my_robots.size(); i++) {
        is_hunter[robots_by_power[i].second] = true; 
    }
    
    vector<pair<int,int> > ps;
    vector<int> cs;
    
    set<pair<int,int> > interesting_spots;
    vector<vector<smer> > which_way_i_went(state.vyska, vector<smer>(state.sirka, TU));
    for (int i = 0; i < my_robots.size(); i++) {
        ps.clear(); ps.reserve(5);
        cs.clear(); cs.reserve(5);

        const int x = my_robots[i].first;
        const int y = my_robots[i].second;
        const int sila = state.mapa[x][y].sila_robota;

        if (is_hunter[i]) {
            // ostat na mieste
            {
                const int nx = x;
                const int ny = y;

                int cost = min(static_cast<int>(-basic_landscape[nx][ny])+50, 0);

                if (find(forbidden_places.begin(), forbidden_places.end(), make_pair(x,y)) != forbidden_places.end()) {
                    // toto miesto je zakazane, musime padat
                    cost = min(-1, cost + 150);
                }

                if (! is_weaker_space(nx, ny, sila, state, m)) {
                    // niekto v okoli je silnejsi, musime padat
                    cost = min(-1, cost + 200);
                }

                ps.push_back(make_pair(nx,ny));
                cs.push_back(cost);
            }

            // hybat sa
            for (int k = 0; k < 4; k++) {
                const int nx = x + dx[k];
                const int ny = y + dy[k];

                int cost = static_cast<int>(-basic_landscape[nx][ny]) + 10;

                // je policko validne?
                if (!is_square_valid(nx, ny, state, m)) {
                    continue;
                }

                // je niekto v okoli silenjsi>
                if (!is_weaker_space(nx, ny, sila, state, m)) {
                    cost = min(cost + 100, -1);
                }

                // je to policko zakazane?
                if (find(forbidden_places.begin(), forbidden_places.end(), make_pair(nx, ny)) != forbidden_places.end()) {
                    cost = min(cost + 150, -1);
                }

                // je na tom policku iny nas robot?
                //if (state.mapa[nx][ny].majitel == 0 && state.mapa[nx][ny].sila_robota > 0) {
                //    cost = min(cost + 100, -1);
                //}

                // je na tomto policku slabsi cudzi robot? to je super ^_^
                if (state.mapa[nx][ny].majitel != 0 && state.mapa[nx][ny].sila_robota < sila) {
                    cost -= 50;
                }

                ps.push_back(make_pair(nx, ny));
                cs.push_back(cost);
            }
        }
        else {
            // find an interesting spot and go there.

            queue<pair<int,int> > Q;
            for (int p = 0; p < state.vyska; p++) {
                for (int q = 0; q < state.sirka; q++) {
                    which_way_i_went[p][q] = TU;
                }
            }

            Q.push(make_pair(x,y));
            which_way_i_went[x][y] = DOLE;
            bool found_interesting_spot = false;
            vector<int> order = {0,1,2,3};

            while (!Q.empty() && !found_interesting_spot) {
                const int a = Q.front().first;
                const int b = Q.front().second;
                Q.pop();

                // is this spot interesting?
                if (is_weaker_space(a,b,sila,state,m) 
                    && state.mapa[a][b].majitel != 0 
                    && interesting_spots.find(make_pair(a,b)) == interesting_spots.end()
                ) {
                    interesting_spots.insert(make_pair(a,b));
                    found_interesting_spot = true;

                    bool very_interesting = (m.squares[a][b] == MESTO || m.squares[a][b] == LAB);

                    int zx = a;
                    int zy = b;

                    while (abs(zx - x) + abs(zy - y) > 1) {
                        const int k = which_way_i_went[zx][zy];
                        zx -= dx[k];
                        zy -= dy[k];
                    }

                    while (!Q.empty()) {
                        Q.pop();
                    }

                    for (int k = 0; k < 4; k++) {
                        const int nx = x + dx[k];
                        const int ny = y + dy[k];

                        ps.push_back(make_pair(nx, ny));

                        if (nx == zx && ny == zy) {
                            cs.push_back(-10 + (very_interesting?(-20):0));
                        }
                        else if (which_way_i_went[nx][ny] != TU) {
                            cs.push_back(-5 + (very_interesting?(-20):0));
                        }
                        else {
                            cs.push_back(-2);
                        }
                    }

                    break;
                }

                // todo new points bfs
                random_shuffle(order.begin(), order.end());
                for (int k = 0; k < 4; k++) {
                    const int nx = a + dx[order[k]];
                    const int ny = b + dy[order[k]];
                    if (is_square_valid(nx,ny,state,m) 
                        && which_way_i_went[nx][ny] == TU 
                        //&& !(state.mapa[nx][ny].majitel == 0 && state.mapa[nx][ny].sila_robota > 0)
                    )  {
                        which_way_i_went[nx][ny] = static_cast<smer>(order[k]);
                        Q.push(make_pair(nx, ny));
                    }
                }
            }

            if (!found_interesting_spot) {
                for (int k = 0; k < 4; k++) {
                    const int nx = x + dx[k];
                    const int ny = y + dy[k];
                    if (is_square_valid(nx,ny,state,m)) {
                        ps.push_back(make_pair(nx, ny));
                        cs.push_back(-3);
                    }
                }
            }

            // ostat na mieste
            {
                const int nx = x;
                const int ny = y;

                //int cost = min(static_cast<int>(-basic_landscape[nx][ny])+50, 0);
                int cost = -2;

                if (find(forbidden_places.begin(), forbidden_places.end(), make_pair(x,y)) != forbidden_places.end()) {
                    // toto miesto je zakazane, musime padat
                    cost = min(-1, cost + 150);
                }

                if (! is_weaker_space(nx, ny, sila, state, m)) {
                    // niekto v okoli je silnejsi, musime padat
                    cost = min(-1, cost + 200);
                }

                ps.push_back(make_pair(nx,ny));
                cs.push_back(cost);
            }
        }

        

        edges[i] = ps;
        costs[i] = cs;
    }
    
    
    vector<Prikaz> movements = generate_movements(state, m, my_robots, edges, costs);

    instructions.reserve(instructions.size() + movements.size());
    for (int i = 0; i < movements.size(); i++) {
        instructions.push_back(movements[i]);
    }
    
    return instructions;
}

vector<Prikaz> get_commands_builder(masked_game_state state, mapa m) {
    vector<Prikaz> instructions;

    vector<pair<int, int> > my_robots = get_my_robots(state);
    vector<bool> unmoved(my_robots.size(), true);
    if (dbg) {
        cerr << "MY ROBOTS:" << endl;
        for (int i = 0; i < my_robots.size(); i++) {
            cerr << "[" << my_robots[i].first << ", " << my_robots[i].second << "] ";
        }
        cerr << ";;" << endl;
    }

    vector<pair<int, int> > my_labs = get_my_labs(state, m);
    if (dbg) {
        cerr << "MY LABS:" << endl;
        for (int i = 0; i < my_labs.size(); i++) {
            cerr << "[" << my_labs[i].first << ", " << my_labs[i].second << "] ";
        }
        cerr << ";;" << endl;
    }

    // decide whether to build new robots (of course yes!)
    if (my_labs.size() > 0 && state.zelezo > 0) {
        random_shuffle(my_labs.begin(), my_labs.end());
        vector<bool> use_labs(my_labs.size(), false);
        for (int i = 0; i < my_labs.size(); i++) {
            const int lx = my_labs[0].first;
            const int ly = my_labs[0].second;
            bool successfully_moved = true;
            if (state.mapa[lx][ly].sila_robota != 0) {
                // treba posunut tohto robota
                successfully_moved = false;
                for (int k = 0; k < 4; k++) {
                    const int nx = lx + dx[k];
                    const int ny = ly + dy[k];
                    if (!is_square_valid(nx, ny, state, m)) continue;
                    if (state.mapa[nx][ny].sila_robota < state.mapa[lx][ly].sila_robota &&
                                state.mapa[nx][ny].majitel != 0 || state.mapa[nx][ny].sila_robota == 0) {
                        // posunieme sem!
                        Prikaz p;
                        p.pr = POSUN;
                        p.riadok = lx; p.stlpec = ly;
                        p.instrukcia = k;
                        instructions.push_back(p);

                        for (int j = 0; j < my_robots.size(); j++) if (my_robots[j] == make_pair(nx, ny)) {
                            unmoved[j] = false;
                        }
                        successfully_moved = true;
                        break;
                    }
                }
            }
            if (successfully_moved) {
                use_labs[i] = true;
            }
        }

        int usable_labs_count = 0;
        for (int i = 0; i < use_labs.size(); i++) {
            if (use_labs[i]) {
                usable_labs_count += 1;
            }
        }

        int cc = 0;
        for (int t = 0; t < my_labs.size(); t++) if(use_labs[t]) {
            Prikaz p;
            p.pr = POSTAV;
            p.riadok = my_labs[t].first; p.stlpec = my_labs[t].second;
            p.instrukcia = ((cc == 0)? ((rand() % 10 == 0)?(1):(state.zelezo - usable_labs_count + 1)) : (1)) ;
            instructions.push_back(p);
            cc++;
        }
    }

    // for every left (maybe handpicked) robot estimate the nearest interesting point,
    // pair robots to points and find the best moves to achieve that
    // use Edmonds later

    vector<vector<bool> > is_available_space(state.vyska, vector<bool>(state.sirka, true));
    for (int i = 0; i < state.vyska; i++) {
        for (int j = 0; j < state.sirka; j++) {
            if (!is_square_valid(i, j, state, m)) is_available_space[i][j] = false;
            if (state.mapa[i][j].sila_robota > 0 && state.mapa[i][j].majitel == 0) is_available_space[i][j] = false; 
        }
    }

    random_shuffle(my_robots.begin(), my_robots.end());

    vector<vector<smer> > which_way_i_went(state.vyska, vector<smer>(state.sirka, TU));
    vector<int> order = {0,1,2,3};
    for (int t = 0; t < my_robots.size(); t++) {
        const int qx = my_robots[t].first;
        const int qy = my_robots[t].second;
        if (!unmoved[t]) continue;

        which_way_i_went.clear();
        which_way_i_went.resize(state.vyska, vector<smer>(state.sirka, TU));

        which_way_i_went[qx][qy] = DOLE;

        queue<pair<int, int> > Q;
        Q.push(my_robots[t]);

        bool found_interesting_spot = false;

        while(!Q.empty() && !found_interesting_spot) {
            int x = Q.front().first;
            int y = Q.front().second;
            Q.pop();

            // check by popping
            // place is good if
            // own not by me and robot strength is less than mine

            if ( (m.squares[x][y] == LAB || my_labs.size() > 0) && 
                        state.mapa[x][y].majitel != 0 && 
                        state.mapa[x][y].sila_robota < state.mapa[qx][qy].sila_robota &&
                        is_weaker_space(x, y, state.mapa[qx][qy].sila_robota, state, m)
                ) {
                // our sweet spot!
                while (abs(x - qx) + abs(y - qy) > 1) {
                    const int k = which_way_i_went[x][y]; 
                    x = x - dx[k];
                    y = y - dy[k];
                }

                Prikaz p;
                p.pr = POSUN;
                p.riadok = qx; p.stlpec = qy;
                p.instrukcia = which_way_i_went[x][y];

                is_available_space[x][y] = false;
                instructions.push_back(p);
                found_interesting_spot = true; 
                break;
            }


            // check by adding
            // place is safe if
            // own by me xor strength of robot is less than mine
            random_shuffle(order.begin(), order.end());
            for (int k = 0; k < 4; k++) {
                const int nx = x + dx[order[k]];
                const int ny = y + dy[order[k]];
                if (is_square_valid(nx, ny, state, m) && is_available_space[nx][ny] && 
                                which_way_i_went[nx][ny] == TU && 
                                state.mapa[nx][ny].sila_robota < state.mapa[qx][qy].sila_robota 
                    ) {

                    which_way_i_went[nx][ny] = static_cast<smer>(order[k]);
                    Q.push(make_pair(nx, ny));
                }
            }
        }


    }

    return instructions;
}


vector<Prikaz> get_commands(masked_game_state state, mapa m) {
    vector<Prikaz> instructions;

    vector<pair<int, int> > my_robots = get_my_robots(state);
    vector<bool> unmoved(my_robots.size(), true);
    if (dbg) {
        cerr << "MY ROBOTS:" << endl;
        for (int i = 0; i < my_robots.size(); i++) {
            cerr << "[" << my_robots[i].first << ", " << my_robots[i].second << "] ";
        }
        cerr << ";;" << endl;
    }

    vector<pair<int, int> > my_labs = get_my_labs(state, m);
    if (dbg) {
        cerr << "MY LABS:" << endl;
        for (int i = 0; i < my_labs.size(); i++) {
            cerr << "[" << my_labs[i].first << ", " << my_labs[i].second << "] ";
        }
        cerr << ";;" << endl;
    }

    // decide whether to build new robots (of course yes!)
    if (my_labs.size() > 0 && state.zelezo > 0) {
        random_shuffle(my_labs.begin(), my_labs.end());
        vector<bool> use_labs(my_labs.size(), false);
        for (int i = 0; i < my_labs.size(); i++) {
            const int lx = my_labs[0].first;
            const int ly = my_labs[0].second;
            bool successfully_moved = true;
            if (state.mapa[lx][ly].sila_robota != 0) {
                // treba posunut tohto robota
                successfully_moved = false;
                for (int k = 0; k < 4; k++) {
                    const int nx = lx + dx[k];
                    const int ny = ly + dy[k];
                    if (!is_square_valid(nx, ny, state, m)) continue;
                    if (state.mapa[nx][ny].sila_robota < state.mapa[lx][ly].sila_robota &&
                                state.mapa[nx][ny].majitel != 0 || state.mapa[nx][ny].sila_robota == 0) {
                        // posunieme sem!
                        Prikaz p;
                        p.pr = POSUN;
                        p.riadok = lx; p.stlpec = ly;
                        p.instrukcia = k;
                        instructions.push_back(p);

                        for (int j = 0; j < my_robots.size(); j++) if (my_robots[j] == make_pair(nx, ny)) {
                            unmoved[j] = false;
                        }
                        successfully_moved = true;
                        break;
                    }
                }
            }
            if (successfully_moved) {
                use_labs[i] = true;
            }
        }


        vector<int> portions;
        int total = 0;
        for (int t = 0; t < my_labs.size(); t++) if(use_labs[t]) {
            // maybe try to distribute resources unevenly (based on threat?)
            if (portions.size() == 0) {
                portions.push_back(1);
            }
            else  if (portions.size() == 1) {
                portions.push_back(portions[portions.size() - 1] + 10);
            }
            else {
                portions.push_back(0);
            }
            total += portions[portions.size() - 1];
        }
        for (int t = 0; t < portions.size(); t++) {
            portions[t] = static_cast<int>(1.00 * portions[t]/total * state.zelezo);
        }

        int cc = 0;
        for (int t = 0; t < my_labs.size(); t++) if(use_labs[t] && portions[cc] > 0) {
            Prikaz p;
            p.pr = POSTAV;
            p.riadok = my_labs[t].first; p.stlpec = my_labs[t].second;
            p.instrukcia = portions[cc];
            instructions.push_back(p);
            cc++;
        }
    }

    // for every left (maybe handpicked) robot estimate the nearest interesting point,
    // pair robots to points and find the best moves to achieve that
    // use Edmonds later

    vector<vector<bool> > is_available_space(state.vyska, vector<bool>(state.sirka, true));
    for (int i = 0; i < state.vyska; i++) {
        for (int j = 0; j < state.sirka; j++) {
            if (!is_square_valid(i, j, state, m)) is_available_space[i][j] = false;
            if (state.mapa[i][j].sila_robota > 0 && state.mapa[i][j].majitel == 0) is_available_space[i][j] = false; 
        }
    }

    random_shuffle(my_robots.begin(), my_robots.end());

    vector<vector<smer> > which_way_i_went(state.vyska, vector<smer>(state.sirka, TU));
    vector<int> order = {0,1,2,3};
    for (int t = 0; t < my_robots.size(); t++) {
        const int qx = my_robots[t].first;
        const int qy = my_robots[t].second;
        if (!unmoved[t]) continue;

        which_way_i_went.clear();
        which_way_i_went.resize(state.vyska, vector<smer>(state.sirka, TU));

        which_way_i_went[qx][qy] = DOLE;

        queue<pair<int, int> > Q;
        Q.push(my_robots[t]);

        bool found_interesting_spot = false;

        while(!Q.empty() && !found_interesting_spot) {
            int x = Q.front().first;
            int y = Q.front().second;
            Q.pop();

            // check by popping
            // place is good if
            // own not by me and robot strength is less than mine

            if ( (m.squares[x][y] == LAB || my_labs.size() > 0) && 
                        state.mapa[x][y].majitel != 0 && 
                        state.mapa[x][y].sila_robota < state.mapa[qx][qy].sila_robota
                ) {
                // our sweet spot!
                while (abs(x - qx) + abs(y - qy) > 1) {
                    const int k = which_way_i_went[x][y]; 
                    x = x - dx[k];
                    y = y - dy[k];
                }

                Prikaz p;
                p.pr = POSUN;
                p.riadok = qx; p.stlpec = qy;
                p.instrukcia = which_way_i_went[x][y];

                is_available_space[x][y] = false;
                instructions.push_back(p);
                found_interesting_spot = true; 
                break;
            }


            // check by adding
            // place is safe if
            // own by me xor strength of robot is less than mine
            random_shuffle(order.begin(), order.end());
            for (int k = 0; k < 4; k++) {
                const int nx = x + dx[order[k]];
                const int ny = y + dy[order[k]];
                if (is_square_valid(nx, ny, state, m) && is_available_space[nx][ny] && 
                                which_way_i_went[nx][ny] == TU && 
                                state.mapa[nx][ny].sila_robota < state.mapa[qx][qy].sila_robota
                    ) {
                    which_way_i_went[nx][ny] = static_cast<smer>(order[k]);
                    Q.push(make_pair(nx, ny));
                }
            }
        }


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
    //instrukcie = get_commands(_stav, _m);
    if (_stav.kolo < 200) {
        instrukcie = get_commands_fields(_stav, _m);    
    }
    else {
        instrukcie = get_commands_builder(_stav, _m);
    }
    
    
    cerr << "-----               KONIEC KOLA                  ------" << endl;
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
        assert(_m.width == _stav.sirka && _m.height == _stav.vyska);
        uloz(cout, zistiTah());
        //cout << "2" << endl;   // 2 a flush = koniec odpovede
    }

    return 0;
}
