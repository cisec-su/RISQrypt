#include <stdio.h>
// Q = 65537 for PASTA
#define Q 65537

int main() {
    // Test if values can exceed Q
    unsigned long a = 65536;  // Close to Q
    unsigned long b = 65536;
    unsigned long sum = a + b;
    
    printf("a = %lu, b = %lu\n", a, b);
    printf("a + b = %lu (0x%lx)\n", sum, sum);
    printf("Q = %d (0x%x)\n", Q, Q);
    printf("sum > Q? %s\n", sum > Q ? "YES - needs reduction!" : "no");
    printf("sum %% Q = %lu\n", sum % Q);
    
    return 0;
}
