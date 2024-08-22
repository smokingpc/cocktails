#include <cuda_runtime.h>
#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include "test.cuh"

template<typename F, unsigned int shift, unsigned int count>
class CopyLoop {
	public:
	__device__ __forceinline__ static
	void _copy(F f, volatile uint32_t * const dst, volatile uint32_t * const src) {
		if(count < 512)
		{
			constexpr unsigned int_index = ((count - 1) << shift);
			f(dst + int_index, src + int_index);
			CopyLoop<F, shift, count - 1>::_copy(f, dst, src);
		}
	}
};

template<typename F, int SFT>
class CopyLoop<F, SFT, 0> {
	public:
	__device__ __forceinline__ static
	void _copy(F f, volatile uint32_t * const dst, volatile uint32_t * const src) {}
};

__device__ __forceinline__ static
void memcpy16(volatile uint32_t *d, volatile uint32_t *s)
{
	asm volatile("\n{\
		.reg .u32 d0;\n\
		.reg .u32 d1;\n\
		.reg .u32 d2;\n\
		.reg .u32 d3;\n\
		ld.cv.v4.u32 { d0,  d1,  d2,  d3}, [%0];\n\
		st.wt.v4.u32 [%1], { d0,  d1,  d2,  d3};}" \
		: : "l"(s), "l"(d) : "memory");
}


__device__ bool DoDeviceCode()
{
    constexpr uint32_t LENGTH = 16;
	constexpr uint32_t copy_nr_64bytes = LENGTH >> 6;
	constexpr uint32_t copy_nr_16bytes = (LENGTH >> 4) - (copy_nr_64bytes << 2);
    uint32_t dst16[64] = {0}, src16[64] = {0};
    CopyLoop<__typeof__(memcpy16), 2, copy_nr_16bytes>::_copy(memcpy16, dst16, src16);
    return true;
}
__global__ void DoGpuJob()
{
    DoDeviceCode();
}

__host__ void GpuThreads(void* ctx)
{
    DoGpuJob();
}

void LaunchGpuThreads(void* ctx)
{
	GpuThreads(ctx);
}
