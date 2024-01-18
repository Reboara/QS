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
#include "Iterator.h"
using namespace std;
double e = 2.71828182845904, sigma = 4, pi = 3.1415926535;
int duration = 10, R1 = 10;

class Worker;
class Iterator;


class QS{
protected:
    Worker W1;
    Worker W2;
public:
    QS(Worker& w1, Worker& w2): W1(w1), W2(w2) {
        // cout << "\tCyle Start\n";
        // cout << "\tCyle End cap = "<<(*w2).ready_time.capacity()<<" size = "
        // <<(*w2).ready_time.size()<<" Future back = "<<(*w1).future_queue.back()<<"\n";
    };

    ~QS() { cout << "QS Distructor.\n"; }

    void Work_with_FQ () { // имитация времени, когда приходит заявка
        double x1 = W1.future_queue.front(), x2 = W1.future_queue.front();
        double x = (double)(std::rand()%100 + 1)/ 100.0;
        W1.future_queue.pop(), W2.future_queue.pop();

        x1 += pow(  (-2)*sigma*sigma*log(x*sigma*pow( 2*pi , 0.5)) + R1*R1  , 0.5);
        W1.future_queue.push(x1);
        // cout<<W1.future_queue.back()<<" x1 = "<<x1<<endl;

        x = (double)(std::rand()%100 + 1)/ 100.0;
        x2 += pow(  (-2)*sigma*sigma*log(x*sigma*pow( 2*pi , 0.5)) + R1*R1  , 0.5);
        W2.future_queue.push((int)x2);
    };

    void modeling (int N) {
        cout << "Start modeling.\n";
        Work_with_FQ();
        for (int i = 0; i <= N; i++) {
            // cout<<"Future time of entry "<<W1.future_queue.front()<<endl;
            if (W1.future_queue.front() == i) {
                // cout<<"Поступила заявка в момент "<< W1.future_queue.front() <<" ";
                // cout<<"New time "<<W1.future_queue.front()<<endl;

                auto R = min_element(W1.queue_range.cbegin(), W1.queue_range.cend());
                int i = distance(W1.queue_range.cbegin(), R);

                // cout << "index " << i  <<" element "<< W1.queue_range.size() << endl;
                // cout <<" Modeling "<< W1 << "\t->";
                W1.queue_range[i] += 1;
                // W1.ready_time[i] += W1.processing_time;
                Work_with_FQ();
            }
            
            // while () {

            // } 
        }
        cout << "End modeling.\n";
    }
};


