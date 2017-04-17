#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>
using namespace std;

#include "common.h"
#include "marshal.h"
#include "update.h"


mapa _m;
masked_game_state _stav;   // vzdy som hrac cislo 0

int dx[4] = {0,0,0,0};
int dy[4] = {0,0,0,0};

// main() zavola tuto funkciu, ked nacita mapu
void inicializuj() {
    // (sem patri vas kod)

    // smery
    // vyska/sirka <-> riadok/stlpec
    tie(dx[DOLE], dy[DOLE])     = make_pair(+1, 0);
    tie(dx[VLAVO], dy[VLAVO])   = make_pair(0, -1);
    tie(dx[VPRAVO], dy[VPRAVO]) = make_pair(0, +1);
    tie(dx[HORE], dy[HORE])     = make_pair(-1, 0);


}

Prikaz move_to_nearest_interesting_square(int w, int h, masked_game_state &state, mapa &m, vector<vector<bool> > &is_free_square) {
    cerr << "MOVE TO NEAREST INTERESTING POINT " << w << " " << h << " " << endl;
    vector<vector<smer> > odkial_som_prisiel(state.vyska, vector<smer>(state.sirka, TU));
    queue<pair<int,int> > Q;
    Q.push(make_pair(w, h));

    vector<int> order = {0,1,2,3};

    while (! Q.empty()) {
        int x, y;
        tie(x, y) = Q.front();
        Q.pop();

        if (state.mapa[x][y].majitel != 0 && is_free_square[x][y]) {
            // nasli sme prvy zaujimavy bod!
            cerr << "FIRST INTERESTING POINT " << x << " " << y << endl;
            cerr << "majitel: " << state.mapa[x][y].majitel << endl;
            is_free_square[x][y] = false;
            Prikaz p;
            p.pr = POSUN;
            p.riadok = w;
            p.stlpec = h;

            while (abs(w - x) + abs(h - y) > 1) {
                int k = odkial_som_prisiel[x][y];
                x = x - dx[k];
                y = y - dy[k];
            }

            p.instrukcia = odkial_som_prisiel[x][y];
            is_free_square[x][y] = false;
            is_free_square[w][h] = true;
            while (!Q.empty()) Q.pop();
            return p;
        }


        random_shuffle(order.begin(), order.end());

        for (int l = 0; l < 4; l++) {
            const int k = order[l];
            int nx = x + dx[k];
            int ny = y + dy[k];
            if (0 <= nx && nx < state.vyska && 0 <= ny && ny < state.sirka && is_free_square[nx][ny] && odkial_som_prisiel[nx][ny] == TU) {
                odkial_som_prisiel[nx][ny] = static_cast<smer>(k);
                Q.push(make_pair(nx, ny));
            }
        }
    }
    cerr << "NO INTERESTING POINT?!" << endl;
    Prikaz p;
    p.pr = POSUN;
    p.riadok = w;
    p.stlpec = h;
    p.instrukcia = TU;

    return p;
}

vector<Prikaz> get_commands(masked_game_state &state, mapa &m) {
    vector<Prikaz> instructions;
    cerr << "TAH " << state.kolo << endl;

    /*
    cerr << "OWNERSHIP:" << endl;
    for (int i = 0; i < state.vyska; i++) {
        for (int j = 0; j < state.sirka; j++) {
            cerr << state.mapa[i][j].majitel << "\t";
        }
        cerr << endl;
    }
    */
    
    // posun kazdeho pesiaka ku najblizsiemu neobsadenemu policku

    vector<vector<bool> > is_free_square(state.vyska, vector<bool>(state.sirka, true));
    for (int i = 0; i < state.vyska; i++) {
        for (int j = 0; j < state.sirka; j++) {
            // zle, ak je tam neprechodne uzemie
            if (m.squares[i][j] == KAMEN) {
                is_free_square[i][j] = false;
            }
            // jeden z nasich panacikov
            if (state.mapa[i][j].sila_robota > 0 && state.mapa[i][j].majitel == 0) {
                is_free_square[i][j] = false;
            }
        }
    }

 
    for (int i = 0; i < state.vyska; i++) {
        for (int j = 0; j < state.sirka; j++) {
            if (state.mapa[i][j].majitel == 0 && state.mapa[i][j].sila_robota > 0) {
                Prikaz p;
                p = move_to_nearest_interesting_square(i, j, state, m, is_free_square);
                instructions.push_back(p);
            }
        }
    }

    cerr << "finished with moves" << endl;

    // vytvor noveho pesiacika na vsetky peniaze (na nahodnom labe)
    int steel_amount = state.zelezo;
    vector<pair<int,int> > my_labs_positions;
    for (int i = 0; i < state.vyska; i++) {
        for (int j = 0; j < state.sirka; j++) {
            if (m.squares[i][j] == LAB && state.mapa[i][j].majitel == 0) {
                my_labs_positions.push_back(make_pair(i, j));
            }
        }
    }

    if (my_labs_positions.size() > 0) {
        random_shuffle(my_labs_positions.begin(), my_labs_positions.end());
        Prikaz build_robot;
        build_robot.pr = POSTAV;
        build_robot.riadok = my_labs_positions[0].first;
        build_robot.stlpec = my_labs_positions[0].second;
        build_robot.instrukcia = static_cast<int>( 0.8 * steel_amount);
        instructions.push_back(build_robot);

        for (int i = 1; i < my_labs_positions.size(); i++) {
            Prikaz build_robot;
            build_robot.pr = POSTAV;
            build_robot.riadok = my_labs_positions[i].first;
            build_robot.stlpec = my_labs_positions[i].second;
            build_robot.instrukcia = static_cast<int>(0.2 / (my_labs_positions.size() -1) * steel_amount);
        }
    }

    return instructions;
}

// main() zavola tuto funkciu, ked nacita novy stav hry, a ocakava instrukcie
// tato funkcia ma vratit vector prikazov
vector<Prikaz> zistiTah() {
    cerr << "zistiTah() begin" << endl;
    vector<Prikaz> instrukcie;
    //  (sem patri vas kod)
    
    // using original structs
    instrukcie = get_commands(_stav, _m);

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
