/*
 * Copyright 2019 Xilinx, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "hjkernel.hpp"
#include "xf_database/hash_join_v3.hpp"

//--------------------------------------read status--------------------------------
void read_status(ap_uint<32> hj_begin_status[BUILD_CFG_DEPTH], hls::stream<ap_uint<32> >& pu_begin_status_strms) {
    for (int i = 0; i < BUILD_CFG_DEPTH; i++) pu_begin_status_strms.write(hj_begin_status[i]);
}

//--------------------------------------write status--------------------------------
void write_status(ap_uint<32> hj_end_status[BUILD_CFG_DEPTH], hls::stream<ap_uint<32> >& pu_end_status_strms) {
    for (int i = 0; i < BUILD_CFG_DEPTH; i++) hj_end_status[i] = pu_end_status_strms.read();
}

//--------------------------------------scan----------------------------------------
void scan(ap_uint<(WKEY + WPAY) * VEC_LEN>* s_unit,
          int num,

          hls::stream<ap_uint<WKEY> > k_strms[VEC_LEN],
          hls::stream<ap_uint<WPAY> > p_strms[VEC_LEN],
          hls::stream<bool> o_e_strm[VEC_LEN]) {
#ifndef __SYNTHESIS__
    unsigned int cnt = 0;
#endif

    for (int i = 0; i < num; i++) {
#pragma HLS pipeline
        for (int j = 0; j < VEC_LEN; j++) {
#pragma HLS unroll

            ap_uint<WKEY> k_strms_temp = s_unit[i]((WKEY + WPAY) * (j + 1) - 1, (WKEY + WPAY) * j + WPAY);
            ap_uint<WPAY> p_strms_temp = s_unit[i]((WKEY + WPAY) * j + WPAY - 1, (WKEY + WPAY) * j);

            k_strms[j].write(k_strms_temp);
            p_strms[j].write(p_strms_temp);
            o_e_strm[j].write(false);

#ifndef __SYNTHESIS__
            if (cnt < 10)
                std::cout << "scan: " << cnt << " key=" << std::hex << k_strms_temp << " pld=" << p_strms_temp
                          << std::endl;
            cnt++;
#endif
        }
    }

    for (int j = 0; j < VEC_LEN; j++) {
#pragma HLS unroll
        o_e_strm[j].write(true);
    }
}

//------------------------------------write back------------------------------------
template <int JW>
void write_back(bool build_probe_flag,
                hls::stream<ap_uint<JW> >& i_jrow_strm,
                hls::stream<bool>& i_e_strm,

                ap_uint<512>* o_buf) {
#pragma HLS INLINE off

    ap_uint<32> i = 0;

    if (build_probe_flag) {
        // write join result back if it is probe
        bool last = i_e_strm.read();
        while (!last) {
            o_buf[i] = i_jrow_strm.read();
            last = i_e_strm.read();
            i++;
        }
    }

#ifndef __SYNTHESIS__
    std::cout << std::dec << "Write back " << i << " rows" << std::endl;
#endif
}

//--------------------------------------hash join build kernel----------------------

extern "C" void hjkernel(
    // input
    uint8_t build_probe_flag,
    uint32_t nrow,                                      // input, number of row in s unit
    ap_uint<(WKEY + WPAY) * VEC_LEN> unit[T_MAX_DEPTH], // input, 4 row per vec. DDR

    // input hash-table
    ap_uint<64> pu0_ht[PU_HT_DEPTH], // PU0 hash-tables
    ap_uint<64> pu1_ht[PU_HT_DEPTH], // PU0 hash-tables
    ap_uint<64> pu2_ht[PU_HT_DEPTH], // PU0 hash-tables
    ap_uint<64> pu3_ht[PU_HT_DEPTH], // PU0 hash-tables
    ap_uint<64> pu4_ht[PU_HT_DEPTH], // PU0 hash-tables
    ap_uint<64> pu5_ht[PU_HT_DEPTH], // PU0 hash-tables
    ap_uint<64> pu6_ht[PU_HT_DEPTH], // PU0 hash-tables
    ap_uint<64> pu7_ht[PU_HT_DEPTH], // PU0 hash-tables

    // input S units
    ap_uint<64> pu0_s[PU_S_DEPTH], // PU0 S units
    ap_uint<64> pu1_s[PU_S_DEPTH], // PU0 S units
    ap_uint<64> pu2_s[PU_S_DEPTH], // PU0 S units
    ap_uint<64> pu3_s[PU_S_DEPTH], // PU0 S units
    ap_uint<64> pu4_s[PU_S_DEPTH], // PU0 S units
    ap_uint<64> pu5_s[PU_S_DEPTH], // PU0 S units
    ap_uint<64> pu6_s[PU_S_DEPTH], // PU0 S units
    ap_uint<64> pu7_s[PU_S_DEPTH], // PU0 S units

    // status
    ap_uint<32> hj_begin_status[BUILD_CFG_DEPTH], // status. DDR
    ap_uint<32> hj_end_status[BUILD_CFG_DEPTH],   // status. DDR

    // output join result
    ap_uint<512> j_res[J_MAX_DEPTH] // output. DDR
    ) {
#pragma HLS INTERFACE m_axi offset = slave latency = 64 num_write_outstanding = 16 num_read_outstanding = \
    16 max_write_burst_length = 64 max_read_burst_length = 64 bundle = gmem0_0 port = unit

#pragma HLS INTERFACE m_axi offset = slave latency = 64 num_write_outstanding = 16 num_read_outstanding = \
    16 max_write_burst_length = 64 max_read_burst_length = 64 bundle = gmem0_1 port = hj_begin_status

#pragma HLS INTERFACE m_axi offset = slave latency = 64 num_write_outstanding = 16 num_read_outstanding = \
    16 max_write_burst_length = 64 max_read_burst_length = 64 bundle = gmem0_1 port = hj_end_status

#pragma HLS INTERFACE m_axi offset = slave latency = 64 num_write_outstanding = 16 num_read_outstanding = \
    16 max_write_burst_length = 64 max_read_burst_length = 64 bundle = gmem0_2 port = j_res

#pragma HLS INTERFACE s_axilite port = build_probe_flag bundle = control
#pragma HLS INTERFACE s_axilite port = unit bundle = control
#pragma HLS INTERFACE s_axilite port = nrow bundle = control
#pragma HLS INTERFACE s_axilite port = hj_begin_status bundle = control
#pragma HLS INTERFACE s_axilite port = hj_end_status bundle = control
#pragma HLS INTERFACE s_axilite port = j_res bundle = control

#pragma HLS INTERFACE m_axi port = pu0_ht bundle = gmem1_0 num_write_outstanding = 32 num_read_outstanding = \
    32 max_read_burst_length = 8 latency = 125
#pragma HLS INTERFACE m_axi port = pu1_ht bundle = gmem1_1 num_write_outstanding = 32 num_read_outstanding = \
    32 max_read_burst_length = 8 latency = 125
#pragma HLS INTERFACE m_axi port = pu2_ht bundle = gmem1_2 num_write_outstanding = 32 num_read_outstanding = \
    32 max_read_burst_length = 8 latency = 125
#pragma HLS INTERFACE m_axi port = pu3_ht bundle = gmem1_3 num_write_outstanding = 32 num_read_outstanding = \
    32 max_read_burst_length = 8 latency = 125
#pragma HLS INTERFACE m_axi port = pu4_ht bundle = gmem1_4 num_write_outstanding = 32 num_read_outstanding = \
    32 max_read_burst_length = 8 latency = 125
#pragma HLS INTERFACE m_axi port = pu5_ht bundle = gmem1_5 num_write_outstanding = 32 num_read_outstanding = \
    32 max_read_burst_length = 8 latency = 125
#pragma HLS INTERFACE m_axi port = pu6_ht bundle = gmem1_6 num_write_outstanding = 32 num_read_outstanding = \
    32 max_read_burst_length = 8 latency = 125
#pragma HLS INTERFACE m_axi port = pu7_ht bundle = gmem1_7 num_write_outstanding = 32 num_read_outstanding = \
    32 max_read_burst_length = 8 latency = 125

#pragma HLS INTERFACE s_axilite port = pu0_ht bundle = control
#pragma HLS INTERFACE s_axilite port = pu1_ht bundle = control
#pragma HLS INTERFACE s_axilite port = pu2_ht bundle = control
#pragma HLS INTERFACE s_axilite port = pu3_ht bundle = control
#pragma HLS INTERFACE s_axilite port = pu4_ht bundle = control
#pragma HLS INTERFACE s_axilite port = pu5_ht bundle = control
#pragma HLS INTERFACE s_axilite port = pu6_ht bundle = control
#pragma HLS INTERFACE s_axilite port = pu7_ht bundle = control

#pragma HLS INTERFACE m_axi port = pu0_s bundle = gmem2_0 num_write_outstanding = 32 num_read_outstanding = \
    32 max_read_burst_length = 8 latency = 125
#pragma HLS INTERFACE m_axi port = pu1_s bundle = gmem2_1 num_write_outstanding = 32 num_read_outstanding = \
    32 max_read_burst_length = 8 latency = 125
#pragma HLS INTERFACE m_axi port = pu2_s bundle = gmem2_2 num_write_outstanding = 32 num_read_outstanding = \
    32 max_read_burst_length = 8 latency = 125
#pragma HLS INTERFACE m_axi port = pu3_s bundle = gmem2_3 num_write_outstanding = 32 num_read_outstanding = \
    32 max_read_burst_length = 8 latency = 125
#pragma HLS INTERFACE m_axi port = pu4_s bundle = gmem2_4 num_write_outstanding = 32 num_read_outstanding = \
    32 max_read_burst_length = 8 latency = 125
#pragma HLS INTERFACE m_axi port = pu5_s bundle = gmem2_5 num_write_outstanding = 32 num_read_outstanding = \
    32 max_read_burst_length = 8 latency = 125
#pragma HLS INTERFACE m_axi port = pu6_s bundle = gmem2_6 num_write_outstanding = 32 num_read_outstanding = \
    32 max_read_burst_length = 8 latency = 125
#pragma HLS INTERFACE m_axi port = pu7_s bundle = gmem2_7 num_write_outstanding = 32 num_read_outstanding = \
    32 max_read_burst_length = 8 latency = 125

#pragma HLS INTERFACE s_axilite port = pu0_s bundle = control
#pragma HLS INTERFACE s_axilite port = pu1_s bundle = control
#pragma HLS INTERFACE s_axilite port = pu2_s bundle = control
#pragma HLS INTERFACE s_axilite port = pu3_s bundle = control
#pragma HLS INTERFACE s_axilite port = pu4_s bundle = control
#pragma HLS INTERFACE s_axilite port = pu5_s bundle = control
#pragma HLS INTERFACE s_axilite port = pu6_s bundle = control
#pragma HLS INTERFACE s_axilite port = pu7_s bundle = control

#pragma HLS INTERFACE s_axilite port = return bundle = control

#pragma HLS dataflow

    // read status
    hls::stream<ap_uint<32> > pu_begin_status_strms; // num s unit, pu s base ptr.

    read_status(hj_begin_status, pu_begin_status_strms);

    // scan
    hls::stream<ap_uint<WKEY> > k_strms[VEC_LEN];
#pragma HLS stream variable = k_strms depth = 8
#pragma HLS array_partition variable = k_strms dim = 0
    hls::stream<ap_uint<WPAY> > p_strms[VEC_LEN];
#pragma HLS stream variable = p_strms depth = 8
#pragma HLS array_partition variable = p_strms dim = 0
#pragma HLS resource variable = p_strms core = FIFO_SRL
    hls::stream<bool> e_strms[VEC_LEN];
#pragma HLS stream variable = e_strms depth = 8
#pragma HLS array_partition variable = e_strms dim = 0

    // write back
    hls::stream<ap_uint<WKEY + 2 * WPAY> > j_strm;
#pragma HLS stream variable = j_strm depth = 8
#pragma HLS resource variable = j_strm core = FIFO_SRL
    hls::stream<bool> j_e_strm;
#pragma HLS stream variable = j_e_strm depth = 8

#ifndef __SYNTHESIS__
    std::cout << "kernel start" << std::endl;
#endif

    scan(unit, nrow, k_strms, p_strms, e_strms);

    // hash join build
    hls::stream<ap_uint<32> > pu_end_status_strms;
    bool flag = build_probe_flag != 0;

    xf::database::hashBuildProbeV3<HJ_MODE,        // 0 - radix, 1 - Jenkins
                                   WKEY,           // key size
                                   WPAY,           // payload maxsize
                                   WPAY,           // S_PW width of payload of small table.
                                   WPAY,           // B_PW width of payload of big table.
                                   WPUHASH,        // PU number=1<<WPUHASH
                                   WHASH,          // width of lower hash value
                                   24,             // addr width
                                   VEC_LEN,        // channel number
                                   BLOOM_FILTER_W, // bloom filter hash width
                                   EN_BLOOM_FILTER // bloom filter switch on-off
                                   >(
        // input
        flag, k_strms, p_strms, e_strms,

        // output hash table
        pu0_ht, // PU0 hash-tables
        pu1_ht, // PU0 hash-tables
        pu2_ht, // PU0 hash-tables
        pu3_ht, // PU0 hash-tables
        pu4_ht, // PU0 hash-tables
        pu5_ht, // PU0 hash-tables
        pu6_ht, // PU0 hash-tables
        pu7_ht, // PU0 hash-tables

        // output PU S unit
        pu0_s, // PU0 S units
        pu1_s, // PU0 S units
        pu2_s, // PU0 S units
        pu3_s, // PU0 S units
        pu4_s, // PU0 S units
        pu5_s, // PU0 S units
        pu6_s, // PU0 S units
        pu7_s, // PU0 S units

        // input PU base ptr
        pu_begin_status_strms,

        // output PU end ptr
        pu_end_status_strms,

        // output join result
        j_strm, j_e_strm);

    // write back
    write_back<WKEY + 2 * WPAY>(flag, j_strm, j_e_strm, j_res);

    // write status
    write_status(hj_end_status, pu_end_status_strms);

#ifndef __SYNTHESIS__
    std::cout << "kernel end" << std::endl;
#endif
}
