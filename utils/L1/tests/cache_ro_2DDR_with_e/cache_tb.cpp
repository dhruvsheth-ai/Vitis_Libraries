#include "cache_tb.hpp"

int main() {
    hls::stream<ap_uint<BUSADDRWIDTH> > raddrStrm("main_raddrStrm");
    hls::stream<bool> e_raddrStrm;
    hls::stream<ap_uint<BUSDATAWIDTH> > rdata0Strm("main_rdata0Strm");
    hls::stream<bool> e_rdata0Strm;
    hls::stream<ap_uint<BUSDATAWIDTH> > rdata1Strm("main_rdata1Strm");
    hls::stream<bool> e_rdata1Strm;

    ap_uint<512>* ddrMem0 = (ap_uint<512>*)malloc(DDRSIZEIN512 * sizeof(ap_uint<512>));
    ap_uint<BUSDATAWIDTH>* ref0 = reinterpret_cast<ap_uint<BUSDATAWIDTH>*>(ddrMem0);
    for (int i = 0; i < (1 << TOTALADDRWIDTH); i++) {
        ref0[i] = i;
    }

    int* rd = (int*)malloc(200000 * sizeof(int));
    int i;
    for (i = 0; i < 1000; i++) {
        rd[i] = (long)rand() * (long)rand() % (1 << TOTALADDRWIDTH);
        raddrStrm.write(rd[i]);
        e_raddrStrm.write(0);
    }

    int base = (long)rand() * (long)rand() % (1 << TOTALADDRWIDTH);
    for (; i < 2000; i++) {
        rd[i] = base;
        raddrStrm.write(rd[i]);
        e_raddrStrm.write(0);
    }
    e_raddrStrm.write(1);

    syn_top(raddrStrm, e_raddrStrm, rdata0Strm, e_rdata0Strm, rdata1Strm, e_rdata1Strm, ddrMem0, ddrMem0);

    int err = 0;
    for (i = 0; i < 2000; i++) {
        int data0 = rdata0Strm.read();
        int data1 = rdata1Strm.read();
        e_rdata0Strm.read();
        e_rdata1Strm.read();
        if (data0 != ref0[rd[i]] || data1 != ref0[rd[i]]) {
            std::cout << "error: " << data0 << " " << data1 << " " << i << " " << rd[i] << " " << ref0[rd[i]]
                      << " ddrMem is: ";
            std::cout << ddrMem0[rd[i] / 8].range(511, 448) << " " << ddrMem0[rd[i] / 8].range(447, 384) << " "
                      << ddrMem0[rd[i] / 8].range(383, 320) << " " << ddrMem0[rd[i] / 8].range(319, 256) << " ";
            std::cout << ddrMem0[rd[i] / 8].range(255, 192) << " " << ddrMem0[rd[i] / 8].range(191, 128) << " "
                      << ddrMem0[rd[i] / 8].range(127, 64) << " " << ddrMem0[rd[i] / 8].range(63, 0) << std::endl;
            ++err;
        }
    }
    e_rdata0Strm.read();
    e_rdata1Strm.read();
    free(ddrMem0);

    std::cout << (err == 0 ? "PASS" : "FAIL") << std::endl;
    return err;
}
