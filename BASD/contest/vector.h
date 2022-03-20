#ifndef VEC
#define VEC

#include <algorithm>

template <typename T>
class vector {
public:
    class const_iterator {
    public:
        friend class vector;
        const_iterator(const T *ptr) : ptr(ptr) {}
        const T &operator*() const {
            return *ptr;
        }
        bool operator!=(const const_iterator &other) const {
            return ptr != other.ptr;
        }
        const_iterator &operator++() {
            ++ptr;
            return *this;
        }
        const_iterator operator++(int) {
            auto old = *this;
            ++(*this);
            return old;
        }
    private:
        const T *ptr;
    };

    class iterator {
    public:
        friend class vector;
        iterator(T *ptr) : ptr(ptr) {}
        T &operator*() const {
            return *ptr;
        }
        bool operator!=(const iterator &other) const {
            return ptr != other.ptr;
        }
        iterator &operator++() {
            ++ptr;
            return *this;
        }
        iterator operator++(int) {
            auto old = *this;
            ++(*this);
            return old;
        }
    private:
        T *ptr;
    };

    vector() {}
    vector(int sz) {
        create(sz);
    }
    vector(const vector &other) {
        create(other.size());
        //TODO: memcpy
        std::copy(other.data, other.data + sz, data);
    }
    vector(vector &&other) noexcept : data(other.data), sz(other.sz), cap(other.cap) {
        other.data = nullptr;
    }
    ~vector() noexcept {
        delete[] data;
    }
    vector &operator=(const vector &other) {
        if (other.size() > cap) {
            delete[] data;
            create(other.size());
        }
        sz = other.size();
        std::copy(other.data, other.data + sz, data);
        return *this;
    }
    vector &operator=(vector &&other) {
        std::swap(data, other.data);
        sz = other.sz;
        cap = other.cap;
        return *this;
    }
    void reserve(int new_cap) {
        cap = new_cap;
        T *new_data = new T[cap];
        //TODO: memcpy
        if (data)
            std::move(data, data+sz, new_data);
        delete[] data;
        data = new_data;
    }
    void resize(int sz) {
        if (!data) return create(sz);
        if (sz > cap)
            reserve(sz);
        this->sz = sz;
    }
    int size() const {
        return sz;
    }
    void push_back(const T &el) {
        if (!cap) {
            this->cap = 2;
            data = new T[cap];
        } else if (sz == cap) {
            reserve(cap * 2);
        }
        data[sz++] = el;
    }
    T &back() {
        return data[sz-1];
    }
    const T &back() const {
        return data[sz-1];
    }
    T &operator[](int idx) {
        return data[idx];
    }
    const T &operator[](int idx) const {
        return data[idx];
    }
    const_iterator begin() const {
        return {data};
    }
    const_iterator end() const {
        return {data + sz};
    }
    iterator begin() {
        return {data};
    }
    iterator end() {
        return {data + sz};
    }
    void clear() {
        delete[] data;
        data = nullptr;
        sz = 0;
        cap = 0;
    }
private:
    T* data = nullptr;
    int sz = 0;
    int cap = 0;

    void create(int sz) {
        this->cap = sz;
        this->sz = sz;
        data = new T[cap]{};
    }
};

class small_vector_int {
public:
    /* small_vector_int(int sz) { */
    /*     create(sz); */
    /* } */
    /* small_vector_int(const small_vector_int &other) { */
    /*     create(other.size()); */
    /*     std::copy(other.data+2, other.data+2 + data[1], data+2); */
    /* } */
    /* small_vector_int &operator=(const small_vector_int &other) { */
    /*     if (other.size() > data[0]) { */
    /*         delete[] data; */
    /*         create(other.size()); */
    /*     } */
    /*     std::copy(other.data+1, other.data+2 + data[1], data+1); */
    /*     return *this; */
    /* } */
    small_vector_int() {}
    small_vector_int(small_vector_int &&other) noexcept : data(other.data) {
        other.data = nullptr;
    }
    small_vector_int &operator=(small_vector_int &&other) {
        std::swap(data, other.data);
        return *this;
    }
    ~small_vector_int() noexcept {
        delete[] data;
    }
    void reserve(int new_cap) {
        int *new_data = new int[new_cap+2];
        new_data[0] = new_cap;
        if (data)
            std::copy(data+1, data+data[1]+2, new_data+1);
        else
            new_data[1] = 0;
        delete[] data;
        data = new_data;
    }
    void resize(int sz) {
        if (!data) return create(sz);
        if (sz > data[0])
            reserve(sz);
        data[1] = sz;
    }
    int size() const {
        if (!data) 
            return 0;
        return data[1];
    }
    void push_back(int el) {
        if (!data) {
            data = new int[2+2]{};
            data[0] = 2;
        } else if (data[0] == data[1]) {
            reserve(data[0] * 2);
        }
        data[2+(data[1]++)] = el;
    }
    int &operator[](int idx) {
        return data[2+idx];
    }
    int operator[](int idx) const {
        return data[2+idx];
    }
    void clear() {
        delete[] data;
        data = nullptr;
    }
private:
    int* data = nullptr;

    void create(int sz) {
        data = new int[sz]{};
        data[0] = sz;
        data[1] = sz;
    }
};

#endif
