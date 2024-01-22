#pragma once // чтобы файл подключался один раз
#include <algorithm>
#include <iterator>
#include <ctime>
#include <cmath>
#include <queue>
#include <iostream>
#include <stdio.h>
#include <random>
#include "Worker.h"
using namespace std;
int R1 = 10;

class Worker;


class QS{
protected:
    Typical_Worker W1;
    Typical_Worker W2;
    Advanced_Worker W3;
public:
    //std::move rvalue lvalue
    QS(int n, int m, int k): 
        W1((char *)"Type 1", n, T1), W2((char *)"Type 2", m, T2), W3((char *)"Type 3", k) {
    };

    // ~QS() { cout << "Distructor QS.\n"; }

    void Work_with_FQ (int time) { 

        // уменьшаем вребя обработки W1
        for (int i = 0; i < W1.size; i ++) {
            if (W1.ready_time[i] > 0) {
                W1.ready_time[i] -= 1;
                W1.UTIL += 1;
            }
            // Если 1 отработал, ready_time == time то надо отправить заявку на второго работника
            if (((W1.ready_time[i] % W1.processing_time) == 0)&&(W1.queue_range[i] > 0)) {
                W1.queue_range[i] -= 1;
                W1.READY += 1;
                int index2 = W2.fmine_qr();
                W2.plus(index2);
                // прибавляем заявку ко второму работнику, у которого минимальная очередь
                // устанвливаем время обработки, это время будет падать до 0
                // а не увеличиваться, тк на увеличение сложно составить формулу, тк есть глобальное время
                index2 = W2.fmine_qr();
            }

        }
        // уменьшаем вребя обработки W2
        for (int i = 0; i < W2.size; i ++) {
            if (W2.ready_time[i] > 0) {
                W2.ready_time[i] -= 1;
                W2.UTIL += 1;
            }
            if (((W2.ready_time[i] % W2.processing_time) == 0)&&(W2.queue_range[i] > 0)) {
                W2.queue_range[i] -= 1;
                W2.READY += 1;
            }
        }

        // уменьшаем вребя обработки W3
        for (int i = 0; i < W3.size; i ++) {
            if (W3.ready_time[i] > 0) {
                W3.ready_time[i] -= 1;
                W3.UTIL += 1;
                if (W3.ready_time[i] == 0) W3.READY += 1;
            }
            
        }

        // отправляем заявку в реальную очередь W1
        // если из future пришла заявка, отправляем ее на W1
        // первая заявка приходит в момент времени 0
        W1.new_transition(time);

        // Имитация работы третьего работника
        // смотрим, есть ли свободный работник и образовались ли очереди у W1 и W2
        W3.transition_proccessing(W3.fmine_rt(), &W1, &W2);
    };

    void traking(int time) {
        printf("%4d   ", time);
        W1.print();
        printf("   ");
        W2.print();
        printf("   ");
        W3.print();
        printf("   ");
        cout << W1.future_queue.front();
        printf("\n");
    };

    void finish () {
        int n = 3;
        Worker w[n] = {W1, W2, W3};
        printf("\nFACILITIES\tENTRIES\tREADY\tUTIL");
        for (int i = 0; i < n; i++)
            printf("\n%s\t\t%d\t%d\t%d\t", w[i].name, w[i].ENTRIES, w[i].READY, w[i].UTIL);
        printf("\n\nFEC\tPLAN TIME\n\t%d\n", W1.future_queue.front());
    };

    void modeling (int N) {
        cout << "Start modeling.\n";
        
        // здесь я буду собирать данные о показателях загруженности системы
        for (int i = 0; i <= N; i++) {
            Work_with_FQ(i);
            traking(i);
        }
        finish();
    };
};

