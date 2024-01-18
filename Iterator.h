#pragma once // чтобы файл подключался один раз
#include <algorithm>
#include <iterator>
#include <ctime>
#include <cmath>
#include <queue>
#include <iostream>
using namespace std;
extern int duration, R1;

class Worker {
protected:
    int size;
    vector<int> ready_time; // время когда закончился обработка у соответствующего работника
    vector<int> queue_range; // размер очереди у соответствующего оператора
    queue<int> future_queue; // очередь из будущих заявок, которая инициализируется в QS
    int processing_time;

public:

    Worker(int n = 1, int time = 1) {
        size = n; 
        ready_time.reserve(n); 
        queue_range.reserve(n);
        for (int i = 0; i < n; i++) {
            ready_time.push_back(0);
            queue_range.push_back(0);
        }
        future_queue.push(0);
        processing_time = time;
        // cout << "Worker Constructor " << ready_time.capacity() << endl;
    };

    bool check_RT () {
        if (*(min_element(ready_time.cbegin(), ready_time.cend())) == 0) 
            return true;
        return false;
    };

    // ~Worker() { 
    //     // ready_time.clear(); 
    //     // queue_range.clear(); 
    //     cout<<"Worker Distructor."<<endl;
    // };
    friend class QS;
    class Iterator; // объявление класса итератора
    Iterator begin_r();
    Iterator end_r();
    Iterator begin_q();
    Iterator end_q();
    Iterator begin();
    Iterator end();

    friend ostream& operator<< (ostream &os, const Worker &w) {
        os << '['<<' ';
        for (int i = 0; i < w.size ; i ++) {
            cout << w.queue_range[i] << ' ';
        }
        return os << "]\n";
    };


    class Iterator {
        private:
            int *_begin, *_end, *_curpos;
        public:
            using iterator_category =  std::random_access_iterator_tag;
            using difference_type = std::ptrdiff_t;
            using value_type = int;
            using pointer = value_type*;
            using reference = value_type&;   

        Iterator(int *begin, int *end ,int *curpos): 
        _begin(begin), _end(end), _curpos(curpos){}

        ~Iterator() {}

        bool operator==(const Iterator &it) const { return _curpos == it._curpos;}
        bool operator!=(const Iterator &it) const { return _curpos != it._curpos;}
        bool operator<(const Iterator &it) const { return _curpos - it._curpos < 0;}
        bool operator>(const Iterator &it) const { return _curpos - it._curpos > 0;}
        bool operator<=(const Iterator &it) const { return _curpos - it._curpos <= 0;}
        bool operator>=(const Iterator &it) const { return _curpos - it._curpos >= 0;}

        int& operator* (){
            //cout << "operator*"<< _curpos <<"\n";
            return *_curpos;
        }
        const int& operator* ()const {return *_curpos;}

        Iterator& operator++() { //++it
            //cout << "operator ++it\n";
            if (_curpos != _end) _curpos++;
            return *this;
        }

        Iterator operator++(int) { //it++
        //cout << "operator it++\n";
            Iterator temp = *this;
            if ( _curpos != _end ) _curpos++;
            return temp;
        }

        Iterator& operator--() { //++it
        //cout << "operator --it\n";
            if (_curpos != _begin) _curpos--;
            return *this;
        }

        Iterator operator--(int) {//it++
            //cout << "operator it--\n";
            Iterator temp = *this;
            if ( _curpos != _begin ) _curpos--;
            return temp;
        }

        Iterator operator+(difference_type n) const {
            //cout << "operator+ dif\n";
            Iterator shiftedIt = *this;
            for(difference_type i = 0; i < n; i++) {
                if (shiftedIt._curpos == shiftedIt._end) return shiftedIt;
                shiftedIt._curpos++;
            }
            return shiftedIt;
        }

        Iterator& operator+=(difference_type n) {
            //cout << "operator +=\n";
            for(difference_type i = 0; i < n; i++) {
                if (_curpos == _end) return *this;
                _curpos++;
            }
            return *this;
        }

        Iterator operator-(difference_type n) const {
            //cout << "operator-\n";
            Iterator shiftedIt = *this;
            for(difference_type i = 0; i < n; i++) {
                if (shiftedIt._curpos == shiftedIt._begin) return shiftedIt;
                shiftedIt._curpos--;
            }
            return shiftedIt;
        }

        Iterator& operator-=(difference_type n) {
            //cout << "operator-=\n";
            for(difference_type i = 0; i < n; i++) {
                if (_curpos == _begin) return *this;
                _curpos--;
            }
            return *this;
        }

        friend Iterator operator+ (difference_type n, const Iterator it) {
            cout << "Iterator operator +\n";
            Iterator shiftedId = it;
            for(difference_type i = 0; i < n; i++) {
                if (shiftedId._curpos == shiftedId._end) return shiftedId;
                shiftedId._curpos++;
            }
            return shiftedId;
        }

        difference_type operator-(const Iterator& it) const {
            //cout << "operator- dif\n";
            return _curpos - it._curpos;
        }

        int& operator[](difference_type n){
            //cout << "operator[]\n";
            return *(_curpos+n);
        }
        const int& operator[](difference_type n) const {return *(_curpos+n);}
    };
};

Worker::Iterator Worker::begin_r() {
    return Worker::Iterator(&ready_time[0], &ready_time[0] + size, &ready_time[0]);
}
Worker::Iterator Worker::end_r() {
    return Worker::Iterator(&ready_time[0], &ready_time[0] + size, &ready_time[0] + size);
}
Worker::Iterator Worker::begin_q() {
    cout << &queue_range[0] << ' ' << queue_range[1] << endl;
    return Worker::Iterator(&queue_range[0], &queue_range[0] + size, &queue_range[0]);
}
Worker::Iterator Worker::end_q() {
    return Worker::Iterator(&queue_range[0], &queue_range[0] + size, &queue_range[0] + size);
}
Worker::Iterator Worker::begin() {
    return Worker::Iterator(&ready_time[0], &ready_time[0] + size, &ready_time[0]);
}
Worker::Iterator Worker::end() {
    return Worker::Iterator(&ready_time[0], &ready_time[0] + size, &ready_time[0] + size);
}