//------------------------------------------------------------------------------
// File: mempool.hpp
// Author: Mikael Nilsson
// Contributor(s): Erik Ström
// Version: 0.2
// Orig. Date: 2016-02-11
// Update: 2019-12-29
//------------------------------------------------------------------------------

#pragma once

#include <iostream>
#include <cstring>
#include <mutex>
#include <initializer_list>
#include <cstdlib>
#include <stdexcept>

// Allocated memory and data structure
class Mem {
 public:
    Mem() : m_mem(nullptr), m_alloc_size(0),
        m_array_operator(false), m_error_destruct(false) {}
    void *m_mem;
    int m_alloc_size;
    bool m_array_operator;
    bool m_error_destruct;
};

class MemPool {
 public:
    // Default constructor (Create list with size 100000).
    MemPool() : ::MemPool(100000) {}

    // Standard constructor
    explicit MemPool(const size_t &initialCapacity)
            : m_data(nullptr), m_capacity(initialCapacity), m_alloc(0), m_dealloc(0) {
        m_tail = m_head = m_data = reinterpret_cast<Mem*>(
                std::malloc(sizeof(Mem) * m_capacity));
        m_end = m_data + m_capacity;
    }

    // Remove copy constructor
    MemPool(const MemPool &array) = delete;

    // Remove move constructor
    MemPool(MemPool &&array) = delete;

    // Cleanup memory
    ~MemPool() {
        int leak_size = 0;
        int error_destruct_size = 0;
        // Print stat
        for (auto it = m_head; it != m_tail; it++) {
            leak_size += it->m_alloc_size;
            error_destruct_size += (it->m_error_destruct ? 1 : 0);
        }

        std::cout << "=================< MEM STAT >==================="
                << std::endl
                << "Alloc        = " << m_alloc << std::endl
                << "Dealloc      = " << m_dealloc << std::endl
                << "Leak         = " << m_alloc - m_dealloc << std::endl
                << "------------------------------------------------"
                << std::endl;

        for (auto it = m_head; it != m_tail; it++) {
            std::cout << "Leak size = " << it->m_alloc_size << " bytes"
                << std::endl;
        }

        std::cout << "------------------------------------------------"
                << std::endl
                << "Total Leak size = " << leak_size << " bytes"  << std::endl
                << "Wrong delete operator used " << error_destruct_size << " times"
                << std::endl
                << "------------------------------------------------" << std::endl;

        if (m_data != nullptr) {
            std::free(m_data);
        }

        m_data = m_head = m_tail = nullptr;
    }

    Mem* find(void* element) {
        for (auto it = m_head; it != m_tail; it++) {
            if (element == it->m_mem) {
                return it;
            }
        }
        return nullptr;
    }

    // Remove (pointer)
    inline void remove(void* element, bool array_operator) {
        if (m_data != nullptr) {
            std::lock_guard<std::mutex> lock(m_mtx);
            Mem* mem = find(element);
            if (mem != nullptr) {
                if (mem->m_array_operator == array_operator) {
                    std::free(mem->m_mem);
                    *mem = *(--m_tail);
                    m_dealloc++;
                } else {
                    mem->m_error_destruct = true;
                }
            }
        } else {
            std::free(element);
        }
    }

    void* create(std::size_t size, bool array_operator) {
        if (m_data != nullptr) {
            std::lock_guard<std::mutex> lock(m_mtx);
            // More memory needed
            if (m_tail == m_end) {
                // double the capacity
                m_capacity *= 2;

                // Copy pointers (deallocate pointer data)
                Mem* tmp_data = reinterpret_cast<Mem*>(
                        std::malloc(sizeof(Mem) * m_capacity));
                memcpy(tmp_data, m_data, sizeof(Mem)*(m_tail - m_head));
                std::free(m_data);

                // copy iterators
                m_tail = tmp_data + (m_tail - m_head);
                m_data = m_head = tmp_data;
                m_end = m_data + m_capacity;
            }
            // Insert new element.
            m_alloc++;
            m_tail->m_alloc_size = size;
            m_tail->m_array_operator = array_operator;
            m_tail->m_error_destruct = false;
            return (m_tail++)->m_mem = std::malloc(size);
        } else {
            return std::malloc(size);
        }
    }

    // Deep copy is not supported!
    MemPool &operator=(const MemPool& array) = delete;

    // Remove move assignment operator
    MemPool &operator=(MemPool&& array) = delete;

 private:
    std::mutex m_mtx;
    Mem *m_data;
    Mem *m_head, *m_tail, *m_end;
    unsigned int m_capacity;
    int m_alloc, m_dealloc;
};


