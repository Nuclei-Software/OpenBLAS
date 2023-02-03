/***************************************************************************
Copyright (c) 2022, The OpenBLAS Project
All rights reserved.
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:
1. Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in
the documentation and/or other materials provided with the
distribution.
3. Neither the name of the OpenBLAS project nor the names of
its contributors may be used to endorse or promote products
derived from this software without specific prior written permission.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE OPENBLAS PROJECT OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*****************************************************************************/

#include "common.h"

#if !defined(DOUBLE)
#define VSETVL(n) vsetvl_e32m4(n)
#define VSETVL_MAX vsetvlmax_e32m4()
#define FLOAT_V_T vfloat32m4_t
#define FLOAT_V_T_M1 vfloat32m1_t
#define VLSEG_FLOAT vlseg2e32_v_f32m4
#define VLSSEG_FLOAT vlsseg2e32_v_f32m4
#define VFREDSUMVS_FLOAT vfredusum_vs_f32m4_f32m1
#define VFMVVF_FLOAT vfmv_v_f_f32m4
#define VFMVVF_FLOAT_M1 vfmv_v_f_f32m1
#define VFMVFS_FLOAT_M1 vfmv_f_s_f32m1_f32
#define VFADDVV_FLOAT vfadd_vv_f32m4
#else
#define VSETVL(n) vsetvl_e64m4(n)
#define VSETVL_MAX vsetvlmax_e64m4()
#define FLOAT_V_T vfloat64m4_t
#define FLOAT_V_T_M1 vfloat64m1_t
#define VLSEG_FLOAT vlseg2e64_v_f64m4
#define VLSSEG_FLOAT vlsseg2e64_v_f64m4
#define VFREDSUMVS_FLOAT vfredusum_vs_f64m4_f64m1
#define VFMVVF_FLOAT vfmv_v_f_f64m4
#define VFMVVF_FLOAT_M1 vfmv_v_f_f64m1
#define VFMVFS_FLOAT_M1 vfmv_f_s_f64m1_f64
#define VFADDVV_FLOAT vfadd_vv_f64m4
#endif

FLOAT CNAME(BLASLONG n, FLOAT *x, BLASLONG inc_x)
{
    FLOAT sumf = 0.0;
    if (n <= 0 || inc_x <= 0) return(sumf);

    FLOAT_V_T v0, v1;
    size_t vlmax = VSETVL_MAX; 
    FLOAT_V_T v_sum = VFMVVF_FLOAT(0, vlmax);

    if(inc_x == 1) {

        for (size_t vl; n > 0; n -= vl, x += vl*2) {
            vl = VSETVL(n);

            VLSEG_FLOAT(&v0, &v1, x, vl);

            v_sum = VFADDVV_FLOAT(v_sum, v0, vl);
            v_sum = VFADDVV_FLOAT(v_sum, v1, vl);
        }

    } else {

        BLASLONG stride_x = inc_x * sizeof(FLOAT) * 2;

        for (size_t vl; n > 0; n -= vl, x += vl*inc_x*2) {
            vl = VSETVL(n);

            VLSSEG_FLOAT(&v0, &v1, x, stride_x, vl);

            v_sum = VFADDVV_FLOAT(v_sum, v0, vl);
            v_sum = VFADDVV_FLOAT(v_sum, v1, vl);
        }

    }

    FLOAT_V_T_M1 v_z0 = VFMVVF_FLOAT_M1(0, vlmax);
    FLOAT_V_T_M1 v_res = VFMVVF_FLOAT_M1(0, vlmax);
    v_res = VFREDSUMVS_FLOAT(v_res, v_sum, v_z0, vlmax);
    sumf += VFMVFS_FLOAT_M1(v_res);

    return(sumf);
}