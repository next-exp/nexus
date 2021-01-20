#include <HistoSampler.h>
#include <catch.hpp>

TEST_CASE("HistoSampler") {
  SECTION("1D sanity check"){
    auto analysisManager = G4AnalysisManager::Instance();
    analysisManager->SetFirstHistoId(1);
    analysisManager->CreateH1("1","hist1", 2, 0., 2.);
    analysisManager->FillH1(1, 0.5, 1.0);
    auto h1 = analysisManager->GetH1(1);
    std::unique_ptr <HistoSampler1D>  h1_pdf (new HistoSampler1D(h1));
    for (int i=0; i<10; i++){
      double x;
      h1_pdf->sample(x, G4UniformRand());
      REQUIRE (x <= 1);
      REQUIRE (x >= 0);
    }

  }
  SECTION("2D sanity check"){
    auto analysisManager = G4AnalysisManager::Instance();
    analysisManager->SetFirstHistoId(1);
    analysisManager->CreateH2("1","hist2", 2, 0., 2., 2, 10., 30.);
    analysisManager->FillH2(1, 0.5, 10.5, 1.0);
    auto h2 = analysisManager->GetH2(1);
    std::unique_ptr <HistoSampler2D>  h2_pdf (new HistoSampler2D(h2));
    for (int i=0; i<10; i++){
      double x, y;
      h2_pdf->sample(x, y, G4UniformRand(), G4UniformRand());
      REQUIRE (x <= 1. );
      REQUIRE (x >= 0. );
      REQUIRE (y <= 20.);
      REQUIRE (y >= 10.);
    }
  }
  SECTION("3D sanity check"){
    auto analysisManager = G4AnalysisManager::Instance();
    analysisManager->SetFirstHistoId(1);
    analysisManager->CreateH3("1","hist3", 2, 0., 2., 2, 10., 30., 2, 100., 300.);
    analysisManager->FillH3(1, 0.5, 10.5, 150., 1.0);
    auto h3 = analysisManager->GetH3(1);
    std::unique_ptr <HistoSampler3D>  h3_pdf (new HistoSampler3D(h3));
    for (int i=0; i<10; i++){
      double x, y, z;
      h3_pdf->sample(x, y, z, G4UniformRand(), G4UniformRand(), G4UniformRand());
      REQUIRE (x <= 1.  );
      REQUIRE (x >= 0.  );
      REQUIRE (y <= 20. );
      REQUIRE (y >= 10. );
      REQUIRE (z <= 200.);
      REQUIRE (z >= 100.);
    }
  }
}
