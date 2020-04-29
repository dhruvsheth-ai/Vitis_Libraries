#!/usr/bin/env python3

# Ensure environmental variables i.e. paths are set to used the modules
from xf_fintech_python import DeviceManager, fdbslv


# State test financial model
print("\nThe fdbslv financial model\n==============================\n")
print("This example follows the C++ Vanilla European test case called case0\n")

# Variables - these are a copy of the configuration parameters and financial data, as stored in csv format in the L2 example
#             at L2\tests\FDBlackScholesLocalVolatilityEngine\data\case0

parameters = [128,256,0.5,50,60,0,189.9999999999999]

xGridList    = [1.609437912434100282e+00, 1.640241243185502906e+00, 1.671044573936905753e+00, 1.701847904688308377e+00, 1.732651235439711224e+00, 1.763454566191113848e+00, 
                1.794257896942516695e+00, 1.825061227693919319e+00, 1.855864558445321943e+00, 1.886667889196724790e+00, 1.917471219948127636e+00, 1.948274550699530261e+00, 
                1.979077881450932885e+00, 2.009881212202335732e+00, 2.040684542953738578e+00, 2.071487873705140981e+00, 2.102291204456543827e+00, 2.133094535207946674e+00, 
                2.163897865959349076e+00, 2.194701196710751923e+00, 2.225504527462154769e+00, 2.256307858213557616e+00, 2.287111188964960018e+00, 2.317914519716362864e+00, 
                2.348717850467765711e+00, 2.379521181219168113e+00, 2.410324511970570960e+00, 2.441127842721973806e+00, 2.471931173473376653e+00, 2.502734504224779055e+00, 
                2.533537834976181902e+00, 2.564341165727584748e+00, 2.595144496478987151e+00, 2.625947827230389997e+00, 2.656751157981792844e+00, 2.687554488733195690e+00, 
                2.718357819484598537e+00, 2.749161150236000939e+00, 2.779964480987403785e+00, 2.810767811738806188e+00, 2.841571142490209034e+00, 2.872374473241611881e+00, 
                2.903177803993014727e+00, 2.933981134744417574e+00, 2.964784465495819976e+00, 2.995587796247222823e+00, 3.026391126998625225e+00, 3.057194457750028072e+00, 
                3.087997788501430918e+00, 3.118801119252833765e+00, 3.149604450004236611e+00, 3.180407780755639013e+00, 3.211211111507041860e+00, 3.242014442258444262e+00, 
                3.272817773009847109e+00, 3.303621103761249955e+00, 3.334424434512652802e+00, 3.365227765264055648e+00, 3.396031096015458051e+00, 3.426834426766860897e+00, 
                3.457637757518263300e+00, 3.488441088269666146e+00, 3.519244419021068992e+00, 3.550047749772471839e+00, 3.580851080523874685e+00, 3.611654411275277532e+00, 
                3.642457742026679490e+00, 3.673261072778082337e+00, 3.704064403529485183e+00, 3.734867734280888030e+00, 3.765671065032290876e+00, 3.796474395783693723e+00, 
                3.827277726535096569e+00, 3.858081057286498528e+00, 3.888884388037901374e+00, 3.919687718789304220e+00, 3.950491049540707067e+00, 3.981294380292109913e+00, 
                4.012097711043512760e+00, 4.042901041794915606e+00, 4.073704372546318453e+00, 4.104507703297720411e+00, 4.135311034049123258e+00, 4.166114364800526104e+00, 
                4.196917695551928951e+00, 4.227721026303331797e+00, 4.258524357054734644e+00, 4.289327687806137490e+00, 4.320131018557539448e+00, 4.350934349308942295e+00, 
                4.381737680060345141e+00, 4.412541010811747988e+00, 4.443344341563150834e+00, 4.474147672314553681e+00, 4.504951003065956527e+00, 4.535754333817358486e+00, 
                4.566557664568761332e+00, 4.597360995320164179e+00, 4.628164326071567025e+00, 4.658967656822969872e+00, 4.689770987574372718e+00, 4.720574318325775565e+00, 
                4.751377649077177523e+00, 4.782180979828580369e+00, 4.812984310579983216e+00, 4.843787641331386062e+00, 4.874590972082788909e+00, 4.905394302834191755e+00, 
                4.936197633585594602e+00, 4.967000964336996560e+00, 4.997804295088399407e+00, 5.028607625839802253e+00, 5.059410956591205100e+00, 5.090214287342607946e+00, 
                5.121017618094010793e+00, 5.151820948845413639e+00, 5.182624279596815597e+00, 5.213427610348218444e+00, 5.244230941099621290e+00, 5.275034271851024137e+00, 
                5.305837602602426983e+00, 5.336640933353829830e+00, 5.367444264105232676e+00, 5.398247594856634635e+00, 5.429050925608037481e+00, 5.459854256359440328e+00, 
                5.490657587110843174e+00, 5.521460917862246021e+00]

tGridList    = [0.000000000000000000e+00, 3.921568627450980338e-03, 7.843137254901960675e-03, 1.176470588235294101e-02, 1.568627450980392135e-02, 1.960784313725490169e-02, 
                2.352941176470588203e-02, 2.745098039215686236e-02, 3.137254901960784270e-02, 3.529411764705882304e-02, 3.921568627450980338e-02, 4.313725490196078372e-02, 
                4.705882352941176405e-02, 5.098039215686274439e-02, 5.490196078431372473e-02, 5.882352941176470507e-02, 6.274509803921568540e-02, 6.666666666666666574e-02, 
                7.058823529411764608e-02, 7.450980392156862642e-02, 7.843137254901960675e-02, 8.235294117647058709e-02, 8.627450980392156743e-02, 9.019607843137254777e-02, 
                9.411764705882352811e-02, 9.803921568627450844e-02, 1.019607843137254888e-01, 1.058823529411764691e-01, 1.098039215686274495e-01, 1.137254901960784298e-01, 
                1.176470588235294101e-01, 1.215686274509803905e-01, 1.254901960784313708e-01, 1.294117647058823373e-01, 1.333333333333333315e-01, 1.372549019607843257e-01, 
                1.411764705882352922e-01, 1.450980392156862586e-01, 1.490196078431372528e-01, 1.529411764705882470e-01, 1.568627450980392135e-01, 1.607843137254901800e-01, 
                1.647058823529411742e-01, 1.686274509803921684e-01, 1.725490196078431349e-01, 1.764705882352941013e-01, 1.803921568627450955e-01, 1.843137254901960898e-01, 
                1.882352941176470562e-01, 1.921568627450980227e-01, 1.960784313725490169e-01, 2.000000000000000111e-01, 2.039215686274509776e-01, 2.078431372549019440e-01, 
                2.117647058823529382e-01, 2.156862745098039325e-01, 2.196078431372548989e-01, 2.235294117647058654e-01, 2.274509803921568596e-01, 2.313725490196078538e-01, 
                2.352941176470588203e-01, 2.392156862745097867e-01, 2.431372549019607809e-01, 2.470588235294117752e-01, 2.509803921568627416e-01, 2.549019607843137081e-01, 
                2.588235294117646745e-01, 2.627450980392156965e-01, 2.666666666666666630e-01, 2.705882352941176294e-01, 2.745098039215686514e-01, 2.784313725490196179e-01, 
                2.823529411764705843e-01, 2.862745098039215508e-01, 2.901960784313725172e-01, 2.941176470588235392e-01, 2.980392156862745057e-01, 3.019607843137254721e-01, 
                3.058823529411764941e-01, 3.098039215686274606e-01, 3.137254901960784270e-01, 3.176470588235293935e-01, 3.215686274509803599e-01, 3.254901960784313819e-01, 
                3.294117647058823484e-01, 3.333333333333333148e-01, 3.372549019607843368e-01, 3.411764705882353033e-01, 3.450980392156862697e-01, 3.490196078431372362e-01, 
                3.529411764705882026e-01, 3.568627450980392246e-01, 3.607843137254901911e-01, 3.647058823529411575e-01, 3.686274509803921795e-01, 3.725490196078431460e-01, 
                3.764705882352941124e-01, 3.803921568627450789e-01, 3.843137254901960453e-01, 3.882352941176470673e-01, 3.921568627450980338e-01, 3.960784313725490002e-01, 
                4.000000000000000222e-01, 4.039215686274509887e-01, 4.078431372549019551e-01, 4.117647058823529216e-01, 4.156862745098038880e-01, 4.196078431372549100e-01, 
                4.235294117647058765e-01, 4.274509803921568429e-01, 4.313725490196078649e-01, 4.352941176470588314e-01, 4.392156862745097978e-01, 4.431372549019607643e-01, 
                4.470588235294117307e-01, 4.509803921568627527e-01, 4.549019607843137192e-01, 4.588235294117646856e-01, 4.627450980392157076e-01, 4.666666666666666741e-01, 
                4.705882352941176405e-01, 4.745098039215686070e-01, 4.784313725490195734e-01, 4.823529411764705954e-01, 4.862745098039215619e-01, 4.901960784313725283e-01, 
                4.941176470588235503e-01, 4.980392156862745168e-01, 5.019607843137254832e-01, 5.058823529411764497e-01, 5.098039215686274161e-01, 5.137254901960783826e-01, 
                5.176470588235293491e-01, 5.215686274509804266e-01, 5.254901960784313930e-01, 5.294117647058823595e-01, 5.333333333333333259e-01, 5.372549019607842924e-01, 
                5.411764705882352589e-01, 5.450980392156862253e-01, 5.490196078431373028e-01, 5.529411764705882693e-01, 5.568627450980392357e-01, 5.607843137254902022e-01, 
                5.647058823529411686e-01, 5.686274509803921351e-01, 5.725490196078431016e-01, 5.764705882352940680e-01, 5.803921568627450345e-01, 5.843137254901961120e-01, 
                5.882352941176470784e-01, 5.921568627450980449e-01, 5.960784313725490113e-01, 5.999999999999999778e-01, 6.039215686274509443e-01, 6.078431372549019107e-01, 
                6.117647058823529882e-01, 6.156862745098039547e-01, 6.196078431372549211e-01, 6.235294117647058876e-01, 6.274509803921568540e-01, 6.313725490196078205e-01, 
                6.352941176470587870e-01, 6.392156862745097534e-01, 6.431372549019607199e-01, 6.470588235294117974e-01, 6.509803921568627638e-01, 6.549019607843137303e-01, 
                6.588235294117646967e-01, 6.627450980392156632e-01, 6.666666666666666297e-01, 6.705882352941175961e-01, 6.745098039215686736e-01, 6.784313725490196401e-01, 
                6.823529411764706065e-01, 6.862745098039215730e-01, 6.901960784313725394e-01, 6.941176470588235059e-01, 6.980392156862744724e-01, 7.019607843137254388e-01, 
                7.058823529411764053e-01, 7.098039215686274828e-01, 7.137254901960784492e-01, 7.176470588235294157e-01, 7.215686274509803821e-01, 7.254901960784313486e-01, 
                7.294117647058823151e-01, 7.333333333333332815e-01, 7.372549019607843590e-01, 7.411764705882353255e-01, 7.450980392156862919e-01, 7.490196078431372584e-01, 
                7.529411764705882248e-01, 7.568627450980391913e-01, 7.607843137254901578e-01, 7.647058823529411242e-01, 7.686274509803920907e-01, 7.725490196078431682e-01, 
                7.764705882352941346e-01, 7.803921568627451011e-01, 7.843137254901960675e-01, 7.882352941176470340e-01, 7.921568627450980005e-01, 7.960784313725489669e-01, 
                8.000000000000000444e-01, 8.039215686274510109e-01, 8.078431372549019773e-01, 8.117647058823529438e-01, 8.156862745098039102e-01, 8.196078431372548767e-01, 
                8.235294117647058432e-01, 8.274509803921568096e-01, 8.313725490196077761e-01, 8.352941176470588536e-01, 8.392156862745098200e-01, 8.431372549019607865e-01, 
                8.470588235294117530e-01, 8.509803921568627194e-01, 8.549019607843136859e-01, 8.588235294117646523e-01, 8.627450980392157298e-01, 8.666666666666666963e-01, 
                8.705882352941176627e-01, 8.745098039215686292e-01, 8.784313725490195957e-01, 8.823529411764705621e-01, 8.862745098039215286e-01, 8.901960784313724950e-01, 
                8.941176470588234615e-01, 8.980392156862745390e-01, 9.019607843137255054e-01, 9.058823529411764719e-01, 9.098039215686274384e-01, 9.137254901960784048e-01, 
                9.176470588235293713e-01, 9.215686274509803377e-01, 9.254901960784314152e-01, 9.294117647058823817e-01, 9.333333333333333481e-01, 9.372549019607843146e-01, 
                9.411764705882352811e-01, 9.450980392156862475e-01, 9.490196078431372140e-01, 9.529411764705881804e-01, 9.568627450980391469e-01, 9.607843137254902244e-01, 
                9.647058823529411908e-01, 9.686274509803921573e-01, 9.725490196078431238e-01, 9.764705882352940902e-01, 9.803921568627450567e-01, 9.843137254901960231e-01, 
                9.882352941176471006e-01, 9.921568627450980671e-01, 9.960784313725490335e-01, 1.000000000000000000e+00 ]

sigmaList    = [2.000000000000000111e-01] * 32768

rateList     = [5.000000000000000278e-02] * 256

initialConditionList = [0.000000000000000000e+00,0.000000000000000000e+00,0.000000000000000000e+00,0.000000000000000000e+00,0.000000000000000000e+00,0.000000000000000000e+00,
                        0.000000000000000000e+00,0.000000000000000000e+00,0.000000000000000000e+00,0.000000000000000000e+00,0.000000000000000000e+00,0.000000000000000000e+00,
                        0.000000000000000000e+00,0.000000000000000000e+00,0.000000000000000000e+00,0.000000000000000000e+00,0.000000000000000000e+00,0.000000000000000000e+00,
                        0.000000000000000000e+00,0.000000000000000000e+00,0.000000000000000000e+00,0.000000000000000000e+00,0.000000000000000000e+00,0.000000000000000000e+00,
                        0.000000000000000000e+00,0.000000000000000000e+00,0.000000000000000000e+00,0.000000000000000000e+00,0.000000000000000000e+00,0.000000000000000000e+00,
                        0.000000000000000000e+00,0.000000000000000000e+00,0.000000000000000000e+00,0.000000000000000000e+00,0.000000000000000000e+00,0.000000000000000000e+00,
                        0.000000000000000000e+00,0.000000000000000000e+00,0.000000000000000000e+00,0.000000000000000000e+00,0.000000000000000000e+00,0.000000000000000000e+00,
                        0.000000000000000000e+00,0.000000000000000000e+00,0.000000000000000000e+00,0.000000000000000000e+00,0.000000000000000000e+00,0.000000000000000000e+00,
                        0.000000000000000000e+00,0.000000000000000000e+00,0.000000000000000000e+00,0.000000000000000000e+00,0.000000000000000000e+00,0.000000000000000000e+00,
                        0.000000000000000000e+00,0.000000000000000000e+00,0.000000000000000000e+00,0.000000000000000000e+00,0.000000000000000000e+00,0.000000000000000000e+00,
                        0.000000000000000000e+00,0.000000000000000000e+00,0.000000000000000000e+00,0.000000000000000000e+00,0.000000000000000000e+00,0.000000000000000000e+00,
                        0.000000000000000000e+00,0.000000000000000000e+00,0.000000000000000000e+00,0.000000000000000000e+00,0.000000000000000000e+00,0.000000000000000000e+00,
                        0.000000000000000000e+00,0.000000000000000000e+00,0.000000000000000000e+00,0.000000000000000000e+00,0.000000000000000000e+00,0.000000000000000000e+00,
                        0.000000000000000000e+00,0.000000000000000000e+00,0.000000000000000000e+00,6.128976718100958010e-01,2.509030484007261919e+00,4.464479378747739702e+00,
                        6.481099921013935727e+00,8.560805722801063666e+00,1.070557025898226300e+01,1.291742873997878860e+01,1.519848004301228173e+01,1.755088870377201715e+01,
                        1.997688697038617534e+01,2.247877692164750840e+01,2.505893265150234583e+01,2.771980252187634619e+01,3.046391148597462006e+01,3.329386348426073994e+01,
                        3.621234391538887110e+01,3.922212218443230825e+01,4.232605433082822799e+01,4.552708573853124108e+01,4.882825393094769595e+01,5.223269145330303331e+01,
                        5.574362884517722705e+01,5.936439770602942190e+01,6.309843385661946513e+01,6.694928059932804842e+01,7.092059208046782715e+01,7.501613675777682033e+01,
                        7.923980097638394682e+01,8.359559265664057648e+01,8.808764509731747694e+01,9.272022089777496490e+01,9.749771600282991812e+01,1.024246638741567494e+02,
                        1.075057397921806910e+02,1.127457652925459399e+02,1.181497127413680914e+02,1.237227100536135538e+02,1.294700455590807451e+02,1.353971730206036455e+02,
                        1.415097168092376876e+02,1.478134772413388589e+02,1.543144360826013326e+02,1.610187622242755481e+02,1.679328175369546159e+02,1.750631629074808018e+02,
                        1.824165644647043791e+02,1.899999999999998863e+02, ]
# Note Python modules have functions for importing from csv files, if desired.

# Extract individual parameters from the parameters list
solverN = parameters[0]
solverM = parameters[1]
solverTheta = parameters[2]
modelS = parameters[3]   # not used in this case 0 example
modelK = parameters[4]   # not used in this case 0 example
boundaryLower = parameters[5]
boundaryUpper = parameters[6]

# Define output as an empty list
outputResultList = []


deviceList = DeviceManager.getDeviceList("u200")
# Identify which cards installed and choose the first available U200 card, alter above value to match your card type
print("Found these {0} device(s):".format(len(deviceList)))
for x in deviceList:
    print(x.getName())
chosenDevice = deviceList[0]
print("Choosing the first, ",str(chosenDevice),"\n")

# Select and claim the chosen card with the financial model to be used
fdbslv = fdbslv(solverN,solverM)
fdbslv.claimDevice(chosenDevice)

# Run the test
result = fdbslv.run(xGridList, tGridList, sigmaList, rateList, initialConditionList, solverTheta, boundaryLower, boundaryUpper, outputResultList)
# Display results
for i in range(len(outputResultList[0])): # Output from code includes an encompassing pair of [] brackets around the list, hence the [0]
    print(outputResultList[0][i])
    i += 1

fdbslv.releaseDevice()





