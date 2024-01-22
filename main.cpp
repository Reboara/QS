#include "engine.cpp"

using namespace std;

int main()
{

    const int R1 = 8, G1 = 9, B1 = 10,
              R2 = 11, G2 = 5, B2 = 11,
              R3 = 10, G3 = 5, B3 = 7;
    System system(R1, G1, B1, R2, G2, B2, R3, G3, B3);
    struct Parameters params;

    system.logChains = false;
    system.logStats = false;

    const int WORKER_COUNT = 9, TIME = 10000;
    int t1best, t2best, t3best;
    double AVECONT_BEST = 9999;
    for (int i = 0; i <= WORKER_COUNT; i++)
    {
        for (int j = 0; i + j <= WORKER_COUNT; j++)
        {
            int k = WORKER_COUNT - i - j;
            params.Workers[0] = i;
            params.Workers[1] = j;
            params.Workers[2] = k;

            system.simulate(TIME, params);
            if (max(system.stats->QAVECONT[0], system.stats->QAVECONT[1]) <= 2)
            {
                if (max(system.stats->QAVECONT[0], system.stats->QAVECONT[1]) < AVECONT_BEST)
                {
                    t1best = i;
                    t2best = j;
                    t3best = k;
                    AVECONT_BEST = max(system.stats->QAVECONT[0], system.stats->QAVECONT[1]);
                }
            }
        }
    }
    if (AVECONT_BEST <= 2)
    {
        cout << "We can keep average continuous queue size not more than " << AVECONT_BEST << " transacts with workers distribution (" << t1best << ", " << t2best << ", " << t3best << ")\n";
        cout << "Calculating stats for this distribution:\n";
        params.Workers[0] = t1best;
        params.Workers[1] = t2best;
        params.Workers[2] = t3best;

        system.simulate(TIME, params);
        cout << *(system.stats) << endl;
    }
    else
    {
        cout << WORKER_COUNT << " workers is not enough to keep average continuous queue size not more than 2\n";
    }
}