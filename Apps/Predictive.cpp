// MaCh3 includes
#include "Fitters/MaCh3Factory.h"
#include "Fitters/PredictiveThrower.h"
#include "Samples/MaCh3DUNEFactory.h"

int main(int argc, char *argv[]) {
  // Initialise manger responsible for config handling
  auto FitManager = MaCh3ManagerFactory(argc, argv);

  // Initialise covariance class reasonable for Systematics
  //auto xsec = MaCh3CovarianceFactory<ParameterHandlerGeneric>(FitManager.get(), "Xsec");

  // Initialise samplePDF
  //auto SampleConfig = Get<std::vector<std::string>>(FitManager->raw()["General"]["TutorialSamples"], __FILE__ , __LINE__);
  auto [param_handler, samples] = MaCh3DuneFactory(FitManager);
  //auto mySamples = MaCh3SampleHandlerFactory<SampleHandlerTutorial>(SampleConfig, xsec.get());

  // Create MCMC Class
  std::unique_ptr<PredictiveThrower> MaCh3Fitter = std::make_unique<PredictiveThrower>(FitManager.get());
  // Add covariance to MCM
  MaCh3Fitter->AddSystObj(param_handler.get());
  for (auto Sample : samples) {
      MaCh3Fitter->AddSampleHandler(Sample);
  }
  // Run Predictive analysis
  MaCh3Fitter->ProduceToys();
  MaCh3Fitter->RunPredictiveAnalysis();

  return 0;
}
