#include "stdio.h"

template<typename F, unsigned int shift, unsigned int count>
class CopyLoop {
	public:
	inline static
	void _copy(F f, volatile unsigned int * const dst, volatile unsigned int * const src) {
		//if(count <= 0 || count > 512)
        if(count <= 512)
        {
            constexpr unsigned int next_count = count - 1;
            constexpr unsigned int int_index = (next_count << shift);
            f(dst + int_index, src + int_index);
            CopyLoop<F, shift, next_count>::_copy(f, dst, src);
        }
	}
};

template<typename F, unsigned int SFT>
class CopyLoop<F, SFT, 0> {
	public:
	inline static
	void _copy(F f, volatile unsigned int * const dst, volatile unsigned int * const src) {}
};

static
void memcpy16(volatile unsigned int *d, volatile unsigned int *s)
{
    int a=1;
	// asm volatile("\n{\
	// 	.reg .u32 d0;\n\
	// 	.reg .u32 d1;\n\
	// 	.reg .u32 d2;\n\
	// 	.reg .u32 d3;\n\
	// 	ld.cv.v4.u32 { d0,  d1,  d2,  d3}, [%0];\n\
	// 	st.wt.v4.u32 [%1], { d0,  d1,  d2,  d3};}" \
	// 	: : "l"(s), "l"(d) : "memory");
}


int main(int argc, char* argv[])
{
    unsigned int src[64] = {0};
    unsigned int dest[64] = {0};
CopyLoop<__typeof__(memcpy16), 2, 16>::_copy(memcpy16, dest, src);
    printf("Hello world.\n");    
    return 0;
}