#define REAL_WOMBA_ 1 // CHANGE THIS TO 1 for real, 0 for johnty's test!

static const int NUM_SENSORS = 5; //note: this must match the next array size

// MANUAL EFFECTS FROM TOUCH SPOTS!
//the ripple/grow effects centered at 7(8), 18(19), 27(28), 40(41), and 53(54)

#if REAL_WOMBA_ == 1
//*******REAL VERSION
static int SENSOR_SPOTS[NUM_SENSORS] = {7, 18, 27, 40, 53};

static const int EFFECT_7_1_size = 7;
static int EFFECT_7_1[EFFECT_7_1_size] = {1, 2, 6, 8, 10, 11, 12};
static const int EFFECT_7_2_size = 11;
static int EFFECT_7_2[EFFECT_7_2_size] = {0, 3, 9, 13, 14, 15, 16, 65, 66, 67, 68};

static const int EFFECT_18_1_size = 7;
static int EFFECT_18_1[EFFECT_18_1_size] = {15, 16, 17, 19, 23, 24, 25};
static const int EFFECT_18_2_size = 8;
static int EFFECT_18_2[EFFECT_18_2_size] = {10, 11, 12, 14, 22, 26, 27, 28};

static const int EFFECT_27_1_size = 7;
static int EFFECT_27_1[EFFECT_27_1_size] = { 23, 24, 25, 26, 28, 32, 33 };
static const int EFFECT_27_2_size = 8;
static int EFFECT_27_2[EFFECT_27_2_size] = { 17, 18, 19, 22, 31, 34, 35, 36 };

static const int EFFECT_40_1_size = 6;
static int EFFECT_40_1[EFFECT_40_1_size] = { 35, 36, 39, 41, 44, 45 };
static const int EFFECT_40_2_size = 10;
static int EFFECT_40_2[EFFECT_40_2_size] = { 31, 32, 34, 37, 42, 43, 46, 48, 49, 50 };

static const int EFFECT_53_1_size = 6;
static int EFFECT_53_1[EFFECT_53_1_size] = { 48, 49, 52, 54, 56, 57 };
static const int EFFECT_53_2_size = 10;
static int EFFECT_53_2[EFFECT_53_2_size] = { 44, 45, 46, 47, 50, 51, 55, 58, 62, 63 };

#else

//*******TEST VERSION
//test spot around 5, johnty's test strip:

static int SENSOR_SPOTS[NUM_SENSORS] = {0, 5, 15, 25, 29}; //testing on johnty's shorter strip

static const int EFFECT_7_1_size = 2;
static int EFFECT_7_1[EFFECT_7_1_size] = { 4, 6};
static const int EFFECT_7_2_size = 3;
static int EFFECT_7_2[EFFECT_7_2_size] = { 3, 7, 8};

//test spot around 12, johnty's test strip:
static const int EFFECT_18_1_size = 2;
static int EFFECT_18_1[EFFECT_18_1_size] = { 10, 12};
static const int EFFECT_18_2_size = 4;
static int EFFECT_18_2[EFFECT_18_2_size] = { 8, 9, 13, 14};

static const int EFFECT_27_1_size = 2;
static int EFFECT_27_1[EFFECT_27_1_size] = { 2,3 };
static const int EFFECT_27_2_size = 1;
static int EFFECT_27_2[EFFECT_27_2_size] = { 4 };

static const int EFFECT_40_1_size = 2;
static int EFFECT_40_1[EFFECT_40_1_size] = { 2,3 };
static const int EFFECT_40_2_size = 1;
static int EFFECT_40_2[EFFECT_40_2_size] = { 4 };

static const int EFFECT_53_1_size = 2;
static int EFFECT_53_1[EFFECT_53_1_size] = { 2,3 };
static const int EFFECT_53_2_size = 1;
static int EFFECT_53_2[EFFECT_53_2_size] = { 4 };

#endif

enum LEDEffectType {
 none,
 breath_blue,
 breath_white,
 breath_gray,
 breath_green,
 grow_green,
 grow_blue,
 grow_pink,
 grow_orange,
 grow_purple
};
