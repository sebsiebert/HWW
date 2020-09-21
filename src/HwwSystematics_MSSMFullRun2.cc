#include "CombineHarvester/HWW/interface/HwwSystematics_MSSMRun2.h"
#include <vector>
#include <string>
#include "CombineHarvester/CombineTools/interface/Systematics.h"
#include "CombineHarvester/CombineTools/interface/Process.h"
#include "CombineHarvester/CombineTools/interface/Utilities.h"

namespace ch {

using ch::syst::SystMap;
using ch::syst::SystMapAsymm;
using ch::syst::era;
using ch::syst::channel;
using ch::syst::bin_id;
using ch::syst::process;
using ch::syst::mass;
using ch::syst::bin;
using ch::JoinStr;

void AddMSSMFullRun2Systematics(CombineHarvester & cb, bool highmass, bool DNNdiscrim, std::string model) {
  // Create a CombineHarvester clone that only contains the signal
  // categories
  CombineHarvester cb_sig = cb.cp();

  std::vector<std::string> ggH = {"ggH_HWW"};
  std::vector<std::string> qqH = {"qqH_HWW"};
  std::vector<std::string> ggHSBI = {"ggH_HWWSBI"};
  std::vector<std::string> qqHSBI = {"qqH_HWWSBI"};

  std::vector<std::string> runyears = {"6", "7", "8"};

  //cb.cp().bin_id({6,10,14}).ForEachObj([&](ch::Object *obj){
  //    obj->set_attribute("njet","0j");
  //});
  //cb.cp().bin_id({7,11,15}).ForEachObj([&](ch::Object *obj){
  //    obj->set_attribute("njet","1j");
  //});
  //cb.cp().bin_id({8,12,16}).ForEachObj([&](ch::Object *obj){
  //    obj->set_attribute("njet","2j");
  //});
  //cb.cp().bin_id({9,13,17}).ForEachObj([&](ch::Object *obj){
  //    obj->set_attribute("njet","vbf");
  //});
  cb.cp().bin_id({6,10,12}).ForEachObj([&](ch::Object *obj){
      obj->set_attribute("njet","ggh");
  });
  cb.cp().bin_id({7,11,13}).ForEachObj([&](ch::Object *obj){
      obj->set_attribute("njet","vbf");
  });

  cb.cp().channel({"em8","em8_dy","em8_top", "em7","em7_dy","em7_top", "em6","em6_dy","em6_top"}).ForEachObj([&](ch::Object *obj){
      obj->set_attribute("decay","em");
  });
  cb.cp().channel({"ee8","ee8_dy","ee8_top", "ee7","ee7_dy","ee7_top", "ee6","ee6_dy","ee6_top"}).ForEachObj([&](ch::Object *obj){
      obj->set_attribute("decay","ee");
  });
  cb.cp().channel({"mm8","mm8_dy","mm8_top", "mm7","mm7_dy","mm7_top", "mm6","mm6_dy","mm6_top"}).ForEachObj([&](ch::Object *obj){
      obj->set_attribute("decay","mm");
  });

  cb.cp().channel({"em8","em8_dy","em8_top", "ee8","ee8_dy","ee8_top", "mm8","mm8_dy","mm8_top"}).ForEachObj([&](ch::Object *obj){
      obj->set_attribute("year","2k18");
  });
  cb.cp().channel({"em7","em7_dy","em7_top", "ee7","ee7_dy","ee7_top", "mm7","mm7_dy","mm7_top"}).ForEachObj([&](ch::Object *obj){
      obj->set_attribute("year","2k17");
  });
  cb.cp().channel({"em6","em6_dy","em6_top", "ee6","ee6_dy","ee6_top", "mm6","mm6_dy","mm6_top"}).ForEachObj([&](ch::Object *obj){
      obj->set_attribute("year","2k16");
  });

  cb.cp().channel({"em8","ee8","mm8", "em7","ee7","mm7", "em6","ee6","mm6"}).ForEachObj([&](ch::Object *obj){
      obj->set_attribute("CR","False");
  });
  cb.cp().channel({"em8_dy","em8_top","ee8_dy","ee8_top","mm8_dy","mm8_top", "em7_dy","em7_top","ee7_dy","ee7_top","mm7_dy","mm7_top", "em6_dy","em6_top","ee6_dy","ee6_top","mm6_dy","mm6_top"}).ForEachObj([&](ch::Object *obj){
      obj->set_attribute("CR","True");
  });

  cb.cp().channel({"em6","em6_dy","em6_top", "ee6","ee6_dy","ee6_top", "mm6","mm6_dy","mm6_top", "em7","em7_dy","em7_top", "ee7","ee7_dy","ee7_top", "mm7","mm7_dy","mm7_top", "em8","em8_dy","em8_top", "ee8","ee8_dy","ee8_top", "mm8","mm8_dy","mm8_top"}).ForEachObj([&](ch::Object *obj){
      obj->set_attribute("analysis","dilep");
  });
  cb.cp().channel({"eqq6","eqq6_dy","eqq6_wj", "mqq6","mqq6_dy","mqq6_wj", "mqq6_bs","mqq6_dy_bs","mqq6_wj_bs", "mqq6_bs","mqq6_dy_bs","mqq6_wj_bs", "eqq7","eqq7_dy","eqq7_wj", "mqq7","mqq7_dy","mqq7_wj", "mqq7_bs","mqq7_dy_bs","mqq7_wj_bs", "mqq7_bs","mqq7_dy_bs","mqq7_wj_bs", "eqq8","eqq8_dy","eqq8_wj", "mqq8","mqq8_dy","mqq8_wj", "mqq8_bs","mqq8_dy_bs","mqq8_wj_bs", "mqq8_bs","mqq8_dy_bs","mqq8_wj_bs"}).ForEachObj([&](ch::Object *obj){
      obj->set_attribute("analysis","semilep");
  });

  cb.cp().channel({"eqq6_bs","eqq6_top_bs","eqq6_wj_bs", "mqq6_bs","mqq6_top_bs","mqq6_wj_bs", "eqq7_bs","eqq7_top_bs","eqq7_wj_bs", "mqq7_bs","mqq7_top_bs","mqq7_wj_bs", "eqq8_bs","eqq8_top_bs","eqq8_wj_bs", "mqq8_bs","mqq8_top_bs","mqq8_wj_bs"}).ForEachObj([&](ch::Object *obj){
      obj->set_attribute("whad","boosted");
  });
  cb.cp().channel({"eqq6","eqq6_top","eqq6_wj", "mqq6","mqq6_top","mqq6_wj", "eqq7","eqq7_top","eqq7_wj", "mqq7","mqq7_top","mqq7_wj", "eqq8","eqq8_top","eqq8_wj", "mqq8","mqq8_top","mqq8_wj"}).ForEachObj([&](ch::Object *obj){
      obj->set_attribute("whad","resolved");
  });


  std::vector<std::string> SM_procs = {"ggH_hww", "qqH_hww", "WH_hww", "ZH_hww", "ggH_htt", "qqH_htt", "WH_htt", "ZH_htt"};

  auto signal = Set2Vec(cb.cp().signals().SetFromProcs(
      std::mem_fn(&Process::process)));

  std::vector<std::string> backgrounds = {"DY", "VVV", "VZ", "Vg", "WW", "WWewk", "VgS_H", "VgS_L", "ggWW", "qqWWqq", "WW2J", "top"}; // All backgrounds, but without Fakes or Embedding!
  std::vector<std::string> backgroundsSemi = {"DY", "VVV", "VZ", "Vg", "WW", "WWewk", "VgS_H", "VgS_L", "ggWW", "qqWWqq", "WW2J", "top", "Wjets", "QCD"};

  //signal = JoinStr({signal,SM_procs});// All Signals, so HighMass and SM. Together with backgrounds: All MC




  //////////////////////////////////////////////////
  ////////// QCD+PDF ///////////////////////////////
  //////////////////////////////////////////////////


  // QCD scale uncertainties
  // -----------------------
  // SM Signal: Official YR4 uncertainties (lnN)
  // HM Signal: Same for VBF (lnN), but ggH is binned by njet: Use either Stuart-Tackmann or envelope of event weights (shape)
  // Background: Envelope of event weight, for which weight exist (shape), otherwise ??? (lnN)
  // SBI: Convert lnN of SBI constituents into shape, because different parts have different uncertainties

  cb.cp().process({"ggH_htt"}).AddSyst(cb, "QCDscale_ggH", "lnN", SystMapAsymm<>::init(0.933,1.046)); // HiggsXS.GetHiggsProdXSNP('YR4','13TeV','ggH','125.09','scale','sm')
  cb.cp().process({"qqH_htt"}).AddSyst(cb, "QCDscale_qqH", "lnN", SystMapAsymm<>::init(0.997,1.004)); // HiggsXS.GetHiggsProdXSNP('YR4','13TeV','vbfH','125.09','scale','sm')
  cb.cp().process({"WH_hww", "ZH_hww", "WH_htt", "ZH_htt"}).AddSyst(cb, "QCDscale_VH", "lnN", SystMapAsymm<process>::init
    ({"WH_hww", "WH_htt"},0.993,1.005) // HiggsXS.GetHiggsProdXSNP('YR4','13TeV','WH','125.09','scale','sm')
    ({"ZH_hww", "ZH_htt"},0.970,1.038)); // HiggsXS.GetHiggsProdXSNP('YR4','13TeV','ZH','125.09','scale','sm')
  //cb.cp().process({"ggZH_hww"}).AddSyst(cb, "QCDscale_ggZH", "lnN", SystMapAsymm<>::init(0.811,1.251)); // HiggsXS.GetHiggsProdXSNP('YR4','13TeV','ggZH','125.0','scale','sm')
  //cb.cp().process({"bbH_hww"}).AddSyst(cb, "QCDscale_bbH", "lnN", SystMapAsymm<>::init(0.761,1.201)); // Checked manually; INCLUDES PDF+ALPHA_S
  //cb.cp().process({"ttH_hww"}).AddSyst(cb, "QCDscale_ttH", "lnN", SystMapAsymm<>::init(0.908,1.058)); // HiggsXS.GetHiggsProdXSNP('YR4','13TeV','ttH','125.09','scale','sm')


  cb.cp().process({"DY"}).AddSyst(cb, "QCDscale_V", "shape", SystMap<>::init(1.00));
  cb.cp().process({"Vg", "VZ", "VgS_H", "VgS_L", "WW2J"}).attr({"dilep"},"analysis").AddSyst(cb, "QCDscale_VV", "shape", SystMap<>::init(1.00)); // TODO Not in Semilep, LHEScaleWeight missing in VZ Semilep? Check2016
  cb.cp().process({"top"}).AddSyst(cb, "QCDscale_ttbar", "shape", SystMap<>::init(1.00));
  //cb.cp().process({"WWewk"}).AddSyst(cb, "QCDscale_VV", "lnN", SystMap<>::init(1.11)); -> No need because rateParam

  // TODO fix: Semilep
  cb.cp().process(JoinStr({ggH, ggHSBI, {"ggH_hww"}})).attr({"dilep"},"analysis").AddSyst(cb, "QCDscale_ggH", "shape", SystMap<>::init(1.00));
  cb.cp().process(JoinStr({ggHSBI, {"ggWW"}})).attr({"dilep"},"analysis").AddSyst(cb, "QCDscale_ggVV", "shape", SystMap<>::init(1.00));
  cb.cp().process(JoinStr({qqH, qqHSBI, {"qqH_hww"}})).attr({"dilep"},"analysis").AddSyst(cb, "QCDscale_qqH", "shape", SystMap<>::init(1.00));
  cb.cp().process(JoinStr({qqHSBI, {"qqWWqq"}})).attr({"dilep"},"analysis").AddSyst(cb, "QCDscale_VV", "shape", SystMap<>::init(1.00));

  // Stuart Tackmann
  //cb.cp().process(JoinStr({ggH, ggHSBI})).attr({"dilep"},"analysis").AddSyst(cb, "QCDscale_ggH1in", "shape", SystMap<>::init(1.00));
  //cb.cp().process(JoinStr({ggH, ggHSBI})).attr({"dilep"},"analysis").AddSyst(cb, "QCDscale_ggH2in", "shape", SystMap<>::init(1.00));
  //cb.cp().process(JoinStr({ggH, ggHSBI})).attr({"dilep"},"analysis").AddSyst(cb, "QCDscale_ggH3in", "shape", SystMap<>::init(1.00));

  // QCD scale acceptance uncertainties
  // -----------------------
  cb.cp().process({"ggH_htt"}).AddSyst(cb,
    "QCDscale_ggH_ACCEPT", "lnN", SystMap<process>::init
    ({"ggH_htt"},1.012)); //"ggZH_hww"

  // TODO fix: Semilep
  cb.cp().process(JoinStr({ggH, ggHSBI, {"ggH_hww", "ggWW"}})).attr({"dilep"},"analysis").AddSyst(cb, "QCDscale_ggH_ACCEPT", "shape", SystMap<>::init(1.00));

  cb.cp().process({"qqH_htt"}).AddSyst(cb,
    "QCDscale_qqH_ACCEPT", "lnN", SystMap<process>::init
    ({"qqH_htt"},1.003));

  // TODO fix: Semilep
  cb.cp().process(JoinStr({qqH, qqHSBI, {"qqH_hww", "qqWWqq"}})).attr({"dilep"},"analysis").AddSyst(cb, "QCDscale_qqH_ACCEPT", "shape", SystMap<>::init(1.00));

  cb.cp().process({"WH_hww", "ZH_hww", "WH_htt", "ZH_htt"}).AddSyst(cb, "QCDscale_VH_ACCEPT", "lnN", SystMap<process>::init
    ({"WH_hww", "WH_htt"},1.010)
    ({"ZH_hww", "ZH_htt"},1.015));


  // PDF uncertainties
  // -----------------------
  cb.cp().process({"ggH_htt"}).AddSyst(cb, "pdf_Higgs_gg", "lnN",
    SystMap<process>::init // bbH_hww PDF uncertainty already included in QCD scale! See above
    ({"ggH_htt"},1.032)); // HiggsXS.GetHiggsProdXSNP('YR4','13TeV','ggH' ,'125.09','pdf','sm')
    //({"ggZH_hww"},1.024)); // HiggsXS.GetHiggsProdXSNP('YR4','13TeV','ggZH' ,'125.0','pdf','sm')
    //({"ttH_hww"},1.036)); // HiggsXS.GetHiggsProdXSNP('YR4','13TeV','ttH','125.09','pdf','sm')

  // TODO fix: Semilep
  cb.cp().process(JoinStr({ggH, ggHSBI, {"ggH_hww"}})).attr({"dilep"},"analysis").AddSyst(cb, "pdf_Higgs_gg", "shape", SystMap<>::init(1.00));
  cb.cp().process(JoinStr({ggHSBI, {"ggWW"}})).attr({"dilep"},"analysis").AddSyst(cb, "pdf_gg", "shape", SystMap<>::init(1.00));

  cb.cp().process({"qqH_htt", "WH_hww", "ZH_hww", "WH_htt", "ZH_htt"}).AddSyst(cb, "pdf_Higgs_qqbar", "lnN",
    SystMap<process>::init
    ({"qqH_htt"},1.021) // HiggsXS.GetHiggsProdXSNP('YR4','13TeV','vbfH','125.09','pdf','sm')
    ({"WH_hww", "WH_htt"},1.019) // HiggsXS.GetHiggsProdXSNP('YR4','13TeV','WH','125.09','pdf','sm')
    ({"ZH_hww", "ZH_htt"},1.016)); // HiggsXS.GetHiggsProdXSNP('YR4','13TeV','ZH','125.09','pdf','sm')

  // TODO fix: Semilep
  cb.cp().process(JoinStr({qqH, qqHSBI, {"qqH_hww"}})).attr({"dilep"},"analysis").AddSyst(cb, "pdf_Higgs_qqbar", "shape", SystMap<>::init(1.00));
  cb.cp().process(JoinStr({qqHSBI, {"qqWWqq"}})).attr({"dilep"},"analysis").AddSyst(cb, "pdf_qqbar", "shape", SystMap<>::init(1.00));

  cb.cp().process({"Vg", "VZ", "VgS_H", "VgS_L"}).AddSyst(cb, "pdf_qqbar", "lnN", SystMap<>::init(1.04));
  if(highmass){
    cb.cp().process({"DY"}).AddSyst(cb, "pdf_qqbar", "lnN", SystMap<>::init(1.002));
  }


  // PDF acceptance uncertainties
  // -----------------------
  cb.cp().process({"ggH_htt"}).AddSyst(cb, "pdf_Higgs_gg_ACCEPT", "lnN",
    SystMap<process>::init
    ({"ggH_htt"},1.006)); //"ggZH_hww"

  // TODO fix: Semilep
  cb.cp().process(JoinStr({ggH, ggHSBI, {"ggH_hww"}})).attr({"dilep"},"analysis").AddSyst(cb, "pdf_Higgs_gg_ACCEPT", "shape", SystMap<>::init(1.00));
  cb.cp().process(JoinStr({ggHSBI, {"ggWW"}})).attr({"dilep"},"analysis").AddSyst(cb, "pdf_gg_ACCEPT", "shape", SystMap<>::init(1.00));

  cb.cp().process({"qqH_htt", "WH_hww", "ZH_hww", "WH_htt", "ZH_htt"}).AddSyst(cb, "pdf_Higgs_qqbar_ACCEPT", "lnN",
    SystMap<process>::init
    ({"qqH_htt"},1.002)
    ({"WH_hww", "WH_htt"},1.003)
    ({"ZH_hww", "ZH_htt"},1.002));

  // TODO fix: Semilep
  cb.cp().process(JoinStr({qqH, qqHSBI, {"qqH_hww"}})).attr({"dilep"},"analysis").AddSyst(cb, "pdf_Higgs_qqbar_ACCEPT", "shape", SystMap<>::init(1.00));
  cb.cp().process(JoinStr({qqHSBI, {"qqWWqq"}})).attr({"dilep"},"analysis").AddSyst(cb, "pdf_qqbar_ACCEPT", "shape", SystMap<>::init(1.00));






  //////////////////////////////////////////////////
  ////////// DILEP /////////////////////////////////
  //////////////////////////////////////////////////

  // Cross-sections and lumi
  // -----------------------
  // No DY, Top, WW, WW2J, WWewk because using rateparam
  std::vector<std::string> LumiBkg = {"VVV", "VZ", "Vg", "VgS_H", "VgS_L", "ggWW", "qqWWqq"};
  // But no DY CR at high mass!
  if(highmass){
    LumiBkg = {"DY", "VVV", "VZ", "Vg", "VgS_H", "VgS_L", "ggWW", "qqWWqq"};
  }

  cb.cp().process(JoinStr({signal, SM_procs, LumiBkg})).attr({"2k18"},"year").attr({"dilep"},"analysis").AddSyst(cb,
    "lumi_13TeV_2018", "lnN", SystMap<>::init(1.015));
  cb.cp().process(JoinStr({signal, SM_procs, LumiBkg})).attr({"2k17"},"year").attr({"dilep"},"analysis").AddSyst(cb,
    "lumi_13TeV_2017", "lnN", SystMap<>::init(1.020));
  cb.cp().process(JoinStr({signal, SM_procs, LumiBkg})).attr({"2k16"},"year").attr({"dilep"},"analysis").AddSyst(cb,
    "lumi_13TeV_2016", "lnN", SystMap<>::init(1.022));

  // Correlated lumi
  cb.cp().process(JoinStr({signal, SM_procs, LumiBkg})).attr({"2k18"},"year").attr({"dilep"},"analysis").AddSyst(cb,
    "lumi_13TeV_XY", "lnN", SystMap<>::init(1.020));
  cb.cp().process(JoinStr({signal, SM_procs, LumiBkg})).attr({"2k18"},"year").attr({"dilep"},"analysis").AddSyst(cb,
    "lumi_13TeV_LS", "lnN", SystMap<>::init(1.002));
  cb.cp().process(JoinStr({signal, SM_procs, LumiBkg})).attr({"2k18"},"year").attr({"dilep"},"analysis").AddSyst(cb,
    "lumi_13TeV_BCC", "lnN", SystMap<>::init(1.002));

  cb.cp().process(JoinStr({signal, SM_procs, LumiBkg})).attr({"2k17"},"year").attr({"dilep"},"analysis").AddSyst(cb,
    "lumi_13TeV_XY", "lnN", SystMap<>::init(1.008));
  cb.cp().process(JoinStr({signal, SM_procs, LumiBkg})).attr({"2k17"},"year").attr({"dilep"},"analysis").AddSyst(cb,
    "lumi_13TeV_LS", "lnN", SystMap<>::init(1.003));
  cb.cp().process(JoinStr({signal, SM_procs, LumiBkg})).attr({"2k17"},"year").attr({"dilep"},"analysis").AddSyst(cb,
    "lumi_13TeV_BBD", "lnN", SystMap<>::init(1.004));
  cb.cp().process(JoinStr({signal, SM_procs, LumiBkg})).attr({"2k17"},"year").attr({"dilep"},"analysis").AddSyst(cb,
    "lumi_13TeV_DB", "lnN", SystMap<>::init(1.005));
  cb.cp().process(JoinStr({signal, SM_procs, LumiBkg})).attr({"2k17"},"year").attr({"dilep"},"analysis").AddSyst(cb,
    "lumi_13TeV_BCC", "lnN", SystMap<>::init(1.003));
  cb.cp().process(JoinStr({signal, SM_procs, LumiBkg})).attr({"2k17"},"year").attr({"dilep"},"analysis").AddSyst(cb,
    "lumi_13TeV_GS", "lnN", SystMap<>::init(1.001));

  cb.cp().process(JoinStr({signal, SM_procs, LumiBkg})).attr({"2k16"},"year").attr({"dilep"},"analysis").AddSyst(cb,
    "lumi_13TeV_XY", "lnN", SystMap<>::init(1.009));
  cb.cp().process(JoinStr({signal, SM_procs, LumiBkg})).attr({"2k16"},"year").attr({"dilep"},"analysis").AddSyst(cb,
    "lumi_13TeV_BBD", "lnN", SystMap<>::init(1.004));
  cb.cp().process(JoinStr({signal, SM_procs, LumiBkg})).attr({"2k16"},"year").attr({"dilep"},"analysis").AddSyst(cb,
    "lumi_13TeV_DB", "lnN", SystMap<>::init(1.005));
  cb.cp().process(JoinStr({signal, SM_procs, LumiBkg})).attr({"2k16"},"year").attr({"dilep"},"analysis").AddSyst(cb,
    "lumi_13TeV_GS", "lnN", SystMap<>::init(1.004));


  // WW stuff
  // -----------------------

  //cb.cp().process({"WW"}).bin_id({6}).attr({"dilep"},"analysis").AddSyst(cb, "CMS_hww_WWqscale_0j", "shape", SystMap<>::init(1.00));
  //cb.cp().process({"WW"}).bin_id({7}).attr({"dilep"},"analysis").AddSyst(cb, "CMS_hww_WWqscale_1j", "shape", SystMap<>::init(1.00));
  //cb.cp().process({"WW"}).bin_id({8}).attr({"dilep"},"analysis").AddSyst(cb, "CMS_hww_WWqscale_2j", "shape", SystMap<>::init(1.00));
  //cb.cp().process({"WW"}).bin_id({9}).attr({"dilep"},"analysis").AddSyst(cb, "CMS_hww_WWqscale_vbf", "shape", SystMap<>::init(1.00));
  //cb.cp().process({"WW"}).bin_id({6}).attr({"dilep"},"analysis").AddSyst(cb, "CMS_hww_WWresum_0j", "shape", SystMap<>::init(1.00));
  //cb.cp().process({"WW"}).bin_id({7}).attr({"dilep"},"analysis").AddSyst(cb, "CMS_hww_WWresum_1j", "shape", SystMap<>::init(1.00));
  //cb.cp().process({"WW"}).bin_id({8}).attr({"dilep"},"analysis").AddSyst(cb, "CMS_hww_WWresum_2j", "shape", SystMap<>::init(1.00));
  //cb.cp().process({"WW"}).bin_id({9}).attr({"dilep"},"analysis").AddSyst(cb, "CMS_hww_WWresum_vbf", "shape", SystMap<>::init(1.00));
  cb.cp().process({"WW"}).bin_id({6}).attr({"dilep"},"analysis").AddSyst(cb, "CMS_hww_WWqscale_ggh", "shape", SystMap<>::init(1.00));
  cb.cp().process({"WW"}).bin_id({7}).attr({"dilep"},"analysis").AddSyst(cb, "CMS_hww_WWqscale_vbf", "shape", SystMap<>::init(1.00));
  cb.cp().process({"WW"}).bin_id({6}).attr({"dilep"},"analysis").AddSyst(cb, "CMS_hww_WWresum_ggh", "shape", SystMap<>::init(1.00));
  cb.cp().process({"WW"}).bin_id({7}).attr({"dilep"},"analysis").AddSyst(cb, "CMS_hww_WWresum_vbf", "shape", SystMap<>::init(1.00));


  // Top stuff
  // -----------------------

  cb.cp().process({"top"}).attr({"dilep"},"analysis").AddSyst(cb, "singleTopToTTbar", "shape", SystMap<>::init(1.00));


  // VgS normalization
  // -----------------------
  cb.cp().process({"VgS_L"}).attr({"dilep"},"analysis").AddSyst(cb, "CMS_hww_VgStarScale", "lnN", SystMap<>::init(1.25));
  cb.cp().process({"VgS_H"}).attr({"dilep"},"analysis").AddSyst(cb, "CMS_hww_VZScale", "lnN", SystMap<>::init(1.16));


  // Electrons
  // -----------------------
  for(auto y : runyears){
    cb.cp().process(JoinStr({signal, SM_procs, backgrounds, {"DYemb"}})).attr({"2k1"+y},"year").attr({"dilep"},"analysis").AddSyst(cb,
      "CMS_eff_e_201"+y, "shape", SystMap<>::init(1.00));
    cb.cp().process(JoinStr({signal, SM_procs, backgrounds, {"DYemb"}})).attr({"2k1"+y},"year").attr({"dilep"},"analysis").AddSyst(cb,
      "CMS_scale_e_201"+y, "shape", SystMap<>::init(1.00));
  }


  // Muons
  // -----------------------
  for(auto y : runyears){
    cb.cp().process(JoinStr({signal, SM_procs, backgrounds, {"DYemb"}})).attr({"2k1"+y},"year").attr({"dilep"},"analysis").AddSyst(cb,
      "CMS_eff_m_201"+y, "shape", SystMap<>::init(1.00));
    cb.cp().process(JoinStr({signal, SM_procs, backgrounds, {"DYemb"}})).attr({"2k1"+y},"year").attr({"dilep"},"analysis").AddSyst(cb,
      "CMS_scale_m_201"+y, "shape", SystMap<>::init(1.00));
  }


  // Jets
  // -----------------------
  //for(auto y : runyears){
  //  cb.cp().process(JoinStr({signal, SM_procs, backgrounds})).attr({"2k1"+y},"year").attr({"dilep"},"analysis").AddSyst(cb,
  //    "CMS_scale_j_201"+y, "shape", SystMap<>::init(1.00));
  //}

  // Jets - Split up
  // -----------------------
  cb.cp().process(JoinStr({signal, SM_procs, backgrounds})).attr({"dilep"},"analysis").AddSyst(cb,
    "CMS_scale_JESAbsolute", "shape", SystMap<>::init(1.00));
  cb.cp().process(JoinStr({signal, SM_procs, backgrounds})).attr({"dilep"},"analysis").AddSyst(cb,
    "CMS_scale_JESBBEC1", "shape", SystMap<>::init(1.00));
  cb.cp().process(JoinStr({signal, SM_procs, backgrounds})).attr({"dilep"},"analysis").AddSyst(cb,
    "CMS_scale_JESEC2", "shape", SystMap<>::init(1.00));
  cb.cp().process(JoinStr({signal, SM_procs, backgrounds})).attr({"dilep"},"analysis").AddSyst(cb,
    "CMS_scale_JESFlavorQCD", "shape", SystMap<>::init(1.00));
  cb.cp().process(JoinStr({signal, SM_procs, backgrounds})).attr({"dilep"},"analysis").AddSyst(cb,
    "CMS_scale_JESHF", "shape", SystMap<>::init(1.00));
  cb.cp().process(JoinStr({signal, SM_procs, backgrounds})).attr({"dilep"},"analysis").AddSyst(cb,
    "CMS_scale_JESRelativeBal", "shape", SystMap<>::init(1.00));

  for(auto y : runyears){
    cb.cp().process(JoinStr({signal, SM_procs, backgrounds})).attr({"2k1"+y},"year").attr({"dilep"},"analysis").AddSyst(cb,
      "CMS_scale_JESAbsolute_201"+y, "shape", SystMap<>::init(1.00));
    cb.cp().process(JoinStr({signal, SM_procs, backgrounds})).attr({"2k1"+y},"year").attr({"dilep"},"analysis").AddSyst(cb,
      "CMS_scale_JESBBEC1_201"+y, "shape", SystMap<>::init(1.00));
    cb.cp().process(JoinStr({signal, SM_procs, backgrounds})).attr({"2k1"+y},"year").attr({"dilep"},"analysis").AddSyst(cb,
      "CMS_scale_JESEC2_201"+y, "shape", SystMap<>::init(1.00));
    cb.cp().process(JoinStr({signal, SM_procs, backgrounds})).attr({"2k1"+y},"year").attr({"dilep"},"analysis").AddSyst(cb,
      "CMS_scale_JESHF_201"+y, "shape", SystMap<>::init(1.00));
    cb.cp().process(JoinStr({signal, SM_procs, backgrounds})).attr({"2k1"+y},"year").attr({"dilep"},"analysis").AddSyst(cb,
      "CMS_scale_JESRelativeSample_201"+y, "shape", SystMap<>::init(1.00));
  }


  // MET
  // -----------------------
  for(auto y : runyears){
    cb.cp().process(JoinStr({signal, SM_procs, backgrounds})).attr({"2k1"+y},"year").attr({"dilep"},"analysis").AddSyst(cb,
      "CMS_scale_met_201"+y, "shape", SystMap<>::init(1.00));
  }

  // Using lnN instead of shapes for signal region for the above 4 uncertainties:
  // -----------------------
  //ch::AddLnNForShapes2017(cb, 1, 1, 1, 1);// e, m, j, met


  // Fakes
  // -----------------------

  cb.cp().process({"Fake_em"}).attr({"dilep"},"analysis").AddSyst(cb,
    "CMS_fake_syst_em", "lnN", SystMap<>::init(1.30));
  cb.cp().process({"Fake_me"}).attr({"dilep"},"analysis").AddSyst(cb,
    "CMS_fake_syst_me", "lnN", SystMap<>::init(1.30));
  cb.cp().process({"Fake_ee"}).attr({"dilep"},"analysis").AddSyst(cb,
    "CMS_fake_syst_ee", "lnN", SystMap<>::init(1.30));
  cb.cp().process({"Fake_mm"}).attr({"dilep"},"analysis").AddSyst(cb,
    "CMS_fake_syst_mm", "lnN", SystMap<>::init(1.30));

  for(auto y : runyears){
    cb.cp().process({"Fake_em", "Fake_me", "Fake_ee"}).attr({"2k1"+y},"year").attr({"dilep"},"analysis").AddSyst(cb,
      "CMS_fake_e_201"+y, "shape", SystMap<>::init(1.00));
    cb.cp().process({"Fake_em", "Fake_me", "Fake_ee"}).attr({"2k1"+y},"year").attr({"dilep"},"analysis").AddSyst(cb,
      "CMS_fake_stat_e_201"+y, "shape", SystMap<>::init(1.00));
    cb.cp().process({"Fake_em", "Fake_me", "Fake_mm"}).attr({"2k1"+y},"year").attr({"dilep"},"analysis").AddSyst(cb,
      "CMS_fake_m_201"+y, "shape", SystMap<>::init(1.00));
    cb.cp().process({"Fake_em", "Fake_me", "Fake_mm"}).attr({"2k1"+y},"year").attr({"dilep"},"analysis").AddSyst(cb,
      "CMS_fake_stat_m_201"+y, "shape", SystMap<>::init(1.00));
  }


  // B-tagging
  // -----------------------
  cb.cp().process(JoinStr({signal, SM_procs, backgrounds})).attr({"dilep"},"analysis").AddSyst(cb,
    "CMS_btag_jes", "shape", SystMap<>::init(1.00));
  cb.cp().process(JoinStr({signal, SM_procs, backgrounds})).attr({"dilep"},"analysis").AddSyst(cb,
    "CMS_btag_lf", "shape", SystMap<>::init(1.00));
  cb.cp().process(JoinStr({signal, SM_procs, backgrounds})).attr({"dilep"},"analysis").AddSyst(cb,
    "CMS_btag_hf", "shape", SystMap<>::init(1.00));
  cb.cp().process(JoinStr({signal, SM_procs, backgrounds})).attr({"dilep"},"analysis").AddSyst(cb,
    "CMS_btag_cferr1", "shape", SystMap<>::init(1.00));
  cb.cp().process(JoinStr({signal, SM_procs, backgrounds})).attr({"dilep"},"analysis").AddSyst(cb,
    "CMS_btag_cferr2", "shape", SystMap<>::init(1.00));

  for(auto y : runyears){
    cb.cp().process(JoinStr({signal, SM_procs, backgrounds})).attr({"2k1"+y},"year").attr({"dilep"},"analysis").AddSyst(cb,
      "CMS_btag_hfstats1_201"+y, "shape", SystMap<>::init(1.00));
    cb.cp().process(JoinStr({signal, SM_procs, backgrounds})).attr({"2k1"+y},"year").attr({"dilep"},"analysis").AddSyst(cb,
      "CMS_btag_hfstats2_201"+y, "shape", SystMap<>::init(1.00));
    cb.cp().process(JoinStr({signal, SM_procs, backgrounds})).attr({"2k1"+y},"year").attr({"dilep"},"analysis").AddSyst(cb,
      "CMS_btag_lfstats1_201"+y, "shape", SystMap<>::init(1.00));
    cb.cp().process(JoinStr({signal, SM_procs, backgrounds})).attr({"2k1"+y},"year").attr({"dilep"},"analysis").AddSyst(cb,
      "CMS_btag_lfstats2_201"+y, "shape", SystMap<>::init(1.00));
  }


  // Trigger
  // -----------------------
  for(auto y : runyears){
    cb.cp().process(JoinStr({signal, SM_procs, backgrounds, {"DYemb"}})).attr({"2k1"+y},"year").attr({"dilep"},"analysis").AddSyst(cb,
      "CMS_eff_hwwtrigger_201"+y, "shape", SystMap<>::init(1.00));
  }


  // Prefiring
  // -----------------------
  cb.cp().process(JoinStr({signal, SM_procs, backgrounds})).attr({"2k17"},"year").attr({"dilep"},"analysis").AddSyst(cb,
    "CMS_eff_prefiring_2017", "shape", SystMap<>::init(1.00));
  cb.cp().process(JoinStr({signal, SM_procs, backgrounds})).attr({"2k16"},"year").attr({"dilep"},"analysis").AddSyst(cb,
    "CMS_eff_prefiring_2016", "shape", SystMap<>::init(1.00));


  // Embedding
  // -----------------------
  for(auto y : runyears){
    cb.cp().process({"DYemb"}).channel({"em","em_dy","em_top"}).attr({"2k1"+y},"year").attr({"dilep"},"analysis").AddSyst(cb,
      "CMS_embed_veto_201"+y, "shape", SystMap<>::init(1.00));
  }


  // PS and UE and PU
  // -----------------------

  cb.cp().process(JoinStr({signal, SM_procs, backgrounds})).attr({"dilep"},"analysis").AddSyst(cb,
    "PS_ISR", "shape", SystMap<>::init(1.00));
  cb.cp().process(JoinStr({signal, SM_procs, backgrounds})).attr({"dilep"},"analysis").AddSyst(cb,
    "PS_FSR", "shape", SystMap<>::init(1.00));

  // An overall 1.5% UE uncertainty will cover all the UEup/UEdo variations, and we don't observe any dependency of UE variations on njet
  cb.cp().process(JoinStr({signal, SM_procs, backgrounds})).attr({"2k18", "2k17"},"year").attr({"dilep"},"analysis").AddSyst(cb,
    "UE_CP5", "lnN", SystMap<>::init(1.015));
  cb.cp().process(JoinStr({signal, SM_procs, backgrounds})).attr({"2k16"},"year").attr({"dilep"},"analysis").AddSyst(cb,
    "UE_CUET", "lnN", SystMap<>::init(1.015));

  for(auto y : runyears){
    cb.cp().process(JoinStr({signal, {"DY", "top", "WW", "ggH_hww", "qqH_hww"}})).attr({"2k1"+y},"year").attr({"dilep"},"analysis").AddSyst(cb,
      "CMS_PU_201"+y, "shape", SystMap<>::init(1.00));
  }


  // CR normalization
  // -----------------------
  if(!highmass){
  //cb.cp().process({"DY"}).bin_id({14, 15, 16, 17}).attr({"dilep"},"analysis").AddSyst(cb, "CMS_hww_CRSR_accept_dy", "lnN", SystMap<>::init(1.02));
  cb.cp().process({"DY"}).bin_id({12, 13}).attr({"dilep"},"analysis").AddSyst(cb, "CMS_hww_CRSR_accept_dy", "lnN", SystMap<>::init(1.02));
  }
  //cb.cp().process({"top"}).bin_id({10, 11, 12, 13}).attr({"dilep"},"analysis").AddSyst(cb, "CMS_hww_CRSR_accept_top", "lnN", SystMap<>::init(1.01));
  cb.cp().process({"top"}).bin_id({10, 11}).attr({"dilep"},"analysis").AddSyst(cb, "CMS_hww_CRSR_accept_top", "lnN", SystMap<>::init(1.01));


  // RateParams
  // -----------------------

  for(auto y : runyears){
  //for(auto y2 : std::vector<std::string> {"0j", "1j", "2j", "vbf"}){
  for(auto y2 : std::vector<std::string> {"ggh", "vbf"}){
  for(auto y3 : std::vector<std::string> {"em", "ee", "mm"}){
    if(!highmass){
    cb.cp().process({"DY", "DYemb"}).attr({y2},"njet").attr({y3},"decay").attr({"2k1"+y},"year").attr({"dilep"},"analysis").AddSyst(cb,
      "CMS_rateparam_DY_"+y3+"_"+y2+"_201"+y, "rateParam", SystMap<>::init(1.00));
    }
    cb.cp().process({"WW", "WW2J", "WWewk"}).attr({y2},"njet").attr({y3},"decay").attr({"2k1"+y},"year").attr({"dilep"},"analysis").AddSyst(cb,
      "CMS_rateparam_WW_"+y3+"_"+y2+"_201"+y, "rateParam", SystMap<>::init(1.00));
    cb.cp().process({"top"}).attr({y2},"njet").attr({y3},"decay").attr({"2k1"+y},"year").attr({"dilep"},"analysis").AddSyst(cb,
      "CMS_rateparam_TOP_"+y3+"_"+y2+"_201"+y, "rateParam", SystMap<>::init(1.00));
  }
  }
  }


  // Correlated bin-by-bin
  //std::vector<std::string> sigmasses = {"115", "120", "124", "125", "126", "130", "135", "140", "145", "150", "155", "160", "165", "170", "175", "180", "190", "200", "210", "230", "250", "270", "300", "350", "400", "450", "500", "550", "600", "650", "700", "750", "800", "900", "1000", "1500", "2000", "2500", "3000", "4000", "5000"};


  // Need to hard-code number of bin from discriminant!
  int maxbin;
  if(DNNdiscrim){
    if(!highmass){
      maxbin=29;
    }else{
      maxbin=21;
    }
  }else{
    if(!highmass){
      maxbin=28;
    }else{
      maxbin=16;
    }
  }
  for(int ibin=1; ibin<=maxbin; ibin++){
    /*for(auto smass : sigmasses){
      std::cout << "ibin"+mssmorindep+"GGH_"+smass+model+"_"+std::to_string(ibin)+"_stat" << std::endl;

      cb.cp().process(JoinStr({ggH, ggHSBI})).attr({"dilep"},"analysis").attr({"False"},"CR").AddSyst(cb,
        "ibin"+mssmorindep+"GGH_"+smass+model+"_"+std::to_string(ibin)+"_stat", "shape", SystMap<mass>::init({smass}, 1.00));

      cb.cp().process(JoinStr({{"ggWW"}, ggHSBI})).attr({"dilep"},"analysis").attr({"False"},"CR").AddSyst(cb,
        "ibinggWW_"+std::to_string(ibin)+"_stat", "shape", SystMap<mass>::init({smass}, 1.00));

      cb.cp().process(JoinStr({{"ggH_hww"}, ggHSBI})).attr({"dilep"},"analysis").attr({"False"},"CR").AddSyst(cb,
        "ibinggH_hww_"+std::to_string(ibin)+"_stat", "shape", SystMap<mass>::init({smass}, 1.00));

      cb.cp().process(JoinStr({qqH, qqHSBI})).attr({"dilep"},"analysis").attr({"False"},"CR").AddSyst(cb,
        "ibin"+mssmorindep+"QQH_"+smass+model+"_"+std::to_string(ibin)+"_stat", "shape", SystMap<mass>::init({smass}, 1.00));

      cb.cp().process(JoinStr({{"qqWWqq"}, qqHSBI})).attr({"dilep"},"analysis").attr({"False"},"CR").AddSyst(cb,
        "ibinqqWWqq_"+std::to_string(ibin)+"_stat", "shape", SystMap<mass>::init({smass}, 1.00));

      cb.cp().process(JoinStr({{"qqH_hww"}, qqHSBI})).attr({"dilep"},"analysis").attr({"False"},"CR").AddSyst(cb,
        "ibinqqH_hww_"+std::to_string(ibin)+"_stat", "shape", SystMap<mass>::init({smass}, 1.00));
    }*/
     //std::cout << "ibin"+mssmorindep+"GGH"+model+"_"+std::to_string(ibin)+"_stat" << std::endl;

     cb.cp().process(JoinStr({ggH, ggHSBI})).attr({"dilep"},"analysis").attr({"False"},"CR").AddSyst(cb,
       "ibinGGH"+model+"_"+std::to_string(ibin)+"_stat", "shape", SystMap<>::init(1.00));

     cb.cp().process(JoinStr({{"ggWW"}, ggHSBI})).attr({"dilep"},"analysis").attr({"False"},"CR").AddSyst(cb,
       "ibinggWW_"+std::to_string(ibin)+"_stat", "shape", SystMap<>::init(1.00));

     cb.cp().process(JoinStr({{"ggH_hww"}, ggHSBI})).attr({"dilep"},"analysis").attr({"False"},"CR").AddSyst(cb,
       "ibinggH_hww_"+std::to_string(ibin)+"_stat", "shape", SystMap<>::init(1.00));

     cb.cp().process(JoinStr({qqH, qqHSBI})).attr({"dilep"},"analysis").attr({"False"},"CR").AddSyst(cb,
       "ibinQQH"+model+"_"+std::to_string(ibin)+"_stat", "shape", SystMap<>::init(1.00));

     cb.cp().process(JoinStr({{"qqWWqq"}, qqHSBI})).attr({"dilep"},"analysis").attr({"False"},"CR").AddSyst(cb,
       "ibinqqWWqq_"+std::to_string(ibin)+"_stat", "shape", SystMap<>::init(1.00));

     cb.cp().process(JoinStr({{"qqH_hww"}, qqHSBI})).attr({"dilep"},"analysis").attr({"False"},"CR").AddSyst(cb,
       "ibinqqH_hww_"+std::to_string(ibin)+"_stat", "shape", SystMap<>::init(1.00));
  }





  //////////////////////////////////////////////////
  ////////// SEMILEP ///////////////////////////////
  //////////////////////////////////////////////////

  // Cross-sections and lumi
  // -----------------------
  // No DY, Top, WW, WW2J, WWewk because using rateparam
  cb.cp().process(JoinStr({signal, SM_procs, {"VVV", "VZ", "Vg", "VgS_H", "VgS_L", "ggWW", "qqWWqq", "Wjets", "QCD"}})).attr({"2k18"},"year").attr({"semilep"},"analysis").AddSyst(cb,
    "lumi_13TeV_2018", "lnN", SystMap<>::init(1.015));
  cb.cp().process(JoinStr({signal, SM_procs, {"VVV", "VZ", "Vg", "VgS_H", "VgS_L", "ggWW", "qqWWqq", "Wjets", "QCD"}})).attr({"2k17"},"year").attr({"semilep"},"analysis").AddSyst(cb,
    "lumi_13TeV_2017", "lnN", SystMap<>::init(1.020));
  cb.cp().process(JoinStr({signal, SM_procs, {"VVV", "VZ", "Vg", "VgS_H", "VgS_L", "ggWW", "qqWWqq", "Wjets", "QCD"}})).attr({"2k16"},"year").attr({"semilep"},"analysis").AddSyst(cb,
    "lumi_13TeV_2016", "lnN", SystMap<>::init(1.022));

  // Correlated lumi
  cb.cp().process(JoinStr({signal, SM_procs, {"VVV", "VZ", "Vg", "VgS_H", "VgS_L", "ggWW", "qqWWqq", "Wjets", "QCD"}})).attr({"2k18"},"year").attr({"semilep"},"analysis").AddSyst(cb,
    "lumi_13TeV_XYFact", "lnN", SystMap<>::init(1.020));
  cb.cp().process(JoinStr({signal, SM_procs, {"VVV", "VZ", "Vg", "VgS_H", "VgS_L", "ggWW", "qqWWqq", "Wjets", "QCD"}})).attr({"2k18"},"year").attr({"semilep"},"analysis").AddSyst(cb,
    "lumi_13TeV_LScale", "lnN", SystMap<>::init(1.002));
  cb.cp().process(JoinStr({signal, SM_procs, {"VVV", "VZ", "Vg", "VgS_H", "VgS_L", "ggWW", "qqWWqq", "Wjets", "QCD"}})).attr({"2k18"},"year").attr({"semilep"},"analysis").AddSyst(cb,
    "lumi_13TeV_CurrCalib", "lnN", SystMap<>::init(1.002));

  cb.cp().process(JoinStr({signal, SM_procs, {"VVV", "VZ", "Vg", "VgS_H", "VgS_L", "ggWW", "qqWWqq", "Wjets", "QCD"}})).attr({"2k17"},"year").attr({"semilep"},"analysis").AddSyst(cb,
    "lumi_13TeV_XYFact", "lnN", SystMap<>::init(1.008));
  cb.cp().process(JoinStr({signal, SM_procs, {"VVV", "VZ", "Vg", "VgS_H", "VgS_L", "ggWW", "qqWWqq", "Wjets", "QCD"}})).attr({"2k17"},"year").attr({"semilep"},"analysis").AddSyst(cb,
    "lumi_13TeV_LScale", "lnN", SystMap<>::init(1.003));
  cb.cp().process(JoinStr({signal, SM_procs, {"VVV", "VZ", "Vg", "VgS_H", "VgS_L", "ggWW", "qqWWqq", "Wjets", "QCD"}})).attr({"2k17"},"year").attr({"semilep"},"analysis").AddSyst(cb,
    "lumi_13TeV_BBDefl", "lnN", SystMap<>::init(1.004));
  cb.cp().process(JoinStr({signal, SM_procs, {"VVV", "VZ", "Vg", "VgS_H", "VgS_L", "ggWW", "qqWWqq", "Wjets", "QCD"}})).attr({"2k17"},"year").attr({"semilep"},"analysis").AddSyst(cb,
    "lumi_13TeV_DynBeta", "lnN", SystMap<>::init(1.005));
  cb.cp().process(JoinStr({signal, SM_procs, {"VVV", "VZ", "Vg", "VgS_H", "VgS_L", "ggWW", "qqWWqq", "Wjets", "QCD"}})).attr({"2k17"},"year").attr({"semilep"},"analysis").AddSyst(cb,
    "lumi_13TeV_CurrCalib", "lnN", SystMap<>::init(1.003));
  cb.cp().process(JoinStr({signal, SM_procs, {"VVV", "VZ", "Vg", "VgS_H", "VgS_L", "ggWW", "qqWWqq", "Wjets", "QCD"}})).attr({"2k17"},"year").attr({"semilep"},"analysis").AddSyst(cb,
    "lumi_13TeV_Ghosts", "lnN", SystMap<>::init(1.001));

  cb.cp().process(JoinStr({signal, SM_procs, {"VVV", "VZ", "Vg", "VgS_H", "VgS_L", "ggWW", "qqWWqq", "Wjets", "QCD"}})).attr({"2k16"},"year").attr({"semilep"},"analysis").AddSyst(cb,
    "lumi_13TeV_XYFact", "lnN", SystMap<>::init(1.009));
  cb.cp().process(JoinStr({signal, SM_procs, {"VVV", "VZ", "Vg", "VgS_H", "VgS_L", "ggWW", "qqWWqq", "Wjets", "QCD"}})).attr({"2k16"},"year").attr({"semilep"},"analysis").AddSyst(cb,
    "lumi_13TeV_BBDefl", "lnN", SystMap<>::init(1.004));
  cb.cp().process(JoinStr({signal, SM_procs, {"VVV", "VZ", "Vg", "VgS_H", "VgS_L", "ggWW", "qqWWqq", "Wjets", "QCD"}})).attr({"2k16"},"year").attr({"semilep"},"analysis").AddSyst(cb,
    "lumi_13TeV_DynBeta", "lnN", SystMap<>::init(1.005));
  cb.cp().process(JoinStr({signal, SM_procs, {"VVV", "VZ", "Vg", "VgS_H", "VgS_L", "ggWW", "qqWWqq", "Wjets", "QCD"}})).attr({"2k16"},"year").attr({"semilep"},"analysis").AddSyst(cb,
    "lumi_13TeV_Ghosts", "lnN", SystMap<>::init(1.004));


  // WW stuff
  // -----------------------

  // TODO: What to do? Need this or not?
  //cb.cp().process({"WW"}).bin_id({6}).attr({"semilep"},"analysis").AddSyst(cb, "CMS_hww_WWqscale_0j", "shape", SystMap<>::init(1.00));
  //cb.cp().process({"WW"}).bin_id({7}).attr({"semilep"},"analysis").AddSyst(cb, "CMS_hww_WWqscale_1j", "shape", SystMap<>::init(1.00));
  //cb.cp().process({"WW"}).bin_id({8}).attr({"semilep"},"analysis").AddSyst(cb, "CMS_hww_WWqscale_2j", "shape", SystMap<>::init(1.00));
  //cb.cp().process({"WW"}).bin_id({9}).attr({"semilep"},"analysis").AddSyst(cb, "CMS_hww_WWqscale_vbf", "shape", SystMap<>::init(1.00));
  //cb.cp().process({"WW"}).bin_id({6}).attr({"semilep"},"analysis").AddSyst(cb, "CMS_hww_WWresum_0j", "shape", SystMap<>::init(1.00));
  //cb.cp().process({"WW"}).bin_id({7}).attr({"semilep"},"analysis").AddSyst(cb, "CMS_hww_WWresum_1j", "shape", SystMap<>::init(1.00));
  //cb.cp().process({"WW"}).bin_id({8}).attr({"semilep"},"analysis").AddSyst(cb, "CMS_hww_WWresum_2j", "shape", SystMap<>::init(1.00));
  //cb.cp().process({"WW"}).bin_id({9}).attr({"semilep"},"analysis").AddSyst(cb, "CMS_hww_WWresum_vbf", "shape", SystMap<>::init(1.00));


  // Top stuff
  // -----------------------

  cb.cp().process({"top"}).attr({"semilep"},"analysis").AddSyst(cb, "singleTopToTTbar", "shape", SystMap<>::init(1.00));
  cb.cp().process({"top"}).attr({"semilep"},"analysis").AddSyst(cb, "CMS_topPtRew", "shape", SystMap<>::init(1.00));


  // VgS normalization
  // -----------------------
  cb.cp().process({"VgS_L"}).attr({"semilep"},"analysis").AddSyst(cb, "CMS_hww_VgStarScale", "lnN", SystMap<>::init(1.25));
  cb.cp().process({"VgS_H"}).attr({"semilep"},"analysis").AddSyst(cb, "CMS_hww_VZScale", "lnN", SystMap<>::init(1.16));


  // TODO All Shapes in Semilep
  // Electrons
  // -----------------------
  for(auto y : runyears){
    cb.cp().process(JoinStr({signal, SM_procs, backgroundsSemi})).attr({"2k1"+y},"year").attr({"semilep"},"analysis").AddSyst(cb,
      "CMS_eff_e_201"+y, "shape", SystMap<>::init(1.00));
    //cb.cp().process(JoinStr({signal, SM_procs, backgroundsSemi})).attr({"2k1"+y},"year").attr({"semilep"},"analysis").AddSyst(cb,
    //  "CMS_scale_e_201"+y, "shape", SystMap<>::init(1.00));
  }


  // Muons
  // -----------------------
  for(auto y : runyears){
    cb.cp().process(JoinStr({signal, SM_procs, backgroundsSemi})).attr({"2k1"+y},"year").attr({"semilep"},"analysis").AddSyst(cb,
      "CMS_eff_m_201"+y, "shape", SystMap<>::init(1.00));
    //cb.cp().process(JoinStr({signal, SM_procs, backgroundsSemi})).attr({"2k1"+y},"year").attr({"semilep"},"analysis").AddSyst(cb,
   //  "CMS_scale_m_201"+y, "shape", SystMap<>::init(1.00));
  }


  // Jets
  // -----------------------
  for(auto y : runyears){
    //cb.cp().process(JoinStr({signal, SM_procs, backgroundsSemi})).attr({"2k1"+y},"year").attr({"semilep"},"analysis").AddSyst(cb,
    //  "CMS_scale_j_201"+y, "shape", SystMap<>::init(1.00));
  }


  // MET
  // -----------------------
  for(auto y : runyears){
    //cb.cp().process(JoinStr({signal, SM_procs, backgroundsSemi})).attr({"2k1"+y},"year").attr({"semilep"},"analysis").AddSyst(cb,
    //  "CMS_scale_met_201"+y, "shape", SystMap<>::init(1.00));
  }

  // Using lnN instead of shapes for signal region for the above 4 uncertainties:
  // -----------------------
  //ch::AddLnNForShapes2017(cb, 1, 1, 1, 1);// e, m, j, met


  // B-tagging
  // -----------------------
  cb.cp().process(JoinStr({signal, SM_procs, backgroundsSemi})).attr({"semilep"},"analysis").AddSyst(cb,
    "CMS_btag_jes", "shape", SystMap<>::init(1.00));
  cb.cp().process(JoinStr({signal, SM_procs, backgroundsSemi})).attr({"semilep"},"analysis").AddSyst(cb,
    "CMS_btag_lf", "shape", SystMap<>::init(1.00));
  cb.cp().process(JoinStr({signal, SM_procs, backgroundsSemi})).attr({"semilep"},"analysis").AddSyst(cb,
    "CMS_btag_hf", "shape", SystMap<>::init(1.00));
  cb.cp().process(JoinStr({signal, SM_procs, backgroundsSemi})).attr({"semilep"},"analysis").AddSyst(cb,
    "CMS_btag_cferr1", "shape", SystMap<>::init(1.00));
  cb.cp().process(JoinStr({signal, SM_procs, backgroundsSemi})).attr({"semilep"},"analysis").AddSyst(cb,
    "CMS_btag_cferr2", "shape", SystMap<>::init(1.00));

  for(auto y : runyears){
    cb.cp().process(JoinStr({signal, SM_procs, backgroundsSemi})).attr({"2k1"+y},"year").attr({"semilep"},"analysis").AddSyst(cb,
      "CMS_btag_hfstats1_201"+y, "shape", SystMap<>::init(1.00));
    cb.cp().process(JoinStr({signal, SM_procs, backgroundsSemi})).attr({"2k1"+y},"year").attr({"semilep"},"analysis").AddSyst(cb,
      "CMS_btag_hfstats2_201"+y, "shape", SystMap<>::init(1.00));
    cb.cp().process(JoinStr({signal, SM_procs, backgroundsSemi})).attr({"2k1"+y},"year").attr({"semilep"},"analysis").AddSyst(cb,
      "CMS_btag_lfstats1_201"+y, "shape", SystMap<>::init(1.00));
    cb.cp().process(JoinStr({signal, SM_procs, backgroundsSemi})).attr({"2k1"+y},"year").attr({"semilep"},"analysis").AddSyst(cb,
      "CMS_btag_lfstats2_201"+y, "shape", SystMap<>::init(1.00));
  }


  // Trigger
  // -----------------------
  for(auto y : runyears){
    cb.cp().process(JoinStr({signal, SM_procs, backgroundsSemi})).attr({"2k1"+y},"year").attr({"semilep"},"analysis").AddSyst(cb,
      "CMS_eff_hwwtrigger_201"+y, "shape", SystMap<>::init(1.00));
  }


  // Prefiring
  // -----------------------
  cb.cp().process(JoinStr({signal, SM_procs, backgroundsSemi})).attr({"2k17"},"year").attr({"semilep"},"analysis").AddSyst(cb,
    "CMS_eff_prefiring_2017", "shape", SystMap<>::init(1.00));
  cb.cp().process(JoinStr({signal, SM_procs, backgroundsSemi})).attr({"2k16"},"year").attr({"semilep"},"analysis").AddSyst(cb,
    "CMS_eff_prefiring_2016", "shape", SystMap<>::init(1.00));


  // PS and UE and PU
  // -----------------------

  //TODO
  //cb.cp().process({"ggH_hww", "qqH_hww", "WW"}).attr({"2k18", "2k17"},"year").AddSyst(cb,
  //  "PS", "shape", SystMap<>::init(1.00));
  //cb.cp().process({"ggH_hww", "qqH_hww", "WW"}).attr({"2k16"},"year").AddSyst(cb,
  //  "PS_Herwig", "shape", SystMap<>::init(1.00));

  //cb.cp().process({"ggH_hww", "qqH_hww", "WW"}).attr({"2k18", "2k17"},"year").AddSyst(cb,
  //  "UE", "shape", SystMap<>::init(1.00));
  //cb.cp().process({"ggH_hww", "qqH_hww", "WW"}).attr({"2k16"},"year").AddSyst(cb,
  //  "UE_CUETP", "shape", SystMap<>::init(1.00));

  for(auto y : runyears){
    cb.cp().process({"DY", "top", "WW", "ggH_hww", "qqH_hww"}).attr({"2k1"+y},"year").attr({"semilep"},"analysis").AddSyst(cb,
      "CMS_PU_201"+y, "shape", SystMap<>::init(1.00));
  }


  // CR normalization
  // -----------------------
  cb.cp().process({"top"}).bin_id({10, 13}).attr({"semilep"},"analysis").AddSyst(cb, "CMS_hww_CRSR_accept_top", "lnN", SystMap<>::init(1.01));


  // RateParams
  // -----------------------

// 2018

  for(auto y : runyears){
  for(auto y2 : std::vector<std::string> {"e", "m"}){
  for(auto y3 : std::vector<std::string> {"", "_bs"}){
    cb.cp().process({"top"}).bin_id({6, 10, 14}).channel({y2+"qq"+y+y3,y2+"qq"+y+"_wj"+y3,y2+"qq"+y+"_top"+y3}).attr({"semilep"},"analysis").AddSyst(cb,
      "CMS_rateparam_TOP_"+y2+"qq"+y3+"_ggf_201"+y, "rateParam", SystMap<>::init(1.00));
    cb.cp().process({"top"}).bin_id({9, 13, 17}).channel({y2+"qq"+y+y3,y2+"qq"+y+"_wj"+y3,y2+"qq"+y+"_top"+y3}).attr({"semilep"},"analysis").AddSyst(cb,
      "CMS_rateparam_TOP_"+y2+"qq"+y3+"_vbf_201"+y, "rateParam", SystMap<>::init(1.00));
  }
  }
  }


}
}
