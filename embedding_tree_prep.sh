# Convert minimcs and MuDsts to TStarJetPicos, following MiniMcsToTStarJetPicos.txt provided by Isaac

cd /gpfs01/star/pwg/prozorov/jets_pp_2012/external/TStarJetPicoMaker/
cd list_20235003
find /star/data105/embedding/pp200_production_2012/ -type f | sort > full.list # different from Isaac!
sed '/minimc/! d' full.list > full_minimcs.list
sed '/MuDst/! d' full.list > full_MuDsts.list

sed '/pt_2_3_/! d' full_minimcs.list > full_minimcs23.list
sed '/pt3_4_/! d' full_minimcs.list > full_minimcs34.list
sed '/pt4_5_/! d' full_minimcs.list > full_minimcs45.list
sed '/pt5_7_/! d' full_minimcs.list > full_minimcs57.list
sed '/pt7_9_/! d' full_minimcs.list > full_minimcs79.list
sed '/pt9_11_/! d' full_minimcs.list > full_minimcs911.list
sed '/pt11_15_/! d' full_minimcs.list > full_minimcs1115.list
sed '/pt_15_20_/! d' full_minimcs.list > full_minimcs1520.list
sed '/pt_20_25_/! d' full_minimcs.list > full_minimcs2025.list # ignore directories of Pythia6_pt20_25*!
sed '/pt_25_35_/! d' full_minimcs.list > full_minimcs2535.list
sed '/pt_35_45_/! d' full_minimcs.list > full_minimcs3545.list
sed '/pt_45_55_/! d' full_minimcs.list > full_minimcs4555.list
sed '/pt_55_infty_/! d' full_minimcs.list > full_minimcs55-1.list

sed '/pt_2_3_/! d' full_MuDsts.list > full_MuDsts23.list
sed '/pt3_4_/! d' full_MuDsts.list > full_MuDsts34.list
sed '/pt4_5_/! d' full_MuDsts.list > full_MuDsts45.list
sed '/pt5_7_/! d' full_MuDsts.list > full_MuDsts57.list
sed '/pt7_9_/! d' full_MuDsts.list > full_MuDsts79.list
sed '/pt9_11_/! d' full_MuDsts.list > full_MuDsts911.list
sed '/pt11_15_/! d' full_MuDsts.list > full_MuDsts1115.list
sed '/pt_15_20_/! d' full_MuDsts.list > full_MuDsts1520.list
sed '/pt_20_25_/! d' full_MuDsts.list > full_MuDsts2025.list
sed '/pt_25_35_/! d' full_MuDsts.list > full_MuDsts2535.list
sed '/pt_35_45_/! d' full_MuDsts.list > full_MuDsts3545.list
sed '/pt_45_55_/! d' full_MuDsts.list > full_MuDsts4555.list
sed '/pt_55_infty_/! d' full_MuDsts.list > full_MuDsts55-1.list

split --suffix-length=3 --numeric-suffixes --lines=100 --additional-suffix=.list --verbose full_minimcs23.list minimcs23_
split --suffix-length=3 --numeric-suffixes --lines=100 --additional-suffix=.list --verbose full_minimcs34.list minimcs34_
split --suffix-length=3 --numeric-suffixes --lines=100 --additional-suffix=.list --verbose full_minimcs45.list minimcs45_
split --suffix-length=3 --numeric-suffixes --lines=100 --additional-suffix=.list --verbose full_minimcs57.list minimcs57_
split --suffix-length=3 --numeric-suffixes --lines=100 --additional-suffix=.list --verbose full_minimcs79.list minimcs79_
split --suffix-length=3 --numeric-suffixes --lines=100 --additional-suffix=.list --verbose full_minimcs911.list minimcs911_
split --suffix-length=3 --numeric-suffixes --lines=100 --additional-suffix=.list --verbose full_minimcs1115.list minimcs1115_
split --suffix-length=3 --numeric-suffixes --lines=100 --additional-suffix=.list --verbose full_minimcs1520.list minimcs1520_
split --suffix-length=3 --numeric-suffixes --lines=100 --additional-suffix=.list --verbose full_minimcs2025.list minimcs2025_
split --suffix-length=3 --numeric-suffixes --lines=100 --additional-suffix=.list --verbose full_minimcs2535.list minimcs2535_
split --suffix-length=3 --numeric-suffixes --lines=100 --additional-suffix=.list --verbose full_minimcs3545.list minimcs3545_
split --suffix-length=3 --numeric-suffixes --lines=100 --additional-suffix=.list --verbose full_minimcs4555.list minimcs4555_
split --suffix-length=3 --numeric-suffixes --lines=100 --additional-suffix=.list --verbose full_minimcs55-1.list minimcs55999_
split --suffix-length=3 --numeric-suffixes --lines=100 --additional-suffix=.list --verbose full_MuDsts55-1.list MuDsts55999_
split --suffix-length=3 --numeric-suffixes --lines=100 --additional-suffix=.list --verbose full_MuDsts4555.list MuDsts4555_
split --suffix-length=3 --numeric-suffixes --lines=100 --additional-suffix=.list --verbose full_MuDsts3545.list MuDsts3545_
split --suffix-length=3 --numeric-suffixes --lines=100 --additional-suffix=.list --verbose full_MuDsts2535.list MuDsts2535_
split --suffix-length=3 --numeric-suffixes --lines=100 --additional-suffix=.list --verbose full_MuDsts2025.list MuDsts2025_
split --suffix-length=3 --numeric-suffixes --lines=100 --additional-suffix=.list --verbose full_MuDsts1520.list MuDsts1520_
split --suffix-length=3 --numeric-suffixes --lines=100 --additional-suffix=.list --verbose full_MuDsts1115.list MuDsts1115_
split --suffix-length=3 --numeric-suffixes --lines=100 --additional-suffix=.list --verbose full_MuDsts911.list MuDsts911_
split --suffix-length=3 --numeric-suffixes --lines=100 --additional-suffix=.list --verbose full_MuDsts79.list MuDsts79_
split --suffix-length=3 --numeric-suffixes --lines=100 --additional-suffix=.list --verbose full_MuDsts57.list MuDsts57_
split --suffix-length=3 --numeric-suffixes --lines=100 --additional-suffix=.list --verbose full_MuDsts45.list MuDsts45_
split --suffix-length=3 --numeric-suffixes --lines=100 --additional-suffix=.list --verbose full_MuDsts34.list MuDsts34_
split --suffix-length=3 --numeric-suffixes --lines=100 --additional-suffix=.list --verbose full_MuDsts23.list MuDsts23_

cd ../
vi macros/MakeTStarJetPico_example.cxx # check to make sure scripts are set up properly
vi submit/submit.py
vi submit/jetPicoProduction_example.xml

starver pro
./macros/cleanup.csh
./macros/compile.csh
python submit/submit.py

# Obtain cross section per event for each pTHat bin, following https://drupal.star.bnl.gov/STAR/blog/djs232/pythia6-xsections-pp-embeeding-pau-200-gev-2015-collisions by Dave

cd /gpfs01/star/pwg/prozorov/jets_pp_2012/data/picoDst/embedding/xsec
grep "All included subprocesses" `ls /star/embed/log/Pythia6_pt3_4_*_20235003/log/*.log` > Xsec_log_lines
grep "All included subprocesses" `ls /star/embed/log/Pythia6_pt4_5_*_20235003/log/*.log` >> Xsec_log_lines
grep "All included subprocesses" `ls /star/embed/log/Pythia6_pt5_7_*_20235003/log/*.log` >> Xsec_log_lines
grep "All included subprocesses" `ls /star/embed/log/Pythia6_pt7_9_*_20235003/log/*.log` >> Xsec_log_lines
grep "All included subprocesses" `ls /star/embed/log/Pythia6_pt9_11_200_20235003/log/*.log` >> Xsec_log_lines
grep "All included subprocesses" `ls /star/embed/log/Pythia6_pt9_11_201_20235003/log/*.log` >> Xsec_log_lines
grep "All included subprocesses" `ls /star/embed/log/Pythia6_pt9_11_202_20235003/log/*.log` >> Xsec_log_lines
grep "All included subprocesses" `ls /star/embed/log/Pythia6_pt9_11_203_20235003/log/*.log` >> Xsec_log_lines
grep "All included subprocesses" `ls /star/embed/log/Pythia6_pt9_11_204_20235003/log/*.log` >> Xsec_log_lines
grep "All included subprocesses" `ls /star/embed/log/Pythia6_pt9_11_205_20235003/log/*.log` >> Xsec_log_lines
grep "All included subprocesses" `ls /star/embed/log/Pythia6_pt9_11_206_20235003/log/*.log` >> Xsec_log_lines
grep "All included subprocesses" `ls /star/embed/log/Pythia6_pt9_11_207_20235003/log/*.log` >> Xsec_log_lines
grep "All included subprocesses" `ls /star/embed/log/Pythia6_pt9_11_208_20235003/log/*.log` >> Xsec_log_lines
grep "All included subprocesses" `ls /star/embed/log/Pythia6_pt9_11_209_20235003/log/*.log` >> Xsec_log_lines
grep "All included subprocesses" `ls /star/embed/log/Pythia6_pt9_11_21*_20235003/log/*.log` >> Xsec_log_lines
grep "All included subprocesses" `ls /star/embed/log/Pythia6_pt11_15_200_20235003/log/*.log` >> Xsec_log_lines
grep "All included subprocesses" `ls /star/embed/log/Pythia6_pt11_15_201_20235003/log/*.log` >> Xsec_log_lines
grep "All included subprocesses" `ls /star/embed/log/Pythia6_pt11_15_202_20235003/log/*.log` >> Xsec_log_lines
grep "All included subprocesses" `ls /star/embed/log/Pythia6_pt11_15_203_20235003/log/*.log` >> Xsec_log_lines
grep "All included subprocesses" `ls /star/embed/log/Pythia6_pt11_15_204_20235003/log/*.log` >> Xsec_log_lines
grep "All included subprocesses" `ls /star/embed/log/Pythia6_pt11_15_205_20235003/log/*.log` >> Xsec_log_lines
grep "All included subprocesses" `ls /star/embed/log/Pythia6_pt11_15_206_20235003/log/*.log` >> Xsec_log_lines
grep "All included subprocesses" `ls /star/embed/log/Pythia6_pt11_15_207_20235003/log/*.log` >> Xsec_log_lines
grep "All included subprocesses" `ls /star/embed/log/Pythia6_pt11_15_208_20235003/log/*.log` >> Xsec_log_lines
grep "All included subprocesses" `ls /star/embed/log/Pythia6_pt11_15_209_20235003/log/*.log` >> Xsec_log_lines
grep "All included subprocesses" `ls /star/embed/log/Pythia6_pt11_15_21*_20235003/log/*.log` >> Xsec_log_lines
grep "All included subprocesses" `ls /star/embed/log/Pythia6_pt_15_20_200_20235003/log/*.log` >> Xsec_log_lines
grep "All included subprocesses" `ls /star/embed/log/Pythia6_pt_15_20_201_20235003/log/*.log` >> Xsec_log_lines
grep "All included subprocesses" `ls /star/embed/log/Pythia6_pt_15_20_202_20235003/log/*.log` >> Xsec_log_lines
grep "All included subprocesses" `ls /star/embed/log/Pythia6_pt_15_20_203_20235003/log/*.log` >> Xsec_log_lines
grep "All included subprocesses" `ls /star/embed/log/Pythia6_pt_15_20_204_20235003/log/*.log` >> Xsec_log_lines
grep "All included subprocesses" `ls /star/embed/log/Pythia6_pt_15_20_205_20235003/log/*.log` >> Xsec_log_lines
grep "All included subprocesses" `ls /star/embed/log/Pythia6_pt_15_20_206_20235003/log/*.log` >> Xsec_log_lines
grep "All included subprocesses" `ls /star/embed/log/Pythia6_pt_15_20_207_20235003/log/*.log` >> Xsec_log_lines
grep "All included subprocesses" `ls /star/embed/log/Pythia6_pt_15_20_208_20235003/log/*.log` >> Xsec_log_lines
grep "All included subprocesses" `ls /star/embed/log/Pythia6_pt_15_20_209_20235003/log/*.log` >> Xsec_log_lines
grep "All included subprocesses" `ls /star/embed/log/Pythia6_pt_15_20_21*_20235003/log/*.log` >> Xsec_log_lines
grep "All included subprocesses" `ls /star/embed/log/Pythia6_pt_20_25_*_20235003/log/*.log` >> Xsec_log_lines
grep "All included subprocesses" `ls /star/embed/log/Pythia6_pt_25_35_*_20235003/log/*.log` >> Xsec_log_lines
grep "All included subprocesses" `ls /star/embed/log/Pythia6_pt_35_45_*_20235003/log/*.log` >> Xsec_log_lines
grep "All included subprocesses" `ls /star/embed/log/Pythia6_pt_45_55_*_20235003/log/*.log` >> Xsec_log_lines
grep "All included subprocesses" `ls /star/embed/log/Pythia6_pt_55_infty_*_20235003/log/*.log` >> Xsec_log_lines
grep "All included subprocesses" `ls /star/embed/log/Pythia6_pt_2_3_*_20235003/log/*.log` >> Xsec_log_lines

python3 get_xsec_dave.py # creates Xsec-nEvents and Xsec-pythia.txt