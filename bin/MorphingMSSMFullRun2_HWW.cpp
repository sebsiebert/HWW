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



bool IsCompletelyZero(TH1 const* h) {
  double content = 0.0;
  for (int i = 1; i <= h->GetNbinsX(); ++i) {
    content = content + h->GetBinContent(i);
  }
  bool itszero = false;
  if (content <= 0.0){itszero = true;}
  return itszero;
}

void SymmetrizeSystematic(TH1 const *hnom, TH1 *hsysgood, TH1 *hsysbad) {
  for (int i = 1; i <= hnom->GetNbinsX(); ++i) {
    //std::cout << "Bin " << i << " : Nom " << hnom->GetBinContent(i) << ", Good " << hsysgood->GetBinContent(i) << ", Bad " << hsysbad->GetBinContent(i) << std::endl;
    //std::cout << "New Bin Content will be " << hnom->GetBinContent(i) + (hnom->GetBinContent(i) - hsysgood->GetBinContent(i)) << std::endl;
    hsysbad->SetBinContent(i, hnom->GetBinContent(i) + (hnom->GetBinContent(i) - hsysgood->GetBinContent(i)));
  }
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
  string mass = "MH"; // Should be "MH" for indep
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
  bool DNNdiscrim = false;
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
  shapedir = "/afs/cern.ch/work/s/ssiebert/CMSSW_10_6_4/src/PlotsConfigurations/Configurations/HWWSemiLepHighMass/"+shapedir+"/";

  if (do2016semi){
    input_dir["eqq6"]        = shapedir+"Full2016_v7/rootFile/plots_hmlnjj_2016v7.root";
    input_dir["eqq6_top"]    = input_dir["eqq6"];
    input_dir["eqq6_wj"]     = input_dir["eqq6"];
    input_dir["mqq6"]        = input_dir["eqq6"];
    input_dir["mqq6_top"]    = input_dir["eqq6"];
    input_dir["mqq6_wj"]     = input_dir["eqq6"];
    input_dir["eqq6_bs"]     = shapedir+"Full2016_v7/rootFile/plots_hmlnjj_2016v7.root";
    input_dir["eqq6_top_bs"] = input_dir["eqq6_bs"];
    input_dir["eqq6_wj_bs"]  = input_dir["eqq6_bs"];
    input_dir["mqq6_bs"]     = input_dir["eqq6_bs"];
    input_dir["mqq6_top_bs"] = input_dir["eqq6_bs"];
    input_dir["mqq6_wj_bs"]  = input_dir["eqq6_bs"];
    // input_dir["eqq6_hm"]     = shapedir+"Full2016_v7/rootFile/plots_hmlnjj_2016_v7.root";
    // input_dir["eqq6_top_hm"] = input_dir["eqq6_hm"];
    // input_dir["eqq6_wj_hm"]  = input_dir["eqq6_hm"];
    // input_dir["mqq6_hm"]     = input_dir["eqq6_hm"];
    // input_dir["mqq6_top_hm"] = input_dir["eqq6_hm"];
    // input_dir["mqq6_wj_hm"]  = input_dir["eqq6_hm"];
  }
  if (do2017semi){
    input_dir["eqq7"]        = shapedir+"Full2017_v7/rootFile/plots_hmlnjj_2017v7.root";
    input_dir["eqq7_top"]    = input_dir["eqq7"];
    input_dir["eqq7_wj"]     = input_dir["eqq7"];
    input_dir["mqq7"]        = input_dir["eqq7"];
    input_dir["mqq7_top"]    = input_dir["eqq7"];
    input_dir["mqq7_wj"]     = input_dir["eqq7"];
    input_dir["eqq7_bs"]     = shapedir+"Full2017_v7/rootFile/plots_hmlnjj_2017v7.root";
    input_dir["eqq7_top_bs"] = input_dir["eqq7_bs"];
    input_dir["eqq7_wj_bs"]  = input_dir["eqq7_bs"];
    input_dir["mqq7_bs"]     = input_dir["eqq7_bs"];
    input_dir["mqq7_top_bs"] = input_dir["eqq7_bs"];
    input_dir["mqq7_wj_bs"]  = input_dir["eqq7_bs"];
    // input_dir["eqq7_hm"]     = shapedir+"Full2017_v7/rootFile/plots_hmlnjj_2017_v7.root";
    // input_dir["eqq7_top_hm"] = input_dir["eqq7_hm"];
    // input_dir["eqq7_wj_hm"]  = input_dir["eqq7_hm"];
    // input_dir["mqq7_hm"]     = input_dir["eqq7_hm"];
    // input_dir["mqq7_top_hm"] = input_dir["eqq7_hm"];
    // input_dir["mqq7_wj_hm"]  = input_dir["eqq7_hm"];
  }

  if (do2018semi){
    input_dir["eqq8"]        = shapedir+"Full2018_v7/rootFile/plots_hmlnjj_2018v7.root";
    input_dir["eqq8_top"]    = input_dir["eqq8"];
    input_dir["eqq8_wj"]     = input_dir["eqq8"];
    input_dir["mqq8"]        = input_dir["eqq8"];
    input_dir["mqq8_top"]    = input_dir["eqq8"];
    input_dir["mqq8_wj"]     = input_dir["eqq8"];
    input_dir["eqq8_bs"]     = shapedir+"Full2018_v7/rootFile/plots_hmlnjj_2018v7.root";
    input_dir["eqq8_top_bs"] = input_dir["eqq8_bs"];
    input_dir["eqq8_wj_bs"]  = input_dir["eqq8_bs"];
    input_dir["mqq8_bs"]     = input_dir["eqq8_bs"];
    input_dir["mqq8_top_bs"] = input_dir["eqq8_bs"];
    input_dir["mqq8_wj_bs"]  = input_dir["eqq8_bs"];
    // input_dir["eqq8_hm"]     = shapedir+"Full2018_v7/rootFile/plots_hmlnjj_2018_v7.root";
    // input_dir["eqq8_top_hm"] = input_dir["eqq8_hm"];
    // input_dir["eqq8_wj_hm"]  = input_dir["eqq8_hm"];
    // input_dir["mqq8_hm"]     = input_dir["eqq8_hm"];
    // input_dir["mqq8_top_hm"] = input_dir["eqq8_hm"];
    // input_dir["mqq8_wj_hm"]  = input_dir["eqq8_hm"];
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
      bkg_procs[it.first] = {"DY", "VVV", "VZ", "Vg", "WW", "WWewk", "VgS_H", "VgS_L", "ggWW", "qqWWqq", "WW2J", "top", "Wjets", "QCD", "FAKE"};
    }
  }

  VString SM_procs = {"ggH_hww", "qqH_hww", "WH_hww", "ZH_hww", "ggH_htt", "qqH_htt", "WH_htt", "ZH_htt"};

  // Create an empty CombineHarvester instance that will hold all of the
  // datacard configuration and histograms etc.
  ch::CombineHarvester cb;
  // Uncomment this next line to see a *lot* of debug information
  // cb.SetVerbosity(3);

  // Here we will just define two categories for an 8TeV analysis. Each entry in
  // the vector below specifies a bin name and corresponding bin_id.
  //
  map<string,Categories> cats;

  for(auto year: std::vector<std::string> {"8", "7", "6"}){

  cats["eqq"+year+"_13TeV"] = {
    {6, "ElCh_GGF_ResolvedSR_"},
    {7, "ElCh_VBF_ResolvedSR_"},
    };
  cats["eqq"+year+"_top_13TeV"] = {
    {10, "ElCh_GGF_ResolvedTopCR_"},
    {11, "ElCh_VBF_ResolvedTopCR_"},
    };
  cats["eqq"+year+"_wj_13TeV"] = {
    {12, "ElCh_GGF_ResolvedSB_"},
    {13, "ElCh_VBF_ResolvedSB_"},
    };
  cats["mqq"+year+"_13TeV"] = {
    {6, "MuCh_GGF_ResolvedSR_"},
    {7, "MuCh_VBF_ResolvedSR_"},
    };
  cats["mqq"+year+"_top_13TeV"] = {
    {10, "MuCh_GGF_ResolvedTopCR_"},
    {11, "MuCh_VBF_ResolvedTopCR_"},
    };
  cats["mqq"+year+"_wj_13TeV"] = {
    {12, "MuCh_GGF_ResolvedSB_"},
    {13, "MuCh_VBF_ResolvedSB_"},
    };
  
  auto deepAK8_tagging = true;
  if (! deepAK8_tagging) {
    cats["eqq"+year+"_bs_13TeV"] = {
      {6, "ElCh_GGF_BoostedSR_"},
      {7, "ElCh_VBF_BoostedSR_"},
      };
    cats["eqq"+year+"_top_bs_13TeV"] = {
      {10, "ElCh_GGF_BoostedTopCR_"},
      {11, "ElCh_VBF_BoostedTopCR_"},
      };
    cats["eqq"+year+"_wj_bs_13TeV"] = {
      {12, "ElCh_GGF_BoostedSB_"},
      {13, "ElCh_VBF_BoostedSB_"},
      };
    cats["mqq"+year+"_bs_13TeV"] = {
      {6, "MuCh_GGF_BoostedSR_"},
      {7, "MuCh_VBF_BoostedSR_"},
      };
    cats["mqq"+year+"_top_bs_13TeV"] = {
      {10, "MuCh_GGF_BoostedTopCR_"},
      {11, "MuCh_VBF_BoostedTopCR_"},
      };
    cats["mqq"+year+"_wj_bs_13TeV"] = {
      {12, "MuCh_GGF_BoostedSB_"},
      {13, "MuCh_VBF_BoostedSB_"},
      };
  }
  else {
    cats["eqq"+year+"_bs_13TeV"] = {
      {6, "ElCh_10_GGF_AK8SR_"},
      {7, "ElCh_10_VBF_AK8SR_"},
      };
    cats["eqq"+year+"_top_bs_13TeV"] = {
      {10, "ElCh_10_GGF_AK8TopCR_"},
      {11, "ElCh_10_VBF_AK8TopCR_"},
      };
    cats["eqq"+year+"_wj_bs_13TeV"] = {
      {12, "ElCh_10_GGF_AK8SB_"},
      {13, "ElCh_10_VBF_AK8SB_"},
      };
    cats["mqq"+year+"_bs_13TeV"] = {
      {6, "MuCh_10_GGF_AK8SR_"},
      {7, "MuCh_10_VBF_AK8SR_"},
      };
    cats["mqq"+year+"_top_bs_13TeV"] = {
      {10, "MuCh_10_GGF_AK8TopCR_"},
      {11, "MuCh_10_VBF_AK8TopCR_"},
      };
    cats["mqq"+year+"_wj_bs_13TeV"] = {
      {12, "MuCh_10_GGF_AK8SB_"},
      {13, "MuCh_10_VBF_AK8SB_"},
      };
  }

  cats["eqq"+year+"_hm_13TeV"] = {
    {6, "ElCh_GGF_55HMSR_"},
    {7, "ElCh_VBF_55HMSR_"},
    };
  cats["eqq"+year+"_top_hm_13TeV"] = {
    {10, "ElCh_GGF_55HMTopCR_"},
    {11, "ElCh_VBF_55HMTopCR_"},
    };
  cats["eqq"+year+"_wj_hm_13TeV"] = {
    {12, "ElCh_GGF_55HMSB_"},
    {13, "ElCh_VBF_55HMSB_"},
    };
  cats["mqq"+year+"_hm_13TeV"] = {
    {6, "MuCh_GGF_55HMSR_"},
    {7, "MuCh_VBF_55HMSR_"},
    };
  cats["mqq"+year+"_top_hm_13TeV"] = {
    {10, "MuCh_GGF_55HMTopCR_"},
    {11, "MuCh_VBF_55HMTopCR_"},
    };
  cats["mqq"+year+"_wj_hm_13TeV"] = {
    {12, "MuCh_GGF_55HMSB_"},
    {13, "MuCh_VBF_55HMSB_"},
    };

  }

  map<string, VString> masses;

  for (auto it : input_dir){
    if(it.first.find("qq") != std::string::npos && it.first.find("bs") != std::string::npos){ //Semilep boosted
      masses[it.first] = {"400", "450", "500", "550", "600", "650", "700", "750", "800", "900", "1000", "1500", "2000", "2500", "3000", "4000", "5000"};
      // masses[it.first] = {"400", "450", "550", "600", "650", "700", "750", "800", "1000", "2000", "2500", "3000", "4000", "5000"};
    }else if(it.first.find("qq") != std::string::npos && it.first.find("bs") == std::string::npos && it.first.find("hm") == std::string::npos){ //Semilep resolved
      masses[it.first] = {"115", "120", "124", "125", "126", "130", "135", "140", "145", "150", "155", "160", "165", "170", "175", "180", "190", "200", "210", "230", "250", "270", "300", "350", "400", "450", "500", "550", "600"};
      // masses[it.first] = {"115", "120", "124", "125", "126", "130", "135", "140", "145", "155", "160", "165", "170", "175", "180", "200", "210", "230", "250", "270", "300", "350", "400", "450", "550", "600"};
    }
    else if(it.first.find("qq") != std::string::npos && it.first.find("hm") != std::string::npos){ //Semilep other
      masses[it.first] = {"600", "650", "700", "750", "800", "900", "1000", "2000", "2500", "3000", "4000", "5000"};
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

  ch::AddMSSMFullRun2Systematics(cb, highindep, model);
  //! [part7]

  for (string chn : chns) {

  if(chn.find("qq") != std::string::npos){    //Semilep

    std::string discrim = "resolvHiggsMass";
    if (chn.find("bs") != std::string::npos) discrim = "boostHiggsMass_vbf";
    if (chn.find("hm") != std::string::npos) discrim = "hmHiggsMass";
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
      // for (auto b : cb.cp().FilterAll(BinIsControlRegion).bin_set()) {
      for (auto b : cb.cp().bin_set()) { // DO asimov also in CR!
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

  // // Rebinning
  // // --------------------
  // // Keep track of shapes before and after rebinning for comparison
  // // and for checking the effect of negative bin contents
  // std::map<std::string, TH1F> before_rebin;
  // std::map<std::string, TH1F> after_rebin;
  // std::map<std::string, TH1F> after_rebin_neg;
  // if (check_neg_bins) {
  //   for (auto b : bins) {
  //     before_rebin[b] = cb.cp().bin({b}).backgrounds().GetShape();
  //   }
  // }
  //
  //
  // auto rebin = ch::AutoRebin()
  //   .SetBinThreshold(0.)
  //   .SetBinUncertFraction(0.9)
  //   .SetRebinMode(1)
  //   .SetPerformRebin(true)
  //   .SetVerbosity(1);
  // if(auto_rebin) rebin.Rebin(cb, cb);
  //
  // vector<double> binning;
  // if (highindep){
  //   //Currently used: [0,400,450,500,550,625,700,800,900,1000,1100,1200,1300,1450,1600,1800,2000,2250,2500,2750,3000,3500]
  //   binning = {0,400,450,500,550,625,700,800,900,1000,1100,1200,1300,1450,1600,1800,2000,2250,3500};
  // }else{
  //   //Currently used: [0,150,175,200,225,250,280,320,360,400,450,500,550,625,700,800,900,1000,1100,1200,1300,1450,1600,1800,2000,2250,2500,2750,3000,3500]
  //   binning = {0,150,175,200,225,250,280,320,360,400,450,500,550,625,700,800,900,1000,1100,1200,1300,1450,1600,1800,2000,2250,3500};
  // }
  // if(manual_rebin) {
  //   for(auto b : cb.cp().FilterAll(BinIsControlRegion).bin_set()) {
  //     std::cout << "Rebinning by hand for bin: " << b <<  std::endl;
  //     cb.cp().bin({b}).VariableRebin(binning);
  //   }
  // }
  //
  // if (check_neg_bins) {
  //   for (auto b : bins) {
  //     after_rebin[b] = cb.cp().bin({b}).backgrounds().GetShape();
  //     // std::cout << "Bin: " << b << " (before)\n";
  //     // before_rebin[b].Print("range");
  //     // std::cout << "Bin: " << b << " (after)\n";
  //     // after_rebin[b].Print("range");
  //     // Build a sum-of-bkgs TH1 that doesn't truncate the negative yields
  //     // like the CH GetShape does
  //     for (auto p : cb.cp().bin({b}).backgrounds().process_set()) {
  //       TH1F proc_hist;
  //       cb.cp().bin({b}).process({p}).ForEachProc([&](ch::Process *proc) {
  //         proc_hist = proc->ShapeAsTH1F();
  //         proc_hist.Scale(proc->no_norm_rate());
  //         for (int i = 1; i <= proc_hist.GetNbinsX(); ++i) {
  //           if (proc_hist.GetBinContent(i) < 0.) {
  //             std::cout << p << " bin " << i << ": " << proc_hist.GetBinContent(i) << "\n";
  //           }
  //         }
  //       });
  //       if (after_rebin_neg.count(b)) {
  //         after_rebin_neg[b].Add(&proc_hist);
  //       } else {
  //         after_rebin_neg[b] = proc_hist;
  //       }
  //     }
  //     std::cout << "Bin: " << b << "\n";
  //     for (int i = 1; i <= after_rebin[b].GetNbinsX(); ++i) {
  //       double offset = after_rebin[b].GetBinContent(i) - after_rebin_neg[b].GetBinContent(i);
  //       double offset_by_yield = offset / after_rebin[b].GetBinContent(i);
  //       double offset_by_err = offset / after_rebin[b].GetBinError(i);
  //       printf("%-2i offset %-10.4f tot %-10.4f err %-10.4f off/tot %-10.4f off/err %-10.4f\n", i , offset, after_rebin[b].GetBinContent(i), after_rebin[b].GetBinError(i), offset_by_yield, offset_by_err);
  //     }
  //   }
  // }


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
  // Don't do bbb for CR because of rateParam
  // auto bbb_ctl = ch::BinByBinFactory()
  //  .SetPattern("CMS_$ANALYSIS_$BIN_$ERA_$PROCESS_bin_$#")
  //  .SetAddThreshold(0.)
  //  .SetMergeThreshold(0.4)
  //  .SetFixNorm(false)  // contrary to signal region, bbb *should* change yield here
  //  .SetVerbosity(1);
  // // Will merge but only for non W and QCD processes, to be on the safe side
  // bbb_ctl.MergeBinErrors(cb.cp().process({"Wjets", "top"}, false).backgrounds().FilterProcs(BinIsNotControlRegion));
  // bbb_ctl.AddBinByBin(cb.cp().process({"Wjets", "top"}, false).backgrounds().FilterProcs(BinIsNotControlRegion), cb); // Added .backgrounds() here when not removing signal from CR
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
                             "norm", true, true, false, &demo); // Middle bool is verbose
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

  // writer.WriteCards("cmb", cb);
  // writercmb.WriteCards("cmb", cb);

  if (!combcard_only){
    // for (auto chn : chns) {
    //   if (chn.find("dy") != std::string::npos  or chn.find("top") != std::string::npos) {
    //     continue;
    //   }
    //   // Add per channel-category
    //   writer.WriteCards("hww_"+chn+"_ggF_13TeV", cb.cp().channel({chn,chn+"_dy",chn+"_top"}).attr({"ggh"},"njet"));
    //   writer.WriteCards("hww_"+chn+"_VBF_13TeV", cb.cp().channel({chn,chn+"_dy",chn+"_top"}).attr({"vbf"},"njet"));
    //   // Add per channel
    //   writer.WriteCards("hww_"+chn+"_13TeV", cb.cp().channel({chn,chn+"_dy",chn+"_top"}).attr({chn},"decay"));

    //   writercmb.WriteCards("hww_"+chn+"_ggF_13TeV", cb.cp().channel({chn,chn+"_dy",chn+"_top"}).attr({"ggh"},"njet"));
    //   writercmb.WriteCards("hww_"+chn+"_VBF_13TeV", cb.cp().channel({chn,chn+"_dy",chn+"_top"}).attr({"vbf"},"njet"));
    //   writercmb.WriteCards("hww_"+chn+"_13TeV", cb.cp().channel({chn,chn+"_dy",chn+"_top"}).attr({chn},"decay"));
    // }
    // Add per category
    writer.WriteCards("hww_ggF_13TeV", cb.cp().attr({"ggh"},"njet"));
    writer.WriteCards("hww_VBF_13TeV", cb.cp().attr({"vbf"},"njet"));
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
    writer.WriteCards("hwwlnuqq_13TeV", cb.cp().attr({"semilep"},"analysis"));
    writercmb.WriteCards("hwwlnuqq_13TeV", cb.cp().attr({"semilep"},"analysis"));
  }

  // NEED to split boosed/resolved too
  // bos+res is either full comb, or semilep only
  if (do2016semi or do2017semi or do2018semi){
    writer.WriteCards("hww_boosted_13TeV", cb.cp().attr({"Boosted"},"whad"));
    writer.WriteCards("hww_resolved_13TeV", cb.cp().attr({"Resolved"},"whad"));
    writer.WriteCards("hww_res+bos_13TeV", cb.cp().attr({"Resolved", "Boosted"},"whad"));
    writer.WriteCards("hww_hm_13TeV", cb.cp().attr({"HM"},"whad"));

    writercmb.WriteCards("hww_boosted_13TeV", cb.cp().attr({"Boosted"},"whad"));
    writercmb.WriteCards("hww_resolved_13TeV", cb.cp().attr({"Resolved"},"whad"));
    writercmb.WriteCards("hww_res+bos_13TeV", cb.cp().attr({"Resolved", "Boosted"},"whad"));
    writercmb.WriteCards("hww_hm_13TeV", cb.cp().attr({"HM"},"whad"));
  }

  cb.PrintAll();
  cout << " done\n";
}
