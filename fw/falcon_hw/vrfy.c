/*
 * Falcon signature verification.
 *
 * ==========================(LICENSE BEGIN)============================
 *
 * Copyright (c) 2017-2019  Falcon Project
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * ===========================(LICENSE END)=============================
 *
 * @author   Thomas Pornin <thomas.pornin@nccgroup.com>
 */

#include "inner.h"
#include "util.h"
#include "middle_val.h"
#include "ntt_lite.h"
#include "poly.h"

/* ===================================================================== */
/*
 * Constants for NTT.
 *
 *   n = 2^logn  (2 <= n <= 1024)
 *   phi = X^n + 1
 *   q = 12289
 *   q0i = -1/q mod 2^16
 *   R = 2^16 mod q
 *   R2 = 2^32 mod q
 */


/*
 * Table for NTT, binary case:
 *   GMb[x] = R*(g^rev(x)) mod q
 * where g = 7 (it is a 2048-th primitive root of 1 modulo q)
 * and rev() is the bit-reversal function over 10 bits.
 */
static const uint16_t GMb[] = {
	 4091,  7888, 11060, 11208,  6960,  4342,  6275,  9759, 1591,  6399,  9477,  5266,   586,  5825,  7538,  9710, 1134,  6407,  1711,   965,  7099,  7674,  3743,  6442,10414,  8100,  1885,  1688,  1364, 10329, 10164,  9180,12210,  6240,   997,   117,  4783,  4407,  1549,  7072, 2829,  6458,  4431,  8877,  7144,  2564,  5664,  4042,12189,   432, 10751,  1237,  7610,  1534,  3983,  7863, 2181,  6308,  8720,  6570,  4843,  1690,    14,  3872, 5569,  9368, 12163,  2019,  7543,  2315,  4673,  7340, 1553,  1156,  8401, 11389,  1020,  2967, 10772,  7045, 3316, 11236,  5285, 11578, 10637, 10086,  9493,  6180, 9277,  6130,  3323,   883, 10469,   489,  1502,  2851,11061,  9729,  2742, 12241,  4970, 10481, 10078,  1195,  730,  1762,  3854,  2030,  5892, 10922,  9020,  5274, 9179,  3604,  3782, 10206,  3180,  3467,  4668,  2446, 7613,  9386,   834,  7703,  6836,  3403,  5351, 12276, 3580,  1739, 10820,  9787, 10209,  4070, 12250,  8525,10401,  2749,  7338, 10574,  6040,   943,  9330,  1477, 6865,  9668,  3585,  6633, 12145,  4063,  3684,  7680, 8188,  6902,  3533,  9807,  6090,   727, 10099,  7003, 6945,  1949,  9731, 10559,  6057,   378,  7871,  8763, 8901,  9229,  8846,  4551,  9589, 11664,  7630,  8821, 5680,  4956,  6251,  8388, 10156,  8723,  2341,  3159, 1467,  5460,  8553,  7783,  2649,  2320,  9036,  6188,  737,  3698,  4699,  5753,  9046,  3687,    16,   914, 5186, 10531,  4552,  1964,  3509,  8436,  7516,  5381,10733,  3281,  7037,  1060,  2895,  7156,  8887,  5357, 6409,  8197,  2962,  6375,  5064,  6634,  5625,   278,  932, 10229,  8927,  7642,   351,  9298,   237,  5858, 7692,  3146, 12126,  7586,  2053, 11285,  3802,  5204, 4602,  1748, 11300,   340,  3711,  4614,   300, 10993, 5070, 10049, 11616, 12247,  7421, 10707,  5746,  5654, 3835,  5553,  1224,  8476,  9237,  3845,   250, 11209, 4225,  6326,  9680, 12254,  4136,  2778,   692,  8808, 6410,  6718, 10105, 10418,  3759,  7356, 11361,  8433, 6437,  3652,  6342,  8978,  5391,  2272,  6476,  7416, 8418, 10824, 11986,  5733,   876,  7030,  2167,  2436, 3442,  9217,  8206,  4858,  5964,  2746,  7178,  1434, 7389,  8879, 10661, 11457,  4220,  1432, 10832,  4328, 8557,  1867,  9454,  2416,  3816,  9076,   686,  5393, 2523,  4339,  6115,   619,   937,  2834,  7775,  3279, 2363,  7488,  6112,  5056,   824, 10204, 11690,  1113, 2727,  9848,   896,  2028,  5075,  2654, 10464,  7884,12169,  5434,  3070,  6400,  9132, 11672, 12153,  4520, 1273,  9739, 11468,  9937, 10039,  9720,  2262,  9399,11192,   315,  4511,  1158,  6061,  6751, 11865,   357, 7367,  4550,   983,  8534,  8352, 10126,  7530,  9253, 4367,  5221,  3999,  8777,  3161,  6990,  4130, 11652, 3374, 11477,  1753,   292,  8681,  2806, 10378, 12188, 5800, 11811,  3181,  1988,  1024,  9340,  2477, 10928, 4582,  6750,  3619,  5503,  5233,  2463,  8470,  7650, 7964,  6395,  1071,  1272,  3474, 11045,  3291, 11344, 8502,  9478,  9837,  1253,  1857,  6233,  4720, 11561, 6034,  9817,  3339,  1797,  2879,  6242,  5200,  2114, 7962,  9353, 11363,  5475,  6084,  9601,  4108,  7323,10438,  9471,  1271,   408,  6911,  3079,   360,  8276,11535,  9156,  9049, 11539,   850,  8617,   784,  7919, 8334, 12170,  1846, 10213, 12184,  7827, 11903,  5600, 9779,  1012,   721,  2784,  6676,  6552,  5348,  4424, 6816,  8405,  9959,  5150,  2356,  5552,  5267,  1333, 8801,  9661,  7308,  5788,  4910,   909, 11613,  4395, 8238,  6686,  4302,  3044,  2285, 12249,  1963,  9216, 4296, 11918,   695,  4371,  9793,  4884,  2411, 10230, 2650,   841,  3890, 10231,  7248,  8505, 11196,  6688, 4059,  6060,  3686,  4722, 11853,  5816,  7058,  6868,11137,  7926,  4894, 12284,  4102,  3908,  3610,  6525, 7938,  7982, 11977,  6755,   537,  4562,  1623,  8227,11453,  7544,   906, 11816,  9548, 10858,  9703,  2815,11736,  6813,  6979,   819,  8903,  6271, 10843,   348, 7514,  8339,  6439,   694,   852,  5659,  2781,  3716,11589,  3024,  1523,  8659,  4114, 10738,  3303,  5885, 2978,  7289, 11884,  9123,  9323, 11830,    98,  2526, 2116,  4131, 11407,  1844,  3645,  3916,  8133,  2224,10871,  8092,  9651,  5989,  7140,  8480,  1670,   159,10923,  4918,   128,  7312,   725,  9157,  5006,  6393, 3494,  6043, 10972,  6181, 11838,  3423, 10514,  7668, 3693,  6658,  6905, 11953, 10212, 11922,  9101,  8365, 5110,    45,  2400,  1921,  4377,  2720,  1695,    51, 2808,   650,  1896,  9997,  9971, 11980,  8098,  4833, 4135,  4257,  5838,  4765, 10985, 11532,   590, 12198,  482, 12173,  2006,  7064, 10018,  3912, 12016, 10519,11362,  6954,  2210,   284,  5413,  6601,  3865, 10339,11188,  6231,   517,  9564, 11281,  3863,  1210,  4604, 8160, 11447,   153,  7204,  5763,  5089,  9248, 12154,11748,  1354,  6672,   179,  5532,  2646,  5941, 12185,  862,  3158,   477,  7279,  5678,  7914,  4254,   302, 2893, 10114,  6890,  9560,  9647, 11905,  4098,  9824,10269,  1353, 10715,  5325,  6254,  3951,  1807,  6449, 5159,  1308,  8315,  3404,  1877,  1231,   112,  6398,11724, 12272,  7286,  1459, 12274,  9896,  3456,   800, 1397, 10678,   103,  7420,  7976,   936,   764,   632, 7996,  8223,  8445,  7758, 10870,  9571,  2508,  1946, 6524, 10158,  1044,  4338,  2457,  3641,  1659,  4139, 4688,  9733, 11148,  3946,  2082,  5261,  2036, 11850, 7636, 12236,  5366,  2380,  1399,  7720,  2100,  3217,10912,  8898,  7578, 11995,  2791,  1215,  3355,  2711, 2267,  2004,  8568, 10176,  3214,  2337,  1750,  4729, 4997,  7415,  6315, 12044,  4374,  7157,  4844,   211, 8003, 10159,  9290, 11481,  1735,  2336,  5793,  9875, 8192,   986,  7527,  1401,   870,  3615,  8465,  2756, 9770,  2034, 10168,  3264,  6132,    54,  2880,  4763,11805,  3074,  8286,  9428,  4881,  6933,  1090, 10038, 2567,   708,   893,  6465,  4962, 10024,  2090,  5718,10743,   780,  4733,  4623,  2134,  2087,  4802,   884, 5372,  5795,  5938,  4333,  6559,  7549,  5269, 10664, 4252,  3260,  5917, 10814,  5768,  9983,  8096,  7791, 6800,  7491,  6272,  1907, 10947,  6289, 11803,  6032,11449,  1171,  9201,  7933,  2479,  7970, 11337,  7062, 8911,  6728,  6542,  8114,  8828,  6595,  3545,  4348, 4610,  2205,  6999,  8106,  5560, 10390,  9321,  2499, 2413,  7272,  6881, 10582,  9308,  9437,  3554,  3326, 5991, 11969,  3415, 12283,  9838, 12063,  4332,  7830,11329,  6605, 12271,  2044, 11611,  7353, 11201, 11582, 3733,  8943,  9978,  1627,  7168,  3935,  5050,  2762, 7496, 10383,   755,  1654, 12053,  4952, 10134,  4394, 6592,  7898,  7497,  8904, 12029,  3581, 10748,  5674,10358,  4901,  7414,  8771,   710,  6764,  8462,  7193, 5371,  7274, 11084,   290,  7864,  6827, 11822,  2509, 6578,  4026,  5807,  1458,  5721,  5762,  4178,  2105,11621,  4852,  8897,  2856, 11510,  9264,  2520,  8776, 7011,  2647,  1898,  7039,  5950, 11163,  5488,  6277, 9182, 11456,   633, 10046, 11554,  5633,  9587,  2333, 7008,  7084,  5047,  7199,  9865,  8997,   569,  6390,10845,  9679,  8268, 11472,  4203,  1997,     2,  9331,  162,  6182,  2000,  3649,  9792,  6363,  7557,  6187, 8510,  9935,  5536,  9019,  3706, 12009,  1452,  3067, 5494,  9692,  4865,  6019,  7106,  9610,  4588, 10165, 6261,  5887,  2652, 10172,  1580, 10379,  4638,  9949
};

/*
 * Table for inverse NTT, binary case:
 *   iGMb[x] = R*((1/g)^rev(x)) mod q
 * Since g = 7, 1/g = 8778 mod 12289.
 */
static const uint16_t iGMb[] = {
	 4091,  4401,  1081,  1229,  2530,  6014,  7947,  5329,  2579,  4751,  6464, 11703,  7023,  2812,  5890, 10698,  3109,  2125,  1960, 10925, 10601, 10404,  4189,  1875,  5847,  8546,  4615,  5190, 11324, 10578,  5882, 11155,  8417, 12275, 10599,  7446,  5719,  3569,  5981, 10108,  4426,  8306, 10755,  4679, 11052,  1538, 11857,   100,  8247,  6625,  9725,  5145,  3412,  7858,  5831,  9460,  5217, 10740,  7882,  7506, 12172, 11292,  6049,    79,    13,  6938,  8886,  5453,  4586, 11455,  2903,  4676,  9843,  7621,  8822,  9109,  2083,  8507,  8685,  3110,  7015,  3269,  1367,  6397, 10259,  8435, 10527, 11559, 11094,  2211,  1808,  7319,    48,  9547,  2560,  1228,  9438, 10787, 11800,  1820, 11406,  8966,  6159,  3012,  6109,  2796,  2203,  1652,   711,  7004,  1053,  8973,  5244,  1517,  9322, 11269,   900,  3888, 11133, 10736,  4949,  7616,  9974,  4746, 10270,   126,  2921,  6720,  6635,  6543,  1582,  4868,    42,   673,  2240,  7219,  1296, 11989,  7675,  8578, 11949,   989, 10541,  7687,  7085,  8487,  1004, 10236,  4703,   163,  9143,  4597,  6431, 12052,  2991, 11938,  4647,  3362,  2060, 11357, 12011,  6664,  5655,  7225,  5914,  9327,  4092,  5880,  6932,  3402,  5133,  9394, 11229,  5252,  9008,  1556,  6908,  4773,  3853,  8780, 10325,  7737,  1758,  7103, 11375, 12273,  8602,  3243,  6536,  7590,  8591, 11552,  6101,  3253,  9969,  9640,  4506,  3736,  6829, 10822,  9130,  9948,  3566,  2133,  3901,  6038,  7333,  6609,  3468,  4659,   625,  2700,  7738,  3443,  3060,  3388,  3526,  4418, 11911,  6232,  1730,  2558, 10340,  5344,  5286,  2190, 11562,  6199,  2482,  8756,  5387,  4101,  4609,  8605,  8226,   144,  5656,  8704,  2621,  5424, 10812,  2959, 11346,  6249,  1715,  4951,  9540,  1888,  3764,    39,  8219,  2080,  2502,  1469, 10550,  8709,  5601,  1093,  3784,  5041,  2058,  8399, 11448,  9639,  2059,  9878,  7405,  2496,  7918, 11594,   371,  7993,  3073, 10326,    40, 10004,  9245,  7987,  5603,  4051,  7894,   676, 11380,  7379,  6501,  4981,  2628,  3488, 10956,  7022,  6737,  9933,  7139,  2330,  3884,  5473,  7865,  6941,  5737,  5613,  9505, 11568, 11277,  2510,  6689,   386,  4462,   105,  2076, 10443,   119,  3955,  4370, 11505,  3672, 11439,   750,  3240,  3133,   754,  4013, 11929,  9210,  5378, 11881, 11018,  2818,  1851,  4966,  8181,  2688,  6205,  6814,   926,  2936,  4327, 10175,  7089,  6047,  9410, 10492,  8950,  2472,  6255,   728,  7569,  6056, 10432, 11036,  2452,  2811,  3787,   945,  8998,  1244,  8815, 11017, 11218,  5894,  4325,  4639,  3819,  9826,  7056,  6786,  8670,  5539,  7707,  1361,  9812,  2949, 11265, 10301,  9108,   478,  6489,   101,  1911,  9483,  3608, 11997, 10536,   812,  8915,   637,  8159,  5299,  9128,  3512,  8290,  7068,  7922,  3036,  4759,  2163,  3937,  3755, 11306,  7739,  4922, 11932,   424,  5538,  6228, 11131,  7778, 11974,  1097,  2890, 10027,  2569,  2250,  2352,   821,  2550, 11016,  7769,   136,   617,  3157,  5889,  9219,  6855,   120,  4405,  1825,  9635,  7214, 10261, 11393,  2441,  9562, 11176,   599,  2085, 11465,  7233,  6177,  4801,  9926,  9010,  4514,  9455, 11352, 11670,  6174,  7950,  9766,  6896, 11603,  3213,  8473,  9873,  2835, 10422,  3732,  7961,  1457, 10857,  8069,   832,  1628,  3410,  4900, 10855,  5111,  9543,  6325,  7431,  4083,  3072,  8847,  9853, 10122,  5259, 11413,  6556,   303,  1465,  3871,  4873,  5813, 10017,  6898,  3311,  5947,  8637,  5852,  3856,   928,  4933,  8530,  1871,  2184,  5571,  5879,  3481, 11597,  9511,  8153,    35,  2609,  5963,  8064,  1080, 12039,  8444,  3052,  3813, 11065,  6736,  8454,  2340,  7651,  1910, 10709,  2117,  9637,  6402,  6028,  2124,  7701,  2679,  5183,  6270,  7424,  2597,  6795,  9222, 10837,   280,  8583,  3270,  6753,  2354,  3779,  6102,  4732,  5926,  2497,  8640, 10289,  6107, 12127,  2958, 12287, 10292,  8086,   817,  4021,  2610,  1444,  5899, 11720,  3292,  2424,  5090,  7242,  5205,  5281,  9956,  2702,  6656,   735,  2243, 11656,   833,  3107,  6012,  6801,  1126,  6339,  5250, 10391,  9642,  5278,  3513,  9769,  3025,   779,  9433,  3392,  7437,   668, 10184,  8111,  6527,  6568, 10831,  6482,  8263,  5711,  9780,   467,  5462,  4425, 11999,  1205,  5015,  6918,  5096,  3827,  5525, 11579,  3518,  4875,  7388,  1931,  6615,  1541,  8708,   260,  3385,  4792,  4391,  5697,  7895,  2155,  7337,   236, 10635, 11534,  1906,  4793,  9527,  7239,  8354,  5121, 10662,  2311,  3346,  8556,   707,  1088,  4936,   678, 10245,    18,  5684,   960,  4459,  7957,   226,  2451,     6,  8874,   320,  6298,  8963,  8735,  2852,  2981,  1707,  5408,  5017,  9876,  9790,  2968,  1899,  6729,  4183,  5290, 10084,  7679,  7941,  8744,  5694,  3461,  4175,  5747,  5561,  3378,  5227,   952,  4319,  9810,  4356,  3088, 11118,   840,  6257,   486,  6000,  1342, 10382,  6017,  4798,  5489,  4498,  4193,  2306,  6521,  1475,  6372,  9029,  8037,  1625,  7020,  4740,  5730,  7956,  6351,  6494,  6917, 11405,  7487, 10202, 10155,  7666,  7556, 11509,  1546,  6571, 10199,  2265,  7327,  5824, 11396, 11581,  9722,  2251, 11199,  5356,  7408,  2861,  4003,  9215,   484,  7526,  9409, 12235,  6157,  9025,  2121, 10255,  2519,  9533,  3824,  8674, 11419, 10888,  4762, 11303,  4097,  2414,  6496,  9953, 10554,   808,  2999,  2130,  4286, 12078,  7445,  5132,  7915,   245,  5974,  4874,  7292,  7560, 10539,  9952,  9075,  2113,  3721, 10285, 10022,  9578,  8934, 11074,  9498,   294,  4711,  3391,  1377,  9072, 10189,  4569, 10890,  9909,  6923,    53,  4653,   439, 10253,  7028, 10207,  8343,  1141,  2556,  7601,  8150, 10630,  8648,  9832,  7951, 11245,  2131,  5765, 10343,  9781,  2718,  1419,  4531,  3844,  4066,  4293, 11657, 11525, 11353,  4313,  4869, 12186,  1611, 10892, 11489,  8833,  2393,    15, 10830,  5003,    17,   565,  5891, 12177, 11058, 10412,  8885,  3974, 10981,  7130,  5840, 10482,  8338,  6035,  6964,  1574, 10936,  2020,  2465,  8191,   384,  2642,  2729,  5399,  2175,  9396, 11987,  8035,  4375,  6611,  5010, 11812,  9131, 11427,   104,  6348,  9643,  6757, 12110,  5617, 10935,   541,   135,  3041,  7200,  6526,  5085, 12136,   842,  4129,  7685, 11079,  8426,  1008,  2725, 11772,  6058,  1101,  1950,  8424,  5688,  6876, 12005, 10079,  5335,   927,  1770,   273,  8377,  2271,  5225, 10283,   116, 11807,    91, 11699,   757,  1304,  7524,  6451,  8032,  8154,  7456,  4191,   309,  2318,  2292, 10393, 11639,  9481, 12238, 10594,  9569,  7912, 10368,  9889, 12244,  7179,  3924,  3188,   367,  2077,   336,  5384,  5631,  8596,  4621,  1775,  8866,   451,  6108,  1317,  6246,  8795,  5896,  7283,  3132, 11564,  4977, 12161,  7371,  1366, 12130, 10619,  3809,  5149,  6300,  2638,  4197,  1418, 10065,  4156,  8373,  8644, 10445,   882,  8158, 10173,  9763, 12191,   459,  2966,  3166,   405,  5000,  9311,  6404,  8986,  1551,  8175,  3630, 10766,  9265,   700,  8573,  9508,  6630, 11437, 11595,  5850,  3950,  4775, 11941,  1446,  6018,  3386, 11470,  5310,  5476,   553,  9474,  2586,  1431,  2741,   473, 11383,  4745,   836,  4062, 10666,  7727, 11752,  5534,   312,  4307,  4351,  5764,  8679,  8381,  8187,     5,  7395,  4363,  1152,  5421,  5231,  6473,   436,  7567,  8603,  6229,  8230
};


#define Q     12289
#define Q0I   12287
#define R      4091
#define R2    10952
#define INV2  6145


/* === FALCON-512 HARDWARE CONSTANTS === */
/* Generated for Dual-Mode (Packed) Architecture */

// Modulus Q = 12289
// Vector Bound (Dual Mode) = 512 / 2 = 256
const uint32_t FALCON_Q     = 0x3001;
const uint32_t FALCON_INV2  = 0x1801;
const uint32_t FALCON_MU[2] = {0xE425E9E0, 0x00055538};

const uint32_t psi[256] = {
	0x2a3a2a3a, 0x1be71be7, 0xfcb0fcb, 0x2ae82ae8, 0x2d202d2, 0x16681668, 0x1fdb1fdb, 0xdd60dd6, 0x22512251, 0x26102610, 0xe250e25, 0x29932993, 0x4bc04bc, 0xc7b0c7b, 0x16e416e4, 0x1d2c1d2c, 0xa4f0a4f, 0x25c025c0, 0x2c4c2c4c, 0x2dce2dce, 0x24622462, 0x24432443, 0x25492549, 0x16601660, 0x1e121e12, 0x139f139f, 0x16c416c4, 0x23012301, 0x19701970, 0x1c8f1c8f, 0x5470547, 0x90f090f, 0x2b6f2b6f, 0x2c462c46, 0x2ceb2ceb, 0x23802380, 0xbc60bc6, 0x13de13de, 0x2ad32ad3, 0x12ee12ee, 0x25462546, 0x23c223c2, 0xe800e80, 0x12c512c5, 0x22202220, 0x2bdb2bdb, 0x270b270b, 0xc130c13, 0x2fb02fb0, 0x1f211f21, 0x2c192c19, 0x246e246e, 0x1ce11ce1, 0x24162416, 0x93e093e, 0x2b682b68, 0x1f621f62, 0x299e299e, 0x25312531, 0x2f752f75, 0x28c428c4, 0x1dfe1dfe, 0x2d2b2d2b, 0x4ec04ec, 0x11241124, 0x12181218, 0x19861986, 0x97a097a, 0x14e014e, 0x5940594, 0x6a006a0, 0x7dd07dd, 0x23282328, 0x2d902d9, 0xca90ca9, 0xb410b41, 0xcd40cd4, 0x1c1d1c1d, 0x27d827d8, 0x21932193, 0x1bc61bc6, 0x29222922, 0x21862186, 0xd360d36, 0x2e9e2e9e, 0x260d260d, 0x1f7a1f7a, 0xe350e35, 0xd830d83, 0x910091, 0x1a5b1a5b, 0x25562556, 0x20a520a5, 0x1ce71ce7, 0x18ea18ea, 0x24e724e7, 0x1e001e0, 0x3fe03fe, 0x90009, 0x265d265d, 0x1530153, 0x169f169f, 0x2200220, 0x29782978, 0x10b610b6, 0x1b2e1b2e, 0x1c841c84, 0x1fb01fb0, 0x22012201, 0x5650565, 0x26242624, 0x2c482c48, 0x215d215d, 0x33b033b, 0x16871687, 0x9ac09ac, 0x760076, 0x8950895, 0x1c361c36, 0xf6d0f6d, 0x23212321, 0x11641164, 0x95c095c, 0x1eff1eff, 0x820082, 0xb150b15, 0x1b031b03, 0x9610961, 0x1ba01ba, 0x1c141c14, 0x2bd62bd6, 0x1860186, 0x3050305, 0x21082108, 0xec20ec2, 0x1620162, 0x12fd12fd, 0x24a124a1, 0x16421642, 0x13941394, 0x26502650, 0xb2b0b2b, 0x2bec2bec, 0x3f903f9, 0x1cec1cec, 0x6600660, 0x1c251c25, 0x1b001b, 0x24072407, 0x214e214e, 0x2a612a61, 0x6010601, 0xf200f2, 0x126a126a, 0x1fd21fd2, 0x258b258b, 0xe780e78, 0x139b139b, 0x2de02de0, 0x3ea03ea, 0x13931393, 0x13e013e0, 0x1f451f45, 0x1c911c91, 0x29ba29ba, 0x213d213d, 0x2c962c96, 0x267c267c, 0xe3e0e3e, 0x17861786, 0xbab0bab, 0x25fb25fb, 0x27762776, 0x186a186a, 0x268b268b, 0x2bd82bd8, 0x85f085f, 0x2e6d2e6d, 0x1ddc1ddc, 0x4900490, 0x149d149d, 0x2b4a2b4a, 0xcb00cb0, 0x1ed01ed, 0x20012001, 0x1abd1abd, 0x94d094d, 0x1f101f10, 0x2e4e2e4e, 0x5620562, 0x7780778, 0x8760876, 0xf4b0f4b, 0x2f902f90, 0x1cca1cca, 0x2f612f61, 0xc4d0c4d, 0x2ffe2ffe, 0x11551155, 0xe340e34, 0x134a134a, 0x14ab14ab, 0xa900a90, 0x2a6f2a6f, 0x1dd31dd3, 0x67f067f, 0x29102910, 0xd240d24, 0x6990699, 0xfd90fd9, 0x233a233a, 0x24e224e2, 0x1ec31ec3, 0x87e087e, 0x11141114, 0x1c4f1c4f, 0x27002700, 0xfd50fd5, 0xa550a55, 0x144b144b, 0x25252525, 0x1ce21ce2, 0x5cc05cc, 0x23522352, 0x25832583, 0x20772077, 0x24682468, 0x26bf26bf, 0xb310b31, 0x14d414d4, 0xdb60db6, 0x65e065e, 0x27b327b3, 0x151f151f, 0xc720c72, 0x2b802b80, 0x24bd24bd, 0x27382738, 0x20312031, 0x26a226a2, 0x22b922b9, 0x1bba1bba, 0x23c123c1, 0x24492449, 0x29f029f, 0xbc80bc8, 0xf300f3, 0x1a4a1a4a, 0x1a401a4, 0x277f277f, 0x6080608, 0xf910f91, 0x13291329, 0xdcb0dcb, 0x1dc01dc, 1
};

const uint32_t psi_inv[256] = {
	0x1ae4, 0x101b, 0xa0d, 0x813, 0x24cd, 0x2e98, 0x1a8d, 0x248f, 0x11c3, 0x2da3, 0x337, 0x10ee, 0x1cf9, 0x6d8, 0x2916, 0x155d, 0x9b9, 0x2349, 0x680, 0x249f, 0xe31, 0x20f8, 0x24d1, 0x55c, 0x5df, 0x1dd0, 0x191a, 0x19db, 0x1d21, 0x12d9, 0x216b, 0x16b, 0x2102, 0x1b9f, 0x46, 0x568, 0x1b32, 0x2050, 0x1a4d, 0x2b62, 0x1df6, 0x990, 0x1dca, 0x1f4, 0x870, 0x1829, 0x11f7, 0x47b, 0x213, 0x1ef1, 0xe9e, 0x28c1, 0x1e20, 0x1d5e, 0x268a, 0x297, 0x2612, 0x2a1e, 0x1e41, 0x18b, 0x19de, 0x249, 0x1379, 0xa7f, 0x1276, 0x2fc0, 0x881, 0x2b53, 0x274f, 0x670, 0x104a, 0x21e6, 0x13b6, 0x2fc6, 0x2b2b, 0xcbd, 0x1663, 0x752, 0x19dd, 0x1cef, 0x154e, 0x700, 0x2029, 0x21bf, 0x226a, 0x27a6, 0x1b45, 0x2ef1, 0xcb1, 0x1757, 0x4d2, 0x17fc, 0x2e02, 0x2f11, 0x58d, 0x238c, 0x98d, 0x7ae, 0x1d56, 0xad3, 0x17b8, 0x113f, 0x10e6, 0x2044, 0x4fa, 0x18b2, 0x2966, 0x1f3e, 0x1b70, 0x221e, 0x737, 0x1c15, 0x9f2, 0x2997, 0x1260, 0x11ac, 0x1694, 0x1e6d, 0x1412, 0x2cb1, 0x2d37, 0x2f5a, 0x2b44, 0x233e, 0x26f5, 0x276f, 0x2d8b, 0x2f13, 0x111b, 0xe6c, 0x1038, 0x2cfd, 0x441, 0x2f2f, 0x22dc, 0x1787, 0x2a1d, 0x16b1, 0x5dc, 0x620, 0x2224, 0x6a4, 0x1cb0, 0x7e8, 0x1c65, 0x5a2, 0x1a41, 0x29c8, 0xd71, 0x427, 0x2cd2, 0x2926, 0x2597, 0x1268, 0x4a1, 0x1dcd, 0x7c5, 0x53f, 0x1e58, 0x2d1b, 0x2190, 0x56e, 0xddb, 0x12d6, 0x1016, 0x1c81, 0x9d9, 0x2777, 0x2bc2, 0x89f, 0x1d90, 0x1e64, 0x1014, 0x14b4, 0x296f, 0x1b79, 0x14c1, 0x917, 0x2c9, 0x2ab9, 0xdab, 0x265c, 0x28e7, 0xf56, 0x1802, 0x11da, 0x50, 0x219c, 0x1839, 0x105b, 0x2bc6, 0x2c45, 0x2d50, 0x18da, 0x2079, 0x135a, 0xaa2, 0x800, 0x170a, 0x29a9, 0x1a5c, 0xdb2, 0x2db9, 0x2113, 0xca, 0x13d1, 0x1a15, 0x4bb, 0x23cc, 0x1c46, 0x503, 0x122b, 0x243e, 0x28e2, 0x1cc3, 0x19b6, 0x762, 0x1b24, 0x9b8, 0x85e, 0x2611, 0xe37, 0x2e0c, 0x1911, 0xe33, 0x28c5, 0x53b, 0x2018, 0x26cc, 0x2f88, 0x1500, 0x2d0, 0x1f5a, 0x5fd, 0x17f3, 0x9ee, 0x2cd1, 0x218b, 0x1604, 0x1a0b, 0x126b, 0x1cd9, 0x2637, 0x24e0, 0x5b0, 0xe82, 0x2f50, 0x28a0, 0x1f7d, 0x167e, 0x2f3e, 0x1a16, 0x21f7, 0x2f24, 0x1350, 1
};

const uint32_t zetas[256] = {
	0x10001, 0x30000001, 0x1be70001, 0x141a0001, 0x2ae80001, 0x5190001, 0x16680001, 0x19990001, 0xdd60001, 0x222b0001, 0x26100001, 0x9f10001, 0x29930001, 0x66e0001, 0xc7b0001, 0x23860001, 0x1d2c0001, 0x12d50001, 0x25c00001, 0xa410001, 0x2dce0001, 0x2330001, 0x24430001, 0xbbe0001, 0x16600001, 0x19a10001, 0x139f0001, 0x1c620001, 0x23010001, 0xd000001, 0x1c8f0001, 0x13720001, 0x90f0001, 0x26f20001, 0x2c460001, 0x3bb0001, 0x23800001, 0xc810001, 0x13de0001, 0x1c230001, 0x12ee0001, 0x1d130001, 0x23c20001, 0xc3f0001, 0x12c50001, 0x1d3c0001, 0x2bdb0001, 0x4260001, 0xc130001, 0x23ee0001, 0x1f210001, 0x10e00001, 0x246e0001, 0xb930001, 0x24160001, 0xbeb0001, 0x2b680001, 0x4990001, 0x299e0001, 0x6630001, 0x2f750001, 0x8c0001, 0x1dfe0001, 0x12030001, 0x4ec0001, 0x2b150001, 0x12180001, 0x1de90001, 0x97a0001, 0x26870001, 0x5940001, 0x2a6d0001, 0x7dd0001, 0x28240001, 0x2d90001, 0x2d280001, 0xb410001, 0x24c00001, 0x1c1d0001, 0x13e40001, 0x21930001, 0xe6e0001, 0x29220001, 0x6df0001, 0xd360001, 0x22cb0001, 0x260d0001, 0x9f40001, 0xe350001, 0x21cc0001, 0x910001, 0x2f700001, 0x25560001, 0xaab0001, 0x1ce70001, 0x131a0001, 0x24e70001, 0xb1a0001, 0x3fe0001, 0x2c030001, 0x265d0001, 0x9a40001, 0x169f0001, 0x19620001, 0x29780001, 0x6890001, 0x1b2e0001, 0x14d30001, 0x1fb00001, 0x10510001, 0x5650001, 0x2a9c0001, 0x2c480001, 0x3b90001, 0x33b0001, 0x2cc60001, 0x9ac0001, 0x26550001, 0x8950001, 0x276c0001, 0xf6d0001, 0x20940001, 0x11640001, 0x1e9d0001, 0x1eff0001, 0x11020001, 0xb150001, 0x24ec0001, 0x9610001, 0x26a00001, 0x1c140001, 0x13ed0001, 0x1860001, 0x2e7b0001, 0x21080001, 0xef90001, 0x1620001, 0x2e9f0001, 0x24a10001, 0xb600001, 0x13940001, 0x1c6d0001, 0xb2b0001, 0x24d60001, 0x3f90001, 0x2c080001, 0x6600001, 0x29a10001, 0x1b0001, 0x2fe60001, 0x214e0001, 0xeb30001, 0x6010001, 0x2a000001, 0x126a0001, 0x1d970001, 0x258b0001, 0xa760001, 0x139b0001, 0x1c660001, 0x3ea0001, 0x2c170001, 0x13e00001, 0x1c210001, 0x1c910001, 0x13700001, 0x213d0001, 0xec40001, 0x267c0001, 0x9850001, 0x17860001, 0x187b0001, 0x25fb0001, 0xa060001, 0x186a0001, 0x17970001, 0x2bd80001, 0x4290001, 0x2e6d0001, 0x1940001, 0x4900001, 0x2b710001, 0x2b4a0001, 0x4b70001, 0x1ed0001, 0x2e140001, 0x1abd0001, 0x15440001, 0x1f100001, 0x10f10001, 0x5620001, 0x2a9f0001, 0x8760001, 0x278b0001, 0x2f900001, 0x710001, 0x2f610001, 0xa00001, 0x2ffe0001, 0x30001, 0xe340001, 0x21cd0001, 0x14ab0001, 0x1b560001, 0x2a6f0001, 0x5920001, 0x67f0001, 0x29820001, 0xd240001, 0x22dd0001, 0xfd90001, 0x20280001, 0x24e20001, 0xb1f0001, 0x87e0001, 0x27830001, 0x1c4f0001, 0x13b20001, 0xfd50001, 0x202c0001, 0x144b0001, 0x1bb60001, 0x1ce20001, 0x131f0001, 0x23520001, 0xcaf0001, 0x20770001, 0xf8a0001, 0x26bf0001, 0x9420001, 0x14d40001, 0x1b2d0001, 0x65e0001, 0x29a30001, 0x151f0001, 0x1ae20001, 0x2b800001, 0x4810001, 0x27380001, 0x8c90001, 0x26a20001, 0x95f0001, 0x1bba0001, 0x14470001, 0x24490001, 0xbb80001, 0xbc80001, 0x24390001, 0x1a4a0001, 0x15b70001, 0x277f0001, 0x8820001, 0xf910001, 0x20700001, 0xdcb0001, 0x22360001
};

void poly_init_q() {

    ntt_lite_load_q(FALCON_Q, FALCON_MU, 8, 14, FALCON_INV2, NTT_LITE_MODE_POLY);
}

void poly_set_q() {
    ntt_lite_set_q(FALCON_Q);
}

void poly_init_ntt() {
    ntt_lite_load_twiddle((uint32_t*) psi);
}

void poly_init_invntt() {
    ntt_lite_load_twiddle((uint32_t*) psi_inv);
}

void poly_basemul(poly *r, const poly *a, const poly *b)
{
  ntt_lite_set_ctrl(7, 14, NTT_LITE_MODE_POLY);
  ntt_lite_load_zeta((uint32_t*) zetas);
  ntt_lite_pwm((uint32_t*) r->coeffs, (uint32_t*) a->coeffs, (uint32_t*) b->coeffs);
  ntt_lite_load_zeta((uint32_t*) zetas + 128);
  ntt_lite_pwm((uint32_t*) r->coeffs + 128, (uint32_t*) a->coeffs + 128, (uint32_t*) b->coeffs + 128);
  ntt_lite_set_ctrl(8, 14, NTT_LITE_MODE_POLY);
}

/*
 * Reduce a small signed integer modulo q. The source integer MUST
 * be between -q/2 and +q/2.
 */
static inline uint32_t
mq_conv_small(int x)
{
	/*
	 * If x < 0, the cast to uint32_t will set the high bit to 1.
	 */
	uint32_t y;

	y = (uint32_t)x;
	y += Q & -(y >> 31);
	return y;
}

/*
 * Addition modulo q. Operands must be in the 0..q-1 range.
 */
static inline uint32_t
mq_add(uint32_t x, uint32_t y)
{
	/*
	 * We compute x + y - q. If the result is negative, then the
	 * high bit will be set, and 'd >> 31' will be equal to 1;
	 * thus '-(d >> 31)' will be an all-one pattern. Otherwise,
	 * it will be an all-zero pattern. In other words, this
	 * implements a conditional addition of q.
	 */
	uint32_t d;

	d = x + y - Q;
	d += Q & -(d >> 31);
	return d;
}

/*
 * Subtraction modulo q. Operands must be in the 0..q-1 range.
 */
static inline uint32_t
mq_sub(uint32_t x, uint32_t y)
{
	/*
	 * As in mq_add(), we use a conditional addition to ensure the
	 * result is in the 0..q-1 range.
	 */
	uint32_t d;

	d = x - y;
	d += Q & -(d >> 31);
	return d;
}

/*
 * Division by 2 modulo q. Operand must be in the 0..q-1 range.
 */
static inline uint32_t
mq_rshift1(uint32_t x)
{
	x += Q & -(x & 1);
	return (x >> 1);
}

/*
 * Montgomery multiplication modulo q. If we set R = 2^16 mod q, then
 * this function computes: x * y / R mod q
 * Operands must be in the 0..q-1 range.
 */
static inline uint32_t
mq_montymul(uint32_t x, uint32_t y)
{
	uint32_t z, w;

	/*
	 * We compute x*y + k*q with a value of k chosen so that the 16
	 * low bits of the result are 0. We can then shift the value.
	 * After the shift, result may still be larger than q, but it
	 * will be lower than 2*q, so a conditional subtraction works.
	 */

	z = x * y;
	w = ((z * Q0I) & 0xFFFF) * Q;

	/*
	 * When adding z and w, the result will have its low 16 bits
	 * equal to 0. Since x, y and z are lower than q, the sum will
	 * be no more than (2^15 - 1) * q + (q - 1)^2, which will
	 * fit on 29 bits.
	 */
	z = (z + w) >> 16;

	/*
	 * After the shift, analysis shows that the value will be less
	 * than 2q. We do a subtraction then conditional subtraction to
	 * ensure the result is in the expected range.
	 */
	z -= Q;
	z += Q & -(z >> 31);
	return z;
}

/*
 * Montgomery squaring (computes (x^2)/R).
 */
static inline uint32_t
mq_montysqr(uint32_t x)
{
	return mq_montymul(x, x);
}

/*
 * Divide x by y modulo q = 12289.
 */
static inline uint32_t
mq_div_12289(uint32_t x, uint32_t y)
{
	/*
	 * We invert y by computing y^(q-2) mod q.
	 *
	 * We use the following addition chain for exponent e = 12287:
	 *
	 *   e0 = 1
	 *   e1 = 2 * e0 = 2
	 *   e2 = e1 + e0 = 3
	 *   e3 = e2 + e1 = 5
	 *   e4 = 2 * e3 = 10
	 *   e5 = 2 * e4 = 20
	 *   e6 = 2 * e5 = 40
	 *   e7 = 2 * e6 = 80
	 *   e8 = 2 * e7 = 160
	 *   e9 = e8 + e2 = 163
	 *   e10 = e9 + e8 = 323
	 *   e11 = 2 * e10 = 646
	 *   e12 = 2 * e11 = 1292
	 *   e13 = e12 + e9 = 1455
	 *   e14 = 2 * e13 = 2910
	 *   e15 = 2 * e14 = 5820
	 *   e16 = e15 + e10 = 6143
	 *   e17 = 2 * e16 = 12286
	 *   e18 = e17 + e0 = 12287
	 *
	 * Additions on exponents are converted to Montgomery
	 * multiplications. We define all intermediate results as so
	 * many local variables, and let the C compiler work out which
	 * must be kept around.
	 */
	uint32_t y0, y1, y2, y3, y4, y5, y6, y7, y8, y9;
	uint32_t y10, y11, y12, y13, y14, y15, y16, y17, y18;

	y0 = mq_montymul(y, R2);
	y1 = mq_montysqr(y0);
	y2 = mq_montymul(y1, y0);
	y3 = mq_montymul(y2, y1);
	y4 = mq_montysqr(y3);
	y5 = mq_montysqr(y4);
	y6 = mq_montysqr(y5);
	y7 = mq_montysqr(y6);
	y8 = mq_montysqr(y7);
	y9 = mq_montymul(y8, y2);
	y10 = mq_montymul(y9, y8);
	y11 = mq_montysqr(y10);
	y12 = mq_montysqr(y11);
	y13 = mq_montymul(y12, y9);
	y14 = mq_montysqr(y13);
	y15 = mq_montysqr(y14);
	y16 = mq_montymul(y15, y10);
	y17 = mq_montysqr(y16);
	y18 = mq_montymul(y17, y0);

	/*
	 * Final multiplication with x, which is not in Montgomery
	 * representation, computes the correct division result.
	 */
	return mq_montymul(y18, x);
}

/*
 * Compute NTT on a ring element.
 */
static void
mq_NTT(uint16_t *a, unsigned logn)
{
	size_t n, t, m;

	n = (size_t)1 << logn;
	t = n;
	for (m = 1; m < n; m <<= 1) {
		size_t ht, i, j1;

		ht = t >> 1;
		for (i = 0, j1 = 0; i < m; i ++, j1 += t) {
			size_t j, j2;
			uint32_t s;

			s = GMb[m + i];
			j2 = j1 + ht;
			for (j = j1; j < j2; j ++) {
				uint32_t u, v;

				u = a[j];
				v = mq_montymul(a[j + ht], s);
				a[j] = (uint16_t)mq_add(u, v);
				a[j + ht] = (uint16_t)mq_sub(u, v);
			}
		}
		t = ht;
	}
}

/*
 * Compute the inverse NTT on a ring element, binary case.
 */
static void
mq_iNTT(uint16_t *a, unsigned logn)
{
	size_t n, t, m;
	uint32_t ni;

	n = (size_t)1 << logn;
	t = 1;
	m = n;
	while (m > 1) {
		size_t hm, dt, i, j1;

		hm = m >> 1;
		dt = t << 1;
		for (i = 0, j1 = 0; i < hm; i ++, j1 += dt) {
			size_t j, j2;
			uint32_t s;

			j2 = j1 + t;
			s = iGMb[hm + i];
			for (j = j1; j < j2; j ++) {
				uint32_t u, v, w;

				u = a[j];
				v = a[j + t];
				a[j] = (uint16_t)mq_add(u, v);
				w = mq_sub(u, v);
				a[j + t] = (uint16_t)
					mq_montymul(w, s);
			}
		}
		t = dt;
		m = hm;
	}

	/*
	 * To complete the inverse NTT, we must now divide all values by
	 * n (the vector size). We thus need the inverse of n, i.e. we
	 * need to divide 1 by 2 logn times. But we also want it in
	 * Montgomery representation, i.e. we also want to multiply it
	 * by R = 2^16. In the common case, this should be a simple right
	 * shift. The loop below is generic and works also in corner cases;
	 * its computation time is negligible.
	 */
	ni = R;
	for (m = n; m > 1; m >>= 1) {
		ni = mq_rshift1(ni);
	}
	for (m = 0; m < n; m ++) {
		a[m] = (uint16_t)mq_montymul(a[m], ni);
	}
}

/*
 * Convert a polynomial (mod q) to Montgomery representation.
 */
static void
mq_poly_tomonty(uint16_t *f, unsigned logn)
{
	size_t u, n;

	n = (size_t)1 << logn;
	for (u = 0; u < n; u ++) {
		f[u] = (uint16_t)mq_montymul(f[u], R2);
	}
}

/*
 * Multiply two polynomials together (NTT representation, and using
 * a Montgomery multiplication). Result f*g is written over f.
 */
static void
mq_poly_montymul_ntt(uint16_t *f, const uint16_t *g, unsigned logn)
{
	size_t u, n;

	n = (size_t)1 << logn;
	for (u = 0; u < n; u ++) {
		f[u] = (uint16_t)mq_montymul(f[u], g[u]);
	}
}

/*
 * Subtract polynomial g from polynomial f.
 */
static void
mq_poly_sub(uint16_t *f, const uint16_t *g, unsigned logn)
{
	size_t u, n;

	n = (size_t)1 << logn;
	for (u = 0; u < n; u ++) {
		f[u] = (uint16_t)mq_sub(f[u], g[u]);
	}
}

/* ===================================================================== */

/* see inner.h */
void
Zf(to_ntt_monty)(uint16_t *h, unsigned logn)
{
	mq_NTT(h, logn);
	mq_poly_tomonty(h, logn);
}


#include "middle_val.h" // Ensure this is present!



/* Helper to compare and report mismatches */

static void compare_intermediate(const char *step_name, const uint16_t *hw_data, const uint16_t *golden_data, size_t n) {

    if (memcmp(hw_data, golden_data, n * sizeof(uint16_t)) != 0) {

        print_string("[FAIL] "); 

        print_string(step_name); 

        print_string(" mismatch!\n");

        

        // Find first mismatch index

        for (size_t i = 0; i < n; i++) {

            if (hw_data[i] != golden_data[i]) {

                print_string("      Index "); 

                print_u32((uint32_t)i);

                print_string(": Got "); 

                print_u32((uint32_t)hw_data[i]); 

                print_string(", Expected "); 

                print_u32((uint32_t)golden_data[i]); 

                print_string("\n");

                break; // Stop after first error

            }

        }

    } else {

        print_string("[PASS] "); 

        print_string(step_name); 

        print_string("\n");

    }

}



/* see inner.h */

int

Zf(verify_raw)(const uint16_t *c0, const int16_t *s2,

    const uint16_t *h, unsigned logn, uint8_t *tmp)

{

    size_t u, n;

    uint16_t *tt;



    // Static counter: 0=Compressed, 1=Padded, 2=CT

    static int call_count = 0;

    const uint16_t *gold_ntt, *gold_mul, *gold_intt, *gold_sub;



    // Select golden vectors based on call count

    if (call_count == 0) {

        print_string("\n--- Checking COMPRESSED Intermediates ---\n");

        gold_ntt  = mid_ntt_s2_comp;

        gold_mul  = mid_montymul_comp;

        gold_intt = mid_intt_comp;

        gold_sub  = mid_sub_c0_comp;

    } else if (call_count == 1) {

        print_string("\n--- Checking PADDED Intermediates ---\n");

        gold_ntt  = mid_ntt_s2_pad;

        gold_mul  = mid_montymul_pad;

        gold_intt = mid_intt_pad;

        gold_sub  = mid_sub_c0_pad;

    } else {

        print_string("\n--- Checking CT Intermediates ---\n");

        gold_ntt  = mid_ntt_s2_ct;

        gold_mul  = mid_montymul_ct;

        gold_intt = mid_intt_ct;

        gold_sub  = mid_sub_c0_ct;

    }



    n = (size_t)1 << logn;

    tt = (uint16_t *)tmp;



    /*

     * Reduce s2 elements modulo q ([0..q-1] range).

     */

    for (u = 0; u < n; u ++) {

        uint32_t w;



        w = (uint32_t)s2[u];

        w += Q & -(w >> 31);

        tt[u] = (uint16_t)w;

    }



    /*

     * Compute -s1 = s2*h - c0 mod phi mod q (in tt[]).

     */

    

    // 1. NTT

    mq_NTT(tt, logn);

    //compare_intermediate("NTT(s2)", tt, gold_ntt, n);



    // 2. Pointwise Mul

    mq_poly_montymul_ntt(tt, h, logn);

    //compare_intermediate("Mul(s2*h)", tt, gold_mul, n);



    // 3. Inverse NTT

    mq_iNTT(tt, logn);

    //compare_intermediate("iNTT(s2*h)", tt, gold_intt, n);



    // 4. Subtract c0

    mq_poly_sub(tt, c0, logn);

    //compare_intermediate("Sub(tt-c0)", tt, gold_sub, n);



    /*

     * Normalize -s1 elements into the [-q/2..q/2] range.

     */

    for (u = 0; u < n; u ++) {

        int32_t w;



        w = (int32_t)tt[u];

        w -= (int32_t)(Q & -(((Q >> 1) - (uint32_t)w) >> 31));

        ((int16_t *)tt)[u] = (int16_t)w;

    }



    // Increment counter for next run

    call_count++;

    if (call_count > 2) call_count = 0; // Reset just in case



    /*

     * Signature is valid if and only if the aggregate (-s1,s2) vector

     * is short enough.

     */

    return Zf(is_short)((int16_t *)tt, s2, logn);

}

/* see inner.h */
int
Zf(compute_public)(uint16_t *h,
	const int8_t *f, const int8_t *g, unsigned logn, uint8_t *tmp)
{
	size_t u, n;
	uint16_t *tt;

	n = (size_t)1 << logn;
	tt = (uint16_t *)tmp;
	for (u = 0; u < n; u ++) {
		tt[u] = (uint16_t)mq_conv_small(f[u]);
		h[u] = (uint16_t)mq_conv_small(g[u]);
	}
	mq_NTT(h, logn);
	mq_NTT(tt, logn);
	for (u = 0; u < n; u ++) {
		if (tt[u] == 0) {
			return 0;
		}
		h[u] = (uint16_t)mq_div_12289(h[u], tt[u]);
	}
	mq_iNTT(h, logn);
	return 1;
}

/* see inner.h */
int
Zf(complete_private)(int8_t *G,
	const int8_t *f, const int8_t *g, const int8_t *F,
	unsigned logn, uint8_t *tmp)
{
	size_t u, n;
	uint16_t *t1, *t2;

	n = (size_t)1 << logn;
	t1 = (uint16_t *)tmp;
	t2 = t1 + n;
	for (u = 0; u < n; u ++) {
		t1[u] = (uint16_t)mq_conv_small(g[u]);
		t2[u] = (uint16_t)mq_conv_small(F[u]);
	}
	mq_NTT(t1, logn);
	mq_NTT(t2, logn);
	mq_poly_tomonty(t1, logn);
	mq_poly_montymul_ntt(t1, t2, logn);
	for (u = 0; u < n; u ++) {
		t2[u] = (uint16_t)mq_conv_small(f[u]);
	}
	mq_NTT(t2, logn);
	for (u = 0; u < n; u ++) {
		if (t2[u] == 0) {
			return 0;
		}
		t1[u] = (uint16_t)mq_div_12289(t1[u], t2[u]);
	}
	mq_iNTT(t1, logn);
	for (u = 0; u < n; u ++) {
		uint32_t w;
		int32_t gi;

		w = t1[u];
		w -= (Q & ~-((w - (Q >> 1)) >> 31));
		gi = *(int32_t *)&w;
		if (gi < -127 || gi > +127) {
			return 0;
		}
		G[u] = (int8_t)gi;
	}
	return 1;
}

/* see inner.h */
int
Zf(is_invertible)(
	const int16_t *s2, unsigned logn, uint8_t *tmp)
{
	size_t u, n;
	uint16_t *tt;
	uint32_t r;

	n = (size_t)1 << logn;
	tt = (uint16_t *)tmp;
	for (u = 0; u < n; u ++) {
		uint32_t w;

		w = (uint32_t)s2[u];
		w += Q & -(w >> 31);
		tt[u] = (uint16_t)w;
	}
	mq_NTT(tt, logn);
	r = 0;
	for (u = 0; u < n; u ++) {
		r |= (uint32_t)(tt[u] - 1);
	}
	return (int)(1u - (r >> 31));
}

/* see inner.h */
int
Zf(verify_recover)(uint16_t *h,
	const uint16_t *c0, const int16_t *s1, const int16_t *s2,
	unsigned logn, uint8_t *tmp)
{
	size_t u, n;
	uint16_t *tt;
	uint32_t r;

	n = (size_t)1 << logn;

	/*
	 * Reduce elements of s1 and s2 modulo q; then write s2 into tt[]
	 * and c0 - s1 into h[].
	 */
	tt = (uint16_t *)tmp;
	for (u = 0; u < n; u ++) {
		uint32_t w;

		w = (uint32_t)s2[u];
		w += Q & -(w >> 31);
		tt[u] = (uint16_t)w;

		w = (uint32_t)s1[u];
		w += Q & -(w >> 31);
		w = mq_sub(c0[u], w);
		h[u] = (uint16_t)w;
	}

	/*
	 * Compute h = (c0 - s1) / s2. If one of the coefficients of s2
	 * is zero (in NTT representation) then the operation fails. We
	 * keep that information into a flag so that we do not deviate
	 * from strict constant-time processing; if all coefficients of
	 * s2 are non-zero, then the high bit of r will be zero.
	 */
	mq_NTT(tt, logn);
	mq_NTT(h, logn);
	r = 0;
	for (u = 0; u < n; u ++) {
		r |= (uint32_t)(tt[u] - 1);
		h[u] = (uint16_t)mq_div_12289(h[u], tt[u]);
	}
	mq_iNTT(h, logn);

	/*
	 * Signature is acceptable if and only if it is short enough,
	 * and s2 was invertible mod phi mod q. The caller must still
	 * check that the rebuilt public key matches the expected
	 * value (e.g. through a hash).
	 */
	r = ~r & (uint32_t)-Zf(is_short)(s1, s2, logn);
	return (int)(r >> 31);
}

/* see inner.h */
int
Zf(count_nttzero)(const int16_t *sig, unsigned logn, uint8_t *tmp)
{
	uint16_t *s2;
	size_t u, n;
	uint32_t r;

	n = (size_t)1 << logn;
	s2 = (uint16_t *)tmp;
	for (u = 0; u < n; u ++) {
		uint32_t w;

		w = (uint32_t)sig[u];
		w += Q & -(w >> 31);
		s2[u] = (uint16_t)w;
	}
	mq_NTT(s2, logn);
	r = 0;
	for (u = 0; u < n; u ++) {
		uint32_t w;

		w = (uint32_t)s2[u] - 1u;
		r += (w >> 31);
	}
	return (int)r;
}
