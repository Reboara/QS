#pragma once // чтобы файл подключался один раз
#include <algorithm>
#include <iterator>
#include <ctime>
#include <cmath>
#include <queue>
#include <iostream>
using namespace std;
extern int T1, T2, T3, T4, R1;


class Worker {
protected:
    int size;
    vector<int> ready_time; // время когда закончился обработка у соответствующего работника

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
    Typical_Worker(int n = 1, int time = 1) {
        cout << "Contructor Typical_Worker.\n";
        size = n; 
        for (int i = 0; i < size; i++){
            ready_time.push_back(0);
            queue_range.push_back(0);
        }
        future_queue.push(0);
        processing_time = time;
    };

    friend ostream& operator<< (ostream &os, const Typical_Worker &w) {
        os << '['<<' ';
        for (int i = 0; i < w.size ; i ++) {
            cout << w.queue_range[i] << '('<<w.ready_time[i]<<") ";
        }
        return os << "]\n";
    };
    
    Typical_Worker(Typical_Worker &s){ std::cout<<"Worker Copy"<<std::endl; };
    ~Typical_Worker() { 
        ready_time.clear(); 
        queue_range.clear(); 
        cout<<"Distructor Typical_Worker."<<endl;
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
    Advanced_Worker(int n = 1) {
        cout << "Contructor Advanced_Worker.\n";
        size = n;
        for (int i = 0; i < size; i++) ready_time.push_back(0);
    };
    
    Advanced_Worker(Worker &s){ std::cout<<"Copy Advanced_Worker."<<std::endl; };
    ~Advanced_Worker() {
        ready_time.clear();
        cout<<"Distructor Advanced_Worker."<<endl;
    };

    friend ostream& operator<< (ostream &os, const Advanced_Worker &w) {
        os << '['<<' ';
        for (int i = 0; i < w.size ; i ++) {
            cout << '('<<w.ready_time[i]<<") ";
        }
        return os << "]\n";
    };
};
