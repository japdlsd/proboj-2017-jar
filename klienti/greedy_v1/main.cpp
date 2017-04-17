#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

#include "common.h"
#include "marshal.h"
#include "update.h"


mapa _m;
masked_game_state _stav;   // vzdy som hrac cislo 0


// main() zavola tuto funkciu, ked nacita mapu
void inicializuj() {
  // (sem patri vas kod)
}

vector<Prikaz> get_commands(masked_game_state &state, mapa &m) {
    vector<Prikaz> instructions;
    // nahodne posun vsetkych pesiacikov
    for (int i = 0; i < state.vyska; i++) {
        for (int j = 0; j < state.sirka; j++) {
            if (state.mapa[i][j].majitel == 0 && state.mapa[i][j].sila_robota > 0) {
                Prikaz p;
                p.pr = POSUN;
                p.riadok = i;
                p.stlpec = j;
                p.instrukcia = rand()%4;
                instructions.push_back(p);
            }
        }
    }



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

    if (my_labs_positions.size() > 0 && (rand()%2 == 0 || state.kolo < 50)) {
        random_shuffle(my_labs_positions.begin(), my_labs_positions.end());
        Prikaz build_robot;
        build_robot.pr = POSTAV;
        build_robot.riadok = my_labs_positions[0].first;
        build_robot.stlpec = my_labs_positions[0].second;
        build_robot.instrukcia = static_cast<int>(0.8 * steel_amount);
        
        instructions.push_back(build_robot);

        for (int i = 1; i < my_labs_positions.size(); i++) {
            Prikaz build_robot;
            build_robot.pr = POSTAV;
            build_robot.riadok = my_labs_positions[i].first;
            build_robot.stlpec = my_labs_positions[i].second;
            build_robot.instrukcia = 1;

        }
    }

    return instructions;
}

// main() zavola tuto funkciu, ked nacita novy stav hry, a ocakava instrukcie
// tato funkcia ma vratit vector prikazov
vector<Prikaz> zistiTah() {
    vector<Prikaz> instrukcie;
    //  (sem patri vas kod)
    
    // using original structs
    instrukcie = get_commands(_stav, _m);

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
