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

void AddMSSMFullRun2Systematics(CombineHarvester & cb, bool highmass, std::string model) {
    // Create a CombineHarvester clone that only contains the signal
    // categories
    CombineHarvester cb_sig = cb.cp();

    std::vector<std::string> ggH = {"ggH_HWW"};
    std::vector<std::string> qqH = {"qqH_HWW"};
    std::vector<std::string> ggHSBI = {"ggH_HWWSBI"};
    std::vector<std::string> qqHSBI = {"qqH_HWWSBI"};

    std::vector<std::string> runyears = {"6", "7", "8"};


    cb.cp().bin_id({6,10,12}).ForEachObj([&](ch::Object *obj){
        obj->set_attribute("njet","ggh");
    });
    cb.cp().bin_id({7,11,13}).ForEachObj([&](ch::Object *obj){
        obj->set_attribute("njet","vbf");
    });

    cb.cp().channel({"eqq8","eqq8_top","eqq8_wj","eqq8_bs","eqq8_top_bs","eqq8_wj_bs",
                    "eqq7","eqq7_top","eqq7_wj","eqq7_bs","eqq7_top_bs","eqq7_wj_bs",
                    "eqq6","eqq6_top","eqq6_wj","eqq6_bs","eqq6_top_bs","eqq6_wj_bs",

                    "eqq8_hm","eqq8_top_hm","eqq8_wj_hm",
                    "eqq7_hm","eqq7_top_hm","eqq7_wj_hm",
                    "eqq6_hm","eqq6_top_hm","eqq6_wj_hm",
            }).ForEachObj([&](ch::Object *obj){
            obj->set_attribute("decay","eqq");
    });

    cb.cp().channel({"mqq8","mqq8_top","mqq8_wj","mqq8_bs","mqq8_top_bs","mqq8_wj_bs",
                    "mqq7","mqq7_top","mqq7_wj","mqq7_bs","mqq7_top_bs","mqq7_wj_bs",
                    "mqq6","mqq6_top","mqq6_wj","mqq6_bs","mqq6_top_bs","mqq6_wj_bs",

                    "mqq8_hm","mqq8_top_hm","mqq8_wj_hm",
                    "mqq7_hm","mqq7_top_hm","mqq7_wj_hm",
                    "mqq6_hm","mqq6_top_hm","mqq6_wj_hm",
            }).ForEachObj([&](ch::Object *obj){
            obj->set_attribute("decay","mqq");
    });

    cb.cp().channel({"eqq8","eqq8_top","eqq8_wj","eqq8_bs","eqq8_top_bs","eqq8_wj_bs",
                    "mqq8","mqq8_top","mqq8_wj","mqq8_bs","mqq8_top_bs","mqq8_wj_bs",

                    "eqq8_hm","mqq8_hm","eqq8_top_hm","eqq8_wj_hm","mqq8_top_hm","mqq8_wj_hm"
        }).ForEachObj([&](ch::Object *obj){
        obj->set_attribute("year","2k18");
    });
    cb.cp().channel({"eqq7","eqq7_top","eqq7_wj","eqq7_bs","eqq7_top_bs","eqq7_wj_bs",
                    "mqq7","mqq7_top","mqq7_wj","mqq7_bs","mqq7_top_bs","mqq7_wj_bs",

                    "eqq7_hm","mqq7_hm","eqq7_top_hm","eqq7_wj_hm","mqq7_top_hm","mqq7_wj_hm"
        }).ForEachObj([&](ch::Object *obj){
        obj->set_attribute("year","2k17");
    });
    cb.cp().channel({"eqq6","eqq6_top","eqq6_wj", "eqq6_bs","eqq6_top_bs","eqq6_wj_bs",
                    "mqq6","mqq6_top","mqq6_wj","mqq6_bs","mqq6_top_bs","mqq6_wj_bs",

                    "eqq6_hm","mqq6_hm","eqq6_top_hm","eqq6_wj_hm","mqq6_top_hm","mqq6_wj_hm"
        }).ForEachObj([&](ch::Object *obj){
        obj->set_attribute("year","2k16");
    });

    cb.cp().channel({"eqq8","mqq8","eqq8_bs","mqq8_bs",
                    "eqq7","mqq7","eqq7_bs","mqq7_bs",
                    "eqq6","mqq6","eqq6_bs","mqq6_bs",

                    "eqq8_hm","mqq8_hm","eqq7_hm","mqq7_hm","eqq6_hm","mqq6_hm"
        }).ForEachObj([&](ch::Object *obj){
        obj->set_attribute("CR","False");
    });
    cb.cp().channel({
        "eqq8_top","mqq8_top","eqq8_top_bs","mqq8_top_bs",
        "eqq8_wj","mqq8_wj","eqq8_wj_bs","mqq8_wj_bs",
        "eqq8_top_hm","eqq8_wj_hm","mqq8_top_hm","mqq8_wj_hm"
        
        "eqq7_top","mqq7_top","eqq7_top_bs","mqq7_top_bs",
        "eqq7_wj","mqq7_wj","eqq7_wj_bs","mqq7_wj_bs",
        "eqq7_top_hm","eqq7_wj_hm","mqq7_top_hm","mqq7_wj_hm"
        
        "eqq6_top","mqq6_top","eqq6_top_bs","mqq6_top_bs",
        "eqq6_wj","mqq6_wj","eqq6_wj_bs","mqq6_wj_bs",
        "eqq6_top_hm","eqq6_wj_hm","mqq6_top_hm","mqq6_wj_hm"
        }).ForEachObj([&](ch::Object *obj){
        obj->set_attribute("CR","True");
    });

    cb.cp().channel({
        "eqq6","eqq6_dy","eqq6_wj", "mqq6","mqq6_dy","mqq6_wj",
        "mqq6_bs","mqq6_dy_bs","mqq6_wj_bs", "mqq6_bs","mqq6_dy_bs","mqq6_wj_bs",
        "eqq6_hm","eqq6_top_hm","eqq6_wj_hm", "mqq6_hm","mqq6_top_hm","mqq6_wj_hm",
        
        "eqq7","eqq7_dy","eqq7_wj", "mqq7","mqq7_dy","mqq7_wj",
        "mqq7_bs","mqq7_dy_bs","mqq7_wj_bs", "mqq7_bs","mqq7_dy_bs","mqq7_wj_bs",
        "eqq7_hm","eqq7_top_hm","eqq7_wj_hm", "mqq7_hm","mqq7_top_hm","mqq7_wj_hm",
        
        "eqq8","eqq8_dy","eqq8_wj", "mqq8","mqq8_dy","mqq8_wj",
        "mqq8_bs","mqq8_dy_bs","mqq8_wj_bs", "mqq8_bs","mqq8_dy_bs","mqq8_wj_bs",
        "eqq8_hm","eqq8_top_hm","eqq8_wj_hm", "mqq8_hm","mqq8_top_hm","mqq8_wj_hm"
        }).ForEachObj([&](ch::Object *obj){
        obj->set_attribute("analysis","semilep");
    });

    cb.cp().channel({"eqq6_bs","eqq6_top_bs","eqq6_wj_bs", "mqq6_bs","mqq6_top_bs","mqq6_wj_bs", "eqq7_bs","eqq7_top_bs","eqq7_wj_bs", "mqq7_bs","mqq7_top_bs","mqq7_wj_bs", "eqq8_bs","eqq8_top_bs","eqq8_wj_bs", "mqq8_bs","mqq8_top_bs","mqq8_wj_bs"}).ForEachObj([&](ch::Object *obj){
        obj->set_attribute("whad","Boosted");
    });
    cb.cp().channel({"eqq6","eqq6_top","eqq6_wj", "mqq6","mqq6_top","mqq6_wj", "eqq7","eqq7_top","eqq7_wj", "mqq7","mqq7_top","mqq7_wj", "eqq8","eqq8_top","eqq8_wj", "mqq8","mqq8_top","mqq8_wj"}).ForEachObj([&](ch::Object *obj){
        obj->set_attribute("whad","Resolved");
    });

    cb.cp().channel({"eqq6_hm","eqq6_top_hm","eqq6_wj_hm", "mqq6_hm","mqq6_top_hm","mqq6_wj_hm", "eqq7_hm","eqq7_top_hm","eqq7_wj_hm", "mqq7_hm","mqq7_top_hm","mqq7_wj_hm", "eqq8_hm","eqq8_top_hm","eqq8_wj_hm", "mqq8_hm","mqq8_top_hm","mqq8_wj_hm"}).ForEachObj([&](ch::Object *obj){
        obj->set_attribute("whad","HM");
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

    cb.cp().process({"VZ"}).attr({"semilep"},"analysis")
        .AddSyst(cb, "QCDscale_VV", "lnN", SystMapAsymm<>::init(0.99, 1.01)); // LHEScaleWeight missing in inclusive samples, initialize with 1% lnN

    // cb.cp().process({"DY"}).AddSyst(cb, "QCDscale_V", "shape", SystMap<>::init(1.00));
    // cb.cp().process({"Vg", "VgS_H", "VgS_L"})
    //     .AddSyst(cb, "QCDscale_VV", "shape", SystMap<>::init(1.00)); // In semilep not to VZ, LHEScaleWeight missing in VZ Semilep? Check2016
    // FIXME: nuisance name in 2016 is different than 2017/18
    // cb.cp().process({"WW2J", "qqWWqq"}).attr({"2k17", "2k18"}, "year")
    //     .AddSyst(cb, "QCDscale_WWJJ", "shape", SystMap<>::init(1.00));
    // cb.cp().process({"WW2J", "qqWWqq"}).attr({"2k16"}, "year")
    //     .AddSyst(cb, "QCDscale_VV", "shape", SystMap<>::init(1.00));

    // cb.cp().process({"top"}).AddSyst(cb, "QCDscale_ttbar", "shape", SystMap<>::init(1.00));
    cb.cp().process({"WWewk"}).AddSyst(cb, "QCDscale_VV", "lnN", SystMap<>::init(1.11));

    // TODO: fix: Semilep
    cb.cp().process(JoinStr({ggH, ggHSBI, {"ggH_hww"}}))
        .AddSyst(cb, "QCDscale_ggH", "shape", SystMap<>::init(1.00));
    cb.cp().process(JoinStr({ggHSBI, {"ggWW"}}))
        .AddSyst(cb, "QCDscale_ggVV", "shape", SystMap<>::init(1.00));
    cb.cp().process(JoinStr({qqH, qqHSBI, {"qqH_hww"}}))
        .AddSyst(cb, "QCDscale_qqH", "shape", SystMap<>::init(1.00));


    // QCD scale acceptance uncertainties
    // -----------------------
    cb.cp().process({"ggH_htt"}).AddSyst(cb,
        "QCDscale_ggH_ACCEPT", "lnN", SystMap<process>::init
        ({"ggH_htt"},1.012)); //"ggZH_hww"

    // TODO fix: Semilep
    cb.cp().process(JoinStr({ggH, ggHSBI, {"ggH_hww", "ggWW"}}))
        .AddSyst(cb, "QCDscale_ggH_ACCEPT", "shape", SystMap<>::init(1.00));

    cb.cp().process({"qqH_htt"}).AddSyst(cb,
        "QCDscale_qqH_ACCEPT", "lnN", SystMap<process>::init
        ({"qqH_htt"},1.003));

    // TODO fix: Semilep
    cb.cp().process(JoinStr({qqH, qqHSBI, {"qqH_hww", "qqWWqq"}}))
        .AddSyst(cb, "QCDscale_qqH_ACCEPT", "shape", SystMap<>::init(1.00));

    cb.cp().process({"WH_hww", "ZH_hww", "WH_htt", "ZH_htt"})
        .AddSyst(cb, "QCDscale_VH_ACCEPT", "lnN", SystMap<process>::init
        ({"WH_hww", "WH_htt"},1.010)
        ({"ZH_hww", "ZH_htt"},1.015));


    // PDF uncertainties
    // -----------------------
    cb.cp().process({"ggH_htt"}).AddSyst(cb, "pdf_Higgs_gg", "lnN",
    SystMap<process>::init // bbH_hww PDF uncertainty already included in QCD scale! See above
    ({"ggH_htt"},1.032)); // HiggsXS.GetHiggsProdXSNP('YR4','13TeV','ggH' ,'125.09','pdf','sm')

    // TODO fix: Semilep
    cb.cp().process(JoinStr({ggH, ggHSBI, {"ggH_hww"}}))
        .AddSyst(cb, "pdf_Higgs_gg", "shape", SystMap<>::init(1.00));
    cb.cp().process(JoinStr({ggHSBI, {"ggWW"}}))
        .AddSyst(cb, "pdf_gg", "shape", SystMap<>::init(1.00));

    cb.cp().process({"qqH_htt", "WH_hww", "ZH_hww", "WH_htt", "ZH_htt"})
        .AddSyst(cb, "pdf_Higgs_qqbar", "lnN", SystMap<process>::init
        ({"qqH_htt"},1.021) // GetHiggsProdXSNP('YR4','13TeV','vbfH','125.09','pdf','sm')
        ({"WH_hww", "WH_htt"},1.019) // GetHiggsProdXSNP('YR4','13TeV','WH','125.09','pdf','sm')
        ({"ZH_hww", "ZH_htt"},1.016)); //GetHiggsProdXSNP('YR4','13TeV','ZH','125.09','pdf','sm')

    // TODO fix: Semilep
    cb.cp().process(JoinStr({qqH, qqHSBI, {"qqH_hww"}}))
        .AddSyst(cb, "pdf_Higgs_qqbar", "shape", SystMap<>::init(1.00));
    cb.cp().process(JoinStr({qqHSBI, {"qqWWqq"}}))
        .AddSyst(cb, "pdf_qqbar", "shape", SystMap<>::init(1.00));

    cb.cp().process({"Vg", "VZ", "VgS_H", "VgS_L"})
        .AddSyst(cb, "pdf_qqbar", "lnN", SystMap<>::init(1.04));
    if(highmass){
    cb.cp().process({"DY"}).AddSyst(cb, "pdf_qqbar", "lnN", SystMap<>::init(1.002));
    }


    // PDF acceptance uncertainties
    // -----------------------
    cb.cp().process({"ggH_htt"}).AddSyst(cb, "pdf_Higgs_gg_ACCEPT", "lnN",
    SystMap<process>::init
    ({"ggH_htt"},1.006)); //"ggZH_hww"

    // TODO fix: Semilep
    cb.cp().process(JoinStr({ggH, ggHSBI, {"ggH_hww"}}))
        .AddSyst(cb, "pdf_Higgs_gg_ACCEPT", "shape", SystMap<>::init(1.00));
    cb.cp().process(JoinStr({ggHSBI, {"ggWW"}}))
        .AddSyst(cb, "pdf_gg_ACCEPT", "shape", SystMap<>::init(1.00));

    cb.cp().process({"qqH_htt", "WH_hww", "ZH_hww", "WH_htt", "ZH_htt"})
        .AddSyst(cb, "pdf_Higgs_qqbar_ACCEPT", "lnN", SystMap<process>::init
        ({"qqH_htt"},1.002)
        ({"WH_hww", "WH_htt"},1.003)
        ({"ZH_hww", "ZH_htt"},1.002));

    // TODO fix: Semilep
    cb.cp().process(JoinStr({qqH, qqHSBI, {"qqH_hww"}})).AddSyst(cb, "pdf_Higgs_qqbar_ACCEPT", "shape", SystMap<>::init(1.00));
    cb.cp().process(JoinStr({qqHSBI, {"qqWWqq"}})).AddSyst(cb, "pdf_qqbar_ACCEPT", "shape", SystMap<>::init(1.00));



    //////////////////////////////////////////////////
    ////////// SEMILEP ///////////////////////////////
    //////////////////////////////////////////////////
    std::vector<std::string> LumiBkg = {"VVV", "VZ", "Vg", "VgS_H", "VgS_L", "ggWW", "qqWWqq"};
    // But no DY CR at high mass!
    if(highmass){
    LumiBkg = {"DY", "VVV", "VZ", "Vg", "VgS_H", "VgS_L", "ggWW", "qqWWqq"};
    }
    // Cross-sections and lumi
    // -----------------------
    // No DY, Top, WW, WW2J, WWewk because using rateparam
    cb.cp().process(JoinStr({signal, SM_procs, LumiBkg}))
        .attr({"2k18"},"year").attr({"semilep"},"analysis")
        .AddSyst(cb,"lumi_13TeV_2018", "lnN", SystMap<>::init(1.015));
    cb.cp().process(JoinStr({signal, SM_procs, LumiBkg}))
        .attr({"2k17"},"year").attr({"semilep"},"analysis")
        .AddSyst(cb,"lumi_13TeV_2017", "lnN", SystMap<>::init(1.020));
    cb.cp().process(JoinStr({signal, SM_procs, LumiBkg}))
        .attr({"2k16"},"year").attr({"semilep"},"analysis")
        .AddSyst(cb, "lumi_13TeV_2016", "lnN", SystMap<>::init(1.022));

    // Correlated lumi
    cb.cp().process(JoinStr({signal, SM_procs, LumiBkg}))
        .attr({"2k18"},"year").attr({"semilep"},"analysis")
        .AddSyst(cb, "lumi_13TeV_XY", "lnN", SystMap<>::init(1.020));
    cb.cp().process(JoinStr({signal, SM_procs, LumiBkg}))
        .attr({"2k18"},"year").attr({"semilep"},"analysis")
        .AddSyst(cb, "lumi_13TeV_LS", "lnN", SystMap<>::init(1.002));
    cb.cp().process(JoinStr({signal, SM_procs, LumiBkg}))
        .attr({"2k18"},"year").attr({"semilep"},"analysis")
        .AddSyst(cb, "lumi_13TeV_BCC", "lnN", SystMap<>::init(1.002));

    cb.cp().process(JoinStr({signal, SM_procs, LumiBkg}))
        .attr({"2k17"},"year").attr({"semilep"},"analysis")
        .AddSyst(cb, "lumi_13TeV_XY", "lnN", SystMap<>::init(1.008));
    cb.cp().process(JoinStr({signal, SM_procs, LumiBkg}))
        .attr({"2k17"},"year").attr({"semilep"},"analysis")
        .AddSyst(cb, "lumi_13TeV_LS", "lnN", SystMap<>::init(1.003));
    cb.cp().process(JoinStr({signal, SM_procs, LumiBkg}))
        .attr({"2k17"},"year").attr({"semilep"},"analysis")
        .AddSyst(cb, "lumi_13TeV_BBD", "lnN", SystMap<>::init(1.004));
    cb.cp().process(JoinStr({signal, SM_procs, LumiBkg}))
        .attr({"2k17"},"year").attr({"semilep"},"analysis")
        .AddSyst(cb, "lumi_13TeV_DB", "lnN", SystMap<>::init(1.005));
    cb.cp().process(JoinStr({signal, SM_procs, LumiBkg}))
        .attr({"2k17"},"year").attr({"semilep"},"analysis")
        .AddSyst(cb, "lumi_13TeV_BCC", "lnN", SystMap<>::init(1.003));
    cb.cp().process(JoinStr({signal, SM_procs, LumiBkg}))
        .attr({"2k17"},"year").attr({"semilep"},"analysis")
        .AddSyst(cb, "lumi_13TeV_GS", "lnN", SystMap<>::init(1.001));

    cb.cp().process(JoinStr({signal, SM_procs, LumiBkg}))
        .attr({"2k16"},"year").attr({"semilep"},"analysis")
        .AddSyst(cb, "lumi_13TeV_XY", "lnN", SystMap<>::init(1.009));
    cb.cp().process(JoinStr({signal, SM_procs, LumiBkg}))
        .attr({"2k16"},"year").attr({"semilep"},"analysis")
        .AddSyst(cb, "lumi_13TeV_BBD", "lnN", SystMap<>::init(1.004));
    cb.cp().process(JoinStr({signal, SM_procs, LumiBkg}))
        .attr({"2k16"},"year").attr({"semilep"},"analysis")
        .AddSyst(cb, "lumi_13TeV_DB", "lnN", SystMap<>::init(1.005));
    cb.cp().process(JoinStr({signal, SM_procs, LumiBkg}))
        .attr({"2k16"},"year").attr({"semilep"},"analysis")
        .AddSyst(cb, "lumi_13TeV_GS", "lnN", SystMap<>::init(1.004));



    // Top stuff
    // -----------------------

    cb.cp().process({"top"}).attr({"semilep"},"analysis").AddSyst(cb, "singleTopToTTbar", "shape", SystMap<>::init(1.00));
    // Top_pTrw uncertainty not needed
    // cb.cp().process({"top"}).attr({"semilep"},"analysis").AddSyst(cb, "CMS_topPtRew", "shape", SystMap<>::init(1.00));


    // VgS normalization
    // -----------------------
    cb.cp().process({"VgS_L"}).attr({"semilep"},"analysis").AddSyst(cb, "CMS_hww_VgStarScale", "lnN", SystMap<>::init(1.25));
    cb.cp().process({"VgS_H"}).attr({"semilep"},"analysis").AddSyst(cb, "CMS_hww_VZScale", "lnN", SystMap<>::init(1.16));


    // TODO All Shapes in Semilep
    // Electrons
    // -----------------------
    for(auto y : runyears){
    cb.cp().process(JoinStr({signal, SM_procs, backgroundsSemi})).attr({"2k1"+y},"year")
        .attr({"semilep"},"analysis")
        .AddSyst(cb, "CMS_eff_e_201"+y, "shape", SystMap<>::init(1.00));
    cb.cp().process(JoinStr({signal, SM_procs, backgroundsSemi})).attr({"2k1"+y},"year")
        .attr({"semilep"},"analysis")
        .AddSyst(cb, "CMS_scale_e_201"+y, "shape", SystMap<>::init(1.00));
    }


    // Muons
    // -----------------------
    for(auto y : runyears){
    cb.cp().process(JoinStr({signal, SM_procs, backgroundsSemi})).attr({"2k1"+y},"year")
        .attr({"semilep"},"analysis")
        .AddSyst(cb, "CMS_eff_m_201"+y, "shape", SystMap<>::init(1.00));
    cb.cp().process(JoinStr({signal, SM_procs, backgroundsSemi})).attr({"2k1"+y},"year")
        .attr({"semilep"},"analysis")
        .AddSyst(cb, "CMS_scale_m_201"+y, "shape", SystMap<>::init(1.00));
    }


    // Jets
    // -----------------------
    std::vector<std::string> jes_systs {
        "JESAbsolute","JESBBEC1","JESEC2","JESFlavorQCD","JESHF","JESRelativeBal"
    };
    for(auto js : jes_systs) {
        cb.cp().process(JoinStr({signal, SM_procs, backgroundsSemi}))
            .attr({"semilep"},"analysis")
            .AddSyst(cb, "CMS_scale_"+js, "shape", SystMap<>::init(1.00));
    }
    
    std::vector<std::string> jes_systs_year {
        "JESAbsolute_201","JESBBEC1_201","JESEC2_201","JESHF_201","JESRelativeSample_201"
    };
    for(auto y : runyears){
        for(auto js : jes_systs_year) {
            cb.cp().process(JoinStr({signal, SM_procs, backgroundsSemi}))
                .attr({"2k1"+y},"year").attr({"semilep"},"analysis")
                .AddSyst(cb, "CMS_scale_"+js+y, "shape", SystMap<>::init(1.00));
        }
    }
    
    
    // Fat Jets
    // -----------------------
    for(auto y : runyears){
        cb.cp().process(JoinStr({signal, SM_procs, backgroundsSemi}))
            .attr({"2k1"+y},"year").attr({"semilep"},"analysis")
            .AddSyst(cb, "CMS_scale_fatj_201"+y, "shape", SystMap<>::init(1.00));
        cb.cp().process(JoinStr({signal, SM_procs, backgroundsSemi}))
            .attr({"2k1"+y},"year").attr({"semilep"},"analysis")
            .AddSyst(cb, "CMS_res_fatjer_201"+y, "shape", SystMap<>::init(1.00));
        cb.cp().process(JoinStr({signal, SM_procs, backgroundsSemi}))
            .attr({"2k1"+y},"year").attr({"semilep"},"analysis")
            .AddSyst(cb, "CMS_fatjms_201"+y, "shape", SystMap<>::init(1.00));
        cb.cp().process(JoinStr({signal, SM_procs, backgroundsSemi}))
            .attr({"2k1"+y},"year").attr({"semilep"},"analysis")
            .AddSyst(cb, "CMS_res_fatjmr_201"+y, "shape", SystMap<>::init(1.00));
    }


    // MET
    // -----------------------
    for(auto y : runyears){
    cb.cp().process(JoinStr({signal, SM_procs, backgroundsSemi})).attr({"2k1"+y},"year").attr({"semilep"},"analysis").AddSyst(cb,
       "CMS_scale_met_201"+y, "shape", SystMap<>::init(1.00));
    }

    // Using lnN instead of shapes for signal region for the above 4 uncertainties:
    // -----------------------
    //ch::AddLnNForShapes2017(cb, 1, 1, 1, 1);// e, m, j, met


    // B-tagging
    // -----------------------
    cb.cp().process(JoinStr({signal, SM_procs, backgroundsSemi})).attr({"semilep"},"analysis")
        .AddSyst(cb, "CMS_btag_jes", "shape", SystMap<>::init(1.00));
    cb.cp().process(JoinStr({signal, SM_procs, backgroundsSemi})).attr({"semilep"},"analysis")
        .AddSyst(cb, "CMS_btag_lf", "shape", SystMap<>::init(1.00));
    cb.cp().process(JoinStr({signal, SM_procs, backgroundsSemi})).attr({"semilep"},"analysis")
        .AddSyst(cb, "CMS_btag_hf", "shape", SystMap<>::init(1.00));
    cb.cp().process(JoinStr({signal, SM_procs, backgroundsSemi})).attr({"semilep"},"analysis")
        .AddSyst(cb, "CMS_btag_cferr1", "shape", SystMap<>::init(1.00));
    cb.cp().process(JoinStr({signal, SM_procs, backgroundsSemi})).attr({"semilep"},"analysis")
        .AddSyst(cb, "CMS_btag_cferr2", "shape", SystMap<>::init(1.00));

    for(auto y : runyears){
        cb.cp().process(JoinStr({signal, SM_procs, backgroundsSemi}))
            .attr({"2k1"+y},"year").attr({"semilep"},"analysis")
            .AddSyst(cb, "CMS_btag_hfstats1_201"+y, "shape", SystMap<>::init(1.00));
        cb.cp().process(JoinStr({signal, SM_procs, backgroundsSemi}))
            .attr({"2k1"+y},"year").attr({"semilep"},"analysis")
            .AddSyst(cb, "CMS_btag_hfstats2_201"+y, "shape", SystMap<>::init(1.00));
        cb.cp().process(JoinStr({signal, SM_procs, backgroundsSemi}))
            .attr({"2k1"+y},"year").attr({"semilep"},"analysis")
            .AddSyst(cb, "CMS_btag_lfstats1_201"+y, "shape", SystMap<>::init(1.00));
        cb.cp().process(JoinStr({signal, SM_procs, backgroundsSemi}))
            .attr({"2k1"+y},"year").attr({"semilep"},"analysis")
            .AddSyst(cb, "CMS_btag_lfstats2_201"+y, "shape", SystMap<>::init(1.00));
    }


    // W-tagging (tau21)
    // -----------------------
    cb.cp().process(JoinStr({signal, SM_procs, backgroundsSemi}))
        .attr({"Resolved", "Boosted"},"whad")
        .AddSyst(cb, "CMS_wtag_eff", "shape", SystMap<>::init(1.00));
    // add lnN for high mass category
    cb.cp().process(JoinStr({signal, SM_procs, backgroundsSemi}))
        .attr({"HM"},"whad")
        .AddSyst(cb, "CMS_wtag_eff_HM", "lnN", SystMap<>::init(1.05));


    // Trigger
    // -----------------------
    for(auto y : runyears){
        cb.cp().process(JoinStr({signal, SM_procs, backgroundsSemi}))
            .attr({"2k1"+y},"year").attr({"semilep"},"analysis")
            .AddSyst(cb,"CMS_eff_hwwtrigger_201"+y, "shape", SystMap<>::init(1.00));
    }


    // Prefiring
    // -----------------------
    cb.cp().process(JoinStr({signal, SM_procs, backgroundsSemi})).attr({"2k17"},"year").attr({"semilep"},"analysis").AddSyst(cb,
    "CMS_eff_prefiring_2017", "shape", SystMap<>::init(1.00));
    cb.cp().process(JoinStr({signal, SM_procs, backgroundsSemi})).attr({"2k16"},"year").attr({"semilep"},"analysis").AddSyst(cb,
    "CMS_eff_prefiring_2016", "shape", SystMap<>::init(1.00));


    // PU and PS and UE
    // -----------------------
    for(auto y : runyears){
        cb.cp().process(JoinStr({signal, {"Wjets", "DY", "top", "WW", "ggH_hww", "qqH_hww"}}))
            .attr({"2k1"+y},"year").attr({"semilep"},"analysis")
            .AddSyst(cb, "CMS_PU_201"+y, "shape", SystMap<>::init(1.00));
    }

    cb.cp().process(JoinStr({signal, SM_procs, {"Wjets", "Vg", "VgS", "ggWW", "WW", "WWewk",
                                                "qqWWqq", "WW2J", "top", "DY", "VVV", "VZ"}}))
        .attr({"semilep"},"analysis")
        .AddSyst(cb,"PS_ISR", "shape", SystMap<>::init(1.00));

    cb.cp().process(JoinStr({signal, SM_procs, {"Wjets", "Vg", "VgS", "ggWW", "WW", "WWewk",
                                                "qqWWqq", "WW2J", "top", "DY", "VVV"}})) //TODO: add VZ back
        .attr({"semilep"},"analysis")
        .AddSyst(cb,"PS_FSR", "shape", SystMap<>::init(1.00));

    cb.cp().process({"ggH_hww", "qqH_hww", "WW"}).attr({"2k18", "2k17"},"year")
        .AddSyst(cb, "UE_CP5", "lnN", SystMap<>::init(1.015));
    cb.cp().process({"ggH_hww", "qqH_hww", "WW"}).attr({"2k16"},"year")
        .AddSyst(cb, "UE_CUET", "lnN", SystMap<>::init(1.015));


    // Fakes
    // -----------------------

    cb.cp().process({"FAKE"}).attr({"semilep"},"analysis")
        .AddSyst(cb, "CMS_fake_syst", "lnN", SystMap<>::init(1.30));

    for(auto y : runyears){
        cb.cp().process({"FAKE"}).attr({"2k1"+y},"year").attr({"semilep"},"analysis")
            .AddSyst(cb, "CMS_fake_e_201"+y, "shape", SystMap<>::init(1.00));
        cb.cp().process({"FAKE"}).attr({"2k1"+y},"year").attr({"semilep"},"analysis")
            .AddSyst(cb, "CMS_fake_stat_e_201"+y, "shape", SystMap<>::init(1.00));
        cb.cp().process({"FAKE"}).attr({"2k1"+y},"year").attr({"semilep"},"analysis")
            .AddSyst(cb, "CMS_fake_m_201"+y, "shape", SystMap<>::init(1.00));
        cb.cp().process({"FAKE"}).attr({"2k1"+y},"year").attr({"semilep"},"analysis")
            .AddSyst(cb, "CMS_fake_stat_m_201"+y, "shape", SystMap<>::init(1.00));
    }


    // CR normalization
    // -----------------------
    // TODO: fix bin ids
    cb.cp().process({"Wjets"}).bin_id({10, 13}).attr({"semilep"},"analysis").AddSyst(cb, "CMS_hww_CRSR_accept_SB", "lnN", SystMap<>::init(1.009));
    cb.cp().process({"Wjets"}).bin_id({10, 13}).attr({"semilep"},"analysis").AddSyst(cb, "CMS_hww_CRSR_accept_SB", "lnN", SystMap<>::init(1.022));
    cb.cp().process({"top"}).bin_id({10, 13}).attr({"semilep"},"analysis").AddSyst(cb, "CMS_hww_CRSR_accept_top", "lnN", SystMap<>::init(1.01));


    // RateParams
    // -----------------------

    // 2018
    // TODO:
    for(auto y : runyears){
        for(auto y2 : std::vector<std::string> {"e", "m"}){
            for(auto y3 : std::vector<std::string> {"", "_bs", "_hm"}){
                cb.cp().process({"top"}).bin_id({6, 10, 12})
                .channel({y2+"qq"+y+y3,y2+"qq"+y+"_wj"+y3,y2+"qq"+y+"_top"+y3})
                .attr({"semilep"},"analysis")
                .AddSyst(cb, "CMS_rateparam_TOP_"+y2+"qq"+y3+"_ggf_201"+y,
                    "rateParam", SystMap<>::init(1.00));
                cb.cp().process({"top"}).bin_id({7, 11, 13})
                .channel({y2+"qq"+y+y3,y2+"qq"+y+"_wj"+y3,y2+"qq"+y+"_top"+y3})
                .attr({"semilep"},"analysis")
                .AddSyst(cb, "CMS_rateparam_TOP_"+y2+"qq"+y3+"_vbf_201"+y,
                    "rateParam", SystMap<>::init(1.00));

                cb.cp().process({"Wjets"}).bin_id({6, 10, 12})
                .channel({y2+"qq"+y+y3,y2+"qq"+y+"_wj"+y3,y2+"qq"+y+"_top"+y3})
                .attr({"semilep"},"analysis")
                .AddSyst(cb, "CMS_rateparam_WJETS_"+y2+"qq"+y3+"_ggf_201"+y,
                    "rateParam", SystMap<>::init(1.00));
                cb.cp().process({"Wjets"}).bin_id({7, 11, 13})
                .channel({y2+"qq"+y+y3,y2+"qq"+y+"_wj"+y3,y2+"qq"+y+"_top"+y3})
                .attr({"semilep"},"analysis")
                .AddSyst(cb, "CMS_rateparam_WJETS_"+y2+"qq"+y3+"_vbf_201"+y,
                    "rateParam", SystMap<>::init(1.00));
            }
        }
    }


    // // Correlated bin-by-bin
    // std::vector<std::string> sigmasses = {"115", "120", "124", "125", "126", "130", "135", "140", "145", "150", "155", "160", "165", "170", "175", "180", "190", "200", "210", "230", "250", "270", "300", "350", "400", "450", "500", "550", "600", "650", "700", "750", "800", "900", "1000", "1500", "2000", "2500", "3000", "4000", "5000"};

    // // TODO:
    // // Need to hard-code number of bin from discriminant!
    // std::map<std::string, int> maxbins = {{"Resolved", 16}, {"Boosted", 18}, {"HM", 8}};

    // for(auto y : runyears){
    // for(auto pro : std::vector<std::string> {"ggh", "vbf"}){
    // for(auto dec : std::vector<std::string> {"eqq", "mqq"}){
    // for(auto cat : std::vector<std::string> {"Boosted", "Resolved", "HM"}){
    // for(int ibin=1; ibin<=maxbins[cat]; ibin++){

    //      //std::cout << "ibin"+mssmorindep+"GGH"+model+"_"+std::to_string(ibin)+"_stat" << std::endl;

    //      cb.cp().process(JoinStr({ggH, ggHSBI})).attr({cat},"whad")
    //         .attr({pro},"njet").attr({dec},"decay").attr({"2k1"+y},"year")
    //         .attr({"semilep"},"analysis").attr({"False"},"CR").AddSyst(cb,
    //         "CMS_hww_"+dec+"_"+pro+"_"+cat+"_201"+y+"_correlbin_GGH"+model+"_"+std::to_string(ibin)+"_stat",
    //         "shape", SystMap<>::init(1.00));

    //      cb.cp().process(JoinStr({{"ggWW"}, ggHSBI})).attr({cat},"whad")
    //         .attr({pro},"njet").attr({dec},"decay").attr({"2k1"+y},"year")
    //         .attr({"semilep"},"analysis").attr({"False"},"CR").AddSyst(cb,
    //         "CMS_hww_"+dec+"_"+pro+"_"+cat+"_201"+y+"_correlbin_ggWW_"+std::to_string(ibin)+"_stat",
    //         "shape", SystMap<>::init(1.00));

    //      cb.cp().process(JoinStr({{"ggH_hww"}, ggHSBI})).attr({cat},"whad")
    //         .attr({pro},"njet").attr({dec},"decay").attr({"2k1"+y},"year")
    //         .attr({"semilep"},"analysis").attr({"False"},"CR").AddSyst(cb,
    //         "CMS_hww_"+dec+"_"+pro+"_"+cat+"_201"+y+"_correlbin_ggH_hww_"+std::to_string(ibin)+"_stat",
    //         "shape", SystMap<>::init(1.00));

    //      cb.cp().process(JoinStr({qqH, qqHSBI})).attr({cat},"whad")
    //         .attr({pro},"njet").attr({dec},"decay").attr({"2k1"+y},"year")
    //         .attr({"semilep"},"analysis").attr({"False"},"CR").AddSyst(cb,
    //         "CMS_hww_"+dec+"_"+pro+"_"+cat+"_201"+y+"_correlbin_QQH"+model+"_"+std::to_string(ibin)+"_stat",
    //         "shape", SystMap<>::init(1.00));

    //      cb.cp().process(JoinStr({{"qqWWqq"}, qqHSBI})).attr({cat},"whad")
    //         .attr({pro},"njet").attr({dec},"decay").attr({"2k1"+y},"year")
    //         .attr({"semilep"},"analysis").attr({"False"},"CR").AddSyst(cb,
    //         "CMS_hww_"+dec+"_"+pro+"_"+cat+"_201"+y+"_correlbin_qqWWqq_"+std::to_string(ibin)+"_stat",
    //         "shape", SystMap<>::init(1.00));

    //      cb.cp().process(JoinStr({{"qqH_hww"}, qqHSBI})).attr({cat},"whad")
    //         .attr({pro},"njet").attr({dec},"decay").attr({"2k1"+y},"year")
    //         .attr({"semilep"},"analysis").attr({"False"},"CR").AddSyst(cb,
    //         "CMS_hww_"+dec+"_"+pro+"_"+cat+"_201"+y+"_correlbin_qqH_hww_"+std::to_string(ibin)+"_stat",
    //         "shape", SystMap<>::init(1.00));
    // }
    // }
    // }
    // }
    // }


}
}
