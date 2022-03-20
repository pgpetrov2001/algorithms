#ifndef MAP
#define MAP

#include <algorithm>
#include "allocator.h"

template <typename K, typename V, int bucksz, allocator<bucksz> &allocator_>
class map {
public:
    map() {};
    map(const map &other) {
        create(other.size());
        //TODO: memcpy
        std::copy(other.datak(), other.datak() + other.size(), datak());
        std::copy(other.datav(), other.datav() + other.size(), datav());
    }
    map(map &&other) noexcept : data(other.data) {
        other.data = nullptr;
    }
    ~map() noexcept {
        allocator_.free(data);
    }
    map &operator=(const map &other) {
        if (aggrcap(other.size()) > aggrcap()) {
            allocator_.free(data);
            create(other.size());
        }
        std::copy(other.datak(), other.datak() + other.size(), datak());
        std::copy(other.datav(), other.datav() + other.size(), datav());
        sz = other.size();
        return *this;
    }
    map &operator=(map &&other) {
        std::swap(data, other.data);
        sz = other.sz;
        cap = other.cap;
        return *this;
    }
    void reserve(int new_cap) {
        void *new_data = allocator_.malloc(aggrcap(new_cap));
        std::copy(datak(), datak() + size(), datak(new_data, new_cap));
        std::copy(datav(), datav() + size(), datav(new_data, new_cap));
        cap = new_cap;
        allocator_.free(data);
        data = new_data;
    }
    int size() const {
        return sz;
    }
    V &operator[](const K &k) {
        for (int i=0; i<sz; ++i) 
            if (datak()[i] == k) 
                return datav()[i];
        return push_back(k);
    }
    const V &operator[](const K &k) const {
        return const_cast<const map*>(this)->operator[](k);
    }
    void clear() {
        allocator_.free(data);
        cap = 0;
        sz = 0;
    }
private:
    void *data = nullptr;
    int cap = 0;
    int sz = 0;

    static int aggrcap(int sz) {
        return sizeof(V) * sz + sizeof(K) * sz;
    }
    int aggrcap() const {
        return sizeof(V) * cap + sizeof(K) * cap;
    }
    static K *datak(void *data, int cap) {
        return reinterpret_cast<K*>(datav(data, cap) + cap);
    }
    static V *datav(void *data, int cap) {
        return reinterpret_cast<V*>(data);
    }
    K *datak() {
        return reinterpret_cast<K*>(datav() + cap);
    }
    const K *datak() const {
        return const_cast<map*>(this)->datak();
    }
    V *datav() {
        return reinterpret_cast<V*>(data);
    }
    const V *datav() const {
        return const_cast<map*>(this)->datav();
    }

    V &push_back(const K &k) {
        if (!cap) {
            this->cap = 1;
            data = allocator_.malloc(aggrcap(cap));
        } else if (sz == cap) {
            //TODO: maybe make 3/2 or something like that
            if (cap == 1) reserve(2);
            else reserve(std::min(26,(cap * 3) / 2));
        }
        datak()[sz] = k;
        datav()[sz] = {};
        return datav()[sz++];
    }

    void create(int sz) {
        this->cap = sz;
        this->sz = sz;
        data = allocator_.malloc(aggrcap(cap));
    }

    void resize(int sz) {
        if (sz > cap)
            reserve(sz);
        this->sz = sz;
    }
};

#endif
