using namespace std;

#include "engine.cpp"

int main() {
    const int R1 = 8, G1 = 9, B1 = 10, R2 = 11, G2 = 5, B2 = 11, R3 = 10, G3 = 5, B3 = 7;
    System system(R1, G1, B1, R2, G2, B2, R3, G3, B3);

    int Workers[3];
    Workers[0] = 3;
    Workers[1] = 3;
    Workers[2] = 3;

    system.logChains = true;
    system.logStats = true;
    system.simulate(100, Workers);
}
