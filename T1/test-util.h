void resetTime();
int getTime();
uint64_t randNum(void);
void srandNum(uint64_t seed);

#define CHECK_I32(exp,ref) check_i32(exp,ref,#exp)
void check_i32(int32_t x, int32_t ref, char *str);

#define CHECK_U64(exp,ref) check_u64(exp,ref,#exp)
void check_u64(uint64_t x, uint64_t ref, char *str);

typedef void (*BenchFun)(void *ptr);

void run_bench( BenchFun init, BenchFun bench, BenchFun clean, void *ptr,
                int tiempo_ref, int tolerancia, int nintentos );

void test_unitario(void);
void benchmark(int refs[], int n);
