#ifndef ALLOC
#define ALLOC

/* #include <cassert> */
#include <vector>
#include <unordered_set>
#include <cstdint>

/* #include <iostream> */
/* using std::cout; using std::endl; */ 

static constexpr int sizes[] = {8, 12, 16, 20, 28, 32, 36, 40, 48, 52, 56, 60, 68, 72, 76, 80, 88, 92, 96, 100, 108, 112, 116, 120, 128, 132};
static constexpr auto do_translation() {
    std::array<int, 133> ret{};
    for (int &x : ret) {
        x = -1;
    }
    int i = 0;
    for (int x : sizes) {
        ret[x] = i++;
    }
    return ret;
}
static constexpr std::array<int, 133> translate = do_translation();

static void erase(std::unordered_set<int> &cont, const std::unordered_set<int>::const_iterator &what) {
    cont.erase(what);
    if (cont.empty()) {
        cont = {};
    } else if (2 * cont.bucket_count() > 3 * cont.size()) {
        std::unordered_set<int> shrunk(cont.begin(), cont.end());
        cont = shrunk;
    }
}


template <int bucksz>
class block {
public:
    block(int sz) : sz(sz) {
        spots.reserve(bucksz / sz);
        int stop = bucksz - bucksz%sz;
        for (int b=0; b<stop; b+=sz) {
            spots.insert(b);
        }
    }
    void *malloc(bool &filled) {
        /* assert(!spots.empty()); */
        int spot = *spots.begin();
        erase(spots, spots.begin());
        filled = spots.empty();
        /* assert(spot + sz <= bucksz); */
        /* assert(spot % sz == 0); */
        /* assert(&bucket[spot] - begin() == spot); */
        return &bucket[spot];
    }
    void free(void *what) {
        size_t offset = reinterpret_cast<char*>(what) - begin(); 
        /* assert(offset >= 0 && offset < bucksz); */
        /* assert(offset % sz == 0); */
        spots.insert(offset);
    }
    char *begin() {
        return &bucket[0];
    }
    block *next = nullptr;
    int sz;
private:
    std::array<char, bucksz> bucket;
    std::unordered_set<int> spots;
};


template <int bucksz>
class allocator {
    int num_blocks = 0;
public:
    allocator() : head(new block<bucksz>(sizes[0])) {
        tail = head;
        free_blocks_for_bucket[0].insert(head);
    }
    ~allocator() {
        purge_memory();
    }
    void purge_memory() {
        auto *curr = head;
        while (curr) {
            auto *next = curr->next;
            /* cout << "deleting curr " << curr << endl; */
            delete curr;
            curr = next;
        }
        head = nullptr;
    }
    void *malloc(int size) {
        /* assert(size / 5 >= 1 && size / 5 <= 26); */
        size += (4-size%4)%4;
        /* assert(translate[size] >= 0 && translate[size] < 26); */
        auto &target_blocks = free_blocks_for_bucket[translate[size]];
        if (target_blocks.empty()) {
            tail->next = new block<bucksz>(size);
            tail = tail->next;
            ++num_blocks;
            /* doit(); */
            /* cout << "at " << num_blocks << endl; */
            target_blocks.insert(tail);
        } 
        auto *curr_block = *target_blocks.begin();
        bool filled;
        auto *ret = curr_block->malloc(filled);
        /* assert(((char*)ret - curr_block->begin()) % curr_block->sz == 0); */
        if (filled) {
            target_blocks.erase(target_blocks.begin());
        }
        return ret;
    }
    void free(void *what) {
        if (!what) return;
        if (!head) return;
        auto *curr = head;
        while (curr) {
            size_t offset = (char*)what - curr->begin();
            if (offset >= 0 && offset < bucksz) {
                /* assert(offset % curr->sz == 0); */
                curr->free(what);
                free_blocks_for_bucket[translate[curr->sz]].insert(curr);
                return;
            }
            curr = curr->next;
        }
        assert(false);
    }

private:
    block<bucksz> *head;
    block<bucksz> *tail;
    std::unordered_set<block<bucksz> *> free_blocks_for_bucket[26];
};

#endif
