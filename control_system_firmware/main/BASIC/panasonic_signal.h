#include "stdio.h"

const uint16_t panasonic_signal_auto[] = {
   3565,1679,467,404,475,1272,473,399,475,399,474,399,484,390,498,377,474,398,475,398,475,398,476,398,473,401,473,400,473,1275,465,407,474,399,498,375,475,398,475,397,474,401,473,399,476,1271,473,1275,473,1275,472,399,476,397,475,1271,473,401,472,401,474,399,474,400,473,400,472,401,471,403,471,401,472,402,470,403,470,403,471,403,469,405,466,407,466,407,463,410,441,433,441,432,463,410,440,434,463,410,440,434,440,432,439,434,463,410,463,411,462,412,439,433,462,413,461,413,438,1307,439,1308,439,434,462,412,459,414,438,436,437,436,438,10003,3528,1737,415,438,436,1331,415,438,436,438,436,439,436,438,434,441,433,442,433,441,433,442,432,441,456,440,403,471,403,1346,402,471,403,472,403,470,403,471,403,471,403,471,403,471,402,1346,402,1345,403,1345,402,471,403,471,404,1343,403,471,403,471,403,471,403,471,403,472,403,471,403,471,403,472,403,471,402,471,403,471,404,471,403,472,403,1345,402,1346,402,471,403,1346,401,472,403,471,403,471,403,473,403,471,402,1346,401,1345,402,471,402,473,402,1346,401,473,402,472,403,471,402,472,402,472,402,472,402,472,402,472,402,472,402,1347,402,1345,401,1346,401,1371,376,1347,400,473,401,1349,400,1348,399,473,402,472,402,472,402,472,402,471,403,471,402,472,403,472,402,473,403,471,402,472,403,470,403,472,403,471,402,473,402,472,402,473,401,473,401,1371,377,1369,377,1371,376,474,400,474,401,474,401,497,377,473,401,497,377,474,400,497,377,474,401,1371,376,1371,375,1373,375,498,377,497,376,498,376,497,376,499,375,499,352,523,375,500,352,521,375,500,352,522,352,522,351,522,351,524,351,523,352,523,351,1398,350,523,350,525,351,1398,349,522,352,523,351,524,350,1424,324,524,351,523,351,523,350,524,350,525,349,525,350,524,350,525,349,524,350,1424,323,525,349,525,349,525,349,525,348,551,324,551,324,1424,323,1425,322,1426,322,1424,322,1452,295,553,322,552,322,1428,321,0
};

const uint16_t panasonic_signal_powerOff[] = {
    3633,1617,561,315,543,1205,535,332,555,320,552,319,546,335,544,330,542,309,570,324,546,327,549,325,545,328,556,316,555,1196,539,332,547,325,545,304,578,319,550,331,540,327,552,324,545,1204,544,1197,553,1198,541,332,547,326,556,1167,575,320,555,326,548,319,550,328,553,320,545,328,552,327,541,325,546,331,546,327,547,334,536,329,545,329,545,327,548,331,542,328,553,328,539,327,552,328,548,325,540,329,552,323,544,329,548,325,549,331,542,324,544,330,543,337,542,328,543,332,546,1200,542,1208,536,338,544,327,543,331,537,337,541,330,544,9895,3634,1623,531,334,545,1207,540,333,544,334,538,337,534,338,537,332,546,329,544,334,547,323,541,339,541,331,537,337,541,1206,536,337,539,344,534,339,535,336,537,336,535,337,541,337,536,1208,539,1210,537,1209,541,332,544,337,535,1215,532,332,539,337,538,340,533,344,533,346,528,346,531,342,533,335,535,342,537,335,533,338,536,348,530,341,532,338,536,1213,539,340,530,1211,539,336,534,348,528,346,529,347,530,1216,528,342,534,347,527,346,528,341,534,1217,533,347,525,351,528,345,527,346,530,346,526,344,529,344,530,346,530,346,529,1221,525,1224,528,1219,529,1214,532,1219,529,343,532,1220,526,345,530,1223,525,351,525,346,527,348,526,346,531,346,523,353,525,350,521,353,526,347,525,352,524,349,523,352,521,353,522,349,525,350,523,358,517,353,522,1225,521,1222,527,1224,522,355,523,352,521,355,518,355,521,352,522,352,519,359,517,358,518,354,516,1231,520,1229,522,1229,515,359,518,357,516,354,522,353,517,359,516,361,515,358,515,358,518,357,517,357,515,361,516,355,517,361,515,369,504,371,503,360,514,1245,502,361,514,372,503,1234,515,360,514,361,512,373,503,1246,499,375,503,371,502,372,501,373,502,372,499,376,498,376,501,364,509,376,498,375,503,370,501,374,500,373,499,376,498,378,501,374,498,1250,500,1247,501,1248,498,376,499,1247,501,375,499,1248,499,1251,498,0
};

const uint16_t panasonic_signal_cool_16[] = {
   3555,1688,438,382,494,1306,415,440,433,439,434,391,505,416,435,440,459,414,460,413,458,416,458,414,436,438,436,436,461,1287,435,438,458,416,435,439,434,437,435,439,434,439,435,438,435,1311,434,1331,416,1314,433,441,433,439,434,1312,434,439,434,439,434,439,435,440,433,440,433,440,433,440,433,440,434,440,432,441,433,440,434,440,433,440,433,441,433,440,432,440,434,440,433,440,433,440,433,442,432,441,432,441,432,441,432,441,432,442,425,448,425,448,426,447,427,447,426,449,425,1341,405,1341,405,449,425,447,426,449,425,448,425,449,425,10029,3496,1746,406,470,404,1341,406,448,426,448,426,450,424,449,425,450,425,471,404,470,404,449,425,448,425,450,424,471,404,1342,405,448,426,450,425,448,426,470,424,428,425,471,403,462,413,1342,405,1343,405,1342,406,470,404,471,403,1342,404,472,403,471,403,471,403,471,402,474,403,470,403,471,403,471,403,470,404,471,403,471,403,472,402,472,403,1343,405,1342,405,471,404,1348,422,1320,404,1343,428,450,401,473,402,472,401,473,402,471,402,472,402,472,403,1341,428,451,423,451,402,473,401,473,401,472,424,450,401,473,401,473,402,472,402,1349,399,1349,399,1349,406,1340,399,1349,399,473,401,1349,399,1349,399,474,401,473,401,473,401,473,400,473,401,474,424,450,401,473,401,475,400,473,401,473,424,449,401,474,400,474,401,473,424,450,401,475,400,474,401,1348,399,1369,377,1371,378,474,400,473,402,474,400,474,401,473,401,473,401,473,401,473,401,474,400,1371,378,1370,377,1371,378,473,401,473,401,474,400,474,400,474,400,475,400,474,400,477,398,474,401,474,400,473,401,473,400,474,401,473,402,473,401,474,401,1372,373,476,400,474,401,1372,375,474,399,476,400,475,399,1374,374,475,399,476,398,475,399,475,399,475,399,475,400,474,400,477,398,475,399,1374,374,499,374,499,352,498,400,475,399,497,353,525,374,1374,373,499,374,477,399,1375,349,524,350,524,351,1400,348,1399,348,0
};

const uint16_t panasonic_signal_cool_17[] = {
   3532,1714,474,399,474,1276,471,399,472,402,474,398,500,373,442,434,473,400,498,375,500,375,471,400,475,399,475,398,499,1250,475,398,473,401,474,398,475,398,500,374,498,374,476,398,475,1272,498,1248,474,1276,495,376,475,398,473,1274,472,400,473,401,472,402,472,400,473,401,472,400,474,401,470,402,472,401,471,403,471,403,470,404,469,403,471,404,470,402,469,405,467,405,469,404,467,407,467,407,466,408,466,407,464,409,466,406,467,407,466,408,463,410,463,411,466,407,466,408,465,1282,464,1283,464,408,464,409,465,408,466,408,464,410,464,9972,3554,1688,465,408,465,1283,440,433,464,410,440,434,464,410,464,410,441,435,464,410,440,434,440,433,440,435,439,435,440,1308,439,434,439,436,439,435,439,436,438,435,439,435,438,436,438,1311,460,1287,437,1311,437,436,437,437,438,1311,436,437,436,437,437,438,436,438,436,439,435,440,435,439,435,438,435,440,433,441,433,441,434,441,431,444,426,1343,404,1343,404,471,403,1345,403,1345,402,1345,403,471,404,472,403,471,403,1344,403,471,403,470,404,470,404,1344,403,471,404,472,403,470,404,470,403,471,403,471,403,471,403,471,403,471,404,1346,402,1346,401,1345,402,1345,402,1346,402,471,403,1346,402,1346,402,472,403,471,403,472,402,471,403,471,402,472,403,471,403,471,404,472,403,470,404,471,403,471,402,471,404,471,401,473,403,471,402,473,403,472,403,1346,401,1345,402,1345,403,471,403,471,403,472,403,472,402,472,403,471,402,472,402,472,402,472,403,1347,400,1348,400,1347,401,473,401,472,402,472,402,472,402,472,402,472,402,473,402,472,403,472,402,472,402,472,401,473,401,472,402,473,402,472,402,473,402,1370,377,472,403,472,402,1347,400,473,401,473,402,472,402,1372,377,471,403,472,402,472,402,473,401,471,402,473,402,472,402,474,402,472,401,1371,377,471,401,473,402,472,402,472,402,473,401,475,401,1370,376,1371,377,473,401,1370,377,474,400,498,376,1372,376,1372,375,0
};

const uint16_t panasonic_signal_cool_18[] = {
   3528,1717,437,436,473,1274,439,433,472,400,475,398,475,401,474,399,474,399,479,396,475,397,498,375,496,378,473,400,473,1274,468,405,472,402,470,405,469,403,470,403,470,404,470,403,469,1278,437,1310,438,1309,439,435,440,433,463,1285,463,409,441,432,441,433,441,433,440,433,464,410,440,434,438,434,463,411,442,432,439,434,439,435,437,437,439,434,439,435,461,412,462,412,438,435,438,436,460,413,438,436,438,435,438,436,437,436,437,437,437,437,437,437,459,415,435,438,436,437,435,1334,414,1332,415,438,435,439,434,440,433,442,432,441,433,10031,3497,1749,403,471,402,1345,403,471,404,470,403,471,403,472,403,471,403,472,403,471,403,471,404,470,403,471,403,471,404,1344,403,471,403,472,404,471,403,471,403,471,402,471,404,470,404,1344,402,1346,402,1345,403,471,404,471,403,1344,403,471,403,471,403,472,402,472,403,472,403,471,403,472,402,471,403,472,402,471,403,472,402,472,402,473,403,1346,401,1346,401,472,403,1345,402,1345,403,1346,402,471,403,472,402,472,402,472,403,1345,425,448,402,472,403,1346,402,472,402,473,402,472,402,471,403,472,401,472,403,471,403,472,402,472,402,1348,400,1348,399,1347,401,1348,399,1348,400,471,403,1349,399,1347,400,474,401,472,402,473,402,471,402,473,401,473,401,472,403,473,401,474,402,472,402,472,402,472,402,471,403,472,402,471,403,472,402,474,402,471,402,1370,377,1371,377,1370,378,472,402,473,402,472,402,473,401,472,402,472,403,472,402,473,401,473,401,1372,376,1371,377,1371,376,475,399,474,400,473,401,472,402,474,401,473,400,474,401,474,402,472,401,474,400,474,400,475,399,475,400,474,400,475,400,475,400,1372,375,476,398,500,375,1372,361,514,376,497,377,498,376,1373,352,522,352,523,356,517,355,518,353,522,352,523,352,522,352,524,355,518,352,1397,351,521,353,522,351,524,350,524,351,523,351,525,351,1397,350,523,351,1399,349,1397,351,523,351,524,350,1400,349,1423,325,0
};

const uint16_t panasonic_signal_cool_19[] = {
   3529,1713,498,375,470,1276,474,399,475,398,475,399,474,400,474,400,474,398,474,400,475,398,474,399,473,401,473,400,473,1275,471,401,473,401,471,403,471,402,469,403,468,406,468,406,465,1281,464,1283,440,1307,440,433,465,408,465,1283,440,433,450,422,464,410,440,433,440,433,463,410,440,434,464,409,463,410,440,433,464,410,438,435,439,436,438,435,438,435,439,434,439,435,438,434,438,436,438,436,436,437,437,436,437,436,437,436,437,437,437,437,435,437,437,437,435,440,435,438,435,1333,413,1333,413,439,434,440,433,441,432,442,425,470,404,10031,3492,1750,403,472,402,1345,402,471,403,471,403,471,402,473,402,472,403,472,403,471,403,470,403,471,403,471,403,471,403,1344,403,471,403,472,403,470,404,471,403,471,403,471,403,471,403,1345,402,1345,403,1345,403,471,403,470,403,1346,402,470,403,472,403,471,403,471,403,472,403,472,402,472,402,471,403,471,403,471,403,471,403,472,403,472,403,1345,402,1345,401,472,402,1347,401,1346,401,1346,402,472,403,473,402,471,403,1346,400,1346,401,472,403,472,402,1346,401,473,402,472,402,472,402,472,401,472,402,472,402,473,402,472,402,471,403,1349,399,1346,401,1346,402,1347,399,1372,375,473,402,1371,377,1371,377,472,402,472,402,472,403,470,403,471,403,471,403,472,403,472,402,472,403,471,403,471,402,472,401,473,402,472,402,472,402,472,402,473,403,472,401,1370,377,1370,376,1372,377,471,402,473,402,473,401,473,401,474,400,474,400,474,401,472,401,473,401,1372,375,1371,376,1372,377,497,376,498,376,497,376,499,376,499,352,522,352,522,352,524,351,523,351,523,375,498,352,522,375,499,352,522,352,523,375,500,352,1396,351,522,352,522,352,1397,351,522,351,524,351,523,352,1397,351,522,352,522,351,524,350,523,352,522,351,523,352,522,352,524,351,523,351,1398,349,523,351,523,351,524,349,524,352,523,350,525,350,1399,348,1424,324,1424,323,1423,324,525,349,525,350,1423,325,1424,324,0
};

const uint16_t panasonic_signal_cool_20[] = {
   3533,1711,473,400,476,1272,498,375,476,397,475,399,474,400,473,402,471,402,474,399,475,399,475,397,476,398,476,398,474,1274,473,399,475,399,498,376,475,399,473,400,473,401,472,400,473,1275,473,1275,471,1277,472,401,473,400,471,1276,468,404,470,403,472,401,473,402,468,406,469,405,468,405,466,408,464,408,467,407,442,432,465,408,442,433,464,409,466,407,465,409,441,433,465,407,466,408,465,408,465,410,465,407,466,408,465,408,465,409,465,408,465,409,464,409,467,408,465,408,466,1282,463,1283,440,433,465,408,442,432,463,410,465,410,464,9977,3554,1690,439,434,441,1308,441,432,463,411,440,435,463,412,439,436,438,437,439,436,461,412,439,436,438,436,438,436,441,1307,437,437,437,439,437,436,438,437,438,436,438,437,437,437,436,1333,414,1335,413,1334,415,439,434,440,434,1335,411,441,434,442,432,464,403,472,403,472,403,471,403,471,404,470,404,449,424,471,403,472,403,472,403,472,403,1345,403,1345,403,471,403,1345,403,1344,404,1344,404,471,403,472,404,470,404,471,402,472,403,1344,402,472,403,1345,402,472,403,471,404,471,403,471,403,471,403,471,403,471,403,472,403,471,403,1346,402,1346,403,1345,403,1345,402,1346,403,471,403,1345,401,1347,402,473,402,470,404,472,403,471,402,472,403,471,403,472,403,472,402,473,402,472,402,472,403,471,403,472,402,471,403,472,402,473,401,475,402,471,403,1345,402,1346,401,1347,425,448,426,448,403,473,401,473,402,471,404,471,402,473,402,472,403,471,402,1349,400,1347,401,1348,401,472,402,472,402,472,402,473,401,472,402,473,402,473,402,472,403,472,403,472,402,471,402,473,402,472,402,473,402,472,402,474,402,1370,377,472,403,472,403,1371,377,472,402,472,403,473,401,1373,377,472,401,474,401,472,403,472,402,472,402,473,401,473,402,473,402,474,400,1371,376,473,402,473,401,473,401,473,400,475,401,476,399,1371,377,473,401,498,377,498,376,1372,376,475,400,1373,351,1395,354,0
};

const uint16_t panasonic_signal_cool_21[] = {
   3531,1713,473,398,473,1276,474,398,475,399,497,376,474,400,500,374,498,376,497,376,474,399,475,398,475,398,499,375,474,1273,474,398,476,398,472,401,473,401,473,399,473,401,472,400,473,1274,472,1275,495,1252,472,401,471,401,472,1275,471,403,470,403,470,403,471,403,470,404,469,404,469,404,469,404,468,406,467,406,466,407,467,406,465,409,465,408,464,409,466,407,465,408,466,408,441,432,464,409,442,432,465,408,441,432,447,426,464,410,440,432,464,410,463,410,440,434,440,434,439,1308,439,1308,437,435,439,435,462,411,463,411,462,412,438,10004,3528,1737,438,415,435,1333,414,438,436,437,436,439,435,440,434,440,434,442,433,441,433,441,431,442,426,471,403,470,404,1344,403,471,403,472,403,471,403,471,404,469,404,470,403,471,403,1344,403,1345,403,1345,402,471,404,470,404,1344,403,470,403,472,403,471,403,471,403,472,403,471,403,470,404,470,403,472,403,471,402,472,403,472,401,473,403,1345,402,1345,403,472,402,1345,402,1346,402,1345,402,471,403,472,404,470,403,1345,402,471,403,1345,402,471,403,1346,401,473,402,472,403,471,403,471,402,472,402,471,404,471,402,471,403,472,402,1347,402,1345,401,1346,402,1346,401,1345,403,470,403,1347,401,1347,400,472,402,472,403,473,401,471,402,472,402,472,402,472,402,473,402,473,402,472,402,472,402,472,402,471,403,472,402,471,402,474,401,473,402,472,402,1370,377,1370,377,1369,377,473,402,472,403,472,402,472,403,472,401,472,402,472,402,472,401,473,402,1370,377,1371,377,1370,377,473,402,472,402,472,401,474,400,473,401,473,402,472,401,474,402,472,401,474,400,473,401,473,401,472,402,473,402,473,400,475,400,1371,377,473,400,498,376,1372,376,498,376,498,376,498,376,1372,353,520,376,499,352,521,352,522,375,499,376,499,351,522,376,500,365,508,353,1394,352,522,352,521,352,523,358,516,351,523,352,523,352,1396,351,1398,350,522,352,523,351,1397,350,523,352,1398,349,1398,349,0
};

const uint16_t panasonic_signal_cool_22[] = {
   3555,1690,473,402,468,1279,467,405,468,406,465,407,442,433,465,408,443,432,441,432,465,409,440,432,441,432,465,409,463,1285,463,410,440,433,440,433,465,409,463,410,463,410,439,435,462,1285,438,1308,439,1309,461,412,439,434,438,1311,436,435,437,436,438,436,437,437,437,436,437,436,438,436,437,436,437,437,436,437,436,438,435,439,435,439,435,438,435,439,434,440,433,440,433,440,433,464,403,470,403,470,403,470,403,471,402,470,404,469,404,470,404,470,403,470,404,471,403,470,403,1344,403,1343,404,469,404,470,403,469,404,470,404,470,404,10037,3492,1750,403,471,403,1344,403,471,403,472,402,471,402,472,402,473,402,472,403,471,403,471,404,470,403,471,403,471,403,1346,402,471,403,473,402,472,402,472,402,472,402,472,402,471,404,1346,401,1346,402,1346,401,472,403,471,403,1346,401,471,403,472,402,471,403,472,403,472,402,472,402,472,403,471,402,473,401,472,402,474,401,473,402,473,402,1346,400,1349,400,472,403,1371,377,1346,400,1346,401,473,401,473,403,472,402,473,402,1346,401,1371,376,473,402,1371,377,473,401,473,401,473,402,471,403,471,403,472,402,473,402,473,401,474,402,1370,377,1371,377,1371,377,1370,377,1371,377,472,402,1371,377,1371,376,474,401,472,402,474,400,497,377,473,401,473,401,474,401,473,401,474,401,473,402,472,401,473,400,498,376,473,402,498,376,499,352,498,376,523,353,1395,375,1372,375,1372,375,499,353,522,376,498,353,521,353,521,352,522,376,499,352,522,352,523,351,1398,350,1397,351,1398,349,524,351,523,351,522,351,523,352,522,352,523,351,524,351,524,351,522,353,524,350,523,351,522,351,524,351,523,351,524,351,524,350,1398,349,525,349,525,350,1397,351,524,350,524,350,526,349,1424,324,524,350,524,351,549,324,525,349,524,351,524,350,525,349,527,348,525,350,1422,324,551,323,525,349,525,348,553,322,527,347,528,347,1426,322,551,323,1426,321,552,296,1452,322,552,322,1426,320,1428,322,0
};

const uint16_t panasonic_signal_cool_23[] = {
   3529,1711,500,374,474,1273,473,399,473,400,475,398,474,400,498,377,497,375,496,377,474,399,475,398,473,400,476,399,473,1273,474,398,475,399,473,400,475,398,473,400,475,399,472,400,497,1250,472,1275,471,1275,471,402,473,399,497,1250,472,400,473,402,471,402,471,402,471,402,471,402,471,402,471,402,469,404,470,405,468,404,468,405,467,408,466,406,468,405,466,407,467,406,467,407,465,408,465,408,465,408,442,431,465,408,442,431,466,408,465,407,442,432,464,409,466,408,441,433,459,1288,464,1281,441,432,465,408,465,409,465,408,464,410,464,9968,3554,1687,464,409,466,1282,464,409,462,411,465,409,441,433,465,410,464,411,464,409,464,410,442,432,464,409,464,409,466,1281,463,411,464,411,463,411,464,410,440,434,463,410,459,416,463,1284,439,1307,464,1284,440,433,464,410,464,1282,439,435,462,411,445,430,439,435,439,435,440,434,440,434,439,436,438,435,438,435,439,435,438,436,438,438,437,1310,437,1311,435,437,437,1313,434,1333,413,1334,414,438,436,439,437,437,436,1333,412,1336,411,1336,411,442,432,1336,404,471,403,471,403,471,403,470,404,471,403,471,403,471,403,471,403,471,404,1345,403,1343,404,1344,402,1345,402,1345,403,471,403,1345,402,1345,402,471,404,471,403,471,403,470,403,471,403,471,402,472,402,472,403,472,403,471,402,471,403,471,403,471,403,471,403,471,402,472,403,472,403,471,402,1346,401,1345,402,1345,402,471,403,471,404,472,402,472,402,471,403,471,403,472,402,471,403,471,403,1345,402,1345,403,1346,400,472,402,472,403,470,403,472,402,472,402,472,402,471,403,472,403,471,403,471,403,471,402,471,403,471,403,471,403,472,402,472,402,1346,402,471,403,471,402,1346,401,472,402,473,401,473,401,1348,401,472,401,472,402,472,402,472,401,472,402,472,402,472,402,474,401,472,402,1347,400,471,402,473,401,472,401,473,402,472,402,473,402,1369,377,1346,400,1347,400,473,401,1371,377,472,401,1372,376,1371,376,0
};

const uint16_t panasonic_signal_cool_24[] = {
   3565,1678,474,400,475,1272,471,400,475,399,474,398,500,376,473,399,501,373,500,374,497,376,476,397,499,373,475,400,472,1274,474,400,497,375,495,379,474,399,474,399,474,400,471,401,473,1275,471,1274,471,1277,496,377,473,400,473,1274,471,401,474,400,470,403,471,403,469,404,469,403,472,402,470,402,469,405,467,407,466,407,466,407,466,409,464,409,463,410,465,408,465,408,441,432,466,409,465,407,442,431,465,408,466,408,465,407,466,409,447,425,441,433,463,410,463,411,440,433,440,1307,464,1282,440,432,444,431,439,434,439,434,439,436,438,10001,3550,1693,436,437,436,1312,437,435,438,436,437,437,437,438,436,438,436,439,436,438,435,439,435,439,435,438,435,441,433,1335,412,441,433,444,431,442,425,471,403,471,403,470,404,471,403,1344,403,1345,403,1345,403,470,402,473,402,1345,403,470,403,471,403,472,402,470,404,472,403,472,402,471,403,471,403,471,402,472,403,472,402,472,403,472,403,1345,402,1345,403,471,403,1345,403,1344,402,1346,402,471,403,472,403,472,401,473,402,471,402,471,403,1346,402,1345,401,473,403,471,403,471,403,471,403,471,402,472,403,470,403,472,403,471,403,1347,401,1345,402,1346,401,1346,401,1347,401,472,402,1346,401,1347,401,473,402,471,403,472,402,471,402,472,402,472,402,473,402,471,403,473,401,473,401,473,401,472,403,472,402,472,402,471,403,472,403,472,402,473,402,1370,376,1370,378,1370,378,472,402,471,403,473,401,474,400,473,402,471,403,472,402,472,402,473,401,1371,377,1371,377,1371,377,474,399,474,401,473,400,475,398,474,401,498,376,474,401,498,377,497,376,475,399,499,369,503,353,522,352,522,376,499,352,521,357,1391,353,521,352,522,375,1373,374,498,353,523,351,523,356,1393,350,523,352,523,351,522,351,523,351,523,351,524,351,522,351,525,350,524,350,1398,349,524,350,523,350,524,350,525,350,523,351,524,351,1397,350,523,351,524,350,1399,348,1424,324,525,349,1423,325,1424,323,0
};

const uint16_t panasonic_signal_cool_25[] = {
   3554,1689,473,399,499,1248,475,398,474,399,474,399,474,400,472,403,471,403,473,400,473,400,471,402,472,402,472,402,468,1278,467,406,465,409,466,407,464,409,463,411,441,432,465,409,440,1306,464,1283,464,1284,463,410,444,430,440,1306,440,433,464,410,440,433,441,433,440,434,463,410,439,436,438,434,438,436,439,434,438,435,461,413,438,436,438,436,438,435,438,435,438,436,438,436,437,437,436,437,436,438,435,438,434,439,436,438,434,439,435,439,435,439,433,440,433,442,433,441,431,1337,404,1344,403,470,403,470,404,469,404,470,403,471,403,10032,3494,1749,403,470,404,1345,402,471,404,470,404,471,403,473,402,472,403,471,404,470,404,471,403,471,403,471,403,471,403,1345,403,471,403,472,402,472,402,472,403,471,402,472,403,471,403,1346,401,1346,403,1346,401,472,402,472,402,1345,403,471,402,472,403,471,402,473,402,472,403,472,402,471,403,472,425,448,403,471,403,472,403,471,403,473,402,1345,402,1345,425,449,402,1346,402,1347,400,1347,402,471,402,474,402,471,402,1347,401,471,403,472,402,1346,401,1347,400,473,402,472,402,472,402,473,402,471,402,473,402,472,401,474,401,473,401,1372,377,1347,402,1370,377,1369,378,1371,377,471,403,1370,378,1370,377,473,402,472,402,472,402,471,402,472,403,471,403,472,402,472,402,473,402,473,401,473,401,473,401,473,401,473,401,473,401,474,401,474,401,474,400,1371,377,1369,376,1373,376,474,400,474,400,475,378,521,375,474,401,497,376,475,399,475,400,498,375,1374,351,1396,376,1372,375,499,375,498,377,497,375,499,352,522,352,524,363,511,351,523,352,523,351,523,352,522,375,500,351,522,352,523,351,523,353,523,351,1397,350,523,352,523,352,1397,350,523,351,524,350,525,350,1399,349,524,351,524,350,524,350,523,351,524,351,524,350,524,350,525,351,524,350,1399,348,524,350,525,349,524,350,525,351,524,349,526,349,1424,324,1424,323,526,348,1425,323,1426,322,551,323,1425,323,1426,322,0
};

const uint16_t panasonic_signal_cool_26[] = {
   3528,1714,472,400,498,1249,474,400,474,398,498,376,473,400,497,377,471,403,471,401,473,401,471,401,473,401,471,402,471,1277,438,434,470,403,467,407,441,432,465,408,440,434,440,432,465,1282,446,1301,439,1308,447,426,463,410,439,1307,439,433,464,409,439,435,463,411,438,434,463,410,439,435,438,434,461,412,439,435,438,435,439,434,462,413,437,435,439,434,438,435,438,435,438,435,437,437,437,437,437,436,436,437,436,437,436,437,435,438,435,439,435,438,435,437,435,440,433,441,432,1334,413,1334,410,442,425,449,423,472,403,469,404,470,404,10037,3493,1749,402,471,403,1345,403,470,403,471,402,472,403,471,404,471,403,473,402,471,402,471,404,470,404,470,403,471,403,1345,402,472,402,472,403,471,402,472,402,472,402,471,403,471,403,1344,403,1345,402,1345,402,471,403,471,403,1345,401,472,402,472,403,470,403,472,403,472,403,470,403,471,403,471,403,470,404,471,403,471,403,471,403,472,403,1346,401,1346,401,473,401,1347,400,1346,402,1345,413,460,403,471,404,471,426,448,402,1346,401,471,403,1347,401,1346,400,474,401,472,402,472,402,471,403,472,402,472,402,472,402,473,402,472,402,1371,377,1371,376,1371,376,1371,377,1370,377,471,403,1370,377,1370,377,473,402,471,403,473,401,472,402,472,401,473,401,473,401,472,403,473,401,473,402,473,401,472,402,472,401,472,402,473,401,472,403,474,400,472,401,1370,377,1371,376,1371,376,473,401,498,376,474,401,474,400,496,377,473,400,474,401,497,378,497,376,1373,375,1372,375,1373,376,474,398,499,376,498,376,498,373,501,351,522,353,522,359,516,375,499,352,522,375,499,352,521,352,522,352,523,352,522,352,523,352,1395,352,522,352,523,351,1397,350,523,351,523,351,524,351,1397,350,523,351,523,351,523,350,524,350,523,351,524,350,523,351,525,350,523,351,1397,349,523,350,524,350,524,351,523,350,524,350,525,350,1424,323,525,349,1423,324,1424,323,1423,324,526,348,1425,323,1423,323,0
};

const uint16_t panasonic_signal_cool_27[] = {
   3529,1713,471,401,474,1272,499,374,497,375,499,376,473,399,499,375,475,399,498,374,499,374,474,399,472,401,474,400,475,1272,471,401,474,399,474,400,472,401,471,402,472,401,473,399,473,1275,464,1284,460,1286,438,434,471,402,470,1276,468,405,468,405,467,406,467,406,470,405,464,409,440,432,464,410,462,411,464,410,463,409,441,432,465,408,464,411,439,433,441,433,439,433,464,410,439,434,462,411,439,434,462,412,462,410,463,410,439,435,438,435,438,434,440,434,439,435,438,435,438,1309,437,1310,435,437,436,436,438,436,437,436,437,436,437,10019,3505,1739,414,439,433,1335,413,439,434,440,432,442,426,470,404,471,403,472,403,470,403,471,404,470,403,470,403,471,403,1344,403,471,403,472,403,471,403,470,403,471,403,470,404,470,403,1345,401,1346,403,1344,403,471,403,472,402,1344,403,470,403,471,403,472,402,472,403,471,403,471,403,472,402,471,402,471,404,470,403,471,403,472,402,473,403,1344,402,1345,402,471,402,1346,402,1346,402,1345,402,471,402,473,403,471,403,1345,402,1345,401,472,401,1347,401,1346,401,472,403,471,403,471,402,473,401,473,402,472,402,472,402,472,403,472,402,1347,400,1348,400,1346,401,1346,401,1345,402,472,402,1347,400,1347,401,473,401,473,402,471,402,471,403,472,402,472,402,472,402,472,402,473,402,472,401,472,402,472,402,472,402,472,402,472,401,473,402,474,401,472,402,1369,377,1370,377,1370,377,473,401,473,401,474,401,473,402,471,402,472,402,471,404,472,401,472,402,1371,377,1371,376,1370,377,472,402,472,402,472,400,498,376,473,402,474,401,497,376,475,401,473,400,473,401,497,376,474,400,474,400,474,401,498,376,498,377,1372,375,497,377,499,375,1373,375,498,376,499,376,498,362,1387,360,514,374,499,352,522,375,498,352,523,375,499,351,523,375,501,351,521,352,1397,350,522,352,523,351,523,350,524,351,524,350,524,351,1399,349,1398,349,1397,351,1398,348,1423,325,523,351,1398,348,1426,322,0
};

const uint16_t panasonic_signal_cool_28[] = {
   3529,1714,466,406,476,1271,494,353,500,398,476,398,475,398,475,400,499,374,474,399,473,401,472,402,470,402,474,399,497,1251,463,410,471,403,469,404,470,402,471,403,468,405,467,407,463,1283,437,1310,438,1308,439,435,439,433,465,1282,465,408,440,433,441,433,463,410,439,434,440,434,439,434,439,434,439,434,440,434,438,435,438,436,437,437,438,435,461,412,437,436,438,435,438,435,461,413,437,436,438,435,438,436,437,435,438,436,460,413,437,437,436,436,437,438,435,439,435,439,435,1332,413,1333,412,440,433,441,433,440,433,442,431,464,404,10036,3495,1750,403,470,403,1345,402,471,403,470,404,471,402,472,403,471,403,472,403,471,403,471,404,470,403,471,403,471,403,1344,403,471,403,472,403,471,403,472,402,471,403,470,404,471,403,1344,404,1344,402,1347,401,471,403,471,403,1344,403,471,402,472,403,471,403,471,403,472,402,472,403,471,402,471,404,471,403,470,403,472,403,471,403,473,402,1346,401,1346,402,471,410,1338,402,1347,399,1348,401,471,403,473,402,471,403,472,402,471,403,1345,402,1347,402,1345,402,473,401,472,402,472,402,472,402,472,402,472,402,472,402,473,403,471,403,1347,400,1346,401,1371,377,1369,377,1347,401,472,401,1348,399,1348,401,472,402,472,402,472,402,472,402,473,401,471,403,472,401,473,402,472,403,472,402,471,403,471,402,472,402,471,403,473,400,473,403,472,402,472,402,1369,377,1371,377,1370,377,473,400,474,401,474,401,473,401,473,400,474,400,474,401,473,401,473,402,1370,376,1371,377,1371,377,497,376,498,375,499,376,474,390,508,376,499,374,499,376,500,353,520,377,497,377,497,376,497,377,498,352,522,376,498,353,522,375,1373,375,499,351,523,352,1396,351,522,351,524,351,522,353,1398,350,522,352,522,352,522,351,523,351,524,350,524,350,524,351,524,351,523,351,1397,350,524,350,523,351,523,350,525,350,524,350,525,350,1399,348,524,350,524,350,525,350,524,349,1425,324,1423,324,1423,324,0
};

const uint16_t panasonic_signal_cool_29[] = {
   3565,1679,469,403,472,1276,467,405,470,404,470,403,469,404,470,405,465,409,466,407,464,409,440,432,442,432,464,410,463,1284,440,432,441,434,439,434,440,433,440,434,462,410,441,433,463,1284,462,1284,439,1310,460,412,438,435,439,1308,437,435,461,411,463,412,461,411,439,435,438,435,437,437,437,435,438,435,438,436,437,437,436,437,436,438,436,437,436,437,436,437,435,439,435,438,435,439,434,440,433,441,432,441,433,441,431,463,404,449,424,470,403,470,404,469,404,470,404,470,404,1343,403,1343,403,470,404,469,403,470,404,470,404,470,403,10039,3494,1750,403,471,402,1346,402,470,403,471,402,472,403,471,403,471,404,471,404,470,404,471,403,471,402,471,404,471,402,1346,402,471,403,473,402,471,402,473,402,470,404,471,402,471,404,1345,401,1346,402,1346,401,471,403,472,402,1346,401,472,402,472,402,471,403,472,403,472,402,472,402,472,402,472,402,471,403,471,403,472,402,471,403,473,402,1346,425,1322,402,473,401,1346,401,1371,376,1347,400,472,403,472,402,472,403,1346,400,471,403,1346,400,1348,401,1370,377,472,404,471,402,471,403,473,401,473,402,471,403,472,402,472,403,472,402,1370,377,1370,377,1370,378,1370,377,1370,377,472,402,1371,377,1370,377,474,401,473,401,496,378,496,377,473,400,474,401,474,400,474,400,474,402,472,402,497,376,473,400,474,399,475,400,475,400,498,375,475,377,497,399,1373,375,1372,376,1371,375,499,375,500,351,522,377,498,375,498,353,521,353,522,376,498,352,522,352,1397,350,1398,350,1397,351,523,351,523,350,523,351,523,351,522,352,523,352,523,351,523,352,523,350,523,352,521,352,523,351,523,351,524,350,523,350,525,352,1397,349,525,349,525,349,1399,349,525,349,526,349,524,350,1424,323,525,349,526,348,526,348,526,348,523,351,550,324,551,323,526,349,525,349,1425,321,550,324,551,323,525,349,551,322,552,323,553,322,1425,323,1452,294,553,321,553,322,579,290,1430,297,1478,295,1480,245,0
};

const uint16_t panasonic_signal_cool_30[] = {
   3551,1694,462,410,438,1309,462,410,461,413,462,412,437,436,460,413,459,416,460,413,460,413,437,436,459,414,437,436,437,1310,436,438,436,438,434,438,437,437,458,415,436,437,436,438,435,1312,437,1310,434,1313,459,414,435,437,436,1311,435,438,435,438,434,439,436,438,435,439,433,439,435,439,434,438,435,439,435,438,434,439,435,439,434,440,433,440,434,439,434,440,434,439,434,439,434,440,433,440,433,441,432,441,433,440,455,418,434,439,426,448,425,448,425,449,425,449,425,447,426,1342,405,1342,403,448,425,449,425,447,426,449,424,472,403,10041,3490,1751,401,471,403,1342,405,471,402,473,402,472,402,472,401,473,403,472,402,471,404,471,402,472,402,472,403,470,403,1346,399,474,402,473,402,471,403,472,402,471,402,472,401,472,403,1346,401,1347,412,1337,398,473,401,473,401,1347,401,473,400,474,401,473,401,473,401,473,402,471,402,473,401,473,401,473,402,471,402,473,425,449,402,474,401,1349,421,1327,398,474,401,1348,398,1370,378,1347,400,473,401,474,401,473,401,473,401,1348,400,1369,378,1369,378,1370,378,473,401,474,401,472,402,472,400,475,401,472,401,473,401,474,401,474,400,1371,378,1370,377,1371,376,1372,376,1371,376,473,401,1371,376,1371,376,476,399,473,402,473,401,472,401,474,400,474,400,474,400,474,401,474,400,474,400,474,400,474,400,473,401,473,401,474,400,474,400,475,399,474,401,1371,374,1373,375,1372,375,499,375,499,374,500,375,476,398,500,374,475,400,474,400,498,376,475,389,1383,350,1399,372,1376,348,524,350,525,349,523,350,525,350,524,373,501,351,523,351,524,352,521,375,500,374,500,350,523,374,501,350,524,375,499,351,525,350,1423,324,524,349,524,351,1401,347,524,349,526,349,525,349,1425,321,527,349,526,348,524,349,525,349,525,349,526,349,525,348,527,349,524,349,1424,323,550,324,526,348,526,348,526,347,552,323,553,322,1427,320,551,323,1426,321,552,323,552,321,1428,319,1454,294,1507,241,0
};