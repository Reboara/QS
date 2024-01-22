#include <iostream>  // Для ввода/вывода
#include <iomanip>   // Для управления форматом вывода
#include <queue>     // Для использования структуры данных очереди
#include <list>      // Для использования структуры данных связного списка
#include <random>    // Для работы с генератором случайных чисел
#include <cstdlib>   // Для функции exit()
#include <algorithm>  // Для использования функции max_element
#include <chrono>     // Для использования chrono и steady_clock

using namespace std;

#include "engine.hpp"

// Функция для нахождения максимальной длины строки в списке, для красоты
size_t maxStringLength(const initializer_list<string>& strings) {
    return max_element(strings.begin(), strings.end(), [](const string& a, const string& b) {
        return a.length() < b.length();
    })->length();
}

ostream &operator<<(ostream &os, const Statistics &stats) {
    // считаем максимальную длинну для красоты вывода *
    size_t maxQueue1Length = maxStringLength({ "1. Transactions at peak:    " + to_string(stats.QMAX[0]),
                                               "2. Remaining transactions:   " + to_string(stats.QCONT[0]),
                                               "3. Entered transactions:     " + to_string(stats.QENTRY[0]),
                                               "4. Processed immediately:    " + to_string(stats.QENTRY0[0]),
                                               "5. Average transactions:     " + to_string(stats.TAVECONT[0]),
                                               "6. Average time in queue:    " + to_string(stats.QAVETIME[0]),
                                               "7. Average time (-0):        " + to_string(stats.QAVETIME_PROC[0]) });
    
    size_t maxQueue2Length = maxStringLength({ "1. Transactions at peak:    " + to_string(stats.QMAX[1]),
                                               "2. Remaining transactions:   " + to_string(stats.QCONT[1]),
                                               "3. Entered transactions:     " + to_string(stats.QENTRY[1]),
                                               "4. Processed immediately:    " + to_string(stats.QENTRY0[1]),
                                               "5. Average transactions:     " + to_string(stats.TAVECONT[1]),
                                               "6. Average time in queue:    " + to_string(stats.QAVETIME[1]),
                                               "7. Average time (-0):        " + to_string(stats.QAVETIME_PROC[1]) });
    
    size_t maxFacultyStatsLength = maxStringLength({ "Entered transactions:      " + to_string(stats.WENTRY[0]),
                                                      "Workload, %:               " + to_string(stats.K_WORKERS[0]),
                                                      "Average service time:      " + to_string(stats.AST[0]) });

    os << "\nQueue 1 Stats:\n" << endl;
    os << string(maxQueue1Length + 4, '*') << endl;
    os << "* 1. Transactions at peak:    " << setw(9) << stats.QMAX[0] << " *" << endl;
    os << "* 2. Remaining transactions:  " << setw(9) << stats.QCONT[0] << " *" << endl;
    os << "* 3. Entered transactions:    " << setw(9) << stats.QENTRY[0] << " *" << endl;
    os << "* 4. Processed immediately:   " << setw(9) << stats.QENTRY0[0] << " *" << endl;
    os << "* 5. Average transactions:    " << setw(9) << stats.TAVECONT[0] << " *" << endl;
    os << "* 6. Average time in queue:   " << setw(9) << stats.QAVETIME[0] << " *" << endl;
    os << "* 7. Average time (-0):       " << setw(9) << stats.QAVETIME_PROC[0] << " *" << endl;
    os << string(maxQueue1Length + 4, '*') << endl;
    os << endl;

    os << "\nQueue 2 Stats:\n" << endl;
    os << string(maxQueue2Length + 4, '*') << endl;
    os << "* 1. Transactions at peak:    " << setw(9) << stats.QMAX[1] << " *" << endl;
    os << "* 2. Remaining transactions:  " << setw(9) << stats.QCONT[1] << " *" << endl;
    os << "* 3. Entered transactions:    " << setw(9) << stats.QENTRY[1] << " *" << endl;
    os << "* 4. Processed immediately:   " << setw(9) << stats.QENTRY0[1] << " *" << endl;
    os << "* 5. Average transactions:    " << setw(9) << stats.TAVECONT[1] << " *" << endl;
    os << "* 6. Average time in queue:   " << setw(9) << stats.QAVETIME[1] << " *" << endl;
    os << "* 7. Average time (-0):       " << setw(9) << stats.QAVETIME_PROC[1] << " *" << endl;
    os << string(maxQueue2Length + 4, '*') << endl;
    os << endl;

    os << "\nFaculty stats:\n" << endl;
    os << string(maxFacultyStatsLength + 4, '*') << endl;
    os << "* Entered transactions:       " << setw(8) << stats.WENTRY[0] << " *" << endl;
    os << "* Workload, %:                " << setw(8) << stats.K_WORKERS[0] << " *" << endl;
    os << "* Average service time:       " << setw(8) << stats.AST[0] << " *" << endl;
    os << string(maxFacultyStatsLength + 4, '*') << endl;
    os << endl;

    return os;
}

// Заявка обслужилась, удаляем ее
void System::removeFromCEC(Transact *tr) {
    for (auto it = CEC.begin();; it++) {
        if (it == CEC.end())
            error("No such transact in CEC");
        if (*it == tr) {
            if (it == CEC_active_ptr)
                CEC_active_ptr++;
            CEC.erase(it);
            return;
        }
    }
}

// Заявка пришла, отправляем ее в обработку работниками
void System::insertToFEC(Transact *tr) {
    for (auto it = FEC.begin(); it != FEC.end(); ++it) {
        if ((*it)->leaveTime > tr->leaveTime) {
            FEC.insert(it, tr);
            return;
        }
    }
    FEC.push_back(tr);
}

void System::simulate(double _timeOfSimulation, struct Parameters params) {
    // Цикл симуляции
    reset();
    currentTime = 0;
    insertToFEC(new Transact(this, exponential_distribution<double>(1 / (double)R1)(rng)));

    freeWorkers[0] = params.Workers[0];
    freeWorkers[1] = params.Workers[1];
    freeWorkers[2] = params.Workers[2];

    if (logChains) {
        auto tr = FEC.back();
        (*print_statistics).width(8);
        (*print_statistics) << currentTime << " Transact " << tr->id << "(S" << tr->stage << ", T=" << tr->leaveTime << ") (initial transact) added to FEC" << endl;
    }

    CEC_active_ptr = CEC.end();

    while (1) {
        Transact *obr = *(FEC.begin());
        double dt = obr->leaveTime - currentTime;
        if (obr->leaveTime > _timeOfSimulation) dt = _timeOfSimulation - currentTime;

        /* Сбор статистики */
        stats->QMAX[0] = max(stats->QMAX[0], (int)Q1.size());
        stats->QMAX[1] = max(stats->QMAX[1], (int)Q2.size());
        stats->TAVECONT[0] += Q1.size() * dt;
        stats->TAVECONT[1] += Q2.size() * dt;
        for (int i = 0; i < 3; i++) {
            stats->K_WORKERS[i] += dt * (params.Workers[i] - freeWorkers[i]);
        }

        /* Освобождение памяти от мёртвых транзактов */
        while (toFree.size()) {
            delete toFree.front();
            toFree.pop();
        }

        /* Условие выхода из цикла */
        if (obr->leaveTime > _timeOfSimulation)
            break;

        /* Логика */
        FEC.pop_front();
        CEC.push_back(obr);
        if (CEC_active_ptr == CEC.end())
            CEC_active_ptr--;
        if (currentTime > obr->leaveTime)
            error("Transact added to FEC with leave time less than current model time");
        currentTime = obr->leaveTime;
        if (logChains)
        {
            (*print_statistics).width(8);
            (*print_statistics) << currentTime << " Transact " << obr->id << "(S" << obr->stage << ") moved to CEC" << endl;
        }
        obr->locked = false;
        obr->eventCallback();
        processCEC();
        assignJobs();
    }
    currentTime = _timeOfSimulation;
    collectStats();
    for (int i = 0; i < 3; i++) {
        stats->K_WORKERS[i] /= (double)params.Workers[i] * _timeOfSimulation;
        stats->K_WORKERS[i] *= 100;
    }
    if (logStats)
        (*print_statistics) << *stats << endl;
}

void System::collectStats() {
    stats->QCONT[0] = Q1.size();
    stats->QCONT[1] = Q2.size();
    for (int i = 0; i < 2; i++) {
        stats->TAVECONT[i] /= currentTime;
        stats->QAVETIME[i] /= stats->QENTRY[i];
        stats->QAVETIME_PROC[i] /= stats->QENTRY[i] - stats->QENTRY0[i];
    }
    for (int i = 0; i < 3; i++) {
        stats->AST[i] /= stats->WENTRY[i];
    }
}

void System::processCEC() {
    while (CEC_active_ptr != CEC.end()) {
        auto tract = *CEC_active_ptr;
        if (!(tract->locked)) {
            tract->locked = true;
            if (tract->stage == 1) {
                Q1.push(tract);
                stats->QENTRY[0]++;
                if (logChains) {
                    (*print_statistics).width(8);
                    (*print_statistics) << currentTime << " Transact " << tract->id << "(S" << tract->stage << ", T=" << tract->leaveTime << ") locked in CEC (Queue1) and added to delay chain" << endl;
                }
            }
            else if (tract->stage == 2) {
                Q2.push(tract);
                stats->QENTRY[1]++;
                if (logChains) {
                    (*print_statistics).width(8);
                    (*print_statistics) << currentTime << " Transact " << tract->id << "(S" << tract->stage << ", T=" << tract->leaveTime << ") locked in CEC (Queue2) and added to delay chain" << endl;
                }
            }
            else
                error("There is transact in CEC with unknown stage");
        }
        CEC_active_ptr++;
    }
}

void System::assign_Transact_to_Worker(int type, Transact *tr, double time) {
    removeFromCEC(tr);

    stats->WENTRY[type - 1]++;

    tr->wtype[tr->stage - 1] = type;
    tr->startTime[tr->stage - 1] = currentTime;

    tr->leaveTime = currentTime + time;

    if (tr->stage == 1) {
        stats->QAVETIME[0] += tr->startTime[0] - tr->birthTime;
        if (tr->birthTime == tr->startTime[0])
            stats->QENTRY0[0]++;
        else
            stats->QAVETIME_PROC[0] += tr->startTime[0] - tr->birthTime;
    }
    if (tr->stage == 2) {
        stats->QAVETIME[1] += tr->startTime[1] - tr->finishTime[0];
        if (tr->finishTime[0] == tr->startTime[1])
            stats->QENTRY0[1]++;
        else
            stats->QAVETIME_PROC[1] += tr->startTime[1] - tr->finishTime[0];
    }

    // Добавить транзакт в FEC
    insertToFEC(tr);
    if (logChains) {
        print_statistics->width(8);
        (*print_statistics) << currentTime << " Transact " << tr->id << "(S" << tr->stage;
        (*print_statistics) << ", T=" << tr->leaveTime << ") moved to FEC by worker type " << type << endl;
    }
    freeWorkers[type - 1]--;
}



void System::assignJobs() {
    while (Q1.size() && freeWorkers[0]) {
        Transact *tr = Q1.front();
        Q1.pop();
        assign_Transact_to_Worker(1, tr, exponential_distribution<double>(1 / (double)(R1 + G1 + B1))(rng));
    }
    while (Q2.size() && freeWorkers[1]) {
        Transact *tr = Q2.front();
        Q2.pop();
        assign_Transact_to_Worker(2, tr, exponential_distribution<double>(1 / (double)(R2 + G2 + B2))(rng));
    }
    while ((Q1.size() || Q2.size()) && freeWorkers[2]) {
        if (Q1.size() >= Q2.size()) {
            Transact *tr = Q1.front();
            Q1.pop();
            assign_Transact_to_Worker(3, tr, exponential_distribution<double>(1 / (double)(R3 + G3 + B3 + G1))(rng));
        }
        else {
            Transact *tr = Q2.front();
            Q2.pop();
            assign_Transact_to_Worker(3, tr, exponential_distribution<double>(1 / (double)(R3 + G3 + B3 + B1))(rng));
        }
    }
}



void Transact::eventCallback() {
    if (stage == 0) {
        stage = 1;
        // "Запланировать" появление нового транзакта
        Transact *tr = new Transact(sys, sys->currentTime + exponential_distribution<double>(1 / (double)sys->R1)(sys->rng));
        sys->insertToFEC(tr);
        if (sys->logChains) {
            sys->print_statistics->width(8);
            *(sys->print_statistics) << sys->currentTime << " Transact " << tr->id << "(S0, T=" << tr->leaveTime << ") added to FEC" << endl;
        }
    }
    else { // Собрать статистику
        finishTime[stage - 1] = sys->currentTime;
        int myW = wtype[stage - 1];
        sys->stats->AST[myW - 1] += finishTime[stage - 1] - startTime[stage - 1];
        sys->freeWorkers[myW - 1] += 1;
        // Перейти на следующую стадию
        if (stage == 2) {
            sys->removeFromCEC(this);
            sys->freeTransact(this);
            if (sys->logChains) {
                sys->print_statistics->width(8);
                *(sys->print_statistics) << sys->currentTime << " Transact " << id << " removed from all chains and terminated" << endl;
            }
        }
        if (stage == 1) {
            stage = 2;
        }
    }
}

void System::reset() {
    uniqueX = 1;
    for (int i = 0; i < 3; i++) {
        stats->AST[i] = 0;
        stats->WENTRY[i] = 0;
        stats->K_WORKERS[i] = 0;
        if (i < 2) {
            stats->TAVECONT[i] = 0;
            stats->QAVETIME_PROC[i] = 0;
            stats->QMAX[i] = 0;
            stats->QAVETIME[i] = 0;
            stats->QCONT[i] = 0;
            stats->QENTRY[i] = 0;
            stats->QENTRY0[i] = 0;
        }
    }
    for (auto ddd : FEC) {
        delete ddd;
    }
    for (auto ddd : CEC) {
        delete ddd;
    }
    FEC.clear();
    CEC.clear();
    while (Q1.size())
        Q1.pop();
    while (Q2.size())
        Q2.pop();
}

Transact::Transact(System *_sys, double _birthTime) {
    sys = _sys;
    stage = 0;
    birthTime = _birthTime;
    leaveTime = _birthTime;
    id = sys->uniqueX;
    sys->uniqueX++;
}

System::System(int _R1, int _G1, int _B1, int _R2, int _G2, int _B2, int _R3, int _G3, int _B3) {
    rng = mt19937(chrono::steady_clock::now().time_since_epoch().count());
    R1 = _R1;
    G1 = _G1;
    B1 = _B1;
    R2 = _R2;
    G2 = _G2;
    B2 = _B2;
    R3 = _R3;
    G3 = _G3;
    B3 = _B3;
    stats = new Statistics(this);
    print_statistics = &cout;
};
