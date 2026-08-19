#include <iostream>
#include <chrono>
#include <iomanip>
#include <vector>

#include <TH1D.h>
#include <THStack.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TRint.h>
#include <TLegend.h>
#include <TColor.h>
#include <TMath.h>

#include "Fitters/MaCh3Factory.h"
#include "Samples/MaCh3DUNEFactory.h"

int main(int argc, char * argv[]) {
  std::cout << "Here 1";
  auto FitManager = MaCh3ManagerFactory(argc, argv);
  std::cout << "Here 2";
  auto OutputFileName = FitManager->raw()["General"]["OutputFile"].as<std::string>();

  //####################################################################################
  //Create sample handler + parameter_handler objects
  std::cout << "Here 3";
  auto [param_handler, samples] = MaCh3DuneFactory(FitManager);
  std::cout << "Here 4";
  //Some place to store the histograms
  std::vector<TH1*> PredictionHistograms;
  std::vector<std::string> sample_names;
  std::cout << "Here 5";
  auto OutputFile = std::unique_ptr<TFile>(TFile::Open(OutputFileName.c_str(), "RECREATE"));
  std::cout << "Here 6";
  OutputFile->cd();
  std::cout << "Here 7";
  for (auto handler : samples) {
    std::cout << "Here 8";

    for (unsigned iSample = 0; iSample < handler->GetNSamples(); ++iSample) {

      std::string name = handler->GetSampleTitle(iSample);
      sample_names.push_back(name);
      TString NameTString = TString(name.c_str());

      handler->Reweight();
      PredictionHistograms.push_back(static_cast<TH1*>(handler->GetMCHist(iSample)->Clone(NameTString+"_DataHist")));

      if (handler->GetNDim(iSample) == 1){
        handler->AddData(iSample, static_cast<TH1D*>(PredictionHistograms.back()));
      } else if (handler->GetNDim(iSample) == 2){
        handler->AddData(iSample, static_cast<TH2D*>(PredictionHistograms.back()));
      }

      else {
        MACH3LOG_ERROR("Unsupported number of dimensions > 2 - Quitting");
        throw MaCh3Exception(__FILE__ , __LINE__ );
      }

      MACH3LOG_INFO("Integrals of nominal hists: ");
      MACH3LOG_INFO("{} : {}",name.c_str(),PredictionHistograms.back()->Integral());
      MACH3LOG_INFO("--------------");
    }
  }

  //###########################################################################################################
  //MCMC
  std::cout << "Here 9";
  auto MaCh3Fitter = MaCh3FitterFactory(FitManager.get());
  std::cout << "Here 10";
  bool StartFromPreviousChain = GetFromManager(FitManager->raw()["General"]["StartFromPos"], false, __FILE__, __LINE__);
  std::cout << "Here 11";

  //Start chain from random position unless continuing a chain
  if(!StartFromPreviousChain){
    if (!GetFromManager(FitManager->raw()["General"]["StatOnly"], false, __FILE__, __LINE__)) {
      param_handler->ThrowParameters();
    }
  }
  std::cout << "Here 12";
  //Add systematic objects
  MaCh3Fitter->AddSystObj(param_handler.get());
  std::cout << "Here 13";
  
  if (StartFromPreviousChain) {
    std::string PreviousChainPath = FitManager->raw()["General"]["PosFileName"].as<std::string>();
    MACH3LOG_INFO("MCMC getting starting position from: {}",PreviousChainPath);
    MaCh3Fitter->StartFromPreviousFit(PreviousChainPath);
  }
  std::cout << "Here 14";

  //Add samples
  for(auto Sample : samples){
    MaCh3Fitter->AddSampleHandler(Sample);
  }
  std::cout << "Here 15";
  MACH3LOG_INFO("Before MCMC Start");

  //Run fit
  MaCh3Fitter->RunMCMC();

  //Writing the memory usage at the end to eventually spot some nasty leak
  MACH3LOG_WARN("\033[0;31mCurrent Total RAM usage is {:.2f} GB\033[0m", M3::Utils::getValue("VmRSS") / 1048576.0);
  MACH3LOG_WARN("\033[0;31mOut of Total available RAM {:.2f} GB\033[0m", M3::Utils::getValue("MemTotal") / 1048576.0);

  return 0;
}
