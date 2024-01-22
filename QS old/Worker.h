#pragma once // чтобы файл подключался один раз
#include <algorithm>
#include <iterator>
#include <ctime>
#include <cmath>
#include <queue>
#include <iostream>
#include <stdio.h>
#include <random>
using namespace std;
extern int T1, T2, T3, T4, R1;
class QS;

class Worker {
protected:
    friend QS;
    int size;
    vector<int> ready_time; // время когда закончился обработка у соответствующего работника
    char * name;
    int ENTRIES; // Сколько заявок прошло через устройство 
    int READY; // Сколько заявок обслужилось
    int UTIL; // Сколько времени устройство было занято
    int PROC_TIME; // Сколько времени заявка пребывала в система
    int QMAX; // Максиальная очередь

    //есть ли свободный работник
    bool check_RT () {
        if (*(min_element(ready_time.begin(), ready_time.end())) == 0) 
            return true;
        return false;
    };
    int fmine_rt () {
        return distance(this->ready_time.begin(),min_element(this->ready_time.begin(), this->ready_time.end()));
    };
    int fmaxe_rt () {
        return distance(this->ready_time.begin(),max_element(this->ready_time.begin(), this->ready_time.end()));
    };

};






class Typical_Worker : public Worker {
protected:
    queue<int> future_queue; // очередь из будущих заявок, которая инициализируется в QS
    vector<int> queue_range; // размер очереди у соответствующего оператора
    // 1 заявка - это еще не очередь. Первая заявка всегда идет в обработке,
    // начиная со 2 заяки очередь начинается
    int processing_time;

public:
    friend class QS;
    friend class Advanced_Worker; 
    Typical_Worker(char * Name, int n = 1, int pt = 1) {
        name = Name;
        // cout << "Contructor Typical_Worker.\n";
        size = n; 
        
        for (int i = 0; i < size; i++){
            ready_time.push_back(0);
            queue_range.push_back(0);
        }
        future_queue.push(0);
        processing_time = pt;
        ENTRIES = 0;
        READY = 0; 
        UTIL = 0; 
        PROC_TIME = 0;
        QMAX = 0;
    };
    
    void new_transition (int time) {
        if (this->future_queue.front() <= time) {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::exponential_distribution<> d(1.0/(double)(R1));
    
            int index1 = this->fmine_qr();
            this->plus(index1);

            // имитация времени, когда придет новая заявка
            double x1 = d(gen) + this->future_queue.front();
            // cout << x1 - this->future_queue.front();
            this->future_queue.pop();
            this->future_queue.push(x1);
        }
    };

// Меняет количество заявок, которое было в очереди устройства
    void plus (int index) {
        this->queue_range[index] += 1;
        this->ready_time[index] += this->processing_time;
        this->ENTRIES += 1;
    };

    friend ostream& operator<< (ostream &os, const Typical_Worker &w) {
        os << '['<<' ';
        for (int i = 0; i < w.size ; i ++) {
            cout << w.queue_range[i] << '('<<w.ready_time[i]<<") ";
        }
        return os << "]\n";
    };

    void print () {
        printf("[");
        for (int i = 0; i < this->size; i++) {
            printf("%2d(%3d)", this->queue_range[i], this->ready_time[i]);
        }
        printf("]");
    };
    
    Typical_Worker(Typical_Worker &s){ std::cout<<"Worker Copy"<<std::endl; };
    ~Typical_Worker() { 
        ready_time.clear(); 
        queue_range.clear(); 
        // cout<<"Distructor Typical_Worker."<<endl;
    };
    int fmaxe_qr () {
        return distance(this->queue_range.begin(),max_element(this->queue_range.begin(), this->queue_range.end()));
    };
    int fmine_qr () {
        return distance(this->queue_range.begin(),min_element(this->queue_range.begin(), this->queue_range.end()));
    };
};





class Advanced_Worker : public Worker {
// у него нет очередей
private:
    int first_processing_time = T3;
    int second_processing_time = T4;
public:
    friend class QS;
    Advanced_Worker(char * Name, int n = 1) {
        // cout << "Contructor Advanced_Worker.\n";
        name = Name;
        size = n;
        for (int i = 0; i < size; i++) ready_time.push_back(0);
        ENTRIES = 0;
        READY = 0; 
        UTIL = 0; 
        PROC_TIME = 0;
        QMAX = 0;
    };
    
    void transition_proccessing (int index, Typical_Worker* W1, Typical_Worker* W2) {
        Typical_Worker* w[] = {W1, W2};
        int index1 = w[0]->fmaxe_qr();
        int index2 = w[1]->fmaxe_qr(), i = 0;
        std::random_device rd;
        std::mt19937 gen(rd());
        while (this->check_RT() && ((w[i]->queue_range[index1] > 1) || (w[i+1]->queue_range[index2] > 1))) {
            if (w[i]->queue_range[index1] > w[i+1]->queue_range[index2]) { 
                std::exponential_distribution<> d(1.0/(double)(this->first_processing_time));
                this->ready_time[index] += this->first_processing_time;
            }
            else {
                i = 1;
                std::exponential_distribution<> d(1.0/(double)(this->second_processing_time));
                this->ready_time[index] += this->first_processing_time;
            }
            this->ENTRIES += 1;
            w[i]->queue_range[index1] -= 1;
            w[i]->ready_time[index1] -= w[i]->processing_time;
            index1 = w[0]->fmaxe_qr();
            index2 = w[1]->fmaxe_qr();
            i = 0;
        }
    };

    Advanced_Worker(Worker &s){ std::cout<<"Copy Advanced_Worker."<<std::endl; };
    ~Advanced_Worker() {
        ready_time.clear();
        // cout<<"Distructor Advanced_Worker."<<endl;
    };

    friend ostream& operator<< (ostream &os, const Advanced_Worker &w) {
        os << '['<<' ';
        for (int i = 0; i < w.size ; i ++) {
            cout << '('<<w.ready_time[i]<<") ";
        }
        return os << "]\n";
    };
    void print () {
        printf("[ ");
        for (int i = 0; i < this->size; i++) {
        printf("(%2d)", this->ready_time[i]);
        }
        printf(" ]");
    };
};
