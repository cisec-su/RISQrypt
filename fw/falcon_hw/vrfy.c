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


const uint32_t psi[256] = {
    0x0FFB0FFB, 0x1B301B30, 0x06370637, 0x024A024A, 
    0x046E046E, 0x1BBB1BBB, 0x28AE28AE, 0x05540554, 
    0x2FB22FB2, 0x12AF12AF, 0x0B0D0B0D, 0x1BE81BE8, 
    0x2F9D2F9D, 0x1DBA1DBA, 0x08850885, 0x12EB12EB, 
    0x15C115C1, 0x1D771D77, 0x06110611, 0x03FC03FC, 
    0x0CF40CF4, 0x298D298D, 0x243D243D, 0x28E528E5, 
    0x2B352B35, 0x136A136A, 0x02DA02DA, 0x17041704, 
    0x23DB23DB, 0x0C6C0C6C, 0x1DBD1DBD, 0x1AB41AB4, 
    0x0DFC0DFC, 0x27E127E1, 0x28A128A1, 0x17981798, 
    0x1AD11AD1, 0x2F712F71, 0x1FFC1FFC, 0x17CA17CA, 
    0x1B211B21, 0x17A917A9, 0x22C522C5, 0x25752575, 
    0x16301630, 0x27AC27AC, 0x05BB05BB, 0x0A590A59, 
    0x02E102E1, 0x23562356, 0x14421442, 0x0DB50DB5, 
    0x29ED29ED, 0x0B4F0B4F, 0x19091909, 0x13C813C8, 
    0x03A403A4, 0x015F015F, 0x1E0C1E0C, 0x08050805, 
    0x11FA11FA, 0x0E7F0E7F, 0x13CE13CE, 0x1CFD1CFD, 
    0x0EFB0EFB, 0x24152415, 0x10811081, 0x10281028, 
    0x190A190A, 0x0EAF0EAF, 0x19251925, 0x150F150F, 
    0x20E220E2, 0x036C036C, 0x0D720D72, 0x174C174C, 
    0x1CDD1CDD, 0x107C107C, 0x216D216D, 0x0EE80EE8, 
    0x09DB09DB, 0x03A903A9, 0x093B093B, 0x03380338, 
    0x0AA70AA7, 0x13D313D3, 0x2F892F89, 0x23AC23AC, 
    0x04F904F9, 0x27372737, 0x2BB82BB8, 0x17AD17AD, 
    0x1CC71CC7, 0x20A020A0, 0x110F110F, 0x0C590C59, 
    0x0D2E0D2E, 0x21E921E9, 0x16A816A8, 0x04000400, 
    0x11E611E6, 0x14711471, 0x1F1C1F1C, 0x0D920D92, 
    0x21362136, 0x07410741, 0x17921792, 0x0B3F0B3F, 
    0x1F1A1F1A, 0x17C417C4, 0x28C628C6, 0x1AFF1AFF, 
    0x2D0F2D0F, 0x03520352, 0x208E208E, 0x2F982F98, 
    0x26332633, 0x1A141A14, 0x1AA01AA0, 0x09340934, 
    0x22612261, 0x132E132E, 0x202E202E, 0x08ED08ED, 
    0x10C810C8, 0x26412641, 0x0A5A0A5A, 0x1C501C50, 
    0x0FDB0FDB, 0x2E4D2E4D, 0x2B812B81, 0x10061006, 
    0x1F021F02, 0x02190219, 0x2CBD2CBD, 0x254C254C, 
    0x2DD82DD8, 0x22C722C7, 0x1D5A1D5A, 0x03540354, 
    0x2D452D45, 0x10121012, 0x0BA20BA2, 0x246B246B, 
    0x08440844, 0x0E3D0E3D, 0x2A772A77, 0x1BE41BE4, 
    0x2AAB2AAB, 0x02D502D5, 0x0DA60DA6, 0x2E3E2E3E, 
    0x0E6D0E6D, 0x27E427E4, 0x13F613F6, 0x11191119, 
    0x0AF80AF8, 0x26F326F3, 0x10271027, 0x2AE92AE9, 
    0x01E201E2, 0x27222722, 0x2C622C62, 0x15251525, 
    0x2BB42BB4, 0x2C112C11, 0x1FE01FE0, 0x16831683, 
    0x2DE42DE4, 0x159C159C, 0x035E035E, 0x162E162E, 
    0x0B4D0B4D, 0x25AF25AF, 0x281D281D, 0x186E186E, 
    0x14271427, 0x07550755, 0x2DCC2DCC, 0x2FF22FF2, 
    0x05750575, 0x1F281F28, 0x1F3C1F3C, 0x2A762A76, 
    0x197C197C, 0x09990999, 0x12501250, 0x08220822, 
    0x1DD41DD4, 0x05770577, 0x2AA02AA0, 0x0AE70AE7, 
    0x08DB08DB, 0x0C8E0C8E, 0x13851385, 0x11161116, 
    0x1F431F43, 0x06C706C7, 0x20002000, 0x03660366, 
    0x262A262A, 0x17F417F4, 0x2E1D2E1D, 0x13111311, 
    0x0A070A07, 0x13621362, 0x29F729F7, 0x08560856, 
    0x14FC14FC, 0x199F199F, 0x109C109C, 0x16881688, 
    0x1A901A90, 0x2AC32AC3, 0x2CB92CB9, 0x09AF09AF, 
    0x22CF22CF, 0x227C227C, 0x12021202, 0x15B815B8, 
    0x096D096D, 0x245C245C, 0x17671767, 0x266E266E, 
    0x2C412C41, 0x2D5B2D5B, 0x0E950E95, 0x1C001C00, 
    0x1D481D48, 0x2F152F15, 0x19C019C0, 0x2EFD2EFD, 
    0x28762876, 0x02C602C6, 0x14FB14FB, 0x1EB81EB8, 
    0x19B219B2, 0x16591659, 0x2D652D65, 0x2CF62CF6, 
    0x1B631B63, 0x173E173E, 0x23DE23DE, 0x2D222D22, 
    0x1B601B60, 0x26892689, 0x2A5D2A5D, 0x106B106B, 
    0x00A200A2, 0x26402640, 0x213E213E, 0x0E7A0E7A, 
    0x15761576, 0x1BC21BC2, 0x18751875, 0x062C062C 
};

const uint32_t psi_inv[256] = {
    0x0FFB0FFB, 0x09E209E2, 0x0A130A13, 0x1B6F1B6F, 
    0x0C250C25, 0x29692969, 0x16D716D7, 0x2C3C2C3C, 
    0x20E120E1, 0x16571657, 0x114A114A, 0x2B2C2B2C, 
    0x20372037, 0x0D540D54, 0x14611461, 0x2F8C2F8C, 
    0x000D000D, 0x11EA11EA, 0x26732673, 0x08230823, 
    0x1B671B67, 0x28132813, 0x2B562B56, 0x00300030, 
    0x24DE24DE, 0x2C8E2C8E, 0x17DD17DD, 0x02C702C7, 
    0x147C147C, 0x03840384, 0x13551355, 0x281E281E, 
    0x19EB19EB, 0x002A002A, 0x05100510, 0x2EAD2EAD, 
    0x1BAD1BAD, 0x125F125F, 0x191F191F, 0x12271227, 
    0x2EEB2EEB, 0x171A171A, 0x1B141B14, 0x2BDD2BDD, 
    0x1AFC1AFC, 0x28552855, 0x2C6F2C6F, 0x19881988, 
    0x17D517D5, 0x119A119A, 0x23AA23AA, 0x0F3D0F3D, 
    0x0D8C0D8C, 0x1E3A1E3A, 0x0DC60DC6, 0x06C206C2, 
    0x14A614A6, 0x09B209B2, 0x12011201, 0x16181618, 
    0x2A3C2A3C, 0x06B306B3, 0x0EB40EB4, 0x09C609C6, 
    0x15E115E1, 0x080A080A, 0x080B080B, 0x1EEE1EEE, 
    0x0C010C01, 0x241D241D, 0x1ED61ED6, 0x19651965, 
    0x2ACC2ACC, 0x1BE31BE3, 0x1EB91EB9, 0x25212521, 
    0x1A211A21, 0x081C081C, 0x11121112, 0x02EE02EE, 
    0x0FAD0FAD, 0x2E692E69, 0x13661366, 0x1A9E1A9E, 
    0x27BF27BF, 0x28FC28FC, 0x02D802D8, 0x2B1C2B1C, 
    0x03B103B1, 0x2B092B09, 0x121F121F, 0x1A821A82, 
    0x05510551, 0x283D283D, 0x00650065, 0x2EDD2EDD, 
    0x027D027D, 0x0DB80DB8, 0x0BDC0BDC, 0x0EAB0EAB, 
    0x2E9C2E9C, 0x2B7B2B7B, 0x0B4A0B4A, 0x09300930, 
    0x1E591E59, 0x17011701, 0x11351135, 0x28152815, 
    0x2BA82BA8, 0x1C411C41, 0x23322332, 0x2D962D96, 
    0x1AF01AF0, 0x26912691, 0x1F191F19, 0x03400340, 
    0x2A672A67, 0x1D071D07, 0x267D267D, 0x199C199C, 
    0x13091309, 0x0CEF0CEF, 0x0F100F10, 0x074F074F, 
    0x0D990D99, 0x00230023, 0x04380438, 0x0EE50EE5, 
    0x09240924, 0x08450845, 0x084C084C, 0x187E187E, 
    0x24062406, 0x0CC60CC6, 0x17D617D6, 0x21C021C0, 
    0x0B8E0B8E, 0x03310331, 0x170B170B, 0x13E213E2, 
    0x26E426E4, 0x08C308C3, 0x177C177C, 0x14821482, 
    0x0DB90DB9, 0x24D924D9, 0x27C827C8, 0x2A4F2A4F, 
    0x26342634, 0x2EDF2EDF, 0x13E813E8, 0x0DBE0DBE, 
    0x19D719D7, 0x0D390D39, 0x1ED71ED7, 0x298B298B, 
    0x25372537, 0x29A629A6, 0x02C302C3, 0x28052805, 
    0x116B116B, 0x00060006, 0x23032303, 0x06AB06AB, 
    0x263E263E, 0x10571057, 0x1F051F05, 0x104F104F, 
    0x146B146B, 0x11041104, 0x18711871, 0x288E288E, 
    0x11921192, 0x05C305C3, 0x06590659, 0x1F141F14, 
    0x2C8D2C8D, 0x1DF21DF2, 0x19AB19AB, 0x16C016C0, 
    0x08CB08CB, 0x0B2D0B2D, 0x1D661D66, 0x23412341, 
    0x253D253D, 0x2A882A88, 0x096E096E, 0x03280328, 
    0x2F2E2F2E, 0x00F500F5, 0x1D881D88, 0x08410841, 
    0x256A256A, 0x01260126, 0x23702370, 0x26B526B5, 
    0x01B701B7, 0x20972097, 0x1FD61FD6, 0x1F0F1F0F, 
    0x28672867, 0x11B311B3, 0x2D892D89, 0x13051305, 
    0x2CE12CE1, 0x2A4E2A4E, 0x17031703, 0x22B522B5, 
    0x16D016D0, 0x1B341B34, 0x09A109A1, 0x0AA90AA9, 
    0x2ED32ED3, 0x13921392, 0x00680068, 0x2F4E2F4E, 
    0x00870087, 0x13DD13DD, 0x1E051E05, 0x0AA50AA5, 
    0x079E079E, 0x2EE52EE5, 0x06EA06EA, 0x14691469, 
    0x005B005B, 0x1D641D64, 0x1D201D20, 0x08F408F4, 
    0x2FCE2FCE, 0x28802880, 0x0F540F54, 0x01500150, 
    0x120D120D, 0x17DC17DC, 0x17081708, 0x13711371, 
    0x2F622F62, 0x189C189C, 0x27512751, 0x28CD28CD, 
    0x26232623, 0x0C5E0C5E, 0x19041904, 0x0E2E0E2E, 
    0x217D217D, 0x2D4B2D4B, 0x2EA52EA5, 0x2CCE2CCE, 
    0x25022502, 0x01D901D9, 0x0FDE0FDE, 0x159E159E, 
    0x16841684, 0x00050005, 0x152D152D, 0x1D8F1D8F 
};


// Modulus Q = 12289
// Vector Bound (Dual Mode) = 512 / 2 = 256
const uint32_t FALCON_Q     = 0x3001;
const uint32_t FALCON_INV2  = 0x1801;
const uint32_t FALCON_MU[2] = {0xE425E9E0, 0x00055538};

void poly_init_q() {

    ntt_lite_load_q(FALCON_Q, FALCON_MU, 8, 14, FALCON_INV2, NTT_LITE_MODE_DUAL);
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

        //print_string("\n--- Checking COMPRESSED Intermediates ---\n");

        gold_ntt  = mid_ntt_s2_comp;

        gold_mul  = mid_montymul_comp;

        gold_intt = mid_intt_comp;

        gold_sub  = mid_sub_c0_comp;

    } else if (call_count == 1) {

        //print_string("\n--- Checking PADDED Intermediates ---\n");

        gold_ntt  = mid_ntt_s2_pad;

        gold_mul  = mid_montymul_pad;

        gold_intt = mid_intt_pad;

        gold_sub  = mid_sub_c0_pad;

    } else {

        //print_string("\n--- Checking CT Intermediates ---\n");

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
