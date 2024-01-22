#include "engine.cpp"

using namespace std;

int main() {

    const int R1 = 10, G1 = 11, B1 = 7, R2 = 8, G2 = 9, B2 = 8, R3 = 5, G3 = 11, B3 = 9;
    System system(R1, G1, B1, R2, G2, B2, R3, G3, B3);
    int Workers[3];

    system.logChains = false;
    system.logStats = false;

    const int WORKER_COUNT = 10, TIME = 10000;
    int t1best, t2best, t3best;
    double AVECONT_BEST = 9999;
    for (int i = 0; i <= WORKER_COUNT; i++) {
        for (int j = 0; i + j <= WORKER_COUNT; j++) {
            int k = WORKER_COUNT - i - j;
            Workers[0] = i;
            Workers[1] = j;
            Workers[2] = k;

            system.simulate(TIME, Workers);
            if (max(system.stats->TAVECONT[0], system.stats->TAVECONT[1]) <= 2) {
                if (max(system.stats->TAVECONT[0], system.stats->TAVECONT[1]) < AVECONT_BEST) {
                    t1best = i;
                    t2best = j;
                    t3best = k;
                    AVECONT_BEST = max(system.stats->TAVECONT[0], system.stats->TAVECONT[1]);
                }
            }
        }
    }
    if (AVECONT_BEST <= 2) {
        cout << "We can keep average continuous queue size not more than " << AVECONT_BEST << " transacts with workers distribution (" << t1best << ", " << t2best << ", " << t3best << ")\n";
        cout << "Calculating stats for this distribution:\n";
        Workers[0] = t1best;
        Workers[1] = t2best;
        Workers[2] = t3best;

        system.simulate(TIME, Workers);
        cout << *(system.stats) << endl;
    }
    else {
        cout << WORKER_COUNT << " workers is not enough to keep average continuous queue size not more than 2\n";
    }
}
