#include <string>
#include <map>
#include <set>
#include <iostream>
#include <utility>
#include <vector>
#include <cstdlib>
#include <stdexcept>
#include "boost/algorithm/string/predicate.hpp"
#include "boost/program_options.hpp"
#include "boost/lexical_cast.hpp"
#include "boost/regex.hpp"
#include "CombineHarvester/CombineTools/interface/CombineHarvester.h"
#include "CombineHarvester/CombineTools/interface/Observation.h"
#include "CombineHarvester/CombineTools/interface/Process.h"
#include "CombineHarvester/CombineTools/interface/Utilities.h"
#include "CombineHarvester/CombineTools/interface/CardWriter.h"
#include "CombineHarvester/CombineTools/interface/Systematics.h"
#include "CombineHarvester/CombineTools/interface/BinByBin.h"
#include "CombineHarvester/CombineTools/interface/Algorithm.h"
#include "CombineHarvester/CombineTools/interface/AutoRebin.h"
#include "CombineHarvester/CombinePdfs/interface/MorphFunctions.h"
#include "CombineHarvester/HWW/interface/HwwSystematics_MSSMRun2.h"
#include "RooWorkspace.h"
#include "RooRealVar.h"
#include "TH2.h"
#include "TF1.h"
#include "TMath.h"

using namespace std;
using boost::starts_with;
namespace po = boost::program_options;


bool IsCompletelyZero(const TH1* const shape) {
    // which one?
    return shape->GetEntries()==0.0;
    // return shape->GetSumOfWeights()==0.0;

    // bool onlyPositive{ true };
    // Double_t integral{ shape->ComputeIntegral(onlyPositive) };
    // if (TMath::IsNaN(integral)) {
    //     throw std::runtime_error("Negative bin content in shape!");
    // }
    // return integral == 0;
}

void SymmetrizeSystematic(const TH1* const nominal, TH1* const up, TH1* const down) {

    unique_ptr<TH1> tempMean{static_cast<TH1*>(nominal->Clone())};

    tempMean->Add(up, down, 0.5, -0.5);
    up->Add(nominal, tempMean.get(), 1, 1);
    down->Add(nominal, tempMean.get(), 1, -1);
}


template <typename T>
void To1Bin(T* proc)
{
    std::unique_ptr<TH1> originalHist = proc->ClonedScaledShape();
    TH1D *hist = new TH1D("hist","hist",1,0,1);
    double err = 0;
    double rate =
        originalHist->IntegralAndError(0, originalHist->GetNbinsX() + 1, err);
    hist->SetDirectory(0);
    hist->SetBinContent(1, rate);
    hist->SetBinError(1, err);
    proc->set_shape(*hist, true);  // True means adjust the process rate to the
                                   // integral of the hist
}

//Treatment is different for single bin control regions than for multi-bin cr's
//Introduce a BinIsSBControlRegion to filter out the single bin cr's and
//Let BinIsControlRegion filter all control regions

//
bool BinIsControlRegion(ch::Object const* obj)
{
    return ( (obj->channel().find("dy") != std::string::npos) || (obj->channel().find("top") != std::string::npos) || (obj->channel().find("wj") != std::string::npos) );
}

bool BinIsNotControlRegion(ch::Object const* obj)
{
    return !BinIsControlRegion(obj);
}

bool BinIsSBIcontribution(ch::Object const* obj)
{
    return ( (obj->process().find("ggH_hww") != std::string::npos) || (obj->process().find("ggH_hww") != std::string::npos) || (obj->process().find("ggWW") != std::string::npos) || (obj->process().find("qqWWqq") != std::string::npos) );
}
bool BinIsNotSBIcontribution(ch::Object const* obj)
{
    return !BinIsSBIcontribution(obj);
}


int main(int argc, char** argv) {
  // First define the location of the "auxiliaries" directory where we can
  // source the input files containing the datacard shapes
  string SM125= "";
  string mass = "mH"; // Should be "MH" for indep
  string output_folder = "FullRun2";
  string model = "_RelW002";//"_c10brn00"; // RelW0.05 TODO Temp Semilep
  string shapedir = "";
  bool auto_rebin = false;
  bool manual_rebin = false;
  bool real_data = false;
  bool check_neg_bins = false; // I think this does nothing without auto_rebin? Neg bin fixing is done anyway afterwards
  bool do_bbb = true;
  bool poisson_bbb = false;
  bool removeSigFromCR = false;
  bool remove0yields = true;
  bool lnNforshapesinCR = true;
  bool combcard_only = true;
  bool highindep = false;
  bool do2018 = false;
  bool do2017 = false;
  bool do2016 = false;
  bool do2016semi = false;
  bool do2017semi = false;
  bool do2018semi = false;
  bool DNNdiscrim = true;
  po::variables_map vm;
  po::options_description config("configuration");
  config.add_options()
    ("mass,m", po::value<string>(&mass)->default_value(mass))
    ("auto_rebin", po::value<bool>(&auto_rebin)->default_value(auto_rebin))
    ("manual_rebin", po::value<bool>(&manual_rebin)->default_value(manual_rebin))
    ("real_data", po::value<bool>(&real_data)->default_value(real_data))
    ("output_folder", po::value<string>(&output_folder)->default_value(output_folder))
    ("SM125,h", po::value<string>(&SM125)->default_value(SM125))
    ("check_neg_bins", po::value<bool>(&check_neg_bins)->default_value(check_neg_bins))
    ("do_bbb", po::value<bool>(&do_bbb)->default_value(do_bbb))
    ("poisson_bbb", po::value<bool>(&poisson_bbb)->default_value(poisson_bbb))
    ("remove0yields", po::value<bool>(&remove0yields)->default_value(remove0yields))
    ("lnNforshapesinCR", po::value<bool>(&lnNforshapesinCR)->default_value(lnNforshapesinCR))
    ("combcard_only", po::value<bool>(&combcard_only)->default_value(combcard_only))
    ("highindep", po::value<bool>(&highindep)->default_value(highindep))
    ("do2018", po::value<bool>(&do2018)->default_value(do2018))
    ("do2017", po::value<bool>(&do2017)->default_value(do2017))
    ("do2016", po::value<bool>(&do2016)->default_value(do2016))
    ("do2018semi", po::value<bool>(&do2018semi)->default_value(do2018semi))
    ("do2017semi", po::value<bool>(&do2017semi)->default_value(do2017semi))
    ("do2016semi", po::value<bool>(&do2016semi)->default_value(do2016semi))
    ("DNNdiscrim", po::value<bool>(&DNNdiscrim)->default_value(DNNdiscrim));
  po::store(po::command_line_parser(argc, argv).options(config).run(), vm);
  po::notify(vm);

  typedef vector<string> VString;
  typedef vector<pair<int, string>> Categories;
  std::map<string, string> input_dir;
  shapedir = "/afs/cern.ch/work/s/ssiebert/public/"+shapedir+"/";

  if (do2018){
    input_dir["em8"]     = shapedir+"plots_Full2018_em_new.root";
    input_dir["em8_top"] = input_dir["em8"];
    input_dir["em8_dy"]  = input_dir["em8"];
    input_dir["ee8"]     = shapedir+"plots_Full2018_ee_new.root";
    input_dir["ee8_top"] = input_dir["ee8"];
    input_dir["ee8_dy"]  = input_dir["ee8"];
    input_dir["mm8"]     = shapedir+"plots_Full2018_mm_new.root";
    input_dir["mm8_top"] = input_dir["mm8"];
    input_dir["mm8_dy"]  = input_dir["mm8"];
  }
  if (do2017){
    input_dir["em7"]     = shapedir+"plots_Full2017_em_new.root";
    input_dir["em7_top"] = input_dir["em7"];
    input_dir["em7_dy"]  = input_dir["em7"];
    input_dir["ee7"]     = shapedir+"plots_Full2017_ee_new.root";
    input_dir["ee7_top"] = input_dir["ee7"];
    input_dir["ee7_dy"]  = input_dir["ee7"];
    input_dir["mm7"]     = shapedir+"plots_Full2017_mm_new.root";
    input_dir["mm7_top"] = input_dir["mm7"];
    input_dir["mm7_dy"]  = input_dir["mm7"];
  }
  if (do2016){
    input_dir["em6"]     = shapedir+"plots_Full2016_em_new.root";
    input_dir["em6_top"] = input_dir["em6"];
    input_dir["em6_dy"]  = input_dir["em6"];
    input_dir["ee6"]     = shapedir+"plots_Full2016_ee_new.root";
    input_dir["ee6_top"] = input_dir["ee6"];
    input_dir["ee6_dy"]  = input_dir["ee6"];
    input_dir["mm6"]     = shapedir+"plots_Full2016_mm_new.root";
    input_dir["mm6_top"] = input_dir["mm6"];
    input_dir["mm6_dy"]  = input_dir["mm6"];
  }
  if (do2016semi){
    input_dir["eqq6"]        = shapedir+"plots_HMlnjj_2016.root";
    input_dir["eqq6_top"]    = input_dir["eqq6"];
    input_dir["eqq6_wj"]     = input_dir["eqq6"];
    input_dir["mqq6"]        = input_dir["eqq6"];
    input_dir["mqq6_top"]    = input_dir["eqq6"];
    input_dir["mqq6_wj"]     = input_dir["eqq6"];
    input_dir["eqq6_bs"]     = input_dir["eqq6"];
    input_dir["eqq6_top_bs"] = input_dir["eqq6"];
    input_dir["eqq6_wj_bs"]  = input_dir["eqq6"];
    input_dir["mqq6_bs"]     = input_dir["eqq6"];
    input_dir["mqq6_top_bs"] = input_dir["eqq6"];
    input_dir["mqq6_wj_bs"]  = input_dir["eqq6"];
  }
  if (do2017semi){
    input_dir["eqq7"]        = shapedir+"plots_HMlnjj_2017.root";
    input_dir["eqq7_top"]    = input_dir["eqq7"];
    input_dir["eqq7_wj"]     = input_dir["eqq7"];
    input_dir["mqq7"]        = input_dir["eqq7"];
    input_dir["mqq7_top"]    = input_dir["eqq7"];
    input_dir["mqq7_wj"]     = input_dir["eqq7"];
    input_dir["eqq7_bs"]     = input_dir["eqq7"];
    input_dir["eqq7_top_bs"] = input_dir["eqq7"];
    input_dir["eqq7_wj_bs"]  = input_dir["eqq7"];
    input_dir["mqq7_bs"]     = input_dir["eqq7"];
    input_dir["mqq7_top_bs"] = input_dir["eqq7"];
    input_dir["mqq7_wj_bs"]  = input_dir["eqq7"];
  }

  if (do2018semi){
    input_dir["eqq8"]        = shapedir+"plots_Semilep_2018_new.root";
    input_dir["eqq8_top"]    = input_dir["eqq8"];
    input_dir["eqq8_wj"]     = input_dir["eqq8"];
    input_dir["mqq8"]        = input_dir["eqq8"];
    input_dir["mqq8_top"]    = input_dir["eqq8"];
    input_dir["mqq8_wj"]     = input_dir["eqq8"];
    input_dir["eqq8_bs"]     = input_dir["eqq8"];
    input_dir["eqq8_top_bs"] = input_dir["eqq8"];
    input_dir["eqq8_wj_bs"]  = input_dir["eqq8"];
    input_dir["mqq8_bs"]     = input_dir["eqq8"];
    input_dir["mqq8_top_bs"] = input_dir["eqq8"];
    input_dir["mqq8_wj_bs"]  = input_dir["eqq8"];
  }

  VString chns;

  for (auto it : input_dir){
    chns.push_back(it.first);
  }

  RooRealVar mH(mass.c_str(), mass.c_str(), 115., 5000.);
  mH.setConstant(true);

  map<string, VString> bkg_procs;

  for (auto it : input_dir){
    if(it.first.find("qq") != std::string::npos){ //Semilep
      bkg_procs[it.first] = {"DY", "VVV", "VZ", "Vg", "WW", "WWewk", "VgS_H", "VgS_L", "ggWW", "qqWWqq", "WW2J", "top", "Wjets", "QCD"};
    }else if(it.first.find("em") != std::string::npos){ //Dilep em
      bkg_procs[it.first] = {"DY", "DYemb", "Fake_em", "Fake_me", "VVV", "VZ", "Vg", "WW", "WWewk", "VgS_H", "VgS_L", "ggWW", "qqWWqq", "WW2J", "top"};
    }else if(it.first.find("ee") != std::string::npos){ //Dilep ee
      bkg_procs[it.first] = {"DY",          "Fake_ee",            "VVV", "VZ", "Vg", "WW", "WWewk", "VgS_H", "VgS_L", "ggWW", "qqWWqq", "WW2J", "top"};
    }else if(it.first.find("mm") != std::string::npos){ //Dilep mm
      bkg_procs[it.first] = {"DY",          "Fake_mm",            "VVV", "VZ", "Vg", "WW", "WWewk", "VgS_H", "VgS_L", "ggWW", "qqWWqq", "WW2J", "top"};
    }
  }

  VString SM_procs = {"ggH_hww", "qqH_hww", "WH_hww", "ZH_hww", "ggH_htt", "qqH_htt", "WH_htt", "ZH_htt"};
  //VString SM_procs = {"ggH_hww", "qqH_hww", "WH_hww", "ZH_hww", "ggH_htt", "qqH_htt", "ZH_htt"}; // TODO Semilep add "WH_htt"

  // Create an empty CombineHarvester instance that will hold all of the
  // datacard configuration and histograms etc.
  ch::CombineHarvester cb;
  // Uncomment this next line to see a *lot* of debug information
  // cb.SetVerbosity(3);

  // Here we will just define two categories for an 8TeV analysis. Each entry in
  // the vector below specifies a bin name and corresponding bin_id.
  //
  map<string,Categories> cats;
  if (!highindep){

  for(auto year: std::vector<std::string> {"8", "7", "6"}){
  cats["em"+year+"_13TeV"] = {
    {6, "em_ggh"},
    {7, "em_vbf"},
    };
  cats["em"+year+"_top_13TeV"] = {
    {10, "em_top_ggh"},
    {11, "em_top_vbf"},
    };
  cats["em"+year+"_dy_13TeV"] = {
    {12, "em_dy_ggh"},
    {13, "em_dy_vbf"},
    };
  cats["ee"+year+"_13TeV"] = {
    {6, "ee_ggh"},
    {7, "ee_vbf"},
    };
  cats["ee"+year+"_top_13TeV"] = {
    {10, "ee_top_ggh"},
    {11, "ee_top_vbf"},
    };
  cats["ee"+year+"_dy_13TeV"] = {
    {12, "ee_dy_ggh"},
    {13, "ee_dy_vbf"},
    };
  cats["mm"+year+"_13TeV"] = {
    {6, "mm_ggh"},
    {7, "mm_vbf"},
    };
  cats["mm"+year+"_top_13TeV"] = {
    {10, "mm_top_ggh"},
    {11, "mm_top_vbf"},
    };
  cats["mm"+year+"_dy_13TeV"] = {
    {12, "mm_dy_ggh"},
    {13, "mm_dy_vbf"},
    };
  }

  }else{

  for(auto year: std::vector<std::string> {"8", "7", "6"}){
  cats["em"+year+"_13TeV"] = {
    {6, "em_highggh"},
    {7, "em_highvbf"},
    };
  cats["em"+year+"_top_13TeV"] = {
    {10, "em_top_highggh"},
    {11, "em_top_highvbf"},
    };
//  cats["em"+year+"_dy_13TeV"] = {
//    {12, "em_dy_highggh"},
//    {13, "em_dy_highvbf"},
//    };
  cats["ee"+year+"_13TeV"] = {
    {6, "ee_highggh"},
    {7, "ee_highvbf"},
    };
  cats["ee"+year+"_top_13TeV"] = {
    {10, "ee_top_highggh"},
    {11, "ee_top_highvbf"},
    };
  //cats["ee"+year+"_dy_13TeV"] = {
  //  {12, "ee_dy_highggh"},
  //  {13, "ee_dy_highvbf"},
  //  };
  cats["mm"+year+"_13TeV"] = {
    {6, "mm_highggh"},
    {7, "mm_highvbf"},
    };
  cats["mm"+year+"_top_13TeV"] = {
    {10, "mm_top_highggh"},
    {11, "mm_top_highvbf"},
    };
  //cats["mm"+year+"_dy_13TeV"] = {
  //  {12, "mm_dy_highggh"},
  //  {13, "mm_dy_highvbf"},
  //  };
  }

  }

  for(auto year: std::vector<std::string> {"8", "7", "6"}){
/*  cats["eqq"+year+"_13TeV"] = {
    {6, "ElCh_Untagged_ResolvedSR_"},
    {7, "ElCh_VBF_ResolvedSR_"},
    };
  cats["eqq"+year+"_top_13TeV"] = {
    {10, "ElCh_Untagged_ResolvedTopCR_"},
    {11, "ElCh_VBF_ResolvedTopCR_"},
    };
  cats["eqq"+year+"_wj_13TeV"] = {
    {12, "ElCh_Untagged_ResolvedSB_"},
    {13, "ElCh_VBF_ResolvedSB_"},
    };
  cats["mqq"+year+"_13TeV"] = {
    {6, "MuCh_Untagged_ResolvedSR_"},
    {7, "MuCh_VBF_ResolvedSR_"},
    };
  cats["mqq"+year+"_top_13TeV"] = {
    {10, "MuCh_Untagged_ResolvedTopCR_"},
    {11, "MuCh_VBF_ResolvedTopCR_"},
    };
  cats["mqq"+year+"_wj_13TeV"] = {
    {12, "MuCh_Untagged_ResolvedSB_"},
    {13, "MuCh_VBF_ResolvedSB_"},
    };
  cats["eqq"+year+"_bs_13TeV"] = {
    {6, "ElCh_Untagged_BoostedSR_"},
    {7, "ElCh_VBF_BoostedSR_"},
    };
  cats["eqq"+year+"_top_bs_13TeV"] = {
    {10, "ElCh_Untagged_BoostedTopCR_"},
    {11, "ElCh_VBF_BoostedTopCR_"},
    };
  cats["eqq"+year+"_wj_bs_13TeV"] = {
    {12, "ElCh_Untagged_BoostedSB_"},
    {13, "ElCh_VBF_BoostedSB_"},
    };
  cats["mqq"+year+"_bs_13TeV"] = {
    {6, "MuCh_Untagged_BoostedSR_"},
    {7, "MuCh_VBF_BoostedSR_"},
    };
  cats["mqq"+year+"_top_bs_13TeV"] = {
    {10, "MuCh_Untagged_BoostedTopCR_"},
    {11, "MuCh_VBF_BoostedTopCR_"},
    };
  cats["mqq"+year+"_wj_bs_13TeV"] = {
    {12, "MuCh_Untagged_BoostedSB_"},
    {13, "MuCh_VBF_BoostedSB_"},
    };*/

  cats["eqq"+year+"_13TeV"] = {
    {6, "ElCh_incl_ResolvedSR_"},
    //{7, "ElCh_VBF_ResolvedSR_"},
    };
  cats["eqq"+year+"_top_13TeV"] = {
    {10, "ElCh_incl_ResolvedTopCR_"},
    //{11, "ElCh_VBF_ResolvedTopCR_"},
    };
  cats["eqq"+year+"_wj_13TeV"] = {
    {12, "ElCh_incl_ResolvedSB_"},
    //{13, "ElCh_VBF_ResolvedSB_"},
    };
  cats["mqq"+year+"_13TeV"] = {
    {6, "MuCh_incl_ResolvedSR_"},
    //{7, "MuCh_VBF_ResolvedSR_"},
    };
  cats["mqq"+year+"_top_13TeV"] = {
    {10, "MuCh_incl_ResolvedTopCR_"},
    //{11, "MuCh_VBF_ResolvedTopCR_"},
    };
  cats["mqq"+year+"_wj_13TeV"] = {
    {12, "MuCh_incl_ResolvedSB_"},
    //{13, "MuCh_VBF_ResolvedSB_"},
    };
  cats["eqq"+year+"_bs_13TeV"] = {
    {6, "ElCh_all6_HMSR_"},
    //{7, "ElCh_VBF_BoostedSR_"},
    };
  cats["eqq"+year+"_top_bs_13TeV"] = {
    {10, "ElCh_all6_HMTopCR_"},
    //{11, "ElCh_VBF_BoostedTopCR_"},
    };
  cats["eqq"+year+"_wj_bs_13TeV"] = {
    {12, "ElCh_all6_HMSB_"},
    //{13, "ElCh_VBF_BoostedSB_"},
    };
  cats["mqq"+year+"_bs_13TeV"] = {
    {6, "MuCh_all6_HMSR_"},
    //{7, "MuCh_VBF_BoostedSR_"},
    };
  cats["mqq"+year+"_top_bs_13TeV"] = {
    {10, "MuCh_all6_HMTopCR_"},
    //{11, "MuCh_VBF_BoostedTopCR_"},
    };
  cats["mqq"+year+"_wj_bs_13TeV"] = {
    {12, "MuCh_all6_HMSB_"},
    //{13, "MuCh_VBF_BoostedSB_"},
    };
  }

  map<string, VString> masses;

  for (auto it : input_dir){
    if(it.first.find("qq") != std::string::npos && it.first.find("bs") != std::string::npos){ //Semilep boosted
      masses[it.first] = {"400", "450", "500", "550", "600", "650", "700", "750", "800", "900", "1000", "1500", "2000", "2500", "3000", "4000", "5000"};
    }else if(it.first.find("qq") != std::string::npos && it.first.find("bs") == std::string::npos){ //Semilep resolved
      masses[it.first] = {"115", "120", "124", "125", "126", "130", "135", "140", "145", "150", "155", "160", "165", "170", "175", "180", "190", "200", "210", "230", "250", "270", "300", "350", "400", "450", "500", "550", "600"};
    }else if(it.first.find("6") == std::string::npos){ //Dilep 2017/18
      masses[it.first] = {"115", "120", "124", "125", "126", "130", "135", "140", "145", "150", "155", "160", "165", "170", "175", "180", "190", "200", "210", "230", "250", "270", "300", "350", "400", "450", "500", "550", "600", "650", "700", "750", "800", "900", "1000", "1500", "2000", "2500", "3000", "4000", "5000"};
    }else if(it.first.find("ee") != std::string::npos){ //Dilep 2016
      masses[it.first] = {"115", "120", "124", "125", "126", "130", "135", "140", "145", "150", "155", "160", "165", "170", "175", "180", "190", "200", "210", "230", "250", "270", "300", "350", "400", "450", "500", "550", "600", "650", "700", "750", "800", "900", "1000", "1500", "2000", "2500", "3000", "4000"};
    }
  }


  map<string, VString> signal_types = {
    {"ggH", {"ggH_HWW", "ggH_HWWSBI"}},
    {"qqH", {"qqH_HWW", "qqH_HWWSBI"}}
  };

  for(auto chn : chns){
    cb.AddObservations({"*"}, {"hww"}, {"13TeV"}, {chn}, cats[chn+"_13TeV"]);

    cb.AddProcesses({"*"}, {"hww"}, {"13TeV"}, {chn}, bkg_procs[chn], cats[chn+"_13TeV"], false);

    cb.AddProcesses(masses[chn], {"hww"}, {"13TeV"}, {chn}, signal_types["ggH"], cats[chn+"_13TeV"], true);
    cb.AddProcesses(masses[chn], {"hww"}, {"13TeV"}, {chn}, signal_types["qqH"], cats[chn+"_13TeV"], true);
    cb.AddProcesses({"*"}, {"hww"}, {"13TeV"}, {chn}, SM_procs, cats[chn+"_13TeV"], false);
    }

  // Filter signal processes in CR -> Avoid ugly potential negative signal bins
  if (removeSigFromCR){
  cb.FilterAll([](ch::Object const* obj) {
          return (BinIsControlRegion(obj) && obj->signal());
          });
  }


  ch::AddMSSMFullRun2Systematics(cb, highindep, DNNdiscrim, model);
  //! [part7]

  for (string chn : chns) {

  if(chn.find("qq") != std::string::npos){    //Semilep

    std::string discrim = "resolvHiggsMass";
    if (chn.find("bs") != std::string::npos) discrim = "boostHigssMass"; // TODO typo
    if (chn.find("top") != std::string::npos || chn.find("wj") != std::string::npos) discrim = "events";

    cb.cp().channel({chn}).backgrounds().ExtractShapes(
        input_dir[chn],
        "$BIN/"+discrim+"/histo_$PROCESS",
        "$BIN/"+discrim+"/histo_$PROCESS_$SYSTEMATIC");

    cb.cp().channel({chn}).process({"ggH_HWW"}).ExtractShapes(
        input_dir[chn],
        "$BIN/"+discrim+"/histo_GGH_$MASS"+model,
        "$BIN/"+discrim+"/histo_GGH_$MASS"+model+"_$SYSTEMATIC");
    cb.cp().channel({chn}).process({"ggH_HWWSBI"}).ExtractShapes(
        input_dir[chn],
        "$BIN/"+discrim+"/histo_GGHSBI_$MASS"+model,
        "$BIN/"+discrim+"/histo_GGHSBI_$MASS"+model+"_$SYSTEMATIC");
    cb.cp().channel({chn}).process({"qqH_HWW"}).ExtractShapes(
        input_dir[chn],
        "$BIN/"+discrim+"/histo_QQH_$MASS"+model,
        "$BIN/"+discrim+"/histo_QQH_$MASS"+model+"_$SYSTEMATIC");
    cb.cp().channel({chn}).process({"qqH_HWWSBI"}).ExtractShapes(
        input_dir[chn],
        "$BIN/"+discrim+"/histo_QQHSBI_$MASS"+model,
        "$BIN/"+discrim+"/histo_QQHSBI_$MASS"+model+"_$SYSTEMATIC");

  }else{    //Dilep

    std::string discrim = "events";
    if( chn.find("top") == std::string::npos && chn.find("dy") == std::string::npos ){
      discrim = "mTi_binning";
      if (highindep) discrim = "mTi_highbinning";
      if (DNNdiscrim) discrim = "DNN_mth_binning";
      if (DNNdiscrim && highindep) discrim = "DNN_mth_highbinning";
    }

    cb.cp().channel({chn}).backgrounds().ExtractShapes(
        input_dir[chn],
        "hww2l2v_13TeV_$BIN/"+discrim+"/histo_$PROCESS",
        "hww2l2v_13TeV_$BIN/"+discrim+"/histo_$PROCESS_$SYSTEMATIC");

    cb.cp().channel({chn}).process({"ggH_HWW"}).ExtractShapes(
        input_dir[chn],
        "hww2l2v_13TeV_$BIN/"+discrim+"/histo_GGH_$MASS"+model,
        "hww2l2v_13TeV_$BIN/"+discrim+"/histo_GGH_$MASS"+model+"_$SYSTEMATIC");
    cb.cp().channel({chn}).process({"ggH_HWWSBI"}).ExtractShapes(
        input_dir[chn],
        "hww2l2v_13TeV_$BIN/"+discrim+"/histo_GGHSBI_$MASS"+model,
        "hww2l2v_13TeV_$BIN/"+discrim+"/histo_GGHSBI_$MASS"+model+"_$SYSTEMATIC");
    cb.cp().channel({chn}).process({"qqH_HWW"}).ExtractShapes(
        input_dir[chn],
        "hww2l2v_13TeV_$BIN/"+discrim+"/histo_QQH_$MASS"+model,
        "hww2l2v_13TeV_$BIN/"+discrim+"/histo_QQH_$MASS"+model+"_$SYSTEMATIC");
    cb.cp().channel({chn}).process({"qqH_HWWSBI"}).ExtractShapes(
        input_dir[chn],
        "hww2l2v_13TeV_$BIN/"+discrim+"/histo_QQHSBI_$MASS"+model,
        "hww2l2v_13TeV_$BIN/"+discrim+"/histo_QQHSBI_$MASS"+model+"_$SYSTEMATIC");

  }

  }


 if (remove0yields){
  //Now delete processes with 0 yield
  cb.FilterProcs([&](ch::Process *p) {
   bool null_yield = !(p->rate() > 0.) && !(p->signal()); // Don't remove ANY signal (even in CR), otherwise problem if low/high edge mass missing
   if (null_yield){
      std::cout << "[Null yield] Removing process with null yield: \n ";
      std::cout << ch::Process::PrintHeader << *p << "\n";
      cb.FilterSysts([&](ch::Systematic *s){
        bool remove_syst = (MatchingProcess(*p,*s));
        return remove_syst;
     });
   }
   return null_yield;
  });
 }

  //Remove systematics that lead to Bogus norms
  std::cout << "[Zero Content] Doing this step now..." << "\n";

/*
  cb.ForEachSyst([&](ch::Systematic *s) {
    if (s->type().find("shape") == std::string::npos) return;
    //  std::cout << "checking" << s->bin()
    //            << "," << s->process() << "," << s->name() << "\n";
    if (IsCompletelyZero(s->shape_u()) || IsCompletelyZero(s->shape_d())) {
      std::cout << "[Zero Content] Removing shape syst" << s->bin()
                << "," << s->process() << "," << s->name() << "\n";

     cb.FilterSysts([&](ch::Systematic *s2){
        bool remove_syst = (MatchingProcess(*s,*s2));
        return remove_syst;
     });
    }
  });*/

  cb.FilterSysts([&](ch::Systematic *s){
    //std::cout << "checking " << s->bin() << "," << s->process() << "," << s->name() << "\n";
    bool remove_syst = (s->type() == "shape");
    if (remove_syst){remove_syst = (s->process().find("HWW") == std::string::npos);} // Don't remove empty systematics from my signal -> Important because it won't work if it removes systematics for only SOME mass points
    if (remove_syst){remove_syst = (IsCompletelyZero(s->shape_u()) || IsCompletelyZero(s->shape_d()));}
    if (remove_syst){std::cout << "[Zero Content] Removing shape syst " << s->bin() << "," << s->process() << "," << s->name() << "\n";}
    return remove_syst;
  });

  // ... but if signal systematic is bad (0 content), then fix it!
  cb.ForEachSyst([&](ch::Systematic *s) {
    if (s->type().find("shape") == std::string::npos) return;
    //if (s->process().find("HWW") != std::string::npos) return;
    //std::cout << "Example Up/Down:" << s->value_u() << " , " << s->value_d() << "\n";
    if (IsCompletelyZero(s->shape_u()) || IsCompletelyZero(s->shape_d())) {

      cb.ForEachProc([&](ch::Process *p) {
        if (MatchingProcess(*p,*s) && !IsCompletelyZero(p->shape())){
          std::cout << "[Null bins] Fixing null bins for syst" << s->bin()
                << "," << s->process() << "," << s->name() << "\n";
          std::cout << "[Null bins] Before:\n";
          s->shape_u()->Print("range");
          s->shape_d()->Print("range");
          p->shape()->Print("range");
          auto newhist_u = s->ClonedShapeU();
          auto newhist_d = s->ClonedShapeD();
          auto newhist = p->ClonedShape();
          std::cout << "[Null bins] Values Up/Down:" << s->value_u() << " , " << s->value_d() << "\n";
          if (!IsCompletelyZero(s->shape_u()) && IsCompletelyZero(s->shape_d())) {
            std::cout << "[Null bins] Up good, Down bad:\n";
            SymmetrizeSystematic(p->shape(), newhist_u.get(), newhist_d.get());
            s->set_shapes(std::move(newhist_u), std::move(newhist_d), nullptr);
            s->set_value_d(1.0/s->value_u());
          }else if (IsCompletelyZero(s->shape_u()) && !IsCompletelyZero(s->shape_d())) {
            std::cout << "[Null bins] Up bad, Down good:\n";
            SymmetrizeSystematic(p->shape(), newhist_d.get(), newhist_u.get());
            s->set_shapes(std::move(newhist_u), std::move(newhist_d), nullptr);
            s->set_value_u(1.0/s->value_d());
          }else if (IsCompletelyZero(s->shape_u()) && IsCompletelyZero(s->shape_d())) {
            std::cout << "[Null bins] Up bad, Down bad:\n";
            //s->set_shapes(std::move(p->shape()), std::move(p->shape()), nullptr);
            //s->set_shapes(p->shape(), p->shape(), nullptr);
            auto newhist2 = p->ClonedShape();
            s->set_shapes(std::move(newhist), std::move(newhist2), nullptr);
            s->set_value_u(1.0);
            s->set_value_d(1.0);
          }
          std::cout << "[Null bins] After:\n";
          s->shape_u()->Print("range");
          s->shape_d()->Print("range");
          std::cout << "[Null bins] Values Up/Down:" << s->value_u() << " , " << s->value_d() << "\n";
        }
      });

    }
  });


  if (lnNforshapesinCR){
    // And convert any shapes in the CRs to lnN:
    cb.cp().FilterSysts(BinIsNotControlRegion).syst_type({"shape"}).ForEachSyst([](ch::Systematic *sys) {
      sys->set_type("lnN");
    });
  }

   //Replacing observation with the sum of the backgrounds (asimov) - nice to ensure blinding
    auto bins = cb.cp().bin_set();
    // For control region bins data (should) = sum of bkgs already
    // useful to be able to check this, so don't do the replacement
    // for these
  if(!real_data){
      for (auto b : cb.cp().FilterAll(BinIsControlRegion).bin_set()) {
          std::cout << " - Replacing data with asimov in bin " << b << "\n";
          cb.cp().bin({b}).ForEachObs([&](ch::Observation *obs) {
            obs->set_shape(cb.cp().bin({b}).backgrounds().GetShape(), true);
          });
        }
  }


  // Merge to one bin for control region bins
  // Done already? Commeting out for now for that reason...
  //cb.cp().FilterAll(BinIsNotControlRegion).ForEachProc(To1Bin<ch::Process>);
  //cb.cp().FilterAll(BinIsNotControlRegion).ForEachObs(To1Bin<ch::Observation>);

  // Rebinning
  // --------------------
  // Keep track of shapes before and after rebinning for comparison
  // and for checking the effect of negative bin contents
  std::map<std::string, TH1F> before_rebin;
  std::map<std::string, TH1F> after_rebin;
  std::map<std::string, TH1F> after_rebin_neg;
  if (check_neg_bins) {
    for (auto b : bins) {
      before_rebin[b] = cb.cp().bin({b}).backgrounds().GetShape();
    }
  }


  auto rebin = ch::AutoRebin()
    .SetBinThreshold(0.)
    .SetBinUncertFraction(0.9)
    .SetRebinMode(1)
    .SetPerformRebin(true)
    .SetVerbosity(1);
  if(auto_rebin) rebin.Rebin(cb, cb);

  vector<double> binning;
  if (highindep){
    //Currently used: [0,400,450,500,550,625,700,800,900,1000,1100,1200,1300,1450,1600,1800,2000,2250,2500,2750,3000,3500]
    binning = {0,400,450,500,550,625,700,800,900,1000,1100,1200,1300,1450,1600,1800,2000,2250,3500};
  }else{
    //Currently used: [0,150,175,200,225,250,280,320,360,400,450,500,550,625,700,800,900,1000,1100,1200,1300,1450,1600,1800,2000,2250,2500,2750,3000,3500]
    binning = {0,150,175,200,225,250,280,320,360,400,450,500,550,625,700,800,900,1000,1100,1200,1300,1450,1600,1800,2000,2250,3500};
  }
  if(manual_rebin) {
    for(auto b : cb.cp().FilterAll(BinIsControlRegion).bin_set()) {
      std::cout << "Rebinning by hand for bin: " << b <<  std::endl;
      cb.cp().bin({b}).VariableRebin(binning);
    }
  }

  if (check_neg_bins) {
    for (auto b : bins) {
      after_rebin[b] = cb.cp().bin({b}).backgrounds().GetShape();
      // std::cout << "Bin: " << b << " (before)\n";
      // before_rebin[b].Print("range");
      // std::cout << "Bin: " << b << " (after)\n";
      // after_rebin[b].Print("range");
      // Build a sum-of-bkgs TH1 that doesn't truncate the negative yields
      // like the CH GetShape does
      for (auto p : cb.cp().bin({b}).backgrounds().process_set()) {
        TH1F proc_hist;
        cb.cp().bin({b}).process({p}).ForEachProc([&](ch::Process *proc) {
          proc_hist = proc->ShapeAsTH1F();
          proc_hist.Scale(proc->no_norm_rate());
          for (int i = 1; i <= proc_hist.GetNbinsX(); ++i) {
            if (proc_hist.GetBinContent(i) < 0.) {
              std::cout << p << " bin " << i << ": " << proc_hist.GetBinContent(i) << "\n";
            }
          }
        });
        if (after_rebin_neg.count(b)) {
          after_rebin_neg[b].Add(&proc_hist);
        } else {
          after_rebin_neg[b] = proc_hist;
        }
      }
      std::cout << "Bin: " << b << "\n";
      for (int i = 1; i <= after_rebin[b].GetNbinsX(); ++i) {
        double offset = after_rebin[b].GetBinContent(i) - after_rebin_neg[b].GetBinContent(i);
        double offset_by_yield = offset / after_rebin[b].GetBinContent(i);
        double offset_by_err = offset / after_rebin[b].GetBinError(i);
        printf("%-2i offset %-10.4f tot %-10.4f err %-10.4f off/tot %-10.4f off/err %-10.4f\n", i , offset, after_rebin[b].GetBinContent(i), after_rebin[b].GetBinError(i), offset_by_yield, offset_by_err);
      }
    }
  }

  // Uncomment this to inject 1 obs event in the last bin of every signal-region
  // category
  // if(!real_data){
  //     for (auto b : cb.cp().FilterAll(BinIsControlRegion).bin_set()) {
  //       std::cout << " - Adjusting data in bin " << b << "\n";
  //         cb.cp().bin({b}).ForEachObs([&](ch::Observation *obs) {
  //           TH1F new_obs = cb.cp().bin({b}).GetObservedShape();
  //           new_obs.SetBinContent(new_obs.GetNbinsX(), 1.);
  //           new_obs.Print("range");
  //           obs->set_shape(new_obs, true);
  //         });
  //       }
  // }

  // At this point we can fix the negative bins
  std::cout << "Fixing negative bins\n";
  cb.ForEachProc([](ch::Process *p) {
    if (ch::HasNegativeBins(p->shape())) {
      std::cout << "[Negative bins] Fixing negative bins for " << p->channel() << "," << p->bin()
                << "," << p->process() << "\n";
      std::cout << "[Negative bins] Before:\n";
      p->shape()->Print("range");
      auto newhist = p->ClonedShape();
      ch::ZeroNegativeBins(newhist.get());
      // Set the new shape but do not change the rate, we want the rate to still
      // reflect the total integral of the events
      p->set_shape(std::move(newhist), false);
      std::cout << "[Negative bins] After:\n";
      p->shape()->Print("range");
    }
  });

  cb.ForEachSyst([](ch::Systematic *s) {
    if (s->type().find("shape") == std::string::npos) return;
    if (ch::HasNegativeBins(s->shape_u()) || ch::HasNegativeBins(s->shape_d())) {
      std::cout << "[Negative bins] Fixing negative bins for syst" << s->bin()
                << "," << s->process() << "," << s->name() << "\n";
      /*std::cout << "[Negative bins] Before:\n";
      s->shape_u()->Print("range");
      s->shape_d()->Print("range");*/
      auto newhist_u = s->ClonedShapeU();
      auto newhist_d = s->ClonedShapeD();
      ch::ZeroNegativeBins(newhist_u.get());
      ch::ZeroNegativeBins(newhist_d.get());
      // Set the new shape but do not change the rate, we want the rate to still
      // reflect the total integral of the events
      s->set_shapes(std::move(newhist_u), std::move(newhist_d), nullptr);
      /*std::cout << "[Negative bins] After:\n";
      s->shape_u()->Print("range");
      s->shape_d()->Print("range");*/
    }
  });

if (do_bbb){
  cout << "Generating bbb uncertainties...";
  auto bbb = ch::BinByBinFactory()
    .SetPattern("CMS_$ANALYSIS_$BIN_$ERA_$PROCESS_bin_$#")
    .SetAddThreshold(0.)
    .SetMergeThreshold(0.4)
    .SetFixNorm(true)
    .SetMergeSaturatedBins(false)
    .SetPoissonErrors(poisson_bbb);
  for (auto chn : chns) {
    std::cout << " - Doing bbb for channel " << chn << "\n";
    bbb.MergeAndAdd(cb.cp().channel({chn}).backgrounds().FilterAll([](ch::Object const* obj) {
                return BinIsSBIcontribution(obj);
                }), cb);
  }
  // And now do bbb for the control region with a slightly different config:
  //auto bbb_ctl = ch::BinByBinFactory()
  //  .SetPattern("CMS_$ANALYSIS_$BIN_$ERA_$PROCESS_bin_$#")
  //  .SetAddThreshold(0.)
  //  .SetMergeThreshold(0.4)
  //  .SetFixNorm(false)  // contrary to signal region, bbb *should* change yield here
  //  .SetVerbosity(1);
  // Will merge but only for non W and QCD processes, to be on the safe side
  //bbb_ctl.MergeBinErrors(cb.cp().process({"DY", "top"}, false).backgrounds().FilterProcs(BinIsNotControlRegion));
  //bbb_ctl.AddBinByBin(cb.cp().process({"DY", "top"}, false).backgrounds().FilterProcs(BinIsNotControlRegion), cb); // Added .backgrounds() here when not removing signal from CR
  cout << " done\n";
}


  // This function modifies every entry to have a standardised bin name of
  // the form: {analysis}_{channel}_{bin_id}_{era}
  // which is commonly used in the htt analyses
  ch::SetStandardBinNames(cb);
  //! [part8]


  //! [part9]
  // First we generate a set of bin names:
  RooWorkspace ws("hww", "hww");

  string demoname = "hww_mssm_demo_"+output_folder+".root";
  TFile demo(demoname.c_str(), "RECREATE");

  bool do_morphing = true;
  map<string, RooAbsReal *> mass_var = {
    {"ggH_HWW", &mH},
    {"qqH_HWW", &mH},
    {"ggH_HWWSBI", &mH},
    {"qqH_HWWSBI", &mH}
  };


  if (do_morphing) {
    auto bins = cb.bin_set();
    for (auto b : bins) {
      std::cout << "Bin:" << b << "\n";
      auto procs = cb.cp().bin({b}).process(ch::JoinStr({signal_types["ggH"], signal_types["qqH"]})).process_set();
      for (auto p : procs) {
        std::cout << "Process:" << p << "\n";
        ch::BuildRooMorphing(ws, cb, b, p, *(mass_var[p]),
                             "norm", true, false, false, &demo); // Middle bool is verbose
      }
    }
  }


  demo.Close();
  cb.AddWorkspace(ws);
  cb.cp().process(ch::JoinStr({signal_types["ggH"], signal_types["qqH"]})).ExtractPdfs(cb, "hww", "$BIN_$PROCESS_morph");

 //Write out datacards. Naming convention important for rest of workflow. We
 //make one directory per chn-cat, one per chn and cmb. In this code we only
 //store the individual datacards for each directory to be combined later, but
 //note that it's also possible to write out the full combined card with CH <- this TODO
  string output_prefix = "output/";
  string root_suffix = "";
  if (do2016){root_suffix = root_suffix + "16";}
  if (do2017){root_suffix = root_suffix + "17";}
  if (do2018){root_suffix = root_suffix + "18";}
  if (do2016semi){root_suffix = root_suffix + "semi16";}
  if (do2017semi){root_suffix = root_suffix + "semi17";}
  if (do2018semi){root_suffix = root_suffix + "semi18";}
  if(output_folder.compare(0,1,"/") == 0) output_prefix="";
  ch::CardWriter writer(output_prefix + output_folder + "/$TAG/$BIN.txt",
                        output_prefix + output_folder + "/$TAG/hww_input"+root_suffix+".root");
  ch::CardWriter writercmb(output_prefix + output_folder + "/$TAG/combined.txt",
                           output_prefix + output_folder + "/$TAG/hww_input"+root_suffix+".root");
  // We're not using mass as an identifier - which we need to tell the CardWriter
  // otherwise it will see "*" as the mass value for every object and skip it
  writer.SetWildcardMasses({});
  writercmb.SetWildcardMasses({});
  writer.SetVerbosity(1);
  writercmb.SetVerbosity(1);

  writer.WriteCards("cmb", cb);
  writercmb.WriteCards("cmb", cb);

  if (!combcard_only){
    for (auto chn : chns) {
      if (chn.find("dy") != std::string::npos  or chn.find("top") != std::string::npos) {
        continue;
      }
      // Add per channel-category
      //writer.WriteCards("hww_"+chn+"_0jet_13TeV", cb.cp().channel({chn,chn+"_dy",chn+"_top"}).attr({"0j"},"njet"));
      //writer.WriteCards("hww_"+chn+"_1jet_13TeV", cb.cp().channel({chn,chn+"_dy",chn+"_top"}).attr({"1j"},"njet"));
      //writer.WriteCards("hww_"+chn+"_2jet_13TeV", cb.cp().channel({chn,chn+"_dy",chn+"_top"}).attr({"2j"},"njet"));
      writer.WriteCards("hww_"+chn+"_ggF_13TeV", cb.cp().channel({chn,chn+"_dy",chn+"_top"}).attr({"ggh"},"njet"));
      writer.WriteCards("hww_"+chn+"_VBF_13TeV", cb.cp().channel({chn,chn+"_dy",chn+"_top"}).attr({"vbf"},"njet"));
      // Add per channel
      writer.WriteCards("hww_"+chn+"_13TeV", cb.cp().channel({chn,chn+"_dy",chn+"_top"}).attr({chn},"decay"));

      //writercmb.WriteCards("hww_"+chn+"_0jet_13TeV", cb.cp().channel({chn,chn+"_dy",chn+"_top"}).attr({"0j"},"njet"));
      //writercmb.WriteCards("hww_"+chn+"_1jet_13TeV", cb.cp().channel({chn,chn+"_dy",chn+"_top"}).attr({"1j"},"njet"));
      //writercmb.WriteCards("hww_"+chn+"_2jet_13TeV", cb.cp().channel({chn,chn+"_dy",chn+"_top"}).attr({"2j"},"njet"));
      writercmb.WriteCards("hww_"+chn+"_ggF_13TeV", cb.cp().channel({chn,chn+"_dy",chn+"_top"}).attr({"ggh"},"njet"));
      writercmb.WriteCards("hww_"+chn+"_VBF_13TeV", cb.cp().channel({chn,chn+"_dy",chn+"_top"}).attr({"vbf"},"njet"));
      writercmb.WriteCards("hww_"+chn+"_13TeV", cb.cp().channel({chn,chn+"_dy",chn+"_top"}).attr({chn},"decay"));
    }
    // Add per category
    //writer.WriteCards("hww_0jet_13TeV", cb.cp().attr({"0j"},"njet"));
    //writer.WriteCards("hww_1jet_13TeV", cb.cp().attr({"1j"},"njet"));
    //writer.WriteCards("hww_2jet_13TeV", cb.cp().attr({"2j"},"njet"));
    writer.WriteCards("hww_ggF_13TeV", cb.cp().attr({"ggh"},"njet"));
    writer.WriteCards("hww_VBF_13TeV", cb.cp().attr({"vbf"},"njet"));
    //writercmb.WriteCards("hww_0jet_13TeV", cb.cp().attr({"0j"},"njet"));
    //writercmb.WriteCards("hww_1jet_13TeV", cb.cp().attr({"1j"},"njet"));
    //writercmb.WriteCards("hww_2jet_13TeV", cb.cp().attr({"2j"},"njet"));
    writercmb.WriteCards("hww_ggF_13TeV", cb.cp().attr({"ggh"},"njet"));
    writercmb.WriteCards("hww_VBF_13TeV", cb.cp().attr({"vbf"},"njet"));
    // Add per year
    writer.WriteCards("hww_2018_13TeV", cb.cp().attr({"2k18"},"year"));
    writer.WriteCards("hww_2017_13TeV", cb.cp().attr({"2k17"},"year"));
    writer.WriteCards("hww_2016_13TeV", cb.cp().attr({"2k16"},"year"));
    writercmb.WriteCards("hww_2018_13TeV", cb.cp().attr({"2k18"},"year"));
    writercmb.WriteCards("hww_2017_13TeV", cb.cp().attr({"2k17"},"year"));
    writercmb.WriteCards("hww_2016_13TeV", cb.cp().attr({"2k16"},"year"));
    // Add per analysis
    writer.WriteCards("hww2l2nu_2018_13TeV", cb.cp().attr({"dilep"},"analysis"));
    writer.WriteCards("hwwlnuqq_13TeV", cb.cp().attr({"semilep"},"analysis"));
    writercmb.WriteCards("hww2l2nu_13TeV", cb.cp().attr({"dilep"},"analysis"));
    writercmb.WriteCards("hwwlnuqq_13TeV", cb.cp().attr({"semilep"},"analysis"));
  }

  // NEED to split boosed/resolved too
  // bos+res is either full comb, or semilep only
  if (do2016semi or do2017semi or do2018semi){
    writer.WriteCards("hww_boosted_13TeV", cb.cp().attr({"boosted"},"whad"));
    writer.WriteCards("hww_resolved_13TeV", cb.cp().attr({"resolved"},"whad"));
    writercmb.WriteCards("hww_boosted_13TeV", cb.cp().attr({"boosted"},"whad"));
    writercmb.WriteCards("hww_resolved_13TeV", cb.cp().attr({"resolved"},"whad"));
  }

  cb.PrintAll();
  cout << " done\n";
}
