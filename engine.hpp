#include <iostream>  // Для ввода/вывода
#include <iomanip>   // Для управления форматом вывода
#include <queue>     // Для использования структуры данных очереди
#include <list>      // Для использования структуры данных связного списка
#include <random>    // Для работы с генератором случайных чисел
#include <cstdlib>   // Для функции exit()
#include <chrono>     // Для использования chrono и steady_clock

using namespace std;

class System;

void error(const char *alekseev)
{
    printf("\n\033[0;31mError: %s\n", alekseev);
    exit(228);
}

struct Parameters
{
    int Workers[3];
};

class Statistics
{
public:
    Statistics(System *_sys) : sys(_sys) {}
    int QMAX[2],
        QCONT[2],
        QENTRY[2],
        QENTRY0[2];
    double QAVECONT[2],
        QAVETIME[2],
        QAVETIME0[2];
    int WENTRY[3];
    double WLOAD[3],
        WAVETIME[3];
    friend ostream &operator<<(ostream &os, const Statistics &stats);

private:
    System *sys;
};

class Transact
{
public:
    int id;
    int wtype[2]; // РўРёРїС‹ СЂР°Р±РѕС‡РёС… РЅР° СЂР°Р·РЅС‹С… СЌС‚Р°РїР°С…
    bool locked;
    System *sys;
    int stage; // 0 - BIRTH, 1,2 - РїРѕРґР»РµР¶Р°С‚ РѕР±СЂР°Р±РѕС‚РєРµ, 3 - С‚РµСЂРјРёРЅР°Р»СЊРЅРѕРµ

    double leaveTime; // РњРѕРґРµР»СЊРЅРѕРµ РІСЂРµРјСЏ РІС‹С…РѕРґР° РёР· FEC

    double birthTime;
    double startTime[2], finishTime[2]; // Р”Р»СЏ СЃС‚Р°С‚РёСЃС‚РёРє

    Transact(System *_sys, double _birthTime);
    void eventCallback();
};

class System
{
public:
    double currentTime; // РњРѕРґРµР»СЊРЅРѕРµ РІСЂРµРјСЏ С‚РµРєСѓС‰РµРіРѕ СЃРѕР±С‹С‚РёСЏ
    int uniqueX;

    int R1, G1, B1, R2, G2, B2, R3, G3, B3; // РџР°СЂР°РјРµС‚СЂС‹ СЃРёРјСѓР»СЏС†РёРё

    queue<Transact *> Q1, Q2;
    int freeWorkers[3]; // РљРѕР»РёС‡РµСЃС‚РІРѕ СЃРІРѕР±РѕРґРЅС‹С… РєР°РЅР°Р»РѕРІ РґР»СЏ РєР°Р¶РґРѕРіРѕ РёР· С‚СЂС‘С… С‚РёРїРѕРІ СЂР°Р±РѕС‡РёС…

    mt19937 rng;

    Statistics *stats;

    void insertToFEC(Transact *);
    void removeFromCEC(Transact *);
    void simulate(double, struct Parameters);
    void setRandomSeed(int seed)
    {
        rng = mt19937(seed);
    }
    System(int _R1, int _G1, int _B1, int _R2, int _G2, int _B2, int _R3, int _G3, int _B3);

    ~System()
    {
        reset();
        delete stats;
    }

    void writeLogsTo(ostream &s) { logs = &s; }
    void freeTransact(Transact *d) { toFree.push(d); }
    bool logChains, logStats;

    ostream *logs;

private:
    queue<Transact *> toFree; // РўСЂР°РЅР·Р°РєС‚С‹ Рє СѓРґР°Р»РµРЅРёСЋ
    list<Transact *> FEC, CEC;
    list<Transact *>::iterator CEC_active_ptr;
    void assignTransact2Worker(int type, Transact *tr, double time); // РќР°Р·РЅР°С‡РёС‚СЊ РѕР±СЂР°Р±РѕС‚РєСѓ С‚СЂР°РЅР·Р°РєС‚Р° СЂР°Р±РѕС‡РµРјСѓ РѕРїСЂРµРґРµР»С‘РЅРЅРѕРіРѕ С‚РёРїР°
    // Рё РїРµСЂРµРјРµСЃС‚РёС‚СЊ С‚СЂР°РЅР·Р°РєС‚ РІ FEC

    void reset();        // РћС‡РёСЃС‚РёС‚СЊ РІСЃРµ РґР°РЅРЅС‹Рµ, СЃРѕР·РґР°РЅРЅС‹Рµ РІ РїСЂРѕС†РµСЃСЃРµ СЃРёРјСѓР»СЏС†РёРё
    void assignJobs();   // РќР°Р·РЅР°С‡РёС‚СЊ СЂР°Р±РѕС‡РёРј С‚СЂР°РЅР·Р°РєС‚С‹ РґР»СЏ РѕР±СЂР°Р±РѕС‚РєРё
    void processCEC();   // Р Р°СЃРїСЂРµРґРµР»РёС‚СЊ С‚СЂР°РЅР·Р°РєС‚С‹ РёР· CEC РїРѕ РѕС‡РµСЂРµРґСЏРј
    void collectStats(); // РџРѕРґРІРµСЃС‚Рё РёС‚РѕРіРё СЃРёРјСѓР»СЏС†РёРё
};
