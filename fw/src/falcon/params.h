#ifndef PARAMS_H
#define PARAMS_H


#define FALCON_NAMESPACE(s) falcon_##s

#ifndef FALCON_LOG_N
#define FALCON_LOG_N 9	/* Change this for different security strengths */
#endif

#define N     (1 << FALCON_LOG_N)

#define Q        12289
#define LOG_Q    14
#define Q0I      12287
#define R        4091
#define R2       10952
#define INV2     6145

#if FALCON_LOG_N == 9
#define L2_BOUND 34034726
#elif FALCON_LOG_N == 10
#define L2_BOUND 70265242
#endif

#ifndef CONFIG_H
#define CONFIG_H


#endif
#endif