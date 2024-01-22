#include <iostream>  // Для ввода/вывода
#include <iomanip>   // Для управления форматом вывода
#include <queue>     // Для использования структуры данных очереди
#include <list>      // Для использования структуры данных связного списка
#include <random>    // Для работы с генератором случайных чисел
#include <cstdlib>   // Для функции exit()
#include <chrono>    // Для использования chrono и steady_clock

using namespace std;

class System;


class Statistics {
public:
    Statistics(System *_sys) : sys(_sys) {}
    int QMAX[2], // максимальная длинна очереди
        QCONT[2], 
        QENTRY[2],
        QENTRY0[2];
    double TAVECONT[2], // Сколько в среднем находится транзактов в системе на каждом этапе обработки
        QAVETIME[2], //Среднее время нахождения в системе
        QAVETIME_PROC[2]; //Среднее время обработки заявки
    int WENTRY[3]; // сколько заявок прошло через рабочих
    double K_WORKERS[3], // процент загрузи рабочих (коэф загрузки операторов)
        AST[3]; // average service time среднее время обслуживания
    friend ostream &operator<<(ostream &os, const Statistics &stats);

private:
    System *sys;
};

class Transact {
public:
    int id;
    int wtype[2]; // Какой тип рабочих обслужил заявку на каждом этапе
    bool locked;
    System *sys;
    int stage; // Сталдия обработки, 0 - заявыка пришла с систему, 1,2 - подлежат обработке, 3 - терминальное

    double leaveTime; // Время выхода из FEC

    double birthTime;
    double startTime[2], finishTime[2]; // Для статистики
    Transact(System *_sys, double _birthTime);
    void eventCallback(); // переход между стадиями обработки заявок 
};

class System {
private:
    queue<Transact *> toFree; // Заявки, которые надо удалить
    list<Transact *> FEC, CEC;
    list<Transact *>::iterator CEC_active_ptr;
    // Назначить обработку транзакта рабочему определённого типа
    // и переместить транзакт в FEC
    void assign_Transact_to_Worker(int type, Transact *tr, double time); 

    void delet();        // отчистка памяти
    void assign_to_Workers();   // распределение заявок по рабочим/очередям
    void from_CEC_to_Workers();   // Распределить транзакты из CEC по очередям
    void collect_statistics(); // Статистика и выводы

public:
    double currentTime; // Время моделирования
    int uniqueX;

    int R1, G1, B1, R2, G2, B2, R3, G3, B3; // Дано

    queue<Transact *> Q1, Q2; // Очереди заявок, находящихся в 
    int freeWorkers[3]; // количество свободных работников каждого типа

    mt19937 rng;

    Statistics *stats;

    void insertToFEC(Transact *);
    void delete_from_ststem(Transact *);
    void simulate(double, int *);
    void setRandomSeed(int seed) {
        rng = mt19937(seed); // генерируем ключ генерации
    };

    System(int r1, int g1, int b1, int r2, int g2, int b2, int r3, int g3, int b3);

    ~System() {
        delet();
        delete stats;
    };

    void writeLogsTo(ostream &s) { print_statistics = &s; };
    void freeTransact(Transact *d) { toFree.push(d); };
    bool logChains, logStats;

    ostream *print_statistics;
};

void error(const char *str ) {
    printf("\nError: %s.\n", str);
    exit(1);
}