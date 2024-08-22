#include <tchar.h>
#include <windows.h>
#include "test.cuh"

template <typename F, int shift, int count>
class CopyLoop {
	public:
	inline static
	void _copy(F f, volatile unsigned int * const dst, volatile unsigned int * const src) {
		if(count <=0)
			return;
		constexpr unsigned int int_index = ((count - 1) << shift);
		f(dst + int_index, src + int_index);
		CopyLoop<F, shift, count - 1>::_copy(f, dst, src);
	}
};
template<typename F, unsigned int SFT>
class CopyLoop<F, SFT, 0> {
	public:
	inline static
	void _copy(F f, volatile unsigned int * const dst, volatile unsigned int * const src) {}
};

static
void memcpy16(volatile UINT32 *d, volatile UINT32 *s)
{
    int a=1;
	a++;
	// asm volatile("\n{\
	// 	.reg .u32 d0;\n\
	// 	.reg .u32 d1;\n\
	// 	.reg .u32 d2;\n\
	// 	.reg .u32 d3;\n\
	// 	ld.cv.v4.u32 { d0,  d1,  d2,  d3}, [%0];\n\
	// 	st.wt.v4.u32 [%1], { d0,  d1,  d2,  d3};}" \
	// 	: : "l"(s), "l"(d) : "memory");
}


int _tmain(int argc, TCHAR* argv[])
{
    //LaunchGpuThreads(nullptr);
    UINT32 src[64] = {0};
    UINT32 dest[64] = {0};
CopyLoop<__typeof__(memcpy16), 2, 16>::_copy(memcpy16, dest, src);
}
