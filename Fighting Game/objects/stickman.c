#include "engine.h"

anim_mesh_t stickman = &(struct anim_mesh) {
.size = 120,
.verts = (struct anim_vert[]) {
{-0.054774411022663116, 0.3335118889808655, 2, 1, 255},
{-0.0023147165775299072, -0.022192716598510742, 6, 5, 127},
{-0.04198623076081276, 0.2823145389556885, 2, 1, 255},
{-0.0025566667318344116, 0.02201753854751587, 6, 5, 128},
{0.14695605635643005, -0.020871996879577637, 6, 5, 254},
{0.14657363295555115, 0.028757154941558838, 6, 5, 255},
{0.05146876722574234, 0.3335118889808655, 2, 1, 255},
{-0.016034871339797974, -0.019498109817504883, 4, 3, 63},
{-0.016056209802627563, 0.025008738040924072, 4, 3, 64},
{0.01378630381077528, 0.35819387435913086, 2, 1, 255},
{0.06036848574876785, 0.35819387435913086, 2, 1, 255},
{0.06036848574876785, 0.4754335880279541, 2, 1, 255},
{0.011384010314941406, 0.02512073516845703, 4, 3, 191},
{0.1425212323665619, -0.017015576362609863, 4, 3, 255},
{0.14243009686470032, 0.027195334434509277, 4, 3, 255},
{0.015424512326717377, 0.00020587444305419922, 14, 13, 128},
{-0.01559504121541977, 0.17446064949035645, 14, 13, 255},
{-0.014556817710399628, -0.0003502368927001953, 14, 13, 128},
{0.01378630381077528, 0.3335118889808655, 2, 1, 255},
{-0.017091944813728333, 0.3335118889808655, 2, 1, 255},
{-0.04198623076081276, 0.2823145389556885, 2, 1, 255},
{0.03868058696389198, 0.038074344396591187, 1, 0, 255},
{-0.04198623076081276, 0.13408765196800232, 1, 0, 254},
{-0.04198623076081276, 0.038074344396591187, 1, 0, 255},
{0.02902720496058464, -0.01270267367362976, 10, 9, 128},
{-0.04198623076081276, 0.038074344396591187, 1, 0, 255},
{-0.024605870246887207, 0.01219874620437622, 10, 9, 128},
{0.03868058696389198, 0.038074344396591187, 1, 0, 255},
{-0.02902720496058464, -0.01270267367362976, 11, 8, 128},
{0.024605870246887207, 0.01219874620437622, 11, 8, 128},
{-0.016056209802627563, 0.025008738040924072, 4, 3, 64},
{0.011908292770385742, -0.019086718559265137, 4, 3, 191},
{0.011384010314941406, 0.02512073516845703, 4, 3, 191},
{-0.04203207790851593, -0.15804213285446167, 10, 9, 255},
{-0.024605870246887207, 0.01219874620437622, 10, 9, 128},
{-0.09400954842567444, -0.15804213285446167, 10, 9, 255},
{0.024605870246887207, 0.01219874620437622, 11, 8, 128},
{0.04203207790851593, -0.15804213285446167, 11, 8, 255},
{0.09400954842567444, -0.15804213285446167, 11, 8, 255},
{0.013154201209545135, -0.0005583763122558594, 7, 6, 255},
{-0.01385197788476944, -0.0013960599899291992, 12, 7, 128},
{-0.01682700216770172, -0.00111466646194458, 7, 6, 255},
{0.0161292627453804, -0.0008398294448852539, 12, 7, 128},
{-0.013518132269382477, 0.0004991292953491211, 13, 12, 127},
{-0.01385197788476944, -0.0013960599899291992, 12, 7, 128},
{0.016463153064250946, 0.0010553598403930664, 13, 12, 128},
{-0.014556817710399628, -0.0003502368927001953, 14, 13, 128},
{-0.013518132269382477, 0.0004991292953491211, 13, 12, 127},
{-0.054774411022663116, 0.3335118889808655, 2, 1, 255},
{-0.0025566667318344116, 0.02201753854751587, 6, 5, 128},
{-0.0023147165775299072, -0.022192716598510742, 6, 5, 127},
{-0.0025566667318344116, 0.02201753854751587, 6, 5, 128},
{-0.0023147165775299072, -0.022192716598510742, 6, 5, 127},
{0.14695605635643005, -0.020871996879577637, 6, 5, 254},
{0.05146876722574234, 0.3335118889808655, 2, 1, 255},
{0.03868058696389198, 0.2823145389556885, 2, 1, 255},
{-0.016034871339797974, -0.019498109817504883, 4, 3, 63},
{0.06036848574876785, 0.4754335880279541, 2, 1, 255},
{-0.06367412954568863, 0.4754335880279541, 2, 1, 255},
{-0.017091944813728333, 0.35819387435913086, 2, 1, 255},
{-0.017091944813728333, 0.35819387435913086, 2, 1, 255},
{0.01378630381077528, 0.35819387435913086, 2, 1, 255},
{0.06036848574876785, 0.4754335880279541, 2, 1, 255},
{-0.06367412954568863, 0.4754335880279541, 2, 1, 255},
{-0.06367412954568863, 0.35819387435913086, 2, 1, 255},
{-0.017091944813728333, 0.35819387435913086, 2, 1, 255},
{0.011384010314941406, 0.02512073516845703, 4, 3, 191},
{0.011908292770385742, -0.019086718559265137, 4, 3, 191},
{0.1425212323665619, -0.017015576362609863, 4, 3, 255},
{0.015424512326717377, 0.00020587444305419922, 14, 13, 128},
{0.014386333525180817, 0.17501676082611084, 14, 13, 255},
{-0.01559504121541977, 0.17446064949035645, 14, 13, 255},
{-0.04198623076081276, 0.2823145389556885, 2, 1, 255},
{-0.04198623076081276, 0.13408765196800232, 1, 0, 254},
{0.03868058696389198, 0.13408765196800232, 1, 0, 255},
{0.03868058696389198, 0.2823145389556885, 2, 1, 255},
{0.05146876722574234, 0.3335118889808655, 2, 1, 255},
{0.01378630381077528, 0.3335118889808655, 2, 1, 255},
{-0.04198623076081276, 0.2823145389556885, 2, 1, 255},
{0.03868058696389198, 0.13408765196800232, 1, 0, 255},
{0.03868058696389198, 0.2823145389556885, 2, 1, 255},
{-0.017091944813728333, 0.3335118889808655, 2, 1, 255},
{-0.054774411022663116, 0.3335118889808655, 2, 1, 255},
{-0.04198623076081276, 0.2823145389556885, 2, 1, 255},
{0.01378630381077528, 0.35819387435913086, 2, 1, 255},
{-0.017091944813728333, 0.35819387435913086, 2, 1, 255},
{-0.017091944813728333, 0.3335118889808655, 2, 1, 255},
{-0.04198623076081276, 0.2823145389556885, 2, 1, 255},
{0.03868058696389198, 0.2823145389556885, 2, 1, 255},
{0.01378630381077528, 0.3335118889808655, 2, 1, 255},
{0.01378630381077528, 0.3335118889808655, 2, 1, 255},
{0.01378630381077528, 0.35819387435913086, 2, 1, 255},
{-0.017091944813728333, 0.3335118889808655, 2, 1, 255},
{0.03868058696389198, 0.038074344396591187, 1, 0, 255},
{0.03868058696389198, 0.13408765196800232, 1, 0, 255},
{-0.04198623076081276, 0.13408765196800232, 1, 0, 254},
{0.02902720496058464, -0.01270267367362976, 10, 9, 128},
{0.03868058696389198, 0.038074344396591187, 1, 0, 255},
{-0.04198623076081276, 0.038074344396591187, 1, 0, 255},
{0.03868058696389198, 0.038074344396591187, 1, 0, 255},
{-0.04198623076081276, 0.038074344396591187, 1, 0, 255},
{-0.02902720496058464, -0.01270267367362976, 11, 8, 128},
{-0.016056209802627563, 0.025008738040924072, 4, 3, 64},
{-0.016034871339797974, -0.019498109817504883, 4, 3, 63},
{0.011908292770385742, -0.019086718559265137, 4, 3, 191},
{-0.04203207790851593, -0.15804213285446167, 10, 9, 255},
{0.02902720496058464, -0.01270267367362976, 10, 9, 128},
{-0.024605870246887207, 0.01219874620437622, 10, 9, 128},
{0.024605870246887207, 0.01219874620437622, 11, 8, 128},
{-0.02902720496058464, -0.01270267367362976, 11, 8, 128},
{0.04203207790851593, -0.15804213285446167, 11, 8, 255},
{0.013154201209545135, -0.0005583763122558594, 7, 6, 255},
{0.0161292627453804, -0.0008398294448852539, 12, 7, 128},
{-0.01385197788476944, -0.0013960599899291992, 12, 7, 128},
{0.0161292627453804, -0.0008398294448852539, 12, 7, 128},
{0.016463153064250946, 0.0010553598403930664, 13, 12, 128},
{-0.013518132269382477, 0.0004991292953491211, 13, 12, 127},
{0.016463153064250946, 0.0010553598403930664, 13, 12, 128},
{0.015424512326717377, 0.00020587444305419922, 14, 13, 128},
{-0.014556817710399628, -0.0003502368927001953, 14, 13, 128},
}};

animation_t stickman_swing = &(struct animation){
.length = 7,
.frames = (struct bone[][MAX_BONES]) {
{
{0.0, 0.0, 0.0, 1.0},
{0.0016528209671378136, 0.3150690495967865, 0.0, 1.0},
{0.0016528209671378136, 0.4485175609588623, 0.0, 1.0},
{0.04793080314993858, 0.7526300549507141, 0.17814576625823975, 1.0},
{0.19688677787780762, 0.7250593304634094, -1.2115702629089355, 1.0},
{-0.04462515935301781, 0.7559356093406677, -0.5047715306282043, 0.9999999403953552},
{-0.1833069771528244, 0.6754829287528992, 0.9021034836769104, 0.9998745918273926},
{-0.11036578565835953, 0.5759965181350708, 0.9021034836769104, 0.9999970197677612},
{0.030291132628917694, 0.3128959536552429, 0.0, 0.9999999403953552},
{-0.030291132628917694, 0.3128959536552429, 0.0, 0.9999999403953552},
{-0.09029312431812286, 0.15804216265678406, 0.0, 0.9999999403953552},
{0.09029312431812286, 0.15804216265678406, 0.0, 0.9999999403953552},
{0.024548329412937164, 0.6877076029777527, 0.9021034836769104, 0.9999975562095642},
{0.1593915820121765, 0.7959970235824585, 0.9021034836769104, 0.9999978542327881},
{0.2972395718097687, 0.9049115180969238, 0.9021034836769104, 0.999997615814209},
},
{
{0.0, 0.0, 0.0, 1.0},
{0.0016528209671378136, 0.3150690495967865, 0.0, 1.0},
{0.0016528209671378136, 0.4485175609588623, -0.07045559585094452, 1.0619221925735474},
{0.025081612169742584, 0.7739179730415344, 0.054566457867622375, 1.0},
{0.17630010843276978, 0.7649185657501221, -1.3324974030256271, 0.9999999403953552},
{-0.06749183684587479, 0.7709038257598877, -0.5192146450281143, 1.0},
{-0.20499727129936218, 0.6884565949440002, 0.8296943753957748, 0.9998776316642761},
{-0.12504896521568298, 0.5945082902908325, 0.8305515320971608, 0.9999945759773254},
{0.030291132628917694, 0.3128959536552429, 0.0, 0.9999999403953552},
{-0.030291132628917694, 0.3128959536552429, 0.0, 0.9999999403953552},
{-0.09029312431812286, 0.15804216265678406, 0.0, 0.9999999403953552},
{0.09029312431812286, 0.15804216265678406, 0.0, 0.9999999403953552},
{0.0015341341495513916, 0.7155774831771851, 0.8305515320971608, 0.9999953508377075},
{0.12829120457172394, 0.8332287669181824, 0.8305515320971608, 0.999995768070221},
{0.2580006420612335, 0.951718270778656, 0.8305515320971608, 0.9999954104423523},
},
{
{0.0, 0.0, 0.0, 1.0},
{0.0016528209671378136, 0.3150690495967865, 0.0, 1.0},
{0.0016528209671378136, 0.4485175609588623, -0.0801132470369339, 1.0527737140655518},
{0.022160612046718597, 0.7713558673858643, -0.11302442848682404, 1.0000001192092896},
{0.17276164889335632, 0.7877069711685181, -1.4897802621126175, 1.0000001192092896},
{-0.07037699222564697, 0.7674176692962646, -0.4454694241285324, 1.0},
{-0.21358326077461243, 0.6953257322311401, 0.8153014928102493, 0.9998837113380432},
{-0.13228976726531982, 0.6025382280349731, 0.8153014928102493, 0.9999889731407166},
{0.030291132628917694, 0.3128959536552429, 0.0, 0.9999999403953552},
{-0.030291132628917694, 0.3128959536552429, 0.0, 0.9999999403953552},
{-0.09029312431812286, 0.15804216265678406, 0.0, 0.9999999403953552},
{0.09029312431812286, 0.15804216265678406, 0.0, 0.9999999403953552},
{-0.007567062973976135, 0.7255217432975769, 0.7701479159295559, 0.9999867677688599},
{0.11185745894908905, 0.8506075143814087, 0.7460098788142204, 0.9999854564666748},
{0.23109933733940125, 0.9796231985092163, 0.7225358858704567, 0.9999828934669495},
},
{
{0.0, 0.0, 0.0, 1.0},
{0.0016528209671378136, 0.3150690495967865, 0.0, 1.0},
{0.0016528209671378136, 0.4485175609588623, -0.00215284526348114, 1.0270051956176758},
{0.04725830629467964, 0.7609415650367737, 0.06251130998134613, 1.0},
{0.19840054214000702, 0.7507410645484924, -1.2877440601587296, 1.0},
{-0.04530474543571472, 0.7641371488571167, -0.3242553025484085, 0.9999999403953552},
{-0.1961774080991745, 0.7098903059959412, 0.8868033140897751, 0.9998879432678223},
{-0.12171966582536697, 0.6115323305130005, 1.0460214763879776, 0.9999954700469971},
{0.030291132628917694, 0.3128959536552429, 0.0, 0.9999999403953552},
{-0.030291132628917694, 0.3128959536552429, 0.0, 0.9999999403953552},
{-0.09029312431812286, 0.15804216265678406, 0.0, 0.9999999403953552},
{0.09029312431812286, 0.15804216265678406, 0.0, 0.9999999403953552},
{0.027821652591228485, 0.7027381062507629, 0.8763760328292847, 0.99998539686203},
{0.15983563661575317, 0.814456045627594, 0.7856875732541084, 0.9999780058860779},
{0.28410136699676514, 0.9386383295059204, 0.6974939778447151, 0.9999680519104004},
},
{
{0.0, 0.0, 0.0, 1.0},
{0.0016528209671378136, 0.3150690495967865, 0.0, 1.0},
{0.0016528209671378136, 0.4485175609588623, 0.051499247550964355, 0.993918776512146},
{0.06342888623476028, 0.7479977607727051, 0.20475605130195618, 1.0},
{0.21159854531288147, 0.7164735198020935, -1.0403535068035126, 1.0},
{-0.0288352370262146, 0.7560432553291321, -0.2387944757938385, 0.9999999403953552},
{-0.18378762900829315, 0.714872419834137, 0.9318214356899261, 0.9998916387557983},
{-0.11383107304573059, 0.6132632493972778, 1.271619588136673, 0.9999997615814209},
{0.030291132628917694, 0.3128959536552429, 0.0, 0.9999999403953552},
{-0.030291132628917694, 0.3128959536552429, 0.0, 0.9999999403953552},
{-0.09029312431812286, 0.15804216265678406, 0.0, 0.9999999403953552},
{0.09029312431812286, 0.15804216265678406, 0.0, 0.9999999403953552},
{0.05232243239879608, 0.6687102317810059, 0.9722264111042023, 0.9999863505363464},
{0.19442228972911835, 0.7672813534736633, 0.8121778815984726, 0.9999720454216003},
{0.32193347811698914, 0.8881272673606873, 0.6565323323011398, 0.9999529719352722},
},
{
{0.0, 0.0, 0.0, 1.0},
{0.0016528209671378136, 0.3150690495967865, 0.0, 1.0},
{0.0016528209671378136, 0.4485175609588623, 0.07790061831474304, 0.9681503176689148},
{0.07070327550172806, 0.7384498119354248, 0.7474379241466522, 1.0},
{0.1813047230243683, 0.6349348425865173, 0.07436659932136536, 1.0000001192092896},
{-0.021322935819625854, 0.748843252658844, -0.1961275339126587, 0.9999999403953552},
{-0.17789040505886078, 0.7143192291259766, 0.951167106628418, 0.9998940229415894},
{-0.10991214215755463, 0.6113757491111755, 1.7178147435188293, 0.9999842047691345},
{0.030291132628917694, 0.3128959536552429, 0.0, 0.9999999403953552},
{-0.030291132628917694, 0.3128959536552429, 0.0, 0.9999999403953552},
{-0.09029312431812286, 0.15804216265678406, 0.0, 0.9999999403953552},
{0.09029312431812286, 0.15804216265678406, 0.0, 0.9999999403953552},
{0.0638999491930008, 0.589701771736145, 1.4729966074228287, 0.9999979138374329},
{0.2358769029378891, 0.6079571843147278, 1.3421225994825363, 1.0},
{0.4069865345954895, 0.647782564163208, 1.214848980307579, 0.9999978542327881},
},
{
{0.0, 0.0, 0.0, 1.0},
{0.0016528209671378136, 0.3150690495967865, 0.0, 1.0},
{0.0016528209671378136, 0.4485175609588623, 0.08549259603023529, 0.9590017795562744},
{0.07266485691070557, 0.7351452112197876, 1.0175627917051315, 1.0},
{0.15163247287273407, 0.6058697700500488, 1.8759120553731918, 1.0000001192092896},
{-0.019282378256320953, 0.7462068796157837, -0.18451951444149017, 0.9999998807907104},
{-0.17624002695083618, 0.7135025858879089, 0.956285372376442, 0.9998944997787476},
{-0.10878945142030716, 0.6102125644683838, 2.1655187755823135, 0.9999342560768127},
{0.030291132628917694, 0.3128959536552429, 0.0, 0.9999999403953552},
{-0.030291132628917694, 0.3128959536552429, 0.0, 0.9999999403953552},
{-0.09029312431812286, 0.15804216265678406, 0.0, 0.9999999403953552},
{0.09029312431812286, 0.15804216265678406, 0.0, 0.9999999403953552},
{0.038504458963871, 0.5154399871826172, 2.1655187755823135, 0.9999323487281799},
{0.18251538276672363, 0.41969940066337585, 2.1655187755823135, 0.9999313354492188},
{0.3280310034751892, 0.32128432393074036, 2.1655187755823135, 0.9999323487281799},
},
{
{0.0, 0.0, 0.0, 1.0},
{0.0016528209671378136, 0.3150690495967865, 0.0, 1.0},
{0.0016528209671378136, 0.4485175609588623, 0.0, 1.0},
{0.04793080314993858, 0.7526300549507141, 0.9320701956748962, 0.9999999403953552},
{0.13764862716197968, 0.6305696964263916, 1.7904194593429565, 1.0000001192092896},
{-0.04462515935301781, 0.7559356093406677, -0.27001211047172546, 1.0},
{-0.1982170045375824, 0.7099483609199524, 0.8707927763462067, 0.9998946189880371},
{-0.12219297885894775, 0.6127950549125671, 2.0800261795520782, 0.9999343752861023},
{0.030291132628917694, 0.3128959536552429, 0.0, 0.9999999403953552},
{-0.030291132628917694, 0.3128959536552429, 0.0, 0.9999999403953552},
{-0.09029312431812286, 0.15804216265678406, 0.0, 0.9999999403953552},
{0.09029312431812286, 0.15804216265678406, 0.0, 0.9999999403953552},
{0.03265547752380371, 0.5309457778930664, 2.0800261795520782, 0.9999324083328247},
{0.18431560695171356, 0.4478517472743988, 2.0800261795520782, 0.9999314546585083},
{0.33770328760147095, 0.36222144961357117, 2.0800261795520782, 0.9999325275421143},
},
}};

animation_t stickman_top = &(struct animation){
.length = 1,
.frames = (struct bone[][MAX_BONES]) {
{
{0.0, 0.0, 0.0, 1.0},
{0.0016528209671378136, 0.3150690495967865, 0.0, 1.0},
{0.0016528209671378136, 0.4485175609588623, 0.0, 1.0},
{0.04793080314993858, 0.7526300549507141, 0.17814576625823975, 1.0},
{0.19688677787780762, 0.7250593304634094, -1.2115702629089355, 1.0},
{-0.04462515935301781, 0.7559356093406677, -0.5047715306282043, 0.9999999403953552},
{-0.1833069771528244, 0.6754829287528992, 0.9021034836769104, 0.9998745918273926},
{-0.11036578565835953, 0.5759965181350708, 0.9021034836769104, 0.9999970197677612},
{0.030291132628917694, 0.3128959536552429, 0.0, 0.9999999403953552},
{-0.030291132628917694, 0.3128959536552429, 0.0, 0.9999999403953552},
{-0.09029312431812286, 0.15804216265678406, 0.0, 0.9999999403953552},
{0.09029312431812286, 0.15804216265678406, 0.0, 0.9999999403953552},
{0.024548329412937164, 0.6877076029777527, 0.9021034836769104, 0.9999975562095642},
{0.1593915820121765, 0.7959970235824585, 0.9021034836769104, 0.9999978542327881},
{0.2972395718097687, 0.9049115180969238, 0.9021034836769104, 0.999997615814209},
},
{
{0.0, 0.0, 0.0, 1.0},
{0.0016528209671378136, 0.3150690495967865, 0.0, 1.0},
{0.0016528209671378136, 0.4485175609588623, 0.0, 1.0},
{0.04793080314993858, 0.7526300549507141, 0.17814576625823975, 1.0},
{0.19688677787780762, 0.7250593304634094, -1.2115702629089355, 1.0},
{-0.04462515935301781, 0.7559356093406677, -0.5047715306282043, 0.9999999403953552},
{-0.1833069771528244, 0.6754829287528992, 0.9021034836769104, 0.9998745918273926},
{-0.11036578565835953, 0.5759965181350708, 0.9021034836769104, 0.9999970197677612},
{0.030291132628917694, 0.3128959536552429, 0.0, 0.9999999403953552},
{-0.030291132628917694, 0.3128959536552429, 0.0, 0.9999999403953552},
{-0.09029312431812286, 0.15804216265678406, 0.0, 0.9999999403953552},
{0.09029312431812286, 0.15804216265678406, 0.0, 0.9999999403953552},
{0.024548329412937164, 0.6877076029777527, 0.9021034836769104, 0.9999975562095642},
{0.1593915820121765, 0.7959970235824585, 0.9021034836769104, 0.9999978542327881},
{0.2972395718097687, 0.9049115180969238, 0.9021034836769104, 0.999997615814209},
},
}};

