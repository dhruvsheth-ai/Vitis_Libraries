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

#ifndef XF_BLAS_GBMV_HPP
#define XF_BLAS_GBMV_HPP

#ifndef __cplusplus
#error "BLAS Library only works with C++."
#endif

#include "ap_int.h"
#include "hls_stream.h"
#include "xf_blas/helpers.hpp"

namespace xf {
namespace linear_algebra {
namespace blas {

/**
 * @brief gbmv function performs general banded matrix-vector multiplication
 *
 * @tparam t_DataType the data type of the vector entries
 * @tparam t_ParEntries the number of parallelly processed entries in the input vector
 * @tparam t_MacType the datawidth of the datatype t_DataType of the input vector
 * @tparam t_IndexType the datatype of the index
 *
 * @param p_n the number of entries in the input vector p_x, p_n % l_ParEntries == 0
 * @param p_x the input stream of packed vector entries
 * @param p_sum the sum, which is 0 if p_n <= 0
 */
template <typename t_DataType,
          unsigned int t_ParEntries,
          unsigned int t_MaxRows,
          typename t_IndexType = unsigned int,
          typename t_MacType = t_DataType>
void gbmv(const unsigned int p_m,
          const unsigned int p_n,
          const unsigned int p_kl,
          const unsigned int p_ku,
          hls::stream<WideType<t_DataType, t_ParEntries> >& p_A,
          hls::stream<WideType<t_DataType, t_ParEntries> >& p_x,
          hls::stream<WideType<t_MacType, t_ParEntries> >& p_y) {
#pragma HLS data_pack variable = p_A
#pragma HLS data_pack variable = p_x
#pragma HLS data_pack variable = p_y
#ifndef __SYNTHESIS__
    assert(p_m <= t_MaxRows);
    assert(p_m == p_n);
    assert(p_m % t_ParEntries == 0);
#endif

    const unsigned int l_MaxIter = t_MaxRows / t_ParEntries;
    const unsigned int l_threshold = p_m / t_ParEntries;
    WideType<t_MacType, t_ParEntries> l_y[l_MaxIter];

    for (t_IndexType j = 0; j < p_kl + 1 + p_ku; j++) {
        for (t_IndexType l = 0; l < l_MaxIter; l++) {
#pragma HLS PIPELINE
            if (l >= l_threshold) break;
            WideType<t_DataType, t_ParEntries> l_A = p_A.read();
            WideType<t_DataType, t_ParEntries> l_x = p_x.read();
            for (t_IndexType k = 0; k < t_ParEntries; k++) {
#pragma HLS UNROLL
                if (j == 0)
                    l_y[l][k] = l_A[k] * l_x[k];
                else
                    l_y[l][k] += l_A[k] * l_x[k];
            }
            if (j == p_kl + p_ku) p_y.write(l_y[l]);
        }
    }
}

/**
 * @brief gbmv function performs general banded matrix-vector multiplication
 * matrix and a vector y = alpha * M * x + beta * y
 *
 * @tparam t_DataType the data type of the vector entries
 * @tparam t_ParEntries the number of parallelly processed entries in the input vector
 * @tparam t_MaxRows the maximum size of buffers for output vector
 * @tparam t_IndexType the datatype of the index
 * @tparam t_MacType the datatype of the output stream
 *
 * @param p_m the number of rows of input matrix p_M
 * @param p_alpha, scalar alpha
 * @param p_M the input stream of packed Matrix entries
 * @param p_x the input stream of packed vector entries
 * @param p_beta, scalar beta
 * @param p_y the output vector
 */
template <typename t_DataType,
          unsigned int t_ParEntries,
          unsigned int t_MaxRows,
          typename t_IndexType = unsigned int,
          typename t_MacType = t_DataType>
void gbmv(const unsigned int p_m,
          const unsigned int p_n,
          const unsigned int p_kl,
          const unsigned int p_ku,
          const t_DataType p_alpha,
          hls::stream<WideType<t_DataType, t_ParEntries> >& p_M,
          hls::stream<WideType<t_DataType, t_ParEntries> >& p_x,
          const t_DataType p_beta,
          hls::stream<WideType<t_DataType, t_ParEntries> >& p_y,
          hls::stream<WideType<t_DataType, t_ParEntries> >& p_yr) {
#pragma HLS data_pack variable = p_M
#pragma HLS data_pack variable = p_x
#pragma HLS data_pack variable = p_y
#pragma HLS data_pack variable = p_yr
    hls::stream<WideType<t_DataType, t_ParEntries> > l_x, l_y;
#pragma HLS DATAFLOW
    gbmv<t_DataType, t_ParEntries, t_MaxRows, t_IndexType, t_MacType>(p_m, p_n, p_kl, p_ku, p_M, p_x, l_x);
    scal<t_DataType, t_ParEntries, t_IndexType>(p_m, p_beta, p_y, l_y);
    axpy<t_DataType, t_ParEntries, t_IndexType>(p_m, p_alpha, l_x, l_y, p_yr);
}

} // end namespace blas
} // namespace linear_algebra
} // end namespace xf

#endif
