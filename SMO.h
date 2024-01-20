/*
В систему поступают заявки, время между приходом соседних заявок
распределено по экспоненциальному закону со средним значением R1
секунд. Их обработка выполняется в 2 стадии. 
    Рабочие типа 1 могут
выполнять только первый тип работ, время выполнения распределено
экспоненциально со средним значением T1(R1+G1+B1) секунд. 
    Рабочие типа 2
могут выполнять только второй тип работ, время выполнения распределено
экспоненциально со средним значением T2(R2+G2+B2) секунд. 
    Рабочие типа 3
могут выполнять оба типа работ, время выполнения распределено
экспоненциально со средним значением T3(R3+G3+B3+G1) секунд для первого
типа работ и T4(R3+G3+B3+B1) секунд для второго типа работ. Рабочие типа 3
задействуются только тогда, когда образуются очереди, при этом они
выбирают для исполнения заявку из более длинной очереди. 

Требуется разработать дискретно-событийную имитационную модель, которая должна
позволять оценивать показатели загруженности системы. Кроме того, модель
должна формировать лог моделирования, который бы иллюстрировал
продвижение заявок в цепях событий FEC и CEC. С помощью разработанной
модели требуется обосновать минимально требуемое число рабочих каждого
типа, чтобы обеспечить среднее число заявок в каждой из очередей на уровне
не более 2.
*/

#pragma once // чтобы файл подключался один раз
#include <algorithm>
#include <iterator>
#include <ctime>
#include <cmath>
#include <queue>
#include <iostream>
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
        W1(n, T1), W2(m, T2), W3(k) {
    };

    ~QS() { cout << "Distructor QS.\n"; }

    void Work_with_FQ (int time) { 
        cout <<"\tСейчас " << time << endl;

        // уменьшаем вребя обработки W1
        for (int i = 0; i < W1.size; i ++) {
            if (W1.ready_time[i] > 0) W1.ready_time[i] -= 1;
            if (((W1.ready_time[i] % W1.processing_time) == 0)&&(W1.queue_range[i] > 0)) {
                W1.queue_range[i] -= 1;
                // Если 1 отработал, ready_time == time то надо отправить заявку на второго работника
                int index2 = W2.fmine_qr();
                W2.queue_range[index2] += 1; // прибавляем заявку ко второму работнику, у которого минимальная очередь
                W2.ready_time[index2] += W2.processing_time; // устанвливаем время обработки, это время будет падать до 0
                // а не увеличиваться, тк на увеличение сложно составить формулу, тк есть глобальное время
                index2 = W2.fmine_qr();
                cout << W1 << W2; 
            }

        }
        // уменьшаем вребя обработки W2
        for (int i = 0; i < W2.size; i ++) {
            if (W2.ready_time[i] > 0) W2.ready_time[i] -= 1;
            if (((W2.ready_time[i] % W2.processing_time) == 0)&&(W2.queue_range[i] > 0)) W2.queue_range[i] -= 1;
        }

        // уменьшаем вребя обработки W3
        for (int i = 0; i < W3.size; i ++) {
            if (W3.ready_time[i] > 0) W3.ready_time[i] -= 1;
        }

        // отправляем заявку в реальную очередь W1
        // если из future пришла заявка, отправляем ее на W1
        // первая заявка приходит в момент времени 0
        if (W1.future_queue.front() == time) {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::exponential_distribution<> d(1);
    
            int index1 = W1.fmine_qr();
            W1.queue_range[index1] += 1;
            W1.ready_time[index1] += W1.processing_time;

            // имитация времени, когда придет новая заявка
            double x1 = W1.future_queue.front();
            W1.future_queue.pop();
            cout << "Пришла заявка через = "<<x1<<endl;

            x1 += R1*d(gen);
            W1.future_queue.push(x1);
            cout << W1 << W2 << W3;
        }

        int index1 = W1.fmaxe_qr();
        int index2 = W2.fmaxe_qr();
        int index3 = W3.fmine_rt();
        // Имитация работы третьего работника
        // смотрим, есть ли свободный работник и образовались ли очереди у W1 и W2
        while (W3.check_RT() && ((W1.queue_range[index1] > 1) || (W2.queue_range[index2] > 1))) {
            if (W1.queue_range[index1] > W2.queue_range[index2]) {
                W3.ready_time[index3] += W3.first_processing_time;
                W1.queue_range[index1] -= 1;
                W1.ready_time[index1] -= W1.processing_time;
            }
            else {
                W3.ready_time[index3] += W3.first_processing_time;
                W2.queue_range[index1] -= 1;
                W2.ready_time[index1] -= W2.processing_time;
            }
            cout<<"   " << W1<<"   " << W2<<"   " << W3;
        }

    };

    void modeling (int N) {
        cout << "Start modeling.\n";
        Work_with_FQ(0);
        
        for (int i = 1; i <= N; i++) {
            Work_with_FQ(i);
        }
    }
};


