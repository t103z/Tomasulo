#ifndef TOMASULO_MEM_H
#define TOMASULO_MEM_H

#include <vector>
#include <cstddef>

/*!
 * 存储器
 */
class Mem {
public:
    std::vector<double> mem;

    Mem(std::size_t n) : mem(n, 0.0) {
        for (int i = 0; i < n; ++i) {
            mem[i] = i;
        }
    }

    /*!
     * 不安全的存储器读
     * @param addr 地址
     * @return 存储器值
     */
    double& operator[](size_t addr){
        return mem[addr];
    }

    /*!
     * 安全的存储器读
     * @param addr 地址
     * @return 存储器值
     */
    double get(std::size_t addr){
        if (addr >= mem.size()) return 0;
        return mem[addr];
    }

    /*!
     * 安全的存储器写
     * @param addr 地址
     * @param value 值
     */
    void set(std::size_t addr, double value) {
        if (addr >= mem.size()) return;
        mem[addr] = value;
    }

    void reset(){
        for(auto& m : mem) m = 0.0;
    }
};


#endif //TOMASULO_MEM_H
