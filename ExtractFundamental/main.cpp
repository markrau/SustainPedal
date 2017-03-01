//main function to test custom C++ classes

#include <stdio.h>
#include <iostream>
#include <fstream>
#include "ExtractFundamental.h"
#include "ExtractFundamental.cpp"

using namespace std;
int main()
{
	int bufLen = 1024;
	//from matlab - contains audio data (Q15 int format) from steady state buffer
	int data[1024] = {920,914,908,899,886,870,849,825,800,772,742,707,
	670,631,588,544,498,450,402,353,301,251,200,150,101,53,8,-35,-74,-111,
	-143,-173,-200,-223,-243,-261,-272,-284,-287,-290,-290,-287,-285,-282,
	-276,-268,-261,-251,-244,-237,-232,-229,-227,-229,-233,-239,-249,-259,
	-271,-283,-299,-318,-336,-355,-374,-394,-413,-429,-444,-455,-465,-470,
	-471,-470,-467,-462,-454,-442,-429,-413,-396,-378,-360,-343,-327,-314,
	-301,-290,-284,-279,-278,-280,-284,-289,-295,-302,-311,-324,-335,-348,
	-359,-365,-371,-375,-374,-369,-362,-349,-334,-312,-290,-265,-236,-206,
	-173,-138,-105,-69,-33,1,35,67,97,128,159,188,218,246,275,304,334,362,
	393,424,459,493,528,562,599,633,669,703,734,765,791,816,835,853,865,
	874,877,878,874,867,856,843,829,812,794,773,756,736,719,700,683,666,
	649,633,617,601,585,568,550,530,510,487,463,435,406,372,337,297,252,
	204,151,93,34,-29,-96,-163,-233,-304,-374,-444,-514,-581,-645,-708,
	-767,-824,-876,-925,-969,-1010,-1050,-1084,-1117,-1148,-1177,-1202,
	-1226,-1247,-1268,-1287,-1301,-1316,-1329,-1340,-1348,-1354,-1356,
	-1357,-1354,-1349,-1340,-1331,-1319,-1305,-1289,-1271,-1249,-1225,
	-1195,-1163,-1127,-1089,-1046,-1001,-952,-900,-845,-789,-729,-667,-602,
	-530,-461,-390,-321,-250,-180,-110,-41,28,94,161,227,289,350,409,464,517,
	567,613,656,699,738,773,804,832,856,877,892,906,916,922,929,933,935,935,935,
	935,932,931,928,927,928,928,929,929,931,934,936,939,941,942,944,942,940,936,
	931,923,913,903,888,872,850,828,804,777,747,712,675,635,593,547,501,451,403,
	352,301,250,201,151,102,55,11,-32,-71,-107,-142,-171,-198,-221,-241,-256,-268,
	-277,-284,-285,-288,-284,-281,-274,-268,-262,-254,-246,-240,-236,-231,-231,-231,
	-233,-234,-239,-246,-258,-272,-290,-308,-326,-345,-365,-384,-403,-420,-434,-447,
	-456,-465,-470,-470,-469,-463,-457,-447,-436,-421,-403,-384,-366,-350,-335,-321,
	-310,-301,-294,-290,-289,-288,-292,-296,-304,-312,-326,-339,-350,-359,-369,-378,
	-384,-388,-390,-389,-383,-374,-361,-345,-326,-301,-276,-249,-220,-187,-155,-122,
	-90,-57,-24,6,37,67,97,125,155,184,211,240,268,298,330,361,395,432,468,504,542,
	579,615,650,684,718,750,778,802,821,835,846,851,855,852,849,842,834,822,807,791,
	773,757,741,724,708,692,677,661,645,631,616,600,587,570,554,537,516,495,471,444,
	414,381,342,302,257,207,152,95,35,-28,-95,-162,-232,-301,-372,-441,-510,-576,
	-640,-700,-758,-811,-860,-906,-950,-992,-1031,-1067,-1101,-1130,-1158,-1182,
	-1206,-1229,-1251,-1272,-1291,-1306,-1318,-1330,-1339,-1345,-1349,-1350,-1348,
	-1343,-1337,-1329,-1317,-1304,-1287,-1267,-1244,-1217,-1188,-1155,-1121,-1083,
	-1041,-997,-948,-897,-842,-783,-723,-660,-594,-526,-458,-389,-317,-247,-177,
	-107,-37,31,97,163,227,290,351,409,463,517,568,615,659,700,738,772,804,830,854,
	873,889,903,916,925,929,932,934,933,934,934,933,932,933,933,933,936,939,943,944,
	948,951,954,956,958,959,958,956,954,949,941,931,919,905,888,868,845,820,790,759,
	724,686,646,602,557,507,459,410,361,310,259,208,158,109,64,18,-24,-64,-102,-137,
	-168,-193,-216,-235,-252,-263,-273,-278,-281,-283,-282,-278,-271,-262,-255,-248,
	-244,-238,-234,-233,-232,-233,-237,-245,-253,-264,-275,-287,-303,-320,-339,-359,
	-378,-395,-413,-429,-445,-456,-465,-469,-471,-471,-468,-462,-453,-441,-429,-411,
	-394,-379,-363,-347,-332,-319,-309,-301,-295,-293,-291,-293,-295,-302,-309,-320,
	-331,-342,-353,-363,-373,-381,-386,-389,-389,-388,-382,-373,-357,-341,-319,-295,-269,-242,
	-211,-182,-151,-120,-89,-58,-28,2,31,60,88,116,143,173,200,230,261,292,325,360,395,433,469,
	508,547,584,621,659,692,725,755,780,803,821,834,842,846,847,846,841,835,828,819,806,790,775,
	759,743,727,710,695,681,668,655,641,629,614,601,584,568,549,529,508,484,458,430,399,360,317,
	270,217,162,102,40,-25,-92,-160,-230,-300,-370,-437,-502,-563,-621,-677,-732,-786,-836,-884,
	-928,-968,-1007,-1043,-1078,-1108,-1140,-1166,-1194,-1218,-1240,-1262,-1279,-1293,-1307,-1316,
	-1324,-1330,-1333,-1333,-1332,-1328,-1322,-1314,-1303,-1291,-1275,-1257,-1235,-1210,-1182,-1151,
	-1116,-1077,-1037,-993,-945,-892,-838,-779,-720,-657,-593,-526,-458,-392,-323,-255,-185,-116,-46,22,89,154,
	218,282,341,401,456,509,558,607,653,696,734,768,796,822,844,863,876,889,898,907,913,917,920,922,922,921,922,
	925,930,935,937,940,943,945,950,955,959,962,965,967,966,966,965,962,957,949,941,928,915,896,875,851,825,796,765,
	730,692,652,609,564,518,468,419,367,318,267,218,169,120,73,28,-12,-53,-88,-121,-151,-177,-199,-219,-235,-248,-258,-264,
	-267,-268,-266,-262,-256,-251,-244,-240,-232,-227,-223,-222,-222,-224,-229,-236,-245,-257,-271,-284,-302,-320,-339,-359,
	-378,-395,-413,-428,-439,-452,-460,-464,-465,-464,-459,-452,-441,-429,-414,-400,-384,-368,-354,-340,-327,-315,-305,-298,
	-293,-290,-292,-296,-302,-311,-318,-326,-337,-349,-362,-373,-384,-392,-395,-398,-397,-392,-383,-371,-355,-336,-314,-290,
	-263,-237,-210,-182,-150,-119,-88,-58,-30,-1,26,53,79};
	
	int data2[1024] = {1014,1022,1025,1030,1035,1037,1040,1042,1043,1041,1038,
	1036,1032,1028,1022,1013,1004,993,981,969,954,938,919,900,877,852,826,798,
	766,734,700,663,624,581,538,493,445,395,344,291,235,179,121,63,2,-60,-122,-185,-247,-310,-376,-439,-504,-569,-635,-699,
	-763,-828,-894,-960,-1024,-1087,-1152,-1216,-1282,-1349,-1417,-1485,-1553,-1619,-1685,-1749,-1815,-1881,-1946,-2013,
	-2077,-2143,-2205,-2267,-2324,-2380,-2433,-2483,-2529,-2571,-2610,-2644,-2670,-2692,-2710,-2718,-2722,-2719,-2709,
	-2692,-2670,-2639,-2604,-2563,-2513,-2458,-2397,-2330,-2260,-2189,-2113,-2034,-1953,-1870,-1783,-1699,-1612,-1528,
	-1442,-1357,-1275,-1193,-1113,-1035,-958,-883,-808,-736,-666,-598,-529,-462,-397,-332,-267,-204,-142,-80,-19,41,99,
	158,215,269,322,373,422,469,514,557,598,634,669,699,727,752,774,792,809,823,838,848,856,863,869,874,878,884,889,894,
	898,905,911,920,928,936,946,958,967,977,987,996,1006,1015,1023,1027,1029,1029,1024,1015,1004,989,972,952,928,899,862,
	822,780,733,683,631,574,517,458,399,340,280,219,157,99,42,-16,-68,-120,-167,-214,-258,-297,-334,-368,-397,-425,-446,
	-466,-481,-494,-503,-511,-515,-516,-515,-510,-505,-498,-487,-474,-458,-439,-418,-396,-370,-343,-313,-278,-244,-207,
	-166,-123,-78,-31,17,66,117,169,219,271,322,374,424,472,517,562,605,643,678,711,739,764,784,800,813,821,825,825,821,
	814,803,787,768,746,722,696,670,640,607,574,541,506,470,434,399,363,330,296,266,235,204,176,149,125,101,79,60,43,29,
	15,5,-2,-8,-11,-9,-7,-2,6,15,28,41,55,69,84,101,120,139,161,182,205,226,250,274,295,318,340,363,384,407,427,449,471,
	491,511,533,552,568,585,600,615,630,644,659,673,686,700,713,727,740,755,767,782,795,809,821,833,847,858,873,885,897,
	910,922,935,947,957,969,980,989,1000,1008,1017,1024,1030,1036,1042,1046,1050,1054,1056,1059,1057,1056,1054,1051,1047,
	1041,1035,1027,1018,1007,994,978,961,942,919,899,875,849,819,790,755,721,682,642,599,556,510,462,412,360,306,251,194,
	136,78,17,-42,-105,-165,-228,-292,-355,-418,-483,-546,-610,-674,-739,-802,-867,-931,-996,-1058,-1123,-1190,-1254,-1322,
	-1390,-1459,-1528,-1596,-1663,-1733,-1800,-1867,-1935,-2002,-2069,-2134,-2199,-2260,-2319,-2376,-2428,-2477,-2524,-2567,
	-2605,-2639,-2665,-2688,-2701,-2710,-2711,-2704,-2691,-2673,-2647,-2614,-2577,-2533,-2483,-2427,-2367,-2304,-2235,-2163,
	-2089,-2012,-1935,-1855,-1774,-1693,-1611,-1530,-1449,-1367,-1288,-1210,-1134,-1059,-986,-914,-843,-768,-696,-626,-555,
	-487,-419,-353,-288,-223,-159,-94,-31,32,92,153,210,265,318,370,418,466,509,550,586,623,654,683,710,731,751,768,783,796,
	806,817,825,832,840,847,853,861,866,874,883,893,904,915,929,944,956,969,982,993,1004,1015,1025,1031,1038,1043,1042,1042,
	1035,1026,1012,995,973,946,916,881,843,804,761,712,662,608,552,494,435,376,317,259,201,143,85,31,-23,-76,-125,-173,-217,
	-256,-290,-324,-355,-384,-410,-434,-455,-473,-487,-501,-511,-516,-519,-520,-518,-514,-507,-497,-486,-470,-453,-432,-409,
	-382,-356,-323,-291,-253,-214,-172,-127,-82,-33,17,67,118,170,223,275,325,375,426,475,521,564,605,643,676,706,730,752,770,
	785,796,802,805,803,798,788,776,761,742,721,697,670,644,612,581,548,515,480,447,413,378,343,310,277,245,214,186,159,135,
	111,87,67,48,34,22,11,4,-2,-5,-4,-6,-2,2,7,14,23,34,48,64,81,99,118,138,161,183,205,226,246,267,288,311,334,355,376,398,
	418,439,460,483,500,520,536,551,565,576,591,609,624,639,654,670,685,699,714,726,740,753,766,778,793,805,819,831,845,857,
	870,883,895,907,919,933,945,958,969,979,991,999,1010,1016,1023,1031,1036,1041,1046,1049,1051,1053,1053,1052,1051,1049,1044,
	1041,1034,1026,1016,1004,992,979,963,944,924,901,877,850,822,790,756,720,682,643,600,555,508,460,409,356,305,249,193,135,75,
	16,-44,-104,-165,-224,-288,-349,-412,-473,-538,-600,-663,-725,-787,-848,-913,-978,-1043,-1110,-1176,-1244,-1313,-1382,-1453,
	-1522,-1592,-1661,-1731,-1802,-1872,-1942,-2012,-2080,-2148,-2214,-2276,-2336,-2394,-2447,-2498,-2544,-2586,-2621,-2650,-2673,
	-2690,-2702,-2708,-2707,-2699,-2686,-2665,-2637,-2604,-2564,-2517,-2466,-2409,-2350,-2285,-2216,-2147,-2076,-2002,-1929,-1853,
	-1776,-1697,-1619,-1543,-1464,-1389,-1315,-1240,-1164,-1089,-1015,-941,-867,-795,-723,-654,-584,-516,-444,-374,-305,-239,-174,
	-108,-43,21,84,146,206,260,315,362,408,452,493,533,570,601,629,655,679,700,719,737,751,762,773,783,792,800,809,818,828,838,850,
	859,870,884,899,914,929,946,961,978,991,1006,1018,1029,1038,1047,1054,1057,1056,1053,1043,1030,1014,993,968,940,907,872,835,790,
	745,694,641,585,528,471,413,355,296,240,185,129,76,21,-29,-79,-126,-169,-211,-250,-286,-320,-349,-378,-404,-428,-448,-465,-481,
	-494,-503,-510,-515,-518,-517,-514,-510,-503,-492,-479,-460,-439,-412,-383,-354,-322,-287,-251,-211,-167,-122,-76,-25,26,79,130,
	183,234,288,339,389,437,483,528,567,606,642,675,701,726,746,763,776,785,792,792,790,783,774,761};
	
	cout << "Testing yin estimator output" << endl;
	long fs = 44100;
	int desired_period = 374;
	ExtractFundamental fund(data2,bufLen,fs);
	int est_period = fund.yin_pitch();
	cout << "Desired period in samples : " << desired_period << endl;
	cout << "Estimated period in samples : " << est_period << endl;
	
	int* output = new int[bufLen];
	output = fund.get_fundamental();
	cout << "Output is :" << endl;
	for(int i = 0; i < bufLen; i++){
		cout << output[i] << endl;
	}

	
	//save to text file so that it can be examined by Matlab.
	//just use dlmread in Matlab to look at this
	ofstream fout("extract_fundamental_result2.txt");
	
	if(fout.is_open()){
		cout << "File opened successfully" << endl;
		for(int i = 0; i < bufLen; i++){
		fout << output[i] << ",";
		}
	}
	else{
		cout << "File could not be opened" << endl;
	}
	
	return 0;
	
}