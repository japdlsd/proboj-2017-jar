#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <iostream>
#include <vector>
using namespace std;

#include "common.h"
#include "marshal.h"
#include "update.h"

typedef pair<int,int> pii;
#define mp make_pair
#define ND(x,y,k) const int n##x = x + dx[k]; const int n##y = y + dy[k]; 

#define FORMAP(i, j) for(int i = 0; i < m.height; i++) for(int j = 0; j < m.width; j++)

mapa m;
masked_game_state _stav;   // vzdy som hrac cislo 0

#define DEBUG

#ifdef DEBUG
    #define dbg true
#elif
    #define dbg false
#endif

int dx[4] = {0,0,0,0};
int dy[4] = {0,0,0,0};
vector<smer> INV(5);

vector<pair<int,int> > ALL_LABS;

// main() zavola tuto funkciu, ked nacita mapu
void inicializuj() {
    // (sem patri vas kod)

    // smery
    // vyska/sirka <-> riadok/stlpec
    tie(dx[DOLE], dy[DOLE])     = mp(+1, 0);
    tie(dx[VLAVO], dy[VLAVO])   = mp(0, -1);
    tie(dx[VPRAVO], dy[VPRAVO]) = mp(0, +1);
    tie(dx[HORE], dy[HORE])     = mp(-1, 0);

    INV[DOLE] = HORE;
    INV[HORE] = DOLE;
    INV[VLAVO] = VPRAVO;
    INV[VPRAVO] = VLAVO;

    // eval all labs
    ALL_LABS.reserve(16);
    FORMAP(r,s) {
        if (m.squares[r][s] == LAB) {
            ALL_LABS.push_back(mp(r, s));
        }
    }
}

Prikaz make_prikaz(const prikaz pr, const int riadok, const int stlpec, const int instrukcia) {
    Prikaz p;
    p.pr = pr;
    p.riadok = riadok; p.stlpec = stlpec;
    p.instrukcia = instrukcia;
    return p;
}

inline bool is_inmap(const int x, const int y) {
    return (0 <= x && x < m.height && 0 <= y && y < m.width);
}

bool is_valid_square(const int x, const int y) {
    const typ_stvorca& typ = m.squares[x][y];
    return (is_inmap(x, y) && (typ == LAB || typ == MESTO || typ == TRAVA || typ == LAB_SPAWN));
}

bool is_safe_around(const int x, const int y, const int sila, const masked_game_state& state) {
    if (!is_valid_square(x,y)) return false;

    if (state.mapa[x][y].majitel != 0 && state.mapa[x][y].sila_robota > sila) return false; 

    for (int k = 0; k < 4; k++) {
        ND(x,y,k);
        if (state.mapa[nx][ny].majitel != 0 && state.mapa[nx][ny].sila_robota > sila) return false;
    }

    return true;
}

inline bool is_my_robot(const int x, const int y, const masked_game_state &state) {
    return (is_valid_square(x, y) && state.mapa[x][y].sila_robota > 0 && state.mapa[x][y].majitel == 0);
}

vector<pii> get_my_robots(const masked_game_state& state) {
    vector<pii> my_robots; my_robots.reserve(100);
    FORMAP(r, s) {
        if (is_my_robot(r,s,state)) {
            my_robots.push_back(mp(r,s));
        }
    }
    return my_robots;
}

vector<pii> get_my_labs(const masked_game_state& state) {
    vector<pii> my_labs; my_labs.reserve(4);
    for (pii l: ALL_LABS) {
        if (state.mapa[l.first][l.second].majitel == 0) {
            my_labs.push_back(l);
        }
    }
    return my_labs;
}


vector<Prikaz> build_minimal_robots(const masked_game_state& state) {
    vector<Prikaz> instructions;
    
    vector<pii> my_labs = get_my_labs(state);
    for (pii l: my_labs) {
        instructions.push_back(make_prikaz(POSTAV, l.first, l.second, 1)); 
    }

    return instructions;
}


vector<Prikaz> test_building(masked_game_state state) {
    vector<Prikaz> instructions;
    
    vector<pii> my_robots = get_my_robots(state);
    vector<pii> my_labs = get_my_labs(state);
    instructions.reserve(my_robots.size());

    for (pii r: my_robots) {
        instructions.push_back(make_prikaz(POSUN, r.first, r.second, DOLE));
    }

    vector<Prikaz> building_instructions = build_minimal_robots(state);
    instructions.insert(instructions.end(), building_instructions.begin(), building_instructions.end());
    return instructions;
}

vector<Prikaz> commands_homogenous_sphere(masked_game_state state) {
    vector<Prikaz> instructions;
    
    vector<pii> my_robots = get_my_robots(state);
    vector<pii> my_labs = get_my_labs(state);
    instructions.reserve(my_robots.size());

    for (pii r: my_robots) {
        instructions.push_back(make_prikaz(POSUN, r.first, r.second, DOLE));
    }

    vector<Prikaz> building_instructions = build_minimal_robots(state);
    instructions.insert(instructions.end(), building_instructions.begin(), building_instructions.end());
    return instructions;
}
 
// main() zavola tuto funkciu, ked nacita novy stav hry, a ocakava instrukcie
// tato funkcia ma vratit vector prikazov
vector<Prikaz> zistiTah() {
    vector<Prikaz> instrukcie;
    //  (sem patri vas kod)

    instrukcie = commands_homogenous_sphere(_stav);

    return instrukcie;
}


int main() {
    // v tejto funkcii su vseobecne veci, nemusite ju menit (ale mozte).
    
    unsigned int seed = time(NULL) * getpid();
    srand(seed);
    
    nacitaj(cin, m);
    fprintf(stderr, "START pid=%d, seed=%u\n", getpid(), seed);
    inicializuj();

    while (cin.good()) {
        nacitaj(cin, _stav);
        uloz(cout, zistiTah());
    }

    return 0;
}
