// Plotting Framework
//
// Copyright (C) 2019  Mario Krüger
// Contact: mario.kruger@cern.ch
// For a full list of contributors please see docs/Credits
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.


#include "PlottingFramework.h"
#include "PlotManager.h"
#include "Plot.h"
#include <sstream>

using PlottingFramework::PlotManager;
using PlottingFramework::Plot;

string GetPtString(int pTbin);
/*
 - categorize plots
 - add data specific colors and markers
 - move derived plots creation to separate class

 - option in main program to list loaded plots
 - possibility to dump and load plot styles (manager should only read in required plot styles)
 - make sure PlotStyle names are unique
 - add graph division feature with tspline3
 - add constructor to plotStyle that already gives useful default markers, colors, 2dstyle, etc
 - use drawing option AXIS and lagrest axis range for ranges or find better way to define which histo should auto-define ranges
 (set limits? is this by definition limited to original or can it be changed?)


 Bugs:
 - make sure figure groups cannot contain '.'
 - exponents on x axis are not in the proper position
 - text boxes width and height is calculated wrong
 - if first histogram has drawing option "band", axes are not drawn properly
 - height of boxes is not calculated correctly (ndc vs relative pad coordinates?)
 - boxes are not transparent by default
 - sometimes text boxes are randomly not drawn?
 - zeroes in histograms should not be drawn if they have no error?
 - if lables are 1, 2,3 height is not calculated correctly?


 Important:
 - possibility to set alias for axis e.g. "ratio"?
 - be clear about coordinate systems (fix relative positioning, maybe)
 - find a general way to automatically determine optimal axis offsets (in particular for 2d and 3d views) (what unit is title offset?)
 - it should be possible to conveniently pipe all root plotting functionality (drawing properties) to the plot
 - add TView for 3d representations of th2
 - get rid of tautological stuff for ratio, histo, graph in main loop over data
 - generalize legend function to handle text boxes in a similar manner
 - pads must inherit plot properties (text size et al unless specified otherwise)
 - text size and style settings should by default be inherited by each object with text but there should be a possiblity to override
 - check if 2d hist is part of plot, then change style...

 Not so important:
 - possibility to load all data from input files
 - load only allowed datatypes
 - setter for csv format string and delimiter
 - would it be possible to define 'top left' 'bottom right' etc default positons for boxes in general manner? maybe with flexible minimal distance to ticks
 - is it possible to set the order in multi column tlegends? left-right vs top-bottom
 - pipe box line fill properties to drawing
 - add possibility to make canvas intransparent (and colored)
 - how to handle multi-plots if not all of them are available?
 - possibility to scale histograms by factor
 - also add use last color feature -1
 - change only color not marker options?
 - add line in ratio plots? in plot style? value of const line flexible?
 - backward compatibility with "ratio" keyword in axis range and title?
 - fix dirty 2d hacks
 - fix dirty hacks for backward compatibility regarding ratio plots
 - check if padID starts with 1 is a problem (vectors maybe)
 - think about more placeholders for legends and texts and how to format them
 - option to set n divisions of axes
 - add option in padstyle that identifies the pad as ratio plot
 - fix overlap between axis title and tick marks if they have too many digits
 - increase number of color steps in 2d plots resp make flexible
 - possibility to load and plot thstack and multigraphs
 - add stack drawing option (thstack)
 - add option to change grid styles
 - add possibility for grey tilted overlayed text like "draft"
 - write case-insensitive "contained in string" function for control string! as lambda function
 - text align, angle features!
 - possibility for user to grep and change specific plotstyle or plot that was already loaded in manager
 - put in some feasible defaults for colors, markers etc
 - for loading and saving styles: possibility for default values as fallback?
 - possibility to specify use of only full or open markers in AddHisto
 - implement check that ensures only valid drawing options are used for each data type (separate 1d 2d info)
 - also define default line styles
 - generalized version of white patch hiding the truncated zero
 - possibility to split the legend?
 - maybe linking axes should also be possible for different axes (x in pad1 to y in pad2)
 
 Code Quality:
 - change arguments referring to internal variables to const ref if possible to be more memory efficient, use lambdas
 - add (copy-, move-) constructors and destructors; make sure all variables are initialized properly

 Long term goals:
  - add support for .yoda and hepmc file formats
  - add shape objects
  - add functions, fitting
  - add data input and stand-alone definable objects (shapes, arrows, functions)
  - add help function for usage!

 
 General:
  - cleanup code
  - imporove documentation
  - port to gitlab
  - add minimal example code
  - add user friendly itroduction int framework (doxygen comment for namespace?)
 
 TODOS for analysis plots:
  - clean up file names and make useful sub-folders

 
 */

int main(int argc, char *argv[]) {

  bool updateInputFiles = false;

  string outputPath = "~/Desktop/testPlots";
  string inputFileConfig = "inputFiles";
  string plotConfig = "plotDefinitions";
  
  string outputFileName = "myPlots.root";
  bool createBinWiseClosureTests = true;

  // create plotting environment
  PlotManager plotEnv;
  plotEnv.SetOutputDirectory(outputPath);
  plotEnv.SetUseUniquePlotNames(false);

  
  vector<string> dataSets;
  if(argc > 1 && (string(argv[1]) == "help" || string(argv[1]) == "--h")){
    cout << "Usage:" << endl;
    cout << "./plot inputID plot1,plot2" << endl;
    return 0;

  }
  else if(argc > 2){
    // plot only specific plots stored in the plotConfig file

    plotEnv.LoadInputDataFiles(inputFileConfig);

    string inputIdentifierString = argv[1];
    std::istringstream inputIdentifierStringStream(inputIdentifierString);
    vector<string> inputIdentifiers;
    string tempName;
    while(std::getline(inputIdentifierStringStream, tempName, ',')) {
        inputIdentifiers.push_back(tempName);
    }

    string fileNameString = argv[2];
    std::istringstream fileNameStringStream(fileNameString);
    vector<string> fileNames;
    while(std::getline(fileNameStringStream, tempName, ',')) {
        fileNames.push_back(tempName);
    }
    if(fileNameString == "all") fileNames = {};

    string outputMode = "interactive";
    if(argc > 3 && argv[3]) outputMode = argv[3];
    
    for(auto& inputIdentifier : inputIdentifiers){
      plotEnv.CreatePlotsFromFile(plotConfig, inputIdentifier, fileNames, outputMode);
    }
    return 0;
  }
  else if(argc > 1){
    // update plots defined in file for specific dataset
    plotEnv.LoadPlots(plotConfig);
    if(!(string(argv[1]) == "none")) dataSets.push_back(argv[1]); // none loads no dataset and therefore only overrides the combined plots
  }
  else
  {
    // override all plots defined in file
    dataSets.push_back("pp_2TeV");
    dataSets.push_back("pp_5TeV");
    dataSets.push_back("pp_7TeV");
    dataSets.push_back("pp_13TeV");

    dataSets.push_back("pPb_5TeV");
    dataSets.push_back("PbPb_5TeV");
    dataSets.push_back("XeXe_5TeV");

    dataSets.push_back("Fits");
    dataSets.push_back("Simulations");
    dataSets.push_back("Energyscan");
  }


  
  //plotEnv.SetDrawTimestamps(true);
  //plotEnv.SetPalette(kRainBow);//kBlueGreenYellow, kDarkRainBow

  if(updateInputFiles)
  {
    for(string dataSet : dataSets){
      string folder = "~/Desktop/AliMultDepSpec/Datasets/" + dataSet + "/";
      string inputFile = folder + dataSet + "_Results.root";
      string inputFileSyst = folder + dataSet + "_Syst.root";

      if(dataSet == "Publications" || dataSet == "Simulations" || dataSet == "Energyscan" || dataSet == "Fits"){
        plotEnv.AddInputDataFiles(dataSet, {inputFile});
      }else{
        plotEnv.AddInputDataFiles(dataSet, {inputFile, inputFileSyst});
      }
    }
    plotEnv.DumpInputDataFiles(inputFileConfig);
    cout << "Updated input files: '" << inputFileConfig << "'." << endl;
    return -1;
  }
  else{
    plotEnv.LoadInputDataFiles(inputFileConfig);
  }
    
  //---- Lable definitions -----------------------------------------------------
  string newLine = " // ";
  string alice = "";
  string aliceWIP = "#bf{ALICE work in progress}";
  string alicePrel = "#bf{ALICE Preliminary}";
  string chargedParticles = "charged particles";
  string erg5TeV_NN = "#sqrt{#it{s}_{NN}} = 5.02 TeV";
  string erg544TeV_NN = "#sqrt{#it{s}_{NN}} = 5.44 TeV";
  string erg2TeV = "#sqrt{#it{s}} = 2.76 TeV";
  string erg5TeV = "#sqrt{#it{s}} = 5.02 TeV";
  string erg7TeV = "#sqrt{#it{s}} = 7 TeV";
  string erg8TeV_NN = "#sqrt{#it{s}_{NN}} = 8 TeV";
  string erg13TeV = "#sqrt{#it{s}} = 13 TeV";
  string eta03 = "|#it{#eta}| < 0.3";
  string eta08 = "|#it{#eta}| < 0.8";
  string pp = "pp";
  string pPb = "p-Pb";
  string PbPb = "Pb-Pb";
  string XeXe = "Xe-Xe";
  string ptRange = "0.15 GeV/#it{c} < #it{p}_{T} < 50 GeV/#it{c}";
  string ptRange10GeV = "0.15 < #it{p}_{T} < 10 GeV/#it{c}";
  //---------------------------------------------------------------------------

  { // -----------------------------------------------------------------------
    string plotName = "dummyPlot";
    Plot myPlot(plotName, "testGroup");
    myPlot.AddGraph("dummy", "CSVTest", "");
    plotEnv.AddPlot(myPlot);
  } // -----------------------------------------------------------------------


  for(string dataSet : dataSets)
  { //==========================================================================
    string erg = "0 TeV";
    string colSys = "x-x";
    double multRange = 60;

    if(dataSet.find("pp") != string::npos) colSys = pp;
    if(dataSet.find("pPb") != string::npos) {colSys = pPb; multRange = 140;}
    if(dataSet.find("PbPb") != string::npos) {colSys = PbPb; multRange = 3700;}
    if(dataSet.find("XeXe") != string::npos) {colSys = XeXe; multRange = 2300; erg = erg544TeV_NN;}

    if(dataSet.find("pp_2TeV") != string::npos) {erg = erg2TeV; multRange = 40;}
    if(dataSet.find("pp_7TeV") != string::npos) erg = erg7TeV;
    if(dataSet.find("pp_13TeV") != string::npos) erg = erg13TeV;
    if(dataSet.find("pp_5TeV") != string::npos) erg = erg5TeV;
    if(dataSet.find("pPb_5TeV") != string::npos) erg = erg5TeV_NN;
    if(dataSet.find("PbPb_5TeV") != string::npos) erg = erg5TeV_NN;
    if(dataSet == "Publications" || dataSet == "Simulations"|| dataSet == "Energyscan" || dataSet == "Fits") continue;

    string datasetLable = alice + newLine + chargedParticles + ", " + colSys + ", " + erg + newLine + eta08 + ", " + ptRange;
    string datasetLablePrel = alicePrel + newLine + chargedParticles + ", " + colSys + ", " + erg + newLine + eta08 + ", " + ptRange;
    string categoryTest = "Test";
    string categoryQA = "QA-Plots";
    string categoryClosure = categoryQA + "/MC_Closure_Tests";
    string categorySystematics = "Systematic_Uncertainties";

// Start layground

{ // -----------------------------------------------------------------------
  string plotName = "covMat_multDistUnfolded";
  Plot myPlot(plotName, dataSet);
  myPlot.AddHisto(plotName, "", "");
  myPlot.AddLegendBox(0.6, 0.7);
  myPlot.AddTextBox(0.4, 0.91, datasetLable);
  plotEnv.AddPlot(myPlot);
} // -----------------------------------------------------------------------

{ // -----------------------------------------------------------------------
  string plotName = "sign_covMat_multDistUnfolded";
  Plot myPlot(plotName, dataSet);
  myPlot.AddHisto(plotName, "", "");
  myPlot.AddLegendBox(0.6, 0.7);
  plotEnv.AddPlot(myPlot);
} // -----------------------------------------------------------------------

{ // -----------------------------------------------------------------------
  string plotName = "abs_covMat_multDistUnfolded";
  Plot myPlot(plotName, dataSet);
  myPlot.SetDrawingProperties("logZ");
  myPlot.AddHisto(plotName, "", "");
  myPlot.AddLegendBox(0.6, 0.7);
  plotEnv.AddPlot(myPlot);
} // -----------------------------------------------------------------------

{ // -----------------------------------------------------------------------
  string plotName = "edgeContamPt";
  Plot myPlot(plotName, dataSet);
  myPlot.SetDrawingProperties("logX");
  myPlot.AddHisto(plotName, "", "");
  myPlot.SetAxisRange("X", 0, 20);
  myPlot.SetAxisTitle("Y", "contamination");
  myPlot.AddLegendBox(0.3, 0.5);
  myPlot.AddTextBox(0.41, 0.3, datasetLable);
  plotEnv.AddPlot(myPlot);
} // -----------------------------------------------------------------------

{ // -----------------------------------------------------------------------
  string plotName = "edgeContamMult";
  Plot myPlot(plotName, dataSet);
  myPlot.AddHisto(plotName, "", "");
  myPlot.SetAxisRange("X", 0, 20);
  myPlot.SetAxisTitle("Y", "contamination");
  myPlot.SetAxisTitle("X", "test");
  myPlot.AddLegendBox(0.3, 0.5);
  myPlot.AddTextBox(0.41, 0.3, datasetLable);
  plotEnv.AddPlot(myPlot);
} // -----------------------------------------------------------------------


{ // -----------------------------------------------------------------------
  string plotName = "triggerEff";
  Plot myPlot(plotName, dataSet);
  myPlot.AddHisto("eventEfficiency", "", "efficiency of selected event to be measured");
  myPlot.AddHisto(plotName, "", "efficiency including event selection");
  myPlot.AddHisto("eventLossEff", "", "remaining");
  myPlot.SetAxisRange("Y", 0.4, 1.01);
  myPlot.SetAxisRange("X", 0, 10);
  myPlot.SetAxisTitle("Y", "efficiency");
  myPlot.AddLegendBox(0.3, 0.5);
  myPlot.AddTextBox(0.41, 0.3, datasetLable);
  plotEnv.AddPlot(myPlot);
} // -----------------------------------------------------------------------

{ // -----------------------------------------------------------------------
  string plotName = "chi2_multDistUnfolded";
  Plot myPlot(plotName, dataSet);
  myPlot.SetDrawingProperties("logY");
  myPlot.AddHisto("chi2_multDistUnfolded", dataSet, "data", kFullCross, kRed+3);
  myPlot.AddHisto("chi2_multDistUnfoldedMC", dataSet, "mc", kFullCross, kBlue+3);
  myPlot.AddHisto("chi2_multDistUnfoldedClosureTestFlat", dataSet, "mc flat prior", kFullCross, kGreen+3);
  myPlot.AddLegendBox(0.6, 0.7);
  myPlot.AddTextBox(0.4, 0.91, datasetLable);
  myPlot.SetAxisRange("Y", 0.1, 1e6);
  plotEnv.AddPlot(myPlot);
} // -----------------------------------------------------------------------

{ // -----------------------------------------------------------------------
  string plotName = "chi2_multPtUnfoldedRaw_PtBin";
  Plot myPlot(plotName, dataSet);
  
  myPlot.SetDrawingProperties("logY");
  vector<int> ptBins = {2, 3, 5, 7, 11, 13, 15, 17, 20, 30, 40, 50, 51};
  int i = 0;
  for(int ptBin : ptBins)
  {
    myPlot.AddHisto(plotName + "_" + std::to_string(ptBin), dataSet, GetPtString(ptBin), 0, 0, "hist");
    i++;
  }
  myPlot.AddLegendBox(0.5, 0.78);
  myPlot.AddTextBox(0.3, 0.92, datasetLable);
  plotEnv.AddPlot(myPlot);
} // -----------------------------------------------------------------------

{ // -----------------------------------------------------------------------
  string plotName = "chi2_multPtUnfoldedRaw_MultBin";
  Plot myPlot(plotName, dataSet);
  myPlot.SetDrawingProperties("logY");
  vector<int> multBins = {2, 6, 9, 11, 16, 21, 31, 41, 51, 61};
  int i = 0;
  for(int multBin : multBins)
  {
    myPlot.AddHisto(plotName + "_" + std::to_string(multBin), dataSet, "Nch = " + std::to_string(multBin-1), 0, 0, "hist");
    i++;
  }
  myPlot.AddLegendBox(0.6, 0.7);
  myPlot.AddTextBox(0.4, 0.91, datasetLable);
  plotEnv.AddPlot(myPlot);
} // -----------------------------------------------------------------------

{ // -----------------------------------------------------------------------
  string plotName = "particleClosure";
  Plot myPlot(plotName, dataSet);
  myPlot.AddHisto(plotName + "MCtruth", "", "MC truth", kOpenSquare, kGreen+2);
  myPlot.AddHisto(plotName + "MC", "", "MC unfolded", kOpenCircle, kRed+1);
  myPlot.AddHisto(plotName, "", "data unfolded", kFullCross, kBlue+3);
  myPlot.AddLegendBox(0.5, 0.7);
  myPlot.SetAxisRange("Y", 0.9, 1.6);
  myPlot.SetAxisRange("X", 0, multRange);
  myPlot.SetAxisTitle("Y", "integrated / true #it{N}_{ch}");
  myPlot.AddTextBox(0.6, 0.5, "particles per event in // measured spectra // over respective #it{N}_{ch}");
  myPlot.AddTextBox(0.15, 0.91, datasetLable);
  plotEnv.AddPlot(myPlot);
} // -----------------------------------------------------------------------

{ // -----------------------------------------------------------------------
  string plotName = "particeContaminationVsNch";
  Plot myPlot(plotName, dataSet, "default ratio");
  myPlot.AddHisto("particleClosureMCmeasVsNch", "", "prim and sec", kOpenCross, kBlue+1);
  myPlot.AddHisto("particleClosureMCprimaries", "", "only prim", kFullCross, kBlack+1);
  myPlot.AddRatio("particleClosureMCmeasVsNch", "", "particleClosureMCprimaries", "", "sec contam");
  myPlot.SetAxisTitle("ratio", "all / prim");
  myPlot.SetAxisTitle("Y", "measured over true particles");
  myPlot.SetAxisRange("X", 0, 60);
  myPlot.SetAxisRange("Y", 0.6, 1.2);
  myPlot.SetAxisRange("ratio", 0.95, 1.1);
  myPlot.AddLegendBox(0.6, 0.6);
  myPlot.AddTextBox(0.15, 0.91, datasetLable);
  myPlot.AddTextBox(0.5, 0.4, "particles per event in // measured spectra // over respective #it{N}_{ch}");
  plotEnv.AddPlot(myPlot);
} // -----------------------------------------------------------------------

{

  int ptBin = 28;

{ // -----------------------------------------------------------------------
  string plotName = "presentation_PtBin_measuredRawMC";
  Plot myPlot(plotName, dataSet);
  myPlot.SetDrawingProperties("logY");
  myPlot.AddHisto(string("multPtMeasuredMC_PtBin_") + std::to_string(ptBin), "", "measured", kFullCross, kGreen+3);
  myPlot.AddLegendBox(0.2, 0.4);
  myPlot.SetAxisRange("Y", 1e-8, 1e-3);
  myPlot.AddTextBox(0.4, 0.91, datasetLable);
  plotEnv.AddPlot(myPlot);
} // -----------------------------------------------------------------------

{ // -----------------------------------------------------------------------
  string plotName = "presentation_PtBin_unfoldedRawMC";
  Plot myPlot(plotName, dataSet);
  myPlot.SetDrawingProperties("logY");
  myPlot.AddHisto(string("multPtUnfoldedMC_PtBin_") + std::to_string(ptBin), "", "un-smeared", kFullCircle, kBlue+1);
  myPlot.AddLegendBox(0.2, 0.4);
  myPlot.SetAxisRange("Y", 1e-8, 1e-3);
  myPlot.AddTextBox(0.4, 0.91, datasetLable);
  plotEnv.AddPlot(myPlot);
} // -----------------------------------------------------------------------

int multBin = 18;

{ // -----------------------------------------------------------------------
  string plotName = "presentation_MultBin_unfoldedRawMC";
  Plot myPlot(plotName, dataSet);
  myPlot.SetDrawingProperties("logY logX");
  //myPlot.AddHisto(string("multPtUnfoldedMC_MultBin_") + std::to_string(multBin), "", "unfolded", kFullCircle, kBlue+1);
  myPlot.AddHisto(string("multPtUnfoldedMC_MultBin_") + std::to_string(multBin), "", "un-smeared", kFullCircle, kBlue+1);
  myPlot.AddLegendBox(0.6, 0.8);
  myPlot.SetAxisRange("Y", 1e-12, 2);
  myPlot.SetAxisTitle("X", "#it{p}^{ meas}_{T}");
  myPlot.AddTextBox(0.16, 0.3, datasetLable);
  plotEnv.AddPlot(myPlot);
} // -----------------------------------------------------------------------


}

// End playground

    //--------------------------------------------------------------------------
    //---------------------- result plots --------------------------------------
    //--------------------------------------------------------------------------
    { // -----------------------------------------------------------------------
      string plotName = "multDensity";
      Plot myPlot(plotName, dataSet, "default ratio");
      myPlot.SetDrawingProperties("logY");
      myPlot.AddHisto("multDensityUnfolded", dataSet, "", kFullCircle, kBlue+1);
      myPlot.AddHisto("multDensityUnfolded_Syst", dataSet, "data", kFullCircle, kBlue+1, "boxes");
      myPlot.AddHisto("multDensityGeneratedMC", dataSet, "mc", kOpenCircle, kBlue+1);
      myPlot.AddRatio("multDensityUnfolded_Syst", "", "multDensityGeneratedMC", "", "");
      myPlot.AddLegendBox(0.2, 0.4);
      myPlot.SetAxisRange("X", 0, 130);
      myPlot.AddTextBox(0.4, 0.91, datasetLable);
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------

    { // -----------------------------------------------------------------------
      string plotName = "multDists";
      Plot myPlot(plotName, dataSet);
      myPlot.SetDrawingProperties("logY");
      myPlot.AddHisto("multDistMeasured", dataSet, "measured");
      myPlot.AddHisto("multDistUnfolded", dataSet, "", kFullCircle, kBlue+1);
      myPlot.AddHisto("multDistUnfolded_Syst", dataSet, "unfolded", kFullCircle, kBlue+1, "boxes");
      //myPlot.SetAxisRange("X", 0, multRange);
      myPlot.AddLegendBox(0.2, 0.4);
      myPlot.AddTextBox(0.4, 0.91, datasetLable);
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------
    { // -----------------------------------------------------------------------
      string plotName = "multDistsMC";
      Plot myPlot(plotName, dataSet);
      myPlot.SetDrawingProperties("logY");
      myPlot.AddHisto("multDistMeasuredMC", dataSet, "measured");
      myPlot.AddHisto("multDistUnfoldedMC", dataSet, "unfolded");
      myPlot.AddHisto("multDistGeneratedMC", dataSet, "generated");
      //myPlot.SetAxisRange("X", 0, multRange);
      myPlot.AddLegendBox(0.2, 0.4);
      myPlot.AddTextBox(0.4, 0.91, datasetLable);
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------
    { // -----------------------------------------------------------------------
      string plotName = "multPtMeasured";
      Plot myPlot(plotName, dataSet);
      myPlot.SetDrawingProperties("logZ logY");
      myPlot.AddHisto(plotName);
      //myPlot.SetAxisRange("X", 0, multRange);
      myPlot.SetAxisRange("Y", 0.15, 50.0);
      myPlot.SetAxisRange("Z", 1e-10, 5);
      //myPlot.SetAxisTitle("X", "#it{N}^{ meas}_{ch}");
      myPlot.SetAxisTitle("Y", "#it{p}^{ meas}_{T}");
      myPlot.SetAxisTitle("Z", "norm. yield");
      //myPlot.SetAxisTitle("Z", "1/#it{N}_{evt} 1/(2#pi #it{p}_{T}) (d^{3}#it{N})/(d#it{p}_{T}d#it{#eta}d#it{N}_{acc}) (GeV/#it{c})^{-2}");
      myPlot.AddTextBox(0.15, 0.8, datasetLable);
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------
    { // -----------------------------------------------------------------------
      string plotName = "multPtMeasuredRaw";
      Plot myPlot(plotName, dataSet);
      myPlot.SetDrawingProperties("logZ logY");
      myPlot.AddHisto(plotName);
      //myPlot.SetAxisRange("X", 0, multRange);
      myPlot.SetAxisRange("Y", 0.15, 50.0);
      //myPlot.SetAxisRange("Z", 1e-10, 5);
      //myPlot.SetAxisTitle("X", "#it{N}^{ meas}_{ch}");
      myPlot.SetAxisTitle("Y", "#it{p}^{ meas}_{T} (GeV/#it{c})");
      //myPlot.SetAxisTitle("Z", "norm. yield");
      myPlot.SetAxisTitle("Z", "raw yield");
      //myPlot.SetAxisTitle("Z", "1/#it{N}_{evt} 1/(2#pi #it{p}_{T}) (d^{3}#it{N})/(d#it{p}_{T}d#it{#eta}d#it{N}_{acc}) (GeV/#it{c})^{-2}");
      myPlot.AddTextBox(0.15, 0.8, datasetLable);
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------
    { // -----------------------------------------------------------------------
      string plotName = "multPtUnfolded";
      Plot myPlot(plotName, dataSet);
      myPlot.SetDrawingProperties("logZ logY");
      myPlot.AddHisto(plotName);
      myPlot.SetAxisRange("X", 0, multRange);
      myPlot.SetAxisRange("Y", 0.15, 30.0);
      myPlot.SetAxisRange("Z", 1e-10, 0.5);
      //myPlot.SetAxisTitle("Z", "raw yield");
      //myPlot.SetAxisTitle("Z", "raw yield");abcde
      myPlot.SetAxisTitle("Y", "#it{p}_{T} (GeV/#it{c})");
      myPlot.SetAxisTitle("X", "#it{N}_{ch}");

      //myPlot.SetAxisTitle("Y", "#it{p}^{ true}_{T} (GeV/#it{c})");
      //myPlot.SetAxisTitle("Z", "norm. yield");
      myPlot.SetAxisTitle("Z", "1/#it{N}_{evt} 1/(2#pi #it{p}_{T}) (d^{3}#it{N})/(d#it{p}_{T}d#it{#eta}d#it{N}_{ch}) [(GeV/#it{c})^{-2}]");
      myPlot.AddTextBox(0.15, 0.5, datasetLable);
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------
    { // -----------------------------------------------------------------------
      string plotName = "multPtUnfoldedNormalized";
      Plot myPlot(plotName, dataSet);
      myPlot.SetDrawingProperties("logZ logY");
      myPlot.AddHisto(plotName);
      myPlot.SetAxisRange("X", 0, multRange);
      myPlot.SetAxisRange("Y", 0.15, 50.0);
//      myPlot.SetAxisRange("Z", 1e-10, 50);
      //myPlot.SetAxisTitle("Z", "raw yield");
      //myPlot.SetAxisTitle("Z", "raw yield");abcde
      myPlot.SetAxisTitle("Y", "#it{p}_{T} (GeV/#it{c})");
      myPlot.SetAxisTitle("X", "#it{N}_{ch}");
      
      //myPlot.SetAxisTitle("Y", "#it{p}^{ true}_{T} (GeV/#it{c})");
      //myPlot.SetAxisTitle("Z", "norm. yield");
      myPlot.SetAxisTitle("Z", "1/#it{N}_{evt}(#it{N}_{ch}) 1/(2#pi #it{p}_{T}) (d^{3}#it{N})/(d#it{p}_{T}d#it{#eta}d#it{N}_{ch}) [(GeV/#it{c})^{-2}]");
//      myPlot.AddTextBox(0.15, 0.8, datasetLable);
      myPlot.AddTextBox(0.3, 0.3, datasetLable);
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------

    { // -----------------------------------------------------------------------
      string plotName = "multPtMeasuredMC";
      Plot myPlot(plotName, dataSet);
      
      myPlot.SetDrawingProperties("logZ logY");
      myPlot.AddHisto(plotName);
      //myPlot.SetAxisRange("X", 0, multRange);
      myPlot.SetAxisRange("Y", 0.15, 50.0);
      myPlot.SetAxisRange("Z", 1e-10, 5);
      //myPlot.SetAxisTitle("X", "#it{N}^{ meas}_{ch}");
      myPlot.SetAxisTitle("Y", "#it{p}^{ meas}_{T}");
      myPlot.SetAxisTitle("Z", "norm. yield");
      //myPlot.SetAxisTitle("Z", "1/#it{N}_{evt} 1/(2#pi #it{p}_{T}) (d^{3}#it{N})/(d#it{p}_{T}d#it{#eta}d#it{N}_{acc}) (GeV/#it{c})^{-2}");
      myPlot.AddTextBox(0.15, 0.8, datasetLable);
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------
    { // -----------------------------------------------------------------------
      string plotName = "multPtMeasuredRawMC";
      Plot myPlot(plotName, dataSet);
      
      myPlot.SetDrawingProperties("logZ logY");
      myPlot.AddHisto(plotName);
      //myPlot.SetAxisRange("X", 0, multRange);
      myPlot.SetAxisRange("Y", 0.15, 50.0);
      //myPlot.SetAxisRange("Z", 1e-10, 5);
      //myPlot.SetAxisTitle("X", "#it{N}^{ meas}_{ch}");
      myPlot.SetAxisTitle("Y", "#it{p}^{ meas}_{T}");
      //myPlot.SetAxisTitle("Z", "norm. yield");
      myPlot.SetAxisTitle("Z", "raw yield");
      //myPlot.SetAxisTitle("Z", "1/#it{N}_{evt} 1/(2#pi #it{p}_{T}) (d^{3}#it{N})/(d#it{p}_{T}d#it{#eta}d#it{N}_{acc}) (GeV/#it{c})^{-2}");
      myPlot.AddTextBox(0.15, 0.8, datasetLable);
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------
    { // -----------------------------------------------------------------------
      string plotName = "multPtUnfoldedMC";
      Plot myPlot(plotName, dataSet);
      
      myPlot.SetDrawingProperties("logZ logY");
      myPlot.AddHisto(plotName);
      //myPlot.SetAxisRange("X", 0, multRange);
      myPlot.SetAxisRange("Y", 0.15, 50.0);
      myPlot.SetAxisRange("Z", 1e-10, 5);
      //myPlot.SetAxisTitle("X", "#it{N}^{ true}_{ch}");
      myPlot.SetAxisTitle("Y", "#it{p}^{ true}_{T}");
      myPlot.SetAxisTitle("Z", "norm. yield");
      //myPlot.SetAxisTitle("Z", "1/#it{N}_{evt} 1/(2#pi #it{p}_{T}) (d^{3}#it{N})/(d#it{p}_{T}d#it{#eta}d#it{N}_{ch}) (GeV/#it{c})^{-2}");
      myPlot.AddTextBox(0.15, 0.8, datasetLable);
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------
    { // -----------------------------------------------------------------------
      string plotName = "multPtGeneratedMC";
      Plot myPlot(plotName, dataSet);
      
      myPlot.SetDrawingProperties("logZ logY");
      myPlot.AddHisto(plotName);
      //myPlot.SetAxisRange("X", 0, multRange);
      //myPlot.SetAxisRange("Y", 0.15, 50.0);
      //myPlot.SetAxisRange("Z", 1e-10, 5);
      //myPlot.SetAxisTitle("X", "#it{N}^{ true}_{ch}");
      myPlot.SetAxisTitle("Y", "#it{p}^{ true}_{T}");
      myPlot.SetAxisTitle("Z", "norm. yield");
      //myPlot.SetAxisTitle("Z", "1/#it{N}_{evt} 1/(2#pi #it{p}_{T}) (d^{3}#it{N})/(d#it{p}_{T}d#it{#eta}d#it{N}_{ch}) (GeV/#it{c})^{-2}");
      myPlot.AddTextBox(0.15, 0.8, datasetLable);
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------
    { // -----------------------------------------------------------------------
      string plotName = "meanPt";
      Plot myPlot(plotName, dataSet);
      
      myPlot.AddHisto("momentUnfolded1", "", "", kFullSquare, kBlue+1, "", multRange);
      myPlot.AddHisto("momentUnfolded1_Syst", "", "data", kFullSquare, kBlue+1, "boxes", multRange);
      myPlot.AddHisto("momentGeneratedMC1", "", "MC", kOpenCircle, kBlack, "", multRange);
      myPlot.SetAxisRange("X", 0, multRange);
      myPlot.SetAxisRange("Y", 0.45, 0.85);
      myPlot.AddLegendBox(0.5, 0.3);
      myPlot.AddTextBox(0.16, 0.92, datasetLable);
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------
    { // -----------------------------------------------------------------------
      string plotName = "variance";
      Plot myPlot(plotName, dataSet);
      
      myPlot.AddHisto("varianceUnfolded", "", "", kFullSquare, kBlue+1);
      myPlot.AddHisto("varianceUnfolded_Syst", "", "unfolded data", kFullSquare, kBlue+1, "boxes");
      myPlot.AddHisto("varianceGeneratedMC", "", "MC", kOpenCircle, kBlack, "");
      myPlot.SetAxisRange("X", 0, multRange);
      myPlot.SetAxisRange("Y", 0., 0.9);
      myPlot.AddLegendBox(0.2, 0.6);
      myPlot.AddTextBox(0.16, 0.92, datasetLable);
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------

    //--------------------------------------------------------------------------
    //---------------------- technical plots -----------------------------------
    //--------------------------------------------------------------------------

    { // -----------------------------------------------------------------------
      string plotName = "multCorrelationMatrix";
      Plot myPlot(plotName, dataSet);
      myPlot.SetDrawingProperties("logZ");
      myPlot.AddHisto(plotName);
      myPlot.SetAxisRange("X", 0, 10);
      myPlot.SetAxisRange("Y", 0, 10);
      myPlot.AddTextBox(0.3, 0.3, datasetLable);
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------
    { // -----------------------------------------------------------------------
      string plotName = "responseMatrixOrig";
      Plot myPlot(plotName, dataSet);
      
      myPlot.SetDrawingProperties("logZ");
      myPlot.AddHisto(plotName);
      //myPlot.SetAxisRange("X", 0, multRange);
      //myPlot.SetAxisRange("Y", 0, multRange);
      myPlot.AddTextBox(0.3, 0.3, datasetLable);
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------
    { // -----------------------------------------------------------------------
      string plotName = "unfoldingMatrix";
      Plot myPlot(plotName, dataSet);
      
      myPlot.SetDrawingProperties("logZ");
      myPlot.AddHisto(plotName);
      //myPlot.SetAxisRange("X", 0, multRange);
      //myPlot.SetAxisRange("Y", 0, multRange);
      myPlot.AddTextBox(0.3, 0.3, datasetLable);
      //myPlot.SetAxisTitle("X", "#it{N}^{ meas}_{ch}");
      //myPlot.SetAxisTitle("Y", "#it{N}^{ true}_{ch}");
      //myPlot.SetAxisTitle("Z", "#it{P}(#it{N}_{acc} | #it{N}_{ch})");
      //myPlot.SetAxisTitle("Z", "#it{P}(#it{N}^{ meas}_{ch} | #it{N}^{ true}_{ch})");
      myPlot.SetAxisTitle("Z", "#it{S}(#it{N}_{acc} | #it{N}_{ch})");
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------
    { // -----------------------------------------------------------------------
      string plotName = "responseMatrix";
      Plot myPlot(plotName, dataSet);
      
      myPlot.SetDrawingProperties("logZ");
      myPlot.AddHisto(plotName);
      //myPlot.SetAxisRange("X", 0, multRange);
      //myPlot.SetAxisRange("Y", 0, multRange);
      myPlot.AddTextBox(0.3, 0.3, datasetLable);
      //myPlot.SetAxisTitle("X", "#it{N}^{ meas}_{ch}");
      //myPlot.SetAxisTitle("Y", "#it{N}^{ true}_{ch}");
      //myPlot.SetAxisTitle("Z", "#it{P}(#it{N}_{acc} | #it{N}_{ch})");
      //myPlot.SetAxisTitle("Z", "#it{P}(#it{N}^{ meas}_{ch} | #it{N}^{ true}_{ch})");
      myPlot.SetAxisTitle("Z", "#it{S}(#it{N}_{acc} | #it{N}_{ch})");
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------
    { // -----------------------------------------------------------------------
      string plotName = "ptResolutionMatrix";
      Plot myPlot(plotName, dataSet);
      
      myPlot.SetDrawingProperties("logX logY logZ");
      //myPlot.SetAxisRange("X", 0.15, 50);
      //myPlot.SetAxisRange("Y", 0.15, 50);
      myPlot.SetAxisTitle("X", "#it{p}^{ meas}_{T}");
      myPlot.SetAxisTitle("Y", "#it{p}^{ true}_{T}");
      myPlot.AddTextBox(0.3, 0.3, datasetLable);
      myPlot.AddHisto(plotName);
      myPlot.SetAxisTitle("Z", "#it{S}(#it{p}^{ meas}_{T} | #it{p}^{ true}_{T})");
      //myPlot.SetAxisTitle("Z", "#it{P}(#it{p}^{ meas}_{T} | #it{p}^{ true}_{T})");
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------
    { // -----------------------------------------------------------------------
      string plotName = "relPtResoFromCovData";
      Plot myPlot(plotName, dataSet);
      
      myPlot.SetDrawingProperties("logZ logX");
      myPlot.SetAxisRange("X", 0.15, 50);
      myPlot.SetAxisRange("Z", 2, 5e7);
      myPlot.SetAxisTitle("Z", "# Tracks");
      myPlot.AddTextBox(0.3, 0.9, datasetLable);
      myPlot.AddHisto(plotName);
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------
    { // -----------------------------------------------------------------------
      string plotName = "relPtResoFromCovMC";
      Plot myPlot(plotName, dataSet);
      
      myPlot.SetDrawingProperties("logZ logX");
      myPlot.SetAxisRange("X", 0.15, 50);
      myPlot.SetAxisTitle("Z", "# Tracks");
      myPlot.AddTextBox(0.3, 0.9, datasetLable);
      myPlot.AddHisto(plotName);
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------

    //--------------------------------------------------------------------------
    //--------------------- QA histograms --------------------------------------
    //--------------------------------------------------------------------------

    // ---------------------- Efficiencies------------------------------------
    { // -----------------------------------------------------------------------
      string plotName = "eventEfficiency";
      Plot myPlot(plotName, dataSet);
      myPlot.AddHisto(plotName);
      myPlot.SetAxisRange("Y", 0.5, 1.01);
      myPlot.SetAxisRange("X", 0, 10);
      myPlot.SetAxisTitle("Y", "efficiency");
      myPlot.AddTextBox(0.41, 0.3, datasetLable);
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------
    { // -----------------------------------------------------------------------
      string plotName = "fakeEventContam";
      Plot myPlot(plotName, dataSet);
      
      myPlot.AddHisto(plotName);
      myPlot.SetAxisRange("X", 0, 10);
      myPlot.AddTextBox(0.41, 0.92, datasetLable);
      myPlot.SetAxisTitle("Y", "contamination");
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------
    { // -----------------------------------------------------------------------
      string plotName = "efficiencyMC";
      Plot myPlot(plotName, dataSet);
      myPlot.AddHisto("primTrackEff", dataSet, "", 0, kBlue);
      myPlot.SetDrawingProperties("logX");
      myPlot.SetAxisRange("X", 0.15, 50);
      myPlot.SetAxisRange("Y", 0.3, 0.9);
      myPlot.AddLegendBox(0.6, 0.4);
      myPlot.AddTextBox(0.15, 0.9, datasetLable);
      myPlot.SetAxisTitle("X", "#it{p}^{ true}_{T}");
      myPlot.SetAxisTitle("Y", "efficiency x acceptance");
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------

    { // -----------------------------------------------------------------------
      string plotName = "secContamMC";
      Plot myPlot(plotName, dataSet);
      
      myPlot.AddHisto(plotName, dataSet, "", 0, kBlue);
      myPlot.SetDrawingProperties("logX");
      myPlot.SetAxisRange("X", 0.15, 50);
      myPlot.SetAxisTitle("X", "#it{p}^{ true}_{T}");
      myPlot.AddTextBox(0.4, 0.9, datasetLable);
      myPlot.SetAxisTitle("Y", "secondary contamination");
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------
    { // -----------------------------------------------------------------------
      string plotName = "secContamMC_mult";
      Plot myPlot(plotName, dataSet);
      
      myPlot.AddHisto(plotName, dataSet, "", 0, kBlue);
      //myPlot.SetDrawingProperties("logX");
      myPlot.SetAxisRange("Y", 0.0, 0.08);
      //myPlot.SetAxisTitle("X", "#it{p}^{ true}_{T}");
      myPlot.AddTextBox(0.4, 0.9, datasetLable);
      myPlot.SetAxisTitle("Y", "secondary contamination");
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------
    { // -----------------------------------------------------------------------
      string plotName = "efficiencyMultPtRaw_MultDep";
      Plot myPlot(plotName, dataSet);
      
      vector<int> ptBins = {3, 5, 7, 11, 13, 15, 17};
      int i = 0;
      for(int ptBin : ptBins)
      {
        string name = "efficiencyMultPtRaw_PtBin_" + std::to_string(ptBin);
        myPlot.AddHisto(name, dataSet, GetPtString(ptBin));
        i++;
      }
      myPlot.SetAxisRange("X", 0., multRange);
      myPlot.SetAxisRange("Y", 0.25, 0.8);
      myPlot.AddLegendBox(0.16, 0.52);
      //myPlot.AddTextBox(0.41, 0.3, datasetLable);
      myPlot.SetAxisTitle("Y", "acceptance x efficiency");
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------
    { // -----------------------------------------------------------------------
      string plotName = "efficiencyMultPtRaw_PtDep";
      Plot myPlot(plotName, dataSet);
      
      myPlot.AddHisto("primTrackEff", dataSet, "integrated");
      vector<int> multBins = {2, 3, 5, 7, 9, 11};
      for(int multBin : multBins)
      {
        string multLable = "#it{N}_{ch} = " + std::to_string(multBin-1);
        string name = "efficiencyMultPtRaw_MultBin_" + std::to_string(multBin);
        myPlot.AddHisto(name, dataSet, multLable);
      }
      myPlot.SetDrawingProperties("logX");
      myPlot.AddLegendBox(0.22, 0.52);
      myPlot.AddTextBox(0.41, 0.3, datasetLable);
      myPlot.SetAxisRange("Y", 0.2, 0.9);
      myPlot.SetAxisTitle("Y", "acceptance x efficiency");
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------
    { // -----------------------------------------------------------------------
      string plotName = "secContamMultPtRaw_MultDep";
      Plot myPlot(plotName, dataSet);
      
      vector<int> ptBins = {3, 5, 7, 11, 13, 15, 17};
      int i = 0;
      for(int ptBin : ptBins)
      {
        string name = "secContamMultPtRaw_PtBin_" + std::to_string(ptBin);
        myPlot.AddHisto(name, dataSet, GetPtString(ptBin));
        i++;
      }
      myPlot.AddLegendBox(0.16, 0.92, "");
      myPlot.SetAxisRange("X", 0., multRange);
      myPlot.SetAxisRange("Y", 0, 0.12);
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------
    { // -----------------------------------------------------------------------
      string plotName = "secondariesMultPtRaw_PtDep";
      Plot myPlot(plotName, dataSet);
      myPlot.AddHisto("secContamMC", dataSet, "MC truth");
      myPlot.AddHisto("secContamMultPtRaw_full", dataSet, "inclusive");
      vector<int> multBins = {2, 3, 5, 7, 9, 11};
      for(int multBin : multBins)
      {
        string name = "secContamMultPtRaw_MultBin_" + std::to_string(multBin);
        myPlot.AddHisto(name, dataSet, std::to_string(multBin));
      }
      myPlot.SetDrawingProperties("logX");
      myPlot.AddLegendBox(0.7, 0.3);
      myPlot.SetAxisRange("Y", 0, 0.15);
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------
    { // -----------------------------------------------------------------------
      string plotName = "meanRelPtReso_DataVsMC";
      Plot myPlot(plotName, dataSet, "default ratio");
      myPlot.SetDrawingProperties("logX");
      myPlot.SetAxisTitle("Y", "< #sigma(#it{p}_{T}) / #it{p}_{T} >");
      myPlot.SetAxisRange("X", 0.15, 50);
      myPlot.SetAxisRange("ratio", 0.95, 1.05);
      myPlot.AddHisto("meanRelPtResoFromCovMC", "", "MC");
      myPlot.AddHisto("meanRelPtResoFromCovData", "", "data");
      myPlot.AddRatio("meanRelPtResoFromCovData", "", "meanRelPtResoFromCovMC");
      myPlot.SetAxisTitle("ratio", "data / MC");
      myPlot.AddLegendBox(0.4, 0.5, "");
      myPlot.AddTextBox(0.3, 0.9, datasetLable);
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------
    // ---------------------- Closure tests ------------------------------------
    { // -----------------------------------------------------------------------
      string plotName = "closureTestMultDist";
      Plot myPlot(plotName, dataSet, "default ratio");
      myPlot.SetFigureCategory(categoryClosure);
      myPlot.AddHisto("multDistGeneratedClosureTest", "", "generated (prior)", kFullSquare, kGray+1);
      myPlot.AddHisto("multDistUnfoldedClosureTest", "", "unfolded", kOpenCircle);
      myPlot.AddHisto("multDistMeasuredClosureTest", "", "measured");
      myPlot.AddRatio("multDistUnfoldedClosureTest", "", "multDistGeneratedClosureTest", "", "");
      myPlot.SetAxisTitle("ratio", "ratio");
      myPlot.SetDrawingProperties("logY");
      myPlot.SetAxisRange("X", 0, multRange);
      //myPlot.SetAxisRange("Y", 0.9, 3e5);
      myPlot.AddLegendBox(0.6, 0.9);
      myPlot.AddTextBox(0.16, 0.2, datasetLable);
      myPlot.SetAxisRange("ratio", 0.95, 1.05);
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------
    { // -----------------------------------------------------------------------
      string plotName = "closureTestMultDistFlat";
      Plot myPlot(plotName, dataSet, "default ratio");
      myPlot.SetFigureCategory(categoryClosure);
      myPlot.AddHisto("multDistGeneratedClosureTest", "", "generated", kFullSquare, kGray+1);
      myPlot.AddHisto("multDistUnfoldedClosureTestFlat", "", "unfolded", kOpenCircle);
      myPlot.AddHisto("multDistMeasuredClosureTest", "", "measured");
      myPlot.AddHisto("multDistInitialClosureTestFlat", "", "prior", 0, 0, "hist");
      myPlot.AddRatio("multDistUnfoldedClosureTestFlat", "", "multDistGeneratedClosureTest", "", "");
      myPlot.SetAxisTitle("ratio", "ratio");
      myPlot.SetDrawingProperties("logY");
      myPlot.SetAxisRange("X", 0, multRange);
      //myPlot.SetAxisRange("Y", 0.9, 3e5);
      myPlot.SetAxisRange("ratio", 0.95, 1.05);
      myPlot.AddLegendBox(0.7, 0.8);
      myPlot.AddTextBox(0.16, 0.2, datasetLable);
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------
    { // -----------------------------------------------------------------------
      string plotName = "closureTestProjectionPt";
      Plot myPlot(plotName, dataSet, "default ratio");
      myPlot.SetFigureCategory(categoryClosure);
      myPlot.SetDrawingProperties("logX logY");
      myPlot.SetAxisRange("X", 0.15, 50);
      myPlot.AddHisto("trueParticlesPt", "", "true");
      myPlot.AddHisto("unfoParticlesPt", "", "unfolded");
      myPlot.AddHisto("TEST", "", "TEST");
      myPlot.AddHisto("TESTDATA", "", "TESTDATA");
      myPlot.AddHisto("TESTDATAMEAS", "", "TESTDATAMEAS");
      myPlot.AddHisto("TESTMCMEAS", "", "TESTMCMEAS");
      myPlot.AddRatio("unfoParticlesPt", "", "trueParticlesPt");
//      myPlot.SetAxisRange("ratio", 0.9, 1.1);
      myPlot.SetAxisTitle("X", "#it{p}_{T} (GeV/#it{c})");
      myPlot.SetAxisTitle("ratio", "ratio");
      myPlot.AddLegendBox(0.7, 0.85);
      myPlot.AddTextBox(0.16, 0.2, datasetLable);
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------
    { // -----------------------------------------------------------------------
      string plotName = "closureTestProjectionMult";
      Plot myPlot(plotName, dataSet, "default ratio");
      myPlot.SetFigureCategory(categoryClosure);
      myPlot.AddHisto("trueParticlesMult", "", "true");
      myPlot.AddHisto("unfoParticlesMult", "", "unfolded");
      myPlot.AddRatio("unfoParticlesMult", "", "trueParticlesMult");
      myPlot.SetAxisTitle("ratio", "ratio");
      myPlot.SetDrawingProperties("logY");
      myPlot.SetAxisRange("ratio", 0.95, 1.05);
      myPlot.AddLegendBox(0.7, 0.7);
      myPlot.AddTextBox(0.16, 0.2, datasetLable);
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------
    { // -----------------------------------------------------------------------
      string plotName = "closureTestMoment1";
      Plot myPlot(plotName, dataSet, "default ratio");
      myPlot.SetFigureCategory(categoryClosure);
      myPlot.AddHisto("momentGeneratedMC1", "", "generated");
      myPlot.AddHisto("momentUnfoldedMC1", "", "unfolded");
      myPlot.AddHisto("momentReweightedMC1", "", "re-weighted");
      myPlot.AddRatio("momentUnfoldedMC1", "", "momentGeneratedMC1");
      myPlot.AddRatio("momentReweightedMC1", "", "momentGeneratedMC1");
      myPlot.SetAxisRange("X", 0, multRange);
      myPlot.SetAxisRange("Y", 0.45, 0.85);
      myPlot.SetAxisRange("ratio", 0.95, 1.05);
      myPlot.SetAxisTitle("ratio", "ratio");
      myPlot.AddLegendBox(0.6, 0.4);
      myPlot.AddTextBox(0.16, 0.92, datasetLable);
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------
    { // -----------------------------------------------------------------------
      string plotName = "closureTestMoment2";
      Plot myPlot(plotName, dataSet, "default ratio");
      myPlot.SetFigureCategory(categoryClosure);
      myPlot.AddHisto("momentGeneratedMC2", "", "generated");
      myPlot.AddHisto("momentUnfoldedMC2", "", "unfolded");
      //myPlot.AddHisto("momentReweightedMC2", "", "re-weighted");
      myPlot.AddRatio("momentUnfoldedMC2", "", "momentGeneratedMC2");
      //myPlot.AddRatio("momentReweightedMC2", "", "momentGeneratedMC2");
      myPlot.SetAxisRange("X", 0, multRange);
      myPlot.SetAxisRange("Y", 0.2, 1.6);
      myPlot.SetAxisRange("ratio", 0.95, 1.05);
      myPlot.SetAxisTitle("ratio", "ratio");
      myPlot.AddLegendBox(0.6, 0.4);
      myPlot.AddTextBox(0.16, 0.92, datasetLable);
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------
    { // -----------------------------------------------------------------------
      string plotName = "closureTestMoment3";
      Plot myPlot(plotName, dataSet, "default ratio");
      myPlot.SetFigureCategory(categoryClosure);
      myPlot.AddHisto("momentGeneratedMC3", "", "generated");
      myPlot.AddHisto("momentUnfoldedMC3", "", "unfolded");
      //myPlot.AddHisto("momentReweightedMC3", "", "re-weighted");
      myPlot.AddRatio("momentUnfoldedMC3", "", "momentGeneratedMC3");
      //myPlot.AddRatio("momentReweightedMC3", "", "momentGeneratedMC3");
      myPlot.SetAxisRange("X", 0, multRange);
      myPlot.SetAxisRange("Y", 0.1, 7.0);
      myPlot.SetAxisRange("ratio", 0.95, 1.05);
      myPlot.SetAxisTitle("ratio", "ratio");
      myPlot.AddLegendBox(0.7, 0.3);
      myPlot.AddTextBox(0.16, 0.92, datasetLable);
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------
    { // -----------------------------------------------------------------------
      for(int ptBin = 2; ptBin < 50; ptBin++){
        if(!createBinWiseClosureTests) break;
        string plotName = "closureTestPtBin_" + std::to_string(ptBin);
        Plot myPlot(plotName, dataSet, "default ratio");
        myPlot.SetFigureCategory(categoryClosure + "/pT-Bins");
        myPlot.AddHisto(string("multPtGeneratedMC_PtBin_") + std::to_string(ptBin), "", "", kFullSquare, kBlack);
        myPlot.AddHisto(string("multPtMeasuredMC_PtBin_") + std::to_string(ptBin), "", "measured", kFullCross, kGreen+3);
        myPlot.AddHisto(string("multPtGeneratedMC_PtBin_") + std::to_string(ptBin), "", "generated", kFullSquare, kBlack);
        myPlot.AddHisto(string("multPtUnfoldedMC_PtBin_") + std::to_string(ptBin), "", "unfolded", kFullCircle, kBlue+1);
        myPlot.AddRatio(string("multPtUnfoldedMC_PtBin_") + std::to_string(ptBin), "", string("multPtGeneratedMC_PtBin_") + std::to_string(ptBin));
        myPlot.SetAxisRange("X", 0, multRange);
        myPlot.SetAxisRange("ratio", 0.95, 1.05);
        myPlot.SetAxisTitle("ratio", "ratio");
        myPlot.AddTextBox(0.6, 0.65, GetPtString(ptBin));
        myPlot.AddLegendBox(0.7, 0.5);
        myPlot.AddTextBox(0.42, 0.92, datasetLable);
        plotEnv.AddPlot(myPlot);
      }
    } // -----------------------------------------------------------------------
    { // -----------------------------------------------------------------------
      for(int multBin = 2; multBin < 101; multBin++){
        if(!createBinWiseClosureTests) break;
        string plotName = "closureTestMultBin_" + std::to_string(multBin);
        Plot myPlot(plotName, dataSet, "default ratio");
        myPlot.SetFigureCategory(categoryClosure + "/Nch-Bins");
        myPlot.SetDrawingProperties("logX logY");
        myPlot.AddHisto(string("multPtGeneratedMC_MultBin_") + std::to_string(multBin), "", "", kFullSquare, kBlack);
        myPlot.AddHisto(string("multPtMeasuredMC_MultBin_") + std::to_string(multBin), "", "measured", kFullCross, kGreen+3);
        myPlot.AddHisto(string("multPtGeneratedMC_MultBin_") + std::to_string(multBin), "", "generated", kFullSquare, kBlack);
        myPlot.AddHisto(string("multPtUnfoldedMC_MultBin_") + std::to_string(multBin), "", "unfolded", kFullCircle, kBlue+1);
        myPlot.AddRatio(string("multPtUnfoldedMC_MultBin_") + std::to_string(multBin), "", string("multPtGeneratedMC_MultBin_") + std::to_string(multBin));
        myPlot.SetAxisRange("ratio", 0.9, 1.1);
        myPlot.SetAxisTitle("ratio", "ratio");
        myPlot.AddTextBox(0.72, 0.9, "#it{N}_{ch} = " + std::to_string(multBin-1));
        myPlot.AddLegendBox(0.7, 0.7);
        myPlot.AddTextBox(0.16, 0.2, datasetLable);
        plotEnv.AddPlot(myPlot);
      }
    } // -----------------------------------------------------------------------

    //--------------------------------------------------------------------------
    //----------------------- Systematics --------------------------------------
    //--------------------------------------------------------------------------


    { // -----------------------------------------------------------------------
      vector<string> histNames = {"multDistUnfolded", "momentUnfolded1", "varianceUnfolded"};//, "momentUnfolded2", "momentUnfolded3", "momentReweighted1", "momentReweighted2", "momentReweighted3"};
      Short_t colorsSyst[14]={kPink+8, kGreen+2, 28, kOrange+2 ,8 , kCyan-6, kMagenta+3, kRed, kBlue ,15, kGreen+4, kMagenta, 17, 12};
      for(string& histName : histNames)
      {
        string plotName = histName + "_SystContrib";
        Plot myPlot(plotName, dataSet, "default");
        myPlot.SetDrawingProperties("thick");
        myPlot.AddHisto(histName + "_RelSyst_total", "", "", 0, kWhite, "hist");
        myPlot.AddHisto(histName + "_RelSyst_DCAtoVertexZ", "", " 1", 0, colorsSyst[0], "hist");
        myPlot.AddHisto(histName + "_RelSyst_DCAtoVertexXYPtDep", "", " 2", 0, colorsSyst[1], "hist");
        myPlot.AddHisto(histName + "_RelSyst_RatioCrossedRowsOverFindableClustersTPC", "", " 3", 0, colorsSyst[2], "hist");
        myPlot.AddHisto(histName + "_RelSyst_FractionSharedClustersTPC", "", " 4", 0, colorsSyst[3], "hist");
        myPlot.AddHisto(histName + "_RelSyst_Maxchi2perTPCclu", "", " 5", 0, colorsSyst[4], "hist");
        myPlot.AddHisto(histName + "_RelSyst_Maxchi2perITSclu", "", " 6", 0, colorsSyst[5], "hist");
        myPlot.AddHisto(histName + "_RelSyst_ClusterReqITS", "", " 7", 0, colorsSyst[6], "hist");
        myPlot.AddHisto(histName + "_RelSyst_DeadzoneWidth", "", " 8", 0, colorsSyst[7], "hist");
        myPlot.AddHisto(histName + "_RelSyst_Ncrnclgeomlength", "", " 9", 0, colorsSyst[8], "hist");
        myPlot.AddHisto(histName + "_RelSyst_MaxChi2TPCConstrained", "", "10", 0, colorsSyst[9], "hist");
        myPlot.AddHisto(histName + "_RelSyst_nonClosure", "", "11", 0, colorsSyst[11], "hist");
        myPlot.AddHisto(histName + "_RelSyst_total", "", "total", 0, kBlack, "hist");
        //myPlot.AddHisto(histName + "_RelSyst_ZVertexSelection", "", "11", 0, colorsSyst[10], "hist");
        myPlot.AddTextBox(0.39, 0.91, datasetLable);
//        myPlot.AddLegendBox(0.15, 0.8, "", 2);
        myPlot.SetAxisRange("X", 0, multRange);
        myPlot.SetAxisRange("X", 0, 100);
        //myPlot.SetAxisRange("Y", 0, 0.06);
        plotEnv.AddPlot(myPlot);
      }
    } // -----------------------------------------------------------------------
  } //==========================================================================

  //if(plotEnv.ContainsDatasets({"pp_5TeV", "pPb_5TeV", "PbPb_5TeV"}))
  { //==========================================================================
    string plotGroup = "system_comparison";
    string systemSizeLable = alice + newLine + chargedParticles + ", " + erg5TeV_NN + newLine + eta08 + ", " + ptRange;
    string systemSizeLablePrel = alicePrel + newLine + chargedParticles + ", " + erg5TeV_NN + newLine + eta08 + ", " + ptRange;
    vector<string> centrality = {"0-5%","5-10%","10-20%","20-30%","30-40%","40-50%","50-60%","60-70%","70-80%"};
    vector<double> nchCent = {2869.49, 2342.32, 1740.05, 1156.23, 750.512, 463.796, 265.249, 138.504, 64.0346};
    vector<double> nchCentErrors = {82.7027, 70.8452, 45.7133, 29.4205, 21.9877, 17.5017, 11.5956, 8.3419, 4.84894};
    //bool drawcent = false;
    bool drawcent = false;

    
    { // -----------------------------------------------------------------------
      string plotName = "meanPtGeometricalScaling";
      Plot myPlot(plotName, plotGroup);
      myPlot.AddGraph("meanPtScaled_pp_2TeV", "GeometricalScaling", "", 0, 0, "", 2.14);
      myPlot.AddGraph("meanPtScaled_pp_2TeV_Syst", "GeometricalScaling", "pp, 2.76 TeV", -1, -1, "boxes", 2.14);
      myPlot.AddGraph("meanPtScaled_pp_5TeV", "GeometricalScaling", "", 0, 0, "", 2.8);
      myPlot.AddGraph("meanPtScaled_pp_5TeV_Syst", "GeometricalScaling", "pp, 5.02 TeV", -1, -1, "boxes", 2.8);
      myPlot.AddGraph("meanPtScaled_pp_7TeV", "GeometricalScaling", "", 0, 0, "", 2.8);
      myPlot.AddGraph("meanPtScaled_pp_7TeV_Syst", "GeometricalScaling", "pp, 7 TeV", -1, -1, "boxes", 2.8);
      myPlot.AddGraph("meanPtScaled_pp_13TeV", "GeometricalScaling", "", 0, 0, "", 3.3);
      myPlot.AddGraph("meanPtScaled_pp_13TeV_Syst", "GeometricalScaling", "pp, 13 TeV", -1, -1, "boxes", 3.3);
      myPlot.AddGraph("meanPtScaled_pPb_5TeV", "GeometricalScaling", "", 0, 0, "", 2.5);
      myPlot.AddGraph("meanPtScaled_pPb_5TeV_Syst", "GeometricalScaling", "p-Pb, 5.02 TeV", -1, -1, "boxes", 2.5);
      myPlot.AddGraph("meanPtScaled_pPb_8TeV", "GeometricalScaling", "", 0, 0, "", 2.6);
      myPlot.AddGraph("meanPtScaled_pPb_8TeV_Syst", "GeometricalScaling", "p-Pb, 8 TeV", -1, -1, "boxes", 2.6);
      myPlot.AddLegendBox(0.15, 0.9);
      myPlot.SetAxisRange("X", 0.7, 3.7);
      myPlot.SetAxisRange("Y", 0.48, 1.0);

      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------
    
    
    { // -----------------------------------------------------------------------
      string plotName = "multPtRatio_pPbOverpp";
      Plot myPlot(plotName, plotGroup);
      myPlot.SetDrawingProperties("logY");
      myPlot.AddRatio("reducedMultPtpPb", "Fits", "multPtUnfoldedNormalized", "pp_5TeV"); // This does not work!!
      myPlot.SetAxisRange("Z", 0.0, 2.0);
      myPlot.SetAxisRange("X", 0, 60);
      myPlot.SetAxisRange("Y", 0.15, 20.0);
      myPlot.SetAxisTitle("Y", "#it{p}_{T} (GeV/#it{c})");
      myPlot.SetAxisTitle("X", "#it{N}_{ch}");
      myPlot.SetAxisTitle("Z", "p-Pb / pp");
      myPlot.AddTextBox(0.2, 0.9, systemSizeLable);
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------
    
    { // -----------------------------------------------------------------------
      string plotName = "multPtRatio_PbPbOverpp";
      Plot myPlot(plotName, plotGroup);
      myPlot.SetDrawingProperties("logY");
      myPlot.AddRatio("reducedMultPtPbPb", "Fits", "multPtUnfoldedNormalized", "pp_5TeV");
      myPlot.SetAxisRange("Z", 0.0, 2.0);
      myPlot.SetAxisRange("X", 0, 60);
      myPlot.SetAxisRange("Y", 0.15, 20.0);
      myPlot.SetAxisTitle("Y", "#it{p}_{T} (GeV/#it{c})");
      myPlot.SetAxisTitle("X", "#it{N}_{ch}");
      myPlot.SetAxisTitle("Z", "Pb-Pb / pp");
      myPlot.AddTextBox(0.2, 0.9, systemSizeLable);
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------

    { // -----------------------------------------------------------------------
      string plotName = "studyExtremeMultPbPbXeXe";
      Plot myPlot(plotName, plotGroup, "default ratio");
      myPlot.SetDrawingProperties("logX logY");
      myPlot.AddHisto("michaelRatio_XeXe", "Fits", "Xe-Xe", 0, kBlue+2);
      myPlot.AddHisto("michaelRatio_PbPb", "Fits", "Pb-Pb", 0, kBlack);
      myPlot.AddRatio("michaelRatio_XeXe", "Fits", "michaelRatio_PbPb", "Fits", "", 0, kBlue+2);
      myPlot.AddLegendBox(0.6, 0.9);
      myPlot.AddTextBox(0.3, 0.5, "1200 < #it{N}_{ch} < 2300");
      myPlot.SetAxisRange("ratio", 0.6, 1.1);
      myPlot.AddTextBox(0.18, 0.2, systemSizeLable);
      
      myPlot.SetAxisTitle("ratio", "extreme / nearby");
      myPlot.ChangePad(2);
      myPlot.SetDrawingProperties("logX");
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------

    { // -----------------------------------------------------------------------
      string plotName = "studyExtremeMultPbPb";
      Plot myPlot(plotName, plotGroup, "default ratio");
      myPlot.SetDrawingProperties("logX logY");
      myPlot.AddHisto("spectraExtreme_PbPb", "Fits", "#it{N}_{ch} > 2000", 0, kBlue+2);
      myPlot.AddHisto("spectraNearby_PbPb", "Fits", "700 < #it{N}_{ch} < 1200", 0, kBlack);
      myPlot.AddRatio("spectraExtreme_PbPb", "Fits", "spectraNearby_PbPb", "Fits", "", 0, kBlue+2);
      myPlot.AddLegendBox(0.6, 0.9);
      myPlot.AddTextBox(0.4, 0.5, "#bf{Pb-Pb}");
      myPlot.SetAxisRange("ratio", 1.3, 3.3);
      myPlot.AddTextBox(0.18, 0.2, systemSizeLable);

      myPlot.SetAxisTitle("ratio", "extreme / nearby");
      myPlot.ChangePad(2);
      myPlot.SetDrawingProperties("logX");
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------

    { // -----------------------------------------------------------------------
      string plotName = "studyExtremeMultXeXe";
      Plot myPlot(plotName, plotGroup, "default ratio");
      myPlot.SetDrawingProperties("logX logY");
      myPlot.AddHisto("spectraExtreme_XeXe", "Fits", "#it{N}_{ch} > 1200", 0, kBlue+2);
      myPlot.AddHisto("spectraNearby_XeXe", "Fits", "500 < #it{N}_{ch} < 800", 0, kBlack);
      myPlot.AddRatio("spectraExtreme_XeXe", "Fits", "spectraNearby_XeXe", "Fits", "", 0, kBlue+2);
      myPlot.AddLegendBox(0.6, 0.9);
      myPlot.AddTextBox(0.4, 0.5, "#bf{Xe-Xe}");
      myPlot.AddTextBox(0.18, 0.2, systemSizeLable);
      myPlot.SetAxisTitle("ratio", "extreme / nearby");
      myPlot.SetAxisRange("ratio", 1.3, 3.3);

      myPlot.ChangePad(2);
      myPlot.SetDrawingProperties("logX");
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------
    
    
    
    { // -----------------------------------------------------------------------
      string plotName = "studyExtremeMultPP";
      Plot myPlot(plotName, plotGroup, "default ratio");
      myPlot.SetDrawingProperties("logX logY");
      myPlot.AddHisto("inclusiveSpectrum", "pp_5TeV", "", 0, kBlack);
      myPlot.AddHisto("spectraExtreme_pp", "Fits", "#it{N}_{ch} > 40", 0, kBlue+2);
      myPlot.AddHisto("inclusiveSpectrum", "pp_5TeV", "all #it{N}_{ch}", 0, kBlack);
      myPlot.AddRatio("spectraExtreme_pp", "Fits", "inclusiveSpectrum", "pp_5TeV", "", 0, kBlue+2);
      myPlot.AddLegendBox(0.6, 0.9);
      myPlot.AddTextBox(0.4, 0.5, "#bf{pp}");
      myPlot.AddTextBox(0.18, 0.2, systemSizeLable);
      myPlot.SetAxisTitle("ratio", "extreme/MB");
//      myPlot.SetAxisRange("ratio", 1.3, 3.3);

      myPlot.ChangePad(2);
      myPlot.SetDrawingProperties("logX");
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------

    { // -----------------------------------------------------------------------
      string plotName = "energyDepHighNchSpectra";
      Plot myPlot(plotName, plotGroup, "default ratio");
      myPlot.SetDrawingProperties("logX logY");
      myPlot.AddHisto("spectraHighNch_pp_13TeV", "Fits", "", 0, 0);
      myPlot.AddHisto("spectraHighNch_pp_2TeV", "Fits", "", 0, 0);
      myPlot.AddHisto("spectraHighNch_pp_5TeV", "Fits", "", 0, 0);
      myPlot.AddHisto("spectraHighNch_pp_7TeV", "Fits", "", 0, 0);
//      myPlot.AddRatio("spectraHighNch_pp_2TeV", "Fits", "spectraHighNch_pp_13TeV", "Fits", "", 0);
      myPlot.AddRatio("spectraHighNch_pp_5TeV", "Fits", "spectraHighNch_pp_13TeV", "Fits", "", 0);
      myPlot.AddRatio("spectraHighNch_pp_7TeV", "Fits", "spectraHighNch_pp_13TeV", "Fits", "", 0);
      myPlot.AddLegendBox(0.6, 0.9);
      myPlot.AddTextBox(0.4, 0.5, "#bf{pp}");
      myPlot.AddTextBox(0.18, 0.2, systemSizeLable);
      myPlot.SetAxisTitle("ratio", "ratio to 13");
//      myPlot.SetAxisRange("ratio", 1.3, 3.3);

      myPlot.ChangePad(2);
      myPlot.SetDrawingProperties("logX");
      myPlot.SetAxisTitle("X", "#it{p}_{T}");

      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------

    
    { // -----------------------------------------------------------------------
      string plotName = "studyExtremeMultPPb";
      Plot myPlot(plotName, plotGroup, "default ratio");
      myPlot.SetDrawingProperties("logX logY");
      myPlot.AddHisto("inclusiveSpectrum", "pPb_5TeV", "", 0, kBlack);
      myPlot.AddHisto("spectraExtreme_pp", "Fits", "#it{N}_{ch} > 100", 0, kBlue+2);
      myPlot.AddHisto("inclusiveSpectrum", "pPb_5TeV", "all #it{N}_{ch}", 0, kBlack);
      myPlot.AddRatio("spectraExtreme_pp", "Fits", "inclusiveSpectrum", "pPb_5TeV", "", 0, kBlue+2);
      myPlot.AddLegendBox(0.6, 0.9);
      myPlot.AddTextBox(0.4, 0.5, "#bf{p-Pb}");
      myPlot.AddTextBox(0.18, 0.2, systemSizeLable);
      myPlot.SetAxisTitle("ratio", "extreme/MB");
//      myPlot.SetAxisRange("ratio", 1.3, 3.3);

      myPlot.ChangePad(2);
      myPlot.SetDrawingProperties("logX");
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------

    { // -----------------------------------------------------------------------
      string plotName = "studyLowMult";
      Plot myPlot(plotName, plotGroup, "default ratio");
      myPlot.SetDrawingProperties("logX logY");
      myPlot.AddHisto("spectraNch2_PbPb", "Fits", "Pb-Pb, #it{N}_{ch} = 2", kFullSquare, kGreen+2);
      myPlot.AddHisto("spectraNch2_XeXe", "Fits", "Xe-Xe, #it{N}_{ch} = 2", kFullCircle, kGreen+2);
      myPlot.AddHisto("spectraNch1_PbPb", "Fits", "Pb-Pb, #it{N}_{ch} = 1", kFullSquare, kBlue+2);
      myPlot.AddHisto("spectraNch1_XeXe", "Fits", "Xe-Xe, #it{N}_{ch} = 1", kFullCircle, kBlue+2);

      myPlot.AddRatio("spectraNch1_PbPb", "Fits", "spectraNch1_XeXe", "Fits", "", kFullSquare, kBlue+2);
      myPlot.AddRatio("spectraNch2_PbPb", "Fits", "spectraNch2_XeXe", "Fits", "", kFullSquare, kGreen+2);
      
      myPlot.SetAxisRange("X", 0.15, 10);
      myPlot.SetAxisRange("ratio", 0.5, 1.5);

      myPlot.AddLegendBox(0.2, 0.4);
      //myPlot.AddTextBox(0.4, 0.3, "#bf{Xe-Xe}");
      myPlot.SetAxisTitle("ratio", "Pb / Xe");
      myPlot.ChangePad(2);
      myPlot.SetDrawingProperties("logX");
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------

    { // -----------------------------------------------------------------------
      string plotName = "meanPtFullRangeLin";
      Plot myPlot(plotName, plotGroup);
      myPlot.AddHisto("momentUnfolded1", "PbPb_5TeV", "", kFullCross, kRed+1, "");
      myPlot.AddHisto("momentUnfolded1_Syst", "PbPb_5TeV", "Pb-Pb", kFullCross, kRed+1, "boxes");
      myPlot.AddHisto("momentUnfolded1", "XeXe_5TeV", "", kFullStar, kGreen+2, "");
      myPlot.AddHisto("momentUnfolded1_Syst", "XeXe_5TeV", "Xe-Xe", kFullStar, kGreen+2, "boxes");
      myPlot.SetAxisTitle("X", "#it{N}_{ch}");
      
      //myPlot.AddLegendBox(0.2, 0.8);
      myPlot.SetAxisRange("Y", 0.69, 0.74);
      myPlot.AddTextBox(0.4, 0.3, systemSizeLable);
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------

    
    { // -----------------------------------------------------------------------
      string plotName = "meanPtFullRange";
      Plot myPlot(plotName, plotGroup);
      myPlot.SetFigureCategory("mostImportantPlots");
      myPlot.SetDrawingProperties("logX");
      myPlot.AddHisto("momentUnfolded1", "PbPb_5TeV", "", kFullCross, kRed+1, "", 3000);

      //p-p
      myPlot.AddHisto("momentUnfolded1", "pp_5TeV", "", kFullSquare, kBlue+1, "", 60);
      myPlot.AddHisto("momentUnfolded1_Syst", "pp_5TeV", "pp", kFullSquare, kBlue+1, "boxes", 60);
      //myPlot.AddHisto("meanPt_pp_EPOS-LHC_5.02TeV", "Simulations", "", kOpenSquare, kBlue+1, "", 60);

      //p-Pb
      myPlot.AddHisto("momentUnfolded1", "pPb_5TeV", "", kFullCircle, kMagenta+1, "", 110);
      myPlot.AddHisto("momentUnfolded1_Syst", "pPb_5TeV", "p-Pb", kFullCircle, kMagenta+1, "boxes", 110);
      //myPlot.AddHisto("momentGeneratedMC1", "pPb_5TeV", "", kOpenCircle, kMagenta+1, "", 110);//DPMJET
      //myPlot.AddHisto("meanPt_pPb_EPOS-LHC_5.02TeV", "Simulations", "", kOpenCircle, kMagenta+1, "", 110);
      //myPlot.AddHisto("meanPt_pPb_Angantyr_5.02TeV", "Simulations", "", kFullStar, kMagenta+1, "", 110);

      myPlot.AddHisto("momentUnfolded1", "pPb_8TeV", "", kFullCircle, kYellow+1, "", 100);
      myPlot.AddHisto("momentUnfolded1_Syst", "pPb_8TeV", "pPb, 8.16 TeV", kFullCircle, kYellow+1, "boxes", 100);

      
      //Pb-Pb
      myPlot.AddHisto("momentUnfolded1", "PbPb_5TeV", "", kFullCross, kRed+1, "", 3000);
      myPlot.AddHisto("momentUnfolded1_Syst", "PbPb_5TeV", "Pb-Pb", kFullCross, kRed+1, "boxes", 3000);
      //myPlot.AddHisto("momentGeneratedMC1", "PbPb_5TeV", "", kOpenCross, kRed+1, "", 3000);//HIJING
      //myPlot.AddHisto("meanPt_PbPb_EPOS-LHC_5.02TeV", "Simulations", "", kOpenCross, kRed+1, "");
      //myPlot.AddHisto("meanPt_PbPb_Angantyr_5.02TeV", "Simulations", "", kFullStar, kRed+1, "", 3000);

      //Xe-Xe
      myPlot.AddHisto("momentUnfolded1", "XeXe_5TeV", "", kFullStar, kGreen+2, "", 2000);
      myPlot.AddHisto("momentUnfolded1_Syst", "XeXe_5TeV", "Xe-Xe", kFullStar, kGreen+2, "", 2000);
      //myPlot.AddHisto("momentGeneratedMC1", "XeXe_5TeV", "HIJING", kFullStar, kGreen+1, "", 2000);


      //myPlot.AddGraph("meanPtPbPb_5TeV", "Publications", "Pb-Pb published", kFullCross, kGreen+2);
      myPlot.SetAxisRange("X", 0.1, 4000);
      myPlot.SetAxisRange("Y", 0.35, 0.85);
      myPlot.SetAxisTitle("X", "#it{N}_{ch}");

      myPlot.AddLegendBox(0.15, 0.85);
      //myPlot.AddTextBox(0.36, 0.4, systemSizeLable + " // Open symbols: EPOS-LHC // Stars Pythia Angantyr");
      myPlot.AddTextBox(0.36, 0.4, systemSizeLable);

      if(drawcent){
        for(int i = 0; i < 9; i++)
        {
          for(int j = 0; j < 6; j++)
          {
            //myPlot.AddTextBox(nchCent[i], 0.705 + 0.01*j, "#bf{|}", true);
          }
          myPlot.AddTextBox(nchCent[i], 0.69 - 0.02*i, string("#bf{") + centrality[i] + string("}"), true);
        }
      }
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------

    { // -----------------------------------------------------------------------
      string plotName = "varianceFullRange";
      Plot myPlot(plotName, plotGroup);
      
      myPlot.SetDrawingProperties("logX");
      myPlot.AddHisto("varianceUnfolded", "PbPb_5TeV", "", kFullCross, kRed+1, "", 3000);
      myPlot.AddHisto("varianceUnfolded", "pp_5TeV", "", kFullSquare, kBlue+1, "", 60);
      myPlot.AddHisto("varianceUnfolded_Syst", "pp_5TeV", "pp", kFullSquare, kBlue+1, "boxes", 60);
      //myPlot.AddHisto("variance_pp_EPOS-LHC_5.02TeV", "Simulations", "", kOpenSquare, kBlue+1, "", 60);
      myPlot.AddHisto("varianceUnfolded", "pPb_5TeV", "", kFullCircle, kMagenta+1, "", 100);
      myPlot.AddHisto("varianceUnfolded_Syst", "pPb_5TeV", "p-Pb", kFullCircle, kMagenta+1, "boxes", 100);
      //myPlot.AddHisto("variance_pPb_EPOS-LHC_5.02TeV", "Simulations", "", kOpenCircle, kMagenta+1, "", 100);
      //myPlot.AddHisto("variance_pPb_Angantyr_5.02TeV", "Simulations", "", kFullStar, kMagenta+1, "", 100);
      myPlot.AddHisto("varianceUnfolded", "PbPb_5TeV", "", kFullCross, kRed+1, "", 3000);
      myPlot.AddHisto("varianceUnfolded_Syst", "PbPb_5TeV", "Pb-Pb", kFullCross, kRed+1, "boxes", 3000);
      //myPlot.AddHisto("variance_PbPb_EPOS-LHC_5.02TeV", "Simulations", "", kOpenCircle, kRed+1, "", 3000);
      //myPlot.AddHisto("variance_PbPb_Angantyr_5.02TeV", "Simulations", "", kFullStar, kRed+1, "", 3000);
      
      myPlot.AddHisto("varianceUnfolded", "pPb_8TeV", "", kFullCircle, kYellow+1, "", 90);
      myPlot.AddHisto("varianceUnfolded_Syst", "pPb_8TeV", "p-Pb, 8.16 TeV", kFullCircle, kYellow+1, "boxes", 90);

      myPlot.AddHisto("varianceUnfolded", "XeXe_5TeV", "", kFullStar, kGreen+2, "", 1100);
      myPlot.AddHisto("varianceUnfolded_Syst", "XeXe_5TeV", "Xe-Xe", kFullStar, kGreen+2, "boxes", 1100);

      myPlot.SetAxisRange("X", 0.1, 4000);
      myPlot.SetAxisRange("Y", 0.001, 0.8);
      myPlot.SetAxisTitle("X", "#it{N}_{ch}");

      myPlot.AddLegendBox(0.15, 0.9);
      //myPlot.AddTextBox(0.15, 0.9, "Open symbols: EPOS-LHC // Stars Pythia Angantyr");
      myPlot.AddTextBox(0.4, 0.3, systemSizeLable);

      if(drawcent){
        for(int i = 0; i < 9; i++)
        {
          for(int j = 0; j < 6; j++)
          {
            myPlot.AddTextBox(nchCent[i], 0.36 + 0.01*j, "#bf{|}", true);
          }
          myPlot.AddTextBox(nchCent[i], 0.46 + 0.025*i, string("#bf{") + centrality[i] + string("}"), true);
        }
      }
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------


    { // -----------------------------------------------------------------------
      string plotName = "moment1FullRangeMC";
      Plot myPlot(plotName, plotGroup);
      
      myPlot.SetDrawingProperties("logX");
      myPlot.AddHisto("momentGeneratedMC1", "PbPb_5TeV", "", kFullCross, kRed+1, "", 3000);
      myPlot.AddHisto("momentGeneratedMC1", "pp_5TeV", "pp (Pythia8)", kFullSquare, kBlue+1, "", 60);
      myPlot.AddHisto("momentGeneratedMC1", "pPb_5TeV", "p-Pb (DPMJET)", kFullCircle, kMagenta+1, "", 110);
      myPlot.AddHisto("momentGeneratedMC1", "PbPb_5TeV", "Pb-Pb (HIJING)", kFullCross, kRed+1, "", 3000);
      myPlot.SetAxisRange("X", 0.1, 4000);
      myPlot.SetAxisRange("Y", 0.4, 0.85);
      myPlot.AddLegendBox(0.6, 0.8);
      myPlot.AddTextBox(0.4, 0.29, systemSizeLable);
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------
    { // -----------------------------------------------------------------------
      string plotName = "multDensityComparison";
      Plot myPlot(plotName, plotGroup);
      myPlot.SetDrawingProperties("logY logX");
      myPlot.AddHisto("multDensityUnfolded", "PbPb_5TeV", "", kFullCross, kRed+1, "");
      myPlot.AddHisto("multDensityUnfolded_Syst", "PbPb_5TeV", "Pb-Pb", kFullCross, kRed+1, "boxes");
      myPlot.AddHisto("multDensityUnfolded", "XeXe_5TeV", "", kFullStar, kGreen+2, "");
      myPlot.AddHisto("multDensityUnfolded_Syst", "XeXe_5TeV", "Xe-Xe", kFullStar, kGreen+2, "boxes");
      myPlot.AddTextBox(0.2, 0.3, systemSizeLable);
      myPlot.AddLegendBox(0.7, 0.8);
      myPlot.SetAxisRange("Y", 1e-4, 3e-2);
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------

    { // -----------------------------------------------------------------------
      string plotName = "multDensityComparisonPerNuclPair";
      Plot myPlot(plotName, plotGroup);
      myPlot.SetDrawingProperties("logY logX");
      myPlot.AddGraph("multDensityVsParticlesPerNucleon_XeXe_5TeV", "Fits", "Xe-Xe", kFullCircle, kGreen+2, "");
      myPlot.AddGraph("multDensityVsParticlesPerNucleon_PbPb_5TeV", "Fits", "Pb-Pb", kFullSquare, kRed+1, "");
      myPlot.AddLegendBox(0.7, 0.8);
      myPlot.AddTextBox(0.2, 0.3, systemSizeLable);
      myPlot.SetAxisRange("Y", 1e-4, 3e-2);
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------
    { // -----------------------------------------------------------------------
      string plotName = "meanPtPerNuclPair";
      Plot myPlot(plotName, plotGroup);
      myPlot.AddGraph("meanPtVsParticlesPerNucleon_XeXe_5TeV", "Fits", "Xe-Xe", kFullCircle, kGreen+2, "");
      myPlot.AddGraph("meanPtVsParticlesPerNucleon_PbPb_5TeV", "Fits", "Pb-Pb", kFullSquare, kRed+1, "");
      //myPlot.SetAxisRange("Y", 1e-4, 3e-2);
//      myPlot.SetAxisRange("Y", 0.48, 0.75);
      myPlot.AddLegendBox(0.7, 0.4);
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------

    { // -----------------------------------------------------------------------
      string plotName = "meanPtPerNuclPairRatio";
      Plot myPlot(plotName, plotGroup, "default ratio");
      myPlot.AddGraph("meanPtVsParticlesPerNucleon_XeXe_5TeV", "Fits", "Xe-Xe", kFullCircle, kGreen+2, "");
      myPlot.AddGraph("meanPtVsParticlesPerNucleon_PbPb_5TeV", "Fits", "Pb-Pb", kFullSquare, kRed+1, "");
      myPlot.SetAxisRange("Y", 0.75, 1.05);
      myPlot.SetAxisRange("X", 0, 10);
      myPlot.AddLegendBox(0.7, 0.4);
      for(int i = 0; i < 9; i++)
      {
          //myPlot.AddTextBox(nchCent[i]/(2*208), 1, "#bf{|}", true);
      }

      
      myPlot.ChangePad(2);
      for(int i = 0; i < 9; i++)
      {
          //myPlot.AddTextBox(nchCent[i]/(2*208), 1, "#bf{|}", true);
      }

      myPlot.AddGraph("ratioMeanPtVsParticlesPerNucleon", "Fits", "Xe / Pb", kFullCircle, kGreen+2, "");
      myPlot.SetAxisRange("Y", 0.99, 1.01);
      myPlot.SetAxisTitle("Y", "Xe / Pb");
      myPlot.SetAxisRange("X", 0, 10);
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------

    
    
    { // -----------------------------------------------------------------------
      string plotName = "multDensityFullRange";
      Plot myPlot(plotName, plotGroup);
      
      myPlot.SetDrawingProperties("logY logX");
      myPlot.AddHisto("multDensityUnfolded", "PbPb_5TeV", "", kFullCross, kRed+1, "", 3000);
      myPlot.AddHisto("multDensityUnfolded", "pp_5TeV", "", kFullSquare, kBlue+1, "", 60);
      myPlot.AddHisto("multDensityUnfolded_Syst", "pp_5TeV", "pp", kFullSquare, kBlue+1, "boxes", 60);
      myPlot.AddHisto("multDensityUnfolded", "pPb_5TeV", "", kFullCircle, kMagenta+1, "", 120);
      myPlot.AddHisto("multDensityUnfolded_Syst", "pPb_5TeV", "p-Pb", kFullCircle, kMagenta+1, "boxes", 120);
      myPlot.AddHisto("multDensityGeneratedMC", "pPb_5TeV", "", kFullCircle, kMagenta+1, "", 120);
      myPlot.AddHisto("multDensityUnfolded", "PbPb_5TeV", "", kFullCross, kRed+1, "", 3000);
      myPlot.AddHisto("multDensityUnfolded_Syst", "PbPb_5TeV", "Pb-Pb", kFullCross, kRed+1, "boxes", 3000);
      myPlot.AddHisto("multDensityUnfolded", "XeXe_5TeV", "", kFullStar, kGreen+2, "", 2000);
      myPlot.AddHisto("multDensityUnfolded_Syst", "XeXe_5TeV", "Xe-Xe", kFullStar, kGreen+2, "boxes", 2000);

      for(int i = 0; i < 9; i++)
      {
        for(int j = 0; j < 6; j++)
        {
          myPlot.AddTextBox(nchCent[i], 1e-4 - 0.00001*j, "#bf{|}", true);
        }
//        myPlot.AddTextBox(nchCent[i], 1e-3 + 0.001*i, string("#bf{") + centrality[i] + string("}"), true);
      }
      myPlot.SetAxisRange("X", 0.1, 3000);
      myPlot.SetAxisRange("Y", 0.00001, 0.15);
      myPlot.AddLegendBox(0.2, 0.5);
      myPlot.AddTextBox(0.35, 0.3, systemSizeLable);
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------

    { // -----------------------------------------------------------------------
      string plotName = "moment1dNchdeta";
      Plot myPlot(plotName, plotGroup);
      myPlot.AddHisto("momentUnfolded1dNchDeta", "pp_5TeV", "pp", 0, kBlue+1);
      myPlot.SetAxisRange("Y", 0.45, 0.85);
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------

    { // -----------------------------------------------------------------------
      string plotName = "varianceFullRangeMC";
      Plot myPlot(plotName, plotGroup);
      myPlot.SetDrawingProperties("logX");
      myPlot.AddHisto("varianceGeneratedMC", "PbPb_5TeV", "", kFullCross, kRed+1, "", 3000);
      myPlot.AddHisto("varianceGeneratedMC", "pp_5TeV", "pp (Pythia8)", kFullSquare, kBlue+1, "", 50);
      myPlot.AddHisto("varianceGeneratedMC", "pPb_5TeV", "p-Pb (DPMJET)", kFullCircle, kMagenta+1, "", 85);
      myPlot.AddHisto("varianceGeneratedMC", "PbPb_5TeV", "Pb-Pb (HIJING)", kFullCross, kRed+1, "", 3000);
      myPlot.SetAxisRange("X", 0.1, 4000);
      myPlot.SetAxisRange("Y", 0.05, 0.8);
      myPlot.AddLegendBox(0.15, 0.6);
      myPlot.AddTextBox(0.14, 0.92, systemSizeLable);
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------

    { // -----------------------------------------------------------------------
      string plotName = "meanPt";
      Plot myPlot(plotName, plotGroup);
      myPlot.AddHisto("momentUnfolded1", "pPb_5TeV", "", kFullCircle, kMagenta+1, "", 120);

      // pp
      myPlot.AddHisto("momentUnfolded1", "pp_5TeV", "", kFullSquare, kBlue+1, "", 60);
      myPlot.AddHisto("momentUnfolded1_Syst", "pp_5TeV", "pp", kFullSquare, kBlue+1, "boxes", 60);
      //myPlot.AddHisto("meanPt_pp_EPOS-LHC_5.02TeV", "Simulations", "", kOpenSquare, kBlue+1, "", 60);

      // pPb
      myPlot.AddHisto("momentUnfolded1", "pPb_5TeV", "", kFullCircle, kMagenta+1, "", 120);
      myPlot.AddHisto("momentUnfolded1_Syst", "pPb_5TeV", "p-Pb", kFullCircle, kMagenta+1, "boxes", 120);
      //myPlot.AddHisto("momentGeneratedMC1", "pPb_5TeV", "DPMJET", kOpenCircle, kMagenta+1, "", 120);
      //myPlot.AddHisto("meanPt_pPb_EPOS-LHC_5.02TeV", "Simulations", "", kOpenCircle, kMagenta+1, "", 120);
      //myPlot.AddHisto("meanPt_pPb_Angantyr_5.02TeV", "Simulations", "", kFullStar, kMagenta+1, "", 120);

      // PbPb
      myPlot.AddHisto("momentUnfolded1", "PbPb_5TeV", "", kFullCross, kRed+1, "");
      myPlot.AddHisto("momentUnfolded1_Syst", "PbPb_5TeV", "Pb-Pb", kFullCross, kRed+1, "boxes");
      //myPlot.AddHisto("momentGeneratedMC1", "PbPb_5TeV", "HIJING", kFullCross, kRed+1, "");
      //myPlot.AddHisto("meanPt_PbPb_EPOS-LHC_5.02TeV", "Simulations", "", kOpenCross, kRed+1, "");
      //myPlot.AddHisto("meanPt_PbPb_Angantyr_5.02TeV", "Simulations", "", kFullStar, kRed+1, "");

      // XeXe
      myPlot.AddHisto("momentUnfolded1", "XeXe_5TeV", "", kFullStar, kGreen+2, "");
      myPlot.AddHisto("momentUnfolded1_Syst", "XeXe_5TeV", "Xe-Xe", kFullStar, kGreen+2, "boxes");
      //myPlot.AddHisto("momentGeneratedMC1", "XeXe_5TeV", "HIJING", kFullStar, kGreen+1, "");

      // additional stuff
      //myPlot.AddHisto("momentUnfolded1", "PbPb_5TeV_LowIR", "", kFullCross, kOrange+1, "");
      //myPlot.AddHisto("momentUnfolded1_Syst", "PbPb_5TeV_LowIR", "PbPb low IR", kFullCross, kOrange+1, "boxes");
      
      myPlot.AddHisto("momentUnfolded1", "pPb_8TeV", "", kFullCircle, kYellow+1, "", 100);
      myPlot.AddHisto("momentUnfolded1_Syst", "pPb_8TeV", "pPb, 8.16 TeV", kFullCircle, kYellow+1, "boxes", 100);

      //myPlot.AddHisto("momentUnfolded1", "Pbp_8TeV", "", kFullCircle, kYellow+2, "", 100);
      //myPlot.AddHisto("momentUnfolded1_Syst", "Pbp_8TeV", "Pbp, 8 TeV", kFullCircle, kYellow+2, "boxes", 100);

      //myPlot.AddHisto("momentUnfolded1", "PbPb_5TeV_2018", "", kFullCross, kBlack, "");
      //myPlot.AddHisto("momentUnfolded1_Syst", "PbPb_5TeV_2018", "PbPb 2018", kFullCross, kBlack, "boxes");

      myPlot.SetAxisRange("X", -0.5, 300);

      myPlot.SetAxisRange("Y", 0.4, 0.95);
      myPlot.AddLegendBox(0.63, 0.92, "");
      myPlot.SetAxisTitle("X", "#it{N}_{ch}");
      myPlot.AddTextBox(0.35, 0.4, systemSizeLable);
      //myPlot.AddTextBox(0.35, 0.4, systemSizeLable + " // Open symbols: EPOS-LHC // Stars: Pythia Angantyr");
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------

    { // -----------------------------------------------------------------------
      string plotName = "meanPtpPb8Over5";
      Plot myPlot(plotName, plotGroup, "default ratio");
      myPlot.AddHisto("momentUnfolded1", "pPb_5TeV", "", kFullCross, kRed+1, "");
      myPlot.AddHisto("momentUnfolded1_Syst", "pPb_5TeV", "p-Pb, 5.02 TeV", kFullCross, kRed+1, "boxes");
      myPlot.AddHisto("momentUnfolded1", "pPb_8TeV", "", kFullStar, kBlue+1, "");
      myPlot.AddHisto("momentUnfolded1_Syst", "pPb_8TeV", "p-Pb, 8.16 TeV", kFullStar, kBlue+1, "boxes");
      myPlot.AddRatio("momentUnfolded1_Syst", "pPb_8TeV", "momentUnfolded1_Syst", "pPb_5TeV", "", kFullStar, kBlue+1, "boxes");

      myPlot.AddHisto("meanPt_pPb_EPOS-LHC_5.02TeV", "Simulations", "", kOpenCross, kRed+1, "", 130);
      myPlot.AddHisto("momentGeneratedMC1", "pPb_8TeV", "", kOpenStar, kBlue+1, "boxes");
      myPlot.AddRatio("momentGeneratedMC1", "pPb_8TeV", "meanPt_pPb_EPOS-LHC_5.02TeV", "Simulations", "", kOpenStar, kBlue+1, "boxes");

      myPlot.SetAxisRange("ratio", 0.9, 1.1);
      myPlot.SetAxisTitle("ratio", "8 / 5 TeV");
      myPlot.SetAxisRange("X", 0, 100);
      myPlot.SetAxisRange("Y", 0.45, 0.95);
      myPlot.AddLegendBox(0.2, 0.8, "");

      myPlot.AddTextBox(0.4, 0.3, alice + newLine + chargedParticles + newLine + eta08 + ", " + ptRange + newLine + "Open symbols: EPOS-LHC");
      myPlot.ChangePad(2);
      myPlot.SetAxisTitle("X", "#it{N}_{ch}");


      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------

    { // -----------------------------------------------------------------------
      string plotName = "meanPtpPbOverPbp";
      Plot myPlot(plotName, plotGroup, "default ratio");
      myPlot.AddHisto("momentUnfolded1", "pPb_8TeV", "", kFullStar, kBlue+1, "");
      myPlot.AddHisto("momentUnfolded1_Syst", "pPb_8TeV", "p-Pb, 8 TeV", kFullStar, kBlue+1, "boxes");
      myPlot.AddHisto("momentUnfolded1", "Pbp_8TeV", "", kFullStar, kGreen+1, "");
      myPlot.AddHisto("momentUnfolded1_Syst", "Pbp_8TeV", "Pb-p, 8 TeV", kFullStar, kGreen+1, "boxes");

      myPlot.AddRatio("momentUnfolded1_Syst", "Pbp_8TeV", "momentUnfolded1_Syst", "pPb_8TeV", "", kFullStar, kGreen+1, "boxes");

      myPlot.SetAxisRange("ratio", 0.98, 1.02);
      myPlot.SetAxisTitle("ratio", "Pbp/pPb");
      myPlot.SetAxisRange("X", 0, 150);
      myPlot.SetAxisRange("Y", 0.45, 0.95);
      myPlot.AddLegendBox(0.4, 0.4, "");
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------

    
    
    { // -----------------------------------------------------------------------
      string plotName = "meanPtPbOverPb";
      Plot myPlot(plotName, plotGroup, "default ratio");
//      myPlot.SetDrawingProperties("logX");
      myPlot.AddHisto("momentUnfolded1", "PbPb_5TeV", "", kFullCross, kRed+1, "", 3500);
      myPlot.AddHisto("momentUnfolded1_Syst", "PbPb_5TeV", "2015", kFullCross, kRed+1, "boxes", 3500);
      myPlot.AddHisto("momentUnfolded1", "PbPb_5TeV_2018", "", kFullStar, kGreen+2, "", 3500);
      myPlot.AddHisto("momentUnfolded1_Syst", "PbPb_5TeV_2018", "2018", kFullStar, kGreen+2, "boxes", 3500);
      myPlot.AddRatio("momentUnfolded1_Syst", "PbPb_5TeV_2018", "momentUnfolded1_Syst", "PbPb_5TeV", "", kFullStar, kGreen+2, "boxes", 3500);
      myPlot.SetAxisRange("ratio", 0.95, 1.05);
      myPlot.SetAxisTitle("ratio", "2018/2015");
      myPlot.SetAxisRange("X", 0, 3400);
      myPlot.SetAxisRange("Y", 0.45, 0.85);
      myPlot.AddLegendBox(0.2, 0.9, "");
      myPlot.AddTextBox(0.4, 0.2, systemSizeLable);
      myPlot.ChangePad(2);
      myPlot.SetAxisTitle("X", "#it{N}_{ch}");
 //     myPlot.SetDrawingProperties("logX");

      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------

        
    { // -----------------------------------------------------------------------
      string plotName = "multDensityPbOverPb";
      Plot myPlot(plotName, plotGroup, "default ratio");
      myPlot.SetDrawingProperties("logY");
      myPlot.AddHisto("multDensityUnfolded", "PbPb_5TeV", "", kFullCross, kRed+1, "", 3500);
      myPlot.AddHisto("multDensityUnfolded_Syst", "PbPb_5TeV", "2015", kFullCross, kRed+1, "boxes", 3500);
      myPlot.AddHisto("multDensityUnfolded", "PbPb_5TeV_2018", "", kFullStar, kGreen+2, "", 3500);
      myPlot.AddHisto("multDensityUnfolded_Syst", "PbPb_5TeV_2018", "2018", kFullStar, kGreen+2, "boxes", 3500);
      myPlot.AddRatio("multDensityUnfolded_Syst", "PbPb_5TeV_2018", "multDensityUnfolded_Syst", "PbPb_5TeV", "", kFullStar, kGreen+2, "boxes", 3500);
      myPlot.SetAxisRange("ratio", 0.95, 1.05);
      myPlot.SetAxisTitle("ratio", "2018/2015");
      myPlot.SetAxisRange("X", 0, 3400);
      myPlot.SetAxisRange("ratio", 0.9, 1.1);
//          myPlot.SetAxisRange("Y", 0.45, 0.85);
      myPlot.AddLegendBox(0.2, 0.9, "");
      myPlot.AddTextBox(0.4, 0.2, systemSizeLable);
      myPlot.ChangePad(2);
      myPlot.SetAxisTitle("X", "#it{N}_{ch}");
 //     myPlot.SetDrawingProperties("logX");

      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------

    { // -----------------------------------------------------------------------
      string plotName = "multDensityPbpOverpPb";
      Plot myPlot(plotName, plotGroup, "default ratio");
      myPlot.SetDrawingProperties("logY");
      myPlot.AddHisto("multDensityUnfolded", "pPb_8TeV", "", kFullCross, kRed+1, "", 3500);
      myPlot.AddHisto("multDensityUnfolded_Syst", "pPb_8TeV", "p-Pb", kFullCross, kRed+1, "boxes", 3500);
      myPlot.AddHisto("multDensityUnfolded", "Pbp_8TeV", "", kFullStar, kGreen+2, "", 3500);
      myPlot.AddHisto("multDensityUnfolded_Syst", "Pbp_8TeV", "Pb-p", kFullStar, kGreen+2, "boxes", 3500);
      myPlot.AddRatio("multDensityUnfolded_Syst", "Pbp_8TeV", "multDensityUnfolded_Syst", "pPb_8TeV", "", kFullStar, kGreen+2, "boxes", 3500);
      myPlot.SetAxisRange("ratio", 0.95, 1.05);
      myPlot.SetAxisTitle("ratio", "Pbp/pPb");
      myPlot.SetAxisRange("X", 0, 120);
      myPlot.SetAxisRange("ratio", 0.9, 1.1);
//          myPlot.SetAxisRange("Y", 0.45, 0.85);
      myPlot.AddLegendBox(0.7, 0.9, "");
      myPlot.AddTextBox(0.4, 0.2, erg8TeV_NN);
      myPlot.ChangePad(2);
      myPlot.SetAxisTitle("X", "#it{N}_{ch}");
 //     myPlot.SetDrawingProperties("logX");

      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------

    
    { // -----------------------------------------------------------------------
      string plotName = "meanPtXeOverPb";
      Plot myPlot(plotName, plotGroup, "default ratio");
      myPlot.AddHisto("momentUnfolded1", "PbPb_5TeV", "", kFullCross, kRed+1, "", 100);
      myPlot.AddHisto("momentUnfolded1_Syst", "PbPb_5TeV", "Pb-Pb", kFullCross, kRed+1, "boxes", 100);
      myPlot.AddHisto("momentUnfolded1", "XeXe_5TeV", "", kFullStar, kGreen+2, "", 100);
      myPlot.AddHisto("momentUnfolded1_Syst", "XeXe_5TeV", "Xe-Xe", kFullStar, kGreen+2, "boxes", 100);
      myPlot.AddRatio("momentUnfolded1_Syst", "XeXe_5TeV", "momentUnfolded1_Syst", "PbPb_5TeV", "", kFullStar, kGreen+2, "boxes", 100);
      myPlot.SetAxisRange("ratio", 0.95, 1.05);
      myPlot.SetAxisTitle("ratio", "Xe / Pb");
      myPlot.SetAxisRange("X", 0, 100);
      myPlot.SetAxisRange("Y", 0.45, 0.85);
      myPlot.AddLegendBox(0.2, 0.9, "");
      myPlot.SetAxisTitle("X", "#it{N}_{ch}");
      myPlot.AddTextBox(0.4, 0.3, systemSizeLable);
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------

    { // -----------------------------------------------------------------------
      string plotName = "meanPtXeOverPbFullRange";
      Plot myPlot(plotName, plotGroup, "default ratio");
      myPlot.AddHisto("momentUnfolded1", "PbPb_5TeV", "", kFullCross, kRed+1, "", 2000);
      myPlot.AddHisto("momentUnfolded1_Syst", "PbPb_5TeV", "Pb-Pb", kFullCross, kRed+1, "boxes", 2000);
      myPlot.AddHisto("momentUnfolded1", "XeXe_5TeV", "", kFullStar, kGreen+2, "", 2000);
      myPlot.AddHisto("momentUnfolded1_Syst", "XeXe_5TeV", "Xe-Xe", kFullStar, kGreen+2, "boxes", 2000);
      myPlot.AddRatio("momentUnfolded1_Syst", "XeXe_5TeV", "momentUnfolded1_Syst", "PbPb_5TeV", "", kFullStar, kGreen+2, "boxes", 2000);
      myPlot.SetAxisRange("ratio", 0.95, 1.05);
      myPlot.SetAxisTitle("ratio", "Xe / Pb");
      myPlot.SetAxisRange("X", 0, 2000);
      myPlot.SetAxisRange("Y", 0.45, 0.85);
      myPlot.AddLegendBox(0.2, 0.9, "");
      myPlot.SetAxisTitle("X", "#it{N}_{ch}");
      myPlot.AddTextBox(0.4, 0.3, systemSizeLable);
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------

    
    { // -----------------------------------------------------------------------
      string plotName = "moment1WithMC";
      Plot myPlot(plotName, plotGroup);
      
      myPlot.AddHisto("momentUnfolded1", "pp_5TeV", "", kFullSquare, kBlue+1, "", 60);
      myPlot.AddHisto("momentUnfolded1_Syst", "pp_5TeV", "pp", kFullSquare, kBlue+1, "boxes", 60);
      myPlot.AddHisto("momentGeneratedMC1", "pp_5TeV", "", 0, kBlue+1, "band", 60);
      myPlot.AddHisto("momentUnfolded1", "pPb_5TeV", "", kFullCircle, kMagenta+1, "", 100);
      myPlot.AddHisto("momentUnfolded1_Syst", "pPb_5TeV", "p-Pb", kFullCircle, kMagenta+1, "boxes", 100);
      myPlot.AddHisto("momentGeneratedMC1", "pPb_5TeV", "", 0, kMagenta+1, "band");
      myPlot.AddHisto("momentUnfolded1", "PbPb_5TeV", "", kFullCross, kRed+1, "", 100);
      myPlot.AddHisto("momentUnfolded1_Syst", "PbPb_5TeV", "Pb-Pb", kFullCross, kRed+1, "boxes", 100);
      myPlot.AddHisto("momentGeneratedMC1", "PbPb_5TeV", "", 0, kRed+1, "band");
      myPlot.SetAxisRange("Y", 0.45, 0.85);
      myPlot.AddLegendBox(0.2, 0.9, "");
      myPlot.SetAxisTitle("X", "#it{N}_{ch}");
      myPlot.AddTextBox(0.4, 0.3, systemSizeLable);
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------

    { // -----------------------------------------------------------------------
      string plotName = "variance";
      Plot myPlot(plotName, plotGroup);
      
      myPlot.AddHisto("varianceUnfolded", "pPb_5TeV", "", kFullCircle, kMagenta+1, "", 100);
      myPlot.AddHisto("varianceUnfolded", "pp_5TeV", "", kFullSquare, kBlue+1, "", 60);
      myPlot.AddHisto("varianceUnfolded_Syst", "pp_5TeV", "pp", kFullSquare, kBlue+1, "boxes", 60);
      myPlot.AddHisto("varianceUnfolded", "pPb_5TeV", "", kFullCircle, kMagenta+1, "", 100);
      myPlot.AddHisto("varianceUnfolded_Syst", "pPb_5TeV", "p-Pb", kFullCircle, kMagenta+1, "boxes", 100);
      //myPlot.AddHisto("variance_pPb_EPOS-LHC_5.02TeV", "Simulations", "", kOpenCircle, kMagenta+1, "", 100);
      //myPlot.AddHisto("variance_pPb_Angantyr_5.02TeV", "Simulations", "", kFullStar, kMagenta+1, "", 100);
      myPlot.AddHisto("varianceUnfolded", "PbPb_5TeV", "", kFullCross, kRed+1, "", 200);
      myPlot.AddHisto("varianceUnfolded_Syst", "PbPb_5TeV", "Pb-Pb", kFullCross, kRed+1, "boxes", 200);
      //myPlot.AddHisto("variance_PbPb_EPOS-LHC_5.02TeV", "Simulations", "", kOpenCircle, kRed+1, "", 200);
      //myPlot.AddHisto("variance_PbPb_Angantyr_5.02TeV", "Simulations", "", kFullStar, kRed+1, "", 200);

      myPlot.AddHisto("varianceUnfolded", "pPb_8TeV", "", kFullCircle, kYellow+1, "", 90);
      myPlot.AddHisto("varianceUnfolded_Syst", "pPb_8TeV", "p-Pb, 8.16 TeV", kFullCircle, kYellow+1, "boxes", 90);

      myPlot.AddHisto("varianceUnfolded", "XeXe_5TeV", "", kFullStar, kGreen+2, "", 200);
      myPlot.AddHisto("varianceUnfolded_Syst", "XeXe_5TeV", "Xe-Xe", kFullStar, kGreen+2, "boxes", 200);
            
      myPlot.SetAxisRange("Y", 0.05, 0.8);
      myPlot.AddLegendBox(0.6, 0.9, "");
      myPlot.SetAxisTitle("X", "#it{N}_{ch}");
      myPlot.AddTextBox(0.35, 0.3, systemSizeLable);
      //myPlot.AddTextBox(0.53, 0.72, "Open symbols: EPOS-LHC // Stars Pythia Angantyr");
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------

    { // -----------------------------------------------------------------------
      string plotName = "varianceMC";
      Plot myPlot(plotName, plotGroup);
      myPlot.AddHisto("varianceGeneratedMC", "pp_5TeV", "pp (Pythia8)", kFullSquare, kBlue+1, "", 60);
      myPlot.AddHisto("varianceGeneratedMC", "pPb_5TeV", "p-Pb (DPMJET)", kFullCircle, kMagenta+1, "", 100);
      myPlot.AddHisto("varianceGeneratedMC", "PbPb_5TeV", "Pb-Pb (HIJING)", kFullCross, kRed+1, "", 100);
      myPlot.SetAxisRange("Y", 0.05, 0.8);
      myPlot.AddLegendBox(0.2, 0.9, "");
      myPlot.AddTextBox(0.4, 0.3, systemSizeLable);
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------


    { // -----------------------------------------------------------------------
      string plotName = "moment1UnfoldedVsReWeighted";
      Plot myPlot(plotName, plotGroup, "default ratio");
      myPlot.AddHisto("momentReweighted1", "pp_5TeV", "", kFullSquare, kBlue+1, "", 60);
      myPlot.AddHisto("momentReweighted1_Syst", "pp_5TeV", "re-weighted", kFullSquare, kBlue+1, "boxes", 60);
      myPlot.AddHisto("momentUnfolded1", "pp_5TeV", "", kFullCross, kRed+1, "", 60);
      myPlot.AddHisto("momentUnfolded1_Syst", "pp_5TeV", "unfolded", kFullCross, kRed+1, "boxes", 60);
      myPlot.AddRatio("momentUnfolded1_Syst", "pp_5TeV", "momentReweighted1_Syst", "pp_5TeV", "", kFullCross, kRed+1, "boxes", 60);
      myPlot.SetAxisRange("X", 0., 60);
      myPlot.SetAxisRange("Y", 0.45, 0.85);
      myPlot.SetAxisRange("ratio", 0.9, 1.1);
      myPlot.AddLegendBox(0.2, 0.9, "");
      myPlot.AddTextBox(0.4, 0.3, systemSizeLable);
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------


    { // -----------------------------------------------------------------------
      string plotName = "moment2";
      Plot myPlot(plotName, plotGroup);
      myPlot.AddHisto("momentUnfolded2", "pp_5TeV", "", kFullSquare, kBlue+1, "", 60);
      myPlot.AddHisto("momentUnfolded2_Syst", "pp_5TeV", "pp", kFullSquare, kBlue+1, "boxes", 60);
      myPlot.AddHisto("momentUnfolded2", "pPb_5TeV", "", kFullCircle, kMagenta+1);
      myPlot.AddHisto("momentUnfolded2_Syst", "pPb_5TeV", "p-Pb", kFullCircle, kMagenta+1, "boxes");
      myPlot.AddHisto("momentUnfolded2", "PbPb_5TeV", "", kFullCross, kRed+1);
      myPlot.AddHisto("momentUnfolded2_Syst", "PbPb_5TeV", "Pb-Pb", kFullCross, kRed+1, "boxes");
      myPlot.AddHisto("momentUnfolded2_Syst", "PbPb_5TeV", "Pb-Pb", kFullCross, kRed+1, "boxes");
      myPlot.SetAxisRange("Y", 0.2, 1.5);
      myPlot.AddLegendBox(0.2, 0.9, "");
      myPlot.AddTextBox(0.4, 0.3, systemSizeLable);
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------

    { // -----------------------------------------------------------------------
      string plotName = "moment3";
      Plot myPlot(plotName, plotGroup);
      myPlot.AddHisto("momentUnfolded3", "pp_5TeV", "", kFullSquare, kBlue+1, "", 60);
      myPlot.AddHisto("momentUnfolded3_Syst", "pp_5TeV", "pp", kFullSquare, kBlue+1, "boxes", 60);
      myPlot.AddHisto("momentUnfolded3", "pPb_5TeV", "", kFullCircle, kMagenta+1);
      myPlot.AddHisto("momentUnfolded3_Syst", "pPb_5TeV", "p-Pb", kFullCircle, kMagenta+1, "boxes");
      myPlot.AddHisto("momentUnfolded3", "PbPb_5TeV", "", kFullCross, kRed+1);
      myPlot.AddHisto("momentUnfolded3_Syst", "PbPb_5TeV", "Pb-Pb", kFullCross, kRed+1, "boxes");
      myPlot.SetAxisRange("Y", 0.2, 5);
      myPlot.AddLegendBox(0.6, 0.4, "");
      myPlot.AddTextBox(0.15, 0.9, systemSizeLable);
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------

    { // -----------------------------------------------------------------------
      string plotName = "moment1MC";
      Plot myPlot(plotName, plotGroup);
      myPlot.AddHisto("momentUnfoldedMC1", "pp_5TeV", "pp", 0, kBlue+1);
      myPlot.AddHisto("momentUnfoldedMC1", "pPb_5TeV", "p-Pb", 0, kMagenta+1);
      myPlot.AddHisto("momentUnfoldedMC1", "PbPb_5TeV", "Pb-Pb", 0, kRed+1);
      myPlot.SetAxisRange("Y", 0.45, 0.85);
      myPlot.AddLegendBox(0.6, 0.4, "");
      myPlot.AddTextBox(0.15, 0.9, systemSizeLable);
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------

    { // -----------------------------------------------------------------------
      string plotName = "moment1MCtruth";
      Plot myPlot(plotName, plotGroup);
      myPlot.AddHisto("momentGeneratedMC1", "pp_5TeV", "pp (Pythia8)", 0, kBlue+1, "", 60);
      myPlot.AddHisto("momentGeneratedMC1", "pPb_5TeV", "p-Pb (DPMJET)", 0, kMagenta+1, "", 100);
      myPlot.AddHisto("momentGeneratedMC1", "PbPb_5TeV", "Pb-Pb (HIJING)", 0, kRed+1, "", 100);
      myPlot.AddLegendBox(0.15, 0.9, "");
      myPlot.AddTextBox(0.4, 0.3, systemSizeLable);
      myPlot.SetAxisRange("Y", 0.45, 0.85);
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------
  } //==========================================================================

  //if(plotEnv.ContainsDatasets({"pp_5TeV", "pp_7TeV", "pp_13TeV"}))
  { //==========================================================================
    string plotGroup = "energy_comparison";
    string energyLable = alice + newLine + chargedParticles + ", " + "pp collisions" + newLine + eta08 + ", " + ptRange;
    string energyLablePrel = alicePrel + newLine + chargedParticles + ", " + "pp collisions" + newLine + eta08 + ", " + ptRange;
    string energyLableMC = "#bf{Pythia8 Monash13}" + newLine + chargedParticles + ", " + "pp collisions" + newLine + eta08 + ", " + ptRange;

    
    
    { // -----------------------------------------------------------------------
      string plotName = "inverseSlope";
      Plot myPlot(plotName, plotGroup);
      myPlot.AddHisto(plotName + "_PbPb_5TeV", "Fits", "Pb-Pb 5.02 TeV");
      myPlot.AddHisto(plotName + "_pp_13TeV", "Fits", "13 TeV");
      myPlot.AddHisto(plotName + "_pp_7TeV", "Fits", "7 TeV");
      myPlot.AddHisto(plotName + "_pp_5TeV", "Fits", "5.02 TeV");
      myPlot.AddHisto(plotName + "_pp_2TeV", "Fits", "2.76 TeV");
      myPlot.AddHisto(plotName + "_pPb_5TeV", "Fits", "p-Pb 5.02 TeV");
      myPlot.AddHisto(plotName + "_PbPb_5TeV", "Fits", "Pb-Pb 5.02 TeV");
//      myPlot.AddHisto(plotName + "_XeXe_5TeV", "Fits", "Xe-Xe 5.44 TeV");
      myPlot.SetAxisRange("X", 0, 3000);
      myPlot.SetAxisRange("Y", 4, 11);
      myPlot.AddLegendBox(0.7, 0.9, "");
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------

    { // -----------------------------------------------------------------------
      string plotName = "yieldFractions";
      Plot myPlot(plotName, plotGroup);
      //      myPlot.SetDrawingProperties("logX logY");
      myPlot.AddHisto(plotName + "_pp_13TeV", "Fits", "13 TeV");
      myPlot.AddHisto(plotName + "_pp_7TeV", "Fits", "7 TeV");
      myPlot.AddHisto(plotName + "_pp_5TeV", "Fits", "5.02 TeV");
      myPlot.AddHisto(plotName + "_pp_2TeV", "Fits", "2.76 TeV");
      myPlot.AddHisto(plotName + "_pPb_5TeV", "Fits", "p-Pb 5.02 TeV");
      myPlot.AddHisto(plotName + "_PbPb_5TeV", "Fits", "Pb-Pb 5.02 TeV");
//      myPlot.AddHisto(plotName + "_XeXe_5TeV", "Fits", "Xe-Xe 5.44 TeV");
//      myPlot.SetAxisRange("Y", 4, 11);
      myPlot.AddLegendBox(0.13, 0.9, "");
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------

    { // -----------------------------------------------------------------------
      string plotName = "yieldFractionsSelfNormalized";
      Plot myPlot(plotName, plotGroup);
      //      myPlot.SetDrawingProperties("logX logY");
      myPlot.AddHisto(plotName + "_pp_13TeV", "Fits", "13 TeV");
      myPlot.AddHisto(plotName + "_pp_7TeV", "Fits", "7 TeV");
      myPlot.AddHisto(plotName + "_pp_5TeV", "Fits", "5.02 TeV");
      myPlot.AddHisto(plotName + "_pp_2TeV", "Fits", "2.76 TeV");
      //      myPlot.SetAxisRange("Y", 4, 11);
      myPlot.AddLegendBox(0.3, 0.9, "");
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------


    { // -----------------------------------------------------------------------
      string plotName = "multPtUnfoldedNormalized";
      Plot myPlot(plotName, plotGroup, "default");
      myPlot.SetDrawingProperties("logY logZ");
      myPlot.AddHisto(plotName, "pp_5TeV", "5.02 TeV");
      myPlot.SetAxisRange("X", 0, 60);
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------

    
    
    
    { // -----------------------------------------------------------------------
      string plotName = "momentUnfolded1SelfNormalized";
      Plot myPlot(plotName, plotGroup, "default");
      myPlot.SetDrawingProperties("logX logY");
      myPlot.AddHisto(plotName, "pp_13TeV", "13 TeV");
      myPlot.AddHisto(plotName, "pp_7TeV", "7 TeV");
      myPlot.AddHisto(plotName, "pp_5TeV", "5.02 TeV");
      myPlot.AddHisto(plotName, "pp_2TeV", "2.76 TeV");
      myPlot.SetAxisRange("X", 0, 9);
      myPlot.SetAxisRange("Y", 0.7, 1.6);
//      myPlot.AddLegendBox(0.4, 0.4);
      myPlot.AddTextBox(0.15, 0.91, energyLable);
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------

    { // -----------------------------------------------------------------------
      string plotName = "momentUnfolded1SelfNormalizedWithPythia";
      Plot myPlot(plotName, plotGroup, "default");
      //myPlot.SetDrawingProperties("logX logY");
      string dataName = "momentUnfolded1SelfNormalized";
      string mcName = "momentGeneratedMC1SelfNormalized";
      myPlot.AddHisto(dataName, "pp_13TeV", "13 TeV");
      myPlot.AddHisto(mcName, "pp_13TeV", "", 0, -1, "band", 7);
      myPlot.AddHisto(dataName, "pp_7TeV", "7 TeV");
      myPlot.AddHisto(mcName, "pp_7TeV", "", 0, -1, "band", 7);
      myPlot.AddHisto(dataName, "pp_5TeV", "5.02 TeV");
      myPlot.AddHisto(mcName, "pp_5TeV", "", 0, -1, "band", 7);
      myPlot.AddHisto(dataName, "pp_2TeV", "2.76 TeV", 0, 0, "", 7);
      myPlot.AddHisto(mcName, "pp_2TeV", "", 0, -1, "band", 7);


      myPlot.SetAxisRange("X", 0, 9);
      myPlot.SetAxisRange("Y", 0.7, 1.6);
      myPlot.AddLegendBox(0.4, 0.4);
      myPlot.AddTextBox(0.15, 0.91, energyLable);
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------

    
    { // -----------------------------------------------------------------------
      string plotName = "momentGeneratedMC1SelfNormalized";
      Plot myPlot(plotName, plotGroup, "default");
      myPlot.SetDrawingProperties("logX logY");
      myPlot.AddHisto(plotName, "pp_13TeV", "13 TeV");
      myPlot.AddHisto(plotName, "pp_7TeV", "7 TeV (Pythia6)");
      myPlot.AddHisto(plotName, "pp_5TeV", "5.02 TeV");
      myPlot.AddHisto(plotName, "pp_2TeV", "2.76 TeV");
      myPlot.SetAxisRange("X", 0, 9);
      myPlot.SetAxisRange("Y", 0.7, 1.6);
//      myPlot.AddLegendBox(0.4, 0.4);
      myPlot.AddTextBox(0.15, 0.91, energyLableMC);
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------

    
    { // -----------------------------------------------------------------------
      string plotName = "inclusiveSpectrum";
      Plot myPlot(plotName, plotGroup, "default ratio");
      myPlot.SetDrawingProperties("logY logX");
      myPlot.AddHisto(plotName, "PbPb_5TeV", "Pb-Pb 5.02 TeV");
      myPlot.AddHisto(plotName, "pp_13TeV", "13 TeV");
      myPlot.AddHisto(plotName, "pp_7TeV", "7 TeV");
      myPlot.AddHisto(plotName, "pp_5TeV", "5.02 TeV");
      myPlot.AddHisto(plotName, "pp_2TeV", "2.76 TeV");
      myPlot.AddLegendBox(0.7, 0.91);
      myPlot.AddRatio(plotName, "pp_7TeV", plotName, "pp_13TeV");
      myPlot.AddRatio(plotName, "pp_5TeV", plotName, "pp_13TeV");
      myPlot.AddRatio(plotName, "pp_2TeV", plotName, "pp_13TeV");
      myPlot.SetAxisTitle("Y", "1/#it{N}_{evt} 1/(2#pi #it{p}_{T}) (d^{2}#it{N})/(d#it{p}_{T}d#it{#eta}) [(GeV/#it{c})^{-2}]");
      myPlot.AddTextBox(0.2, 0.25, energyLable);
      myPlot.ChangePad(2);
      myPlot.SetDrawingProperties("logX");
      myPlot.SetAxisTitle("ratio", "ratio to 13");
      myPlot.SetAxisTitle("X", "#it{p}_{T} (GeV/#it{c})");
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------

    { // -----------------------------------------------------------------------
      string plotName = "multPtRatio_5Over13";
      Plot myPlot(plotName, plotGroup);
      myPlot.SetDrawingProperties("logY");
      myPlot.AddRatio("multPtUnfoldedNormalized", "pp_5TeV", "multPtUnfoldedNormalized", "pp_13TeV");
//      myPlot.AddRatio("multPtUnfolded", "pp_5TeV", "multPtUnfolded", "pp_13TeV");
      myPlot.SetAxisRange("Z", 0, 1.5);
      myPlot.SetAxisRange("X", 0, 60);
      myPlot.SetAxisRange("Y", 0.15, 20.0);
      myPlot.SetAxisTitle("Y", "#it{p}_{T} (GeV/#it{c})");
      myPlot.SetAxisTitle("X", "#it{N}_{ch}");
      myPlot.SetAxisTitle("Z", "5 TeV / 13 TeV");
//      myPlot.AddTextBox(0.3, 0.6, "5 TeV / 13 TeV");
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------

    { // -----------------------------------------------------------------------
      string plotName = "multPtRatio_PbPbOverpPb";
      Plot myPlot(plotName, plotGroup);
      myPlot.SetDrawingProperties("logY logZ");
      myPlot.AddRatio("multPtUnfolded", "PbPb_5TeV", "multPtUnfolded", "pPb_5TeV");
//      myPlot.SetAxisRange("Z", 1e-4, 5e-1);
      myPlot.SetAxisRange("X", 0, 60);
      myPlot.SetAxisRange("Y", 0.15, 20.0);
      myPlot.SetAxisTitle("Y", "#it{p}_{T} (GeV/#it{c})");
      myPlot.SetAxisTitle("X", "#it{N}_{ch}");
      myPlot.SetAxisTitle("Z", "Pb-Pb / p-Pb");
      //      myPlot.AddTextBox(0.3, 0.6, "5 TeV / 13 TeV");
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------

    
    { // -----------------------------------------------------------------------
      string plotName = "multPtRatioToPythia_2.76TeV";
      Plot myPlot(plotName, plotGroup);
      myPlot.SetDrawingProperties("logY logZ");
      myPlot.AddRatio("multPtUnfolded", "pp_2TeV", "multPt_2.76TeV", "Simulations");
      myPlot.SetAxisRange("Z", 0.5, 2.0);
      myPlot.SetAxisRange("X", 0, 40);
      myPlot.SetAxisRange("Y", 0.15, 20.0);
      myPlot.SetAxisTitle("Y", "#it{p}_{T} (GeV/#it{c})");
      myPlot.SetAxisTitle("X", "#it{N}_{ch}");
      myPlot.SetAxisTitle("Z", "ratio to Pythia8 Monash13");
      myPlot.AddTextBox(0.4, 0.4, "pp 2.76 TeV");
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------

    { // -----------------------------------------------------------------------
      string plotName = "multPtRatioToPythia_5TeV";
      Plot myPlot(plotName, plotGroup);
      myPlot.SetDrawingProperties("logY logZ");
      myPlot.AddRatio("multPtUnfolded", "pp_5TeV", "multPt_5.02TeV", "Simulations");
      myPlot.SetAxisRange("Z", 0.5, 2.0);
      myPlot.SetAxisRange("X", 0, 60);
      myPlot.SetAxisRange("Y", 0.15, 20.0);
      myPlot.SetAxisTitle("Y", "#it{p}_{T} (GeV/#it{c})");
      myPlot.SetAxisTitle("X", "#it{N}_{ch}");
      myPlot.SetAxisTitle("Z", "ratio to Pythia8 Monash13");
      myPlot.AddTextBox(0.4, 0.4, "pp 5 TeV");
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------
    { // -----------------------------------------------------------------------
      string plotName = "multPtRatioToPythia_7TeV";
      Plot myPlot(plotName, plotGroup);
      myPlot.SetDrawingProperties("logY logZ");
      myPlot.AddRatio("multPtUnfolded", "pp_7TeV", "multPt_7TeV", "Simulations");
      myPlot.SetAxisRange("Z", 0.5, 2.0);
      myPlot.SetAxisRange("X", 0, 60);
      myPlot.SetAxisRange("Y", 0.15, 20.0);
      myPlot.SetAxisTitle("Y", "#it{p}_{T} (GeV/#it{c})");
      myPlot.SetAxisTitle("X", "#it{N}_{ch}");
      myPlot.SetAxisTitle("Z", "ratio to Pythia8 Monash13");
      myPlot.AddTextBox(0.4, 0.4, "pp 7 TeV");
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------
    { // -----------------------------------------------------------------------
      string plotName = "multPtRatioToPythia_13TeV";
      Plot myPlot(plotName, plotGroup);
      myPlot.SetDrawingProperties("logY logZ");
      myPlot.AddRatio("multPtUnfolded", "pp_13TeV", "multPt_13TeV", "Simulations");
      myPlot.SetAxisRange("Z", 0.5, 2.0);
      myPlot.SetAxisRange("X", 0, 60);
      myPlot.SetAxisRange("Y", 0.15, 20.0);
      myPlot.SetAxisTitle("Y", "#it{p}_{T} (GeV/#it{c})");
      myPlot.SetAxisTitle("X", "#it{N}_{ch}");
      myPlot.SetAxisTitle("Z", "ratio to Pythia8 Monash13");
      myPlot.AddTextBox(0.4, 0.4, "pp 13 TeV");
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------


    { // -----------------------------------------------------------------------
      string plotName = "energyScanMean";
      Plot myPlot(plotName, plotGroup);
      
      vector<int> multBins = {2, 9, 16, 26, 36};
      //vector<int> multBins = {2, 6, 9, 11, 16, 21, 26, 31, 36};
      for(auto& multBin : multBins)
      {
        myPlot.AddHisto(string("meanPt_") + std::to_string(multBin), "Energyscan", string("Nch = ") + std::to_string(multBin-1), 0, 0, "boxes");
      }
      myPlot.SetAxisRange("Y", 0.3, 1.0);
      myPlot.AddLegendBox(0.24, 0.91, "", 3);
      myPlot.AddTextBox(0.4, 0.3, energyLable);
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------

    { // -----------------------------------------------------------------------
      string plotName = "energyScanMeanMC";
      Plot myPlot(plotName, plotGroup);
      vector<int> multBins = {2, 9, 16, 26, 36};
      for(auto& multBin : multBins)
      {
        myPlot.AddHisto(string("meanPt_") + std::to_string(multBin), "Energyscan", string("#it{N}_{ch} = ") + std::to_string(multBin-1), 0, 0, "boxes");
        myPlot.AddHisto(string("meanPtMC_") + std::to_string(multBin), "Energyscan", "", 0, -1, "band");
//        myPlot.AddHisto(string("meanPtMC_") + std::to_string(multBin), "Energyscan", string("Nch = ") + std::to_string(multBin-1), 0, 0, "boxes");
      }
      myPlot.SetAxisRange("Y", 0.3, 0.9);
      myPlot.AddLegendBox(0.15, 0.92, "", false, 2);
      myPlot.AddTextBox(0.15, 0.35, energyLable + " // MC: Pythia8 Monash13");
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------
    { // -----------------------------------------------------------------------
      string plotName = "energyScanMeanMCLog";
      Plot myPlot(plotName, plotGroup);
      vector<int> multBins = {2, 9, 16, 26, 36};
      for(auto& multBin : multBins)
      {
        myPlot.AddHisto(string("meanPtMC_Log_") + std::to_string(multBin), "Energyscan", "", 0, 0, "band");
//        myPlot.AddHisto(string("meanPtMC_") + std::to_string(multBin), "Energyscan", string("Nch = ") + std::to_string(multBin-1), 0, 0, "boxes");
        myPlot.AddHisto(string("meanPt_Log_") + std::to_string(multBin), "Energyscan", string("Nch = ") + std::to_string(multBin-1), 0, -1, "boxes");
      }
      myPlot.SetAxisRange("Y", 0.3, 1.0);
      myPlot.AddLegendBox(0.24, 0.91, "", 3);
      //myPlot.AddTextBox(0.4, 0.3, energyLable);
      myPlot.AddTextBox(0.4, 0.3, energyLableMC);
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------

    { // -----------------------------------------------------------------------
      string plotName = "energyScanVariance";
      Plot myPlot(plotName, plotGroup);
      
      vector<int> multBins = {2, 6, 9, 11, 16, 21, 26, 31, 36};
      for(auto& multBin : multBins)
      {
        myPlot.AddHisto(string("variance_") + std::to_string(multBin), "Energyscan", string("Nch = ") + std::to_string(multBin-1), 0, 0, "boxes");
      }
      myPlot.SetAxisRange("Y", -0.4, 0.8);
      myPlot.AddLegendBox(0.24, 0.91, "", 3);
      myPlot.AddTextBox(0.4, 0.3, energyLable);
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------

    { // -----------------------------------------------------------------------
      string plotName = "energyScanVarianceMC";
      Plot myPlot(plotName, plotGroup);
      
      vector<int> multBins = {2, 9, 16, 26, 36};
      for(auto& multBin : multBins)
      {
        myPlot.AddHisto(string("variance_") + std::to_string(multBin), "Energyscan", string("#it{N}_{ch} = ") + std::to_string(multBin-1), 0, 0, "boxes");
        myPlot.AddHisto(string("varianceMC_") + std::to_string(multBin), "Energyscan", "", 0, -1, "band");
      }
      myPlot.SetAxisRange("Y", -0.4, 0.8);
      myPlot.AddLegendBox(0.24, 0.91, "",false, 2);
      myPlot.AddTextBox(0.15, 0.35, energyLable + " // MC: Pythia8 Monash13");
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------

    { // -----------------------------------------------------------------------
      string plotName = "energyScanVarianceMCLog";
      Plot myPlot(plotName, plotGroup);
      
      vector<int> multBins = {2, 6, 9, 11, 16, 21, 26, 31, 36};
      for(auto& multBin : multBins)
      {
        myPlot.AddHisto(string("varianceMC_Log_") + std::to_string(multBin), "Energyscan", "", 0, 0, "band");
        myPlot.AddHisto(string("variance_Log_") + std::to_string(multBin), "Energyscan", string("Nch = ") + std::to_string(multBin-1), 0, -1, "boxes");
      }
      myPlot.SetAxisRange("Y", -0.4, 1.4);
      myPlot.AddLegendBox(0.24, 0.91, "", 3);
      //myPlot.AddTextBox(0.4, 0.3, energyLable);
      myPlot.AddTextBox(0.4, 0.3, energyLableMC);
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------



    { // -----------------------------------------------------------------------
      string plotName = "meanPt";
      Plot myPlot(plotName, plotGroup);
      
      myPlot.AddHisto("momentUnfolded1", "pp_13TeV", "", kFullSquare, kRed+1, "", 60);
      myPlot.AddHisto("momentUnfolded1_Syst", "pp_13TeV", "13 TeV", kFullSquare, kRed+1, "boxes", 60);
      //myPlot.AddHisto("mbMeanPtMeanNchUnfolded", "pp_13TeV", "", kStar, kRed+1, "");
      myPlot.AddHisto("momentUnfolded1", "pp_7TeV", "", kFullCircle, kMagenta+1, "", 60);
      myPlot.AddHisto("momentUnfolded1_Syst", "pp_7TeV", "7 TeV", kFullCircle, kMagenta+1, "boxes", 60);
      //myPlot.AddHisto("mbMeanPtMeanNchUnfolded", "pp_7TeV", "", kStar, kMagenta+1, "");
      myPlot.AddHisto("momentUnfolded1", "pp_5TeV", "", kFullCross, kBlue+1, "", 60);
      myPlot.AddHisto("momentUnfolded1_Syst", "pp_5TeV", "5.02 TeV", kFullCross, kBlue+1, "boxes", 60);
      //myPlot.AddHisto("mbMeanPtMeanNchUnfolded", "pp_5TeV", "", kStar, kBlue+1, "");
      myPlot.AddHisto("momentUnfolded1", "pp_2TeV", "", kFullDiamond, kGreen+3, "", 40);
      myPlot.AddHisto("momentUnfolded1_Syst", "pp_2TeV", "2.76 TeV", kFullDiamond, kGreen+3, "boxes", 40);
      //myPlot.AddHisto("mbMeanPtMeanNchUnfolded", "pp_2TeV", "", kStar, kGreen+3, "");

      myPlot.SetAxisTitle("X", "#it{N}_{ch}");
      myPlot.SetAxisRange("X", 0, 60);
      myPlot.SetAxisRange("Y", 0.45, 0.85);
      myPlot.AddLegendBox(0.2, 0.9, "");
      myPlot.AddTextBox(0.4, 0.3, energyLable);
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------

    { // -----------------------------------------------------------------------
      string plotName = "variance";
      Plot myPlot(plotName, plotGroup);
      
      myPlot.AddHisto("varianceUnfolded", "pp_13TeV", "", kFullSquare, kRed+1, "", 60);
      myPlot.AddHisto("varianceUnfolded_Syst", "pp_13TeV", "13 TeV", kFullSquare, kRed+1, "boxes", 60);
      //myPlot.AddHisto("mbVarianceMeanNchUnfolded", "pp_13TeV_old", "", kStar, kRed+1, "");
      myPlot.AddHisto("varianceUnfolded", "pp_7TeV", "", kFullCircle, kMagenta+1, "", 60);
      myPlot.AddHisto("varianceUnfolded_Syst", "pp_7TeV", "7 TeV", kFullCircle, kMagenta+1, "boxes", 60);
      //myPlot.AddHisto("mbVarianceMeanNchUnfolded", "pp_7TeV", "", kStar, kMagenta+1, "");
      myPlot.AddHisto("varianceUnfolded", "pp_5TeV", "", kFullCross, kBlue+1, "", 60);
      myPlot.AddHisto("varianceUnfolded_Syst", "pp_5TeV", "5.02 TeV", kFullCross, kBlue+1, "boxes", 60);
      //myPlot.AddHisto("mbVarianceMeanNchUnfolded", "pp_5TeV", "", kStar, kBlue+1, "");
      myPlot.AddHisto("varianceUnfolded", "pp_2TeV", "", kFullDiamond, kGreen+3, "", 40);
      myPlot.AddHisto("varianceUnfolded_Syst", "pp_2TeV", "2.76 TeV", kFullDiamond, kGreen+3, "boxes", 40);
      //myPlot.AddHisto("mbVarianceMeanNchUnfolded", "pp_2TeV", "", kStar, kGreen+3, "");

      myPlot.SetAxisTitle("X", "#it{N}_{ch}");
      myPlot.SetAxisRange("X", 0, 60);
      myPlot.SetAxisRange("Y", 0.05, 0.75);
      myPlot.AddLegendBox(0.2, 0.9, "");
      myPlot.AddTextBox(0.4, 0.3, energyLable);
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------



    { // -----------------------------------------------------------------------
      string plotName = "varianceMC";
      Plot myPlot(plotName, plotGroup);
      
      myPlot.AddHisto("variance_2TeV", "Simulations", "2 TeV", 0, 0, "band");
      myPlot.AddHisto("variance_2.5TeV", "Simulations", "2.5 TeV", 0, 0, "band");
      myPlot.AddHisto("variance_3TeV", "Simulations", "3 TeV", 0, 0, "band");
      myPlot.AddHisto("variance_3.5TeV", "Simulations", "3.5 TeV", 0, 0, "band");
      myPlot.AddHisto("variance_4TeV", "Simulations", "4 TeV", 0, 0, "band");
      myPlot.AddHisto("variance_4.5TeV", "Simulations", "4.5 TeV", 0, 0, "band");
      myPlot.AddHisto("variance_5TeV", "Simulations", "5 TeV", 0, 0, "band");
      myPlot.AddHisto("variance_5.5TeV", "Simulations", "5.5 TeV", 0, 0, "band");
      myPlot.AddHisto("variance_6TeV", "Simulations", "6 TeV", 0, 0, "band");
      myPlot.AddHisto("variance_6.5TeV", "Simulations", "6.5 TeV", 0, 0, "band");
      myPlot.AddHisto("variance_7TeV", "Simulations", "7 TeV", 0, 0, "band");
      myPlot.AddHisto("variance_7.5TeV", "Simulations", "7.5 TeV", 0, 0, "band");
      myPlot.AddHisto("variance_8TeV", "Simulations", "8 TeV", 0, 0, "band");
      myPlot.AddHisto("variance_8.5TeV", "Simulations", "8.5 TeV", 0, 0, "band");
      myPlot.AddHisto("variance_9TeV", "Simulations", "9 TeV", 0, 0, "band");
      myPlot.AddHisto("variance_9.5TeV", "Simulations", "9.5 TeV", 0, 0, "band");
      myPlot.AddHisto("variance_10TeV", "Simulations", "10 TeV", 0, 0, "band");
      myPlot.AddHisto("variance_10.5TeV", "Simulations", "10.5 TeV", 0, 0, "band");
      myPlot.AddHisto("variance_11TeV", "Simulations", "11 TeV", 0, 0, "band");
      myPlot.AddHisto("variance_11.5TeV", "Simulations", "11.5 TeV", 0, 0, "band");
      myPlot.AddHisto("variance_12TeV", "Simulations", "12 TeV", 0, 0, "band");
      myPlot.AddHisto("variance_12.5TeV", "Simulations", "12.5 TeV", 0, 0, "band");
      myPlot.AddHisto("variance_13TeV", "Simulations", "13 TeV", 0, 0, "band");
      myPlot.AddHisto("variance_13.5TeV", "Simulations", "13.5 TeV", 0, 0, "band");
      myPlot.AddHisto("variance_14TeV", "Simulations", "14 TeV", 0, 0, "band");
      //myPlot.AddHisto("variance_2.76TeV", "Simulations", "2.76 TeV", kFullSquare, kGreen+3, "", 40);
      //myPlot.AddHisto("varianceGeneratedMC", "pp_5TeV", "5.02 TeV", kFullSquare, kBlue+1);
      //myPlot.AddHisto("variance_7TeV", "Simulations", "7 TeV", kFullCircle, kMagenta+1);
      //myPlot.AddHisto("varianceGeneratedMC", "pp_13TeV", "13 TeV", kFullCross, kRed+1);
      myPlot.SetAxisRange("X", 0, 40);
      myPlot.SetAxisRange("Y", 0.05, 0.9);
      myPlot.AddLegendBox(0.13, 0.9, "",4);
      myPlot.AddTextBox(0.4, 0.3, energyLableMC);
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------

    { // -----------------------------------------------------------------------
      string plotName = "moment1MC";
      Plot myPlot(plotName, plotGroup);
      
      myPlot.AddHisto("meanPt_2TeV", "Simulations", "2 TeV", 0, 0, "band");
      myPlot.AddHisto("meanPt_2.5TeV", "Simulations", "", 0, 0, "band");
      myPlot.AddHisto("meanPt_3TeV", "Simulations", "", 0, 0, "band");
      myPlot.AddHisto("meanPt_3.5TeV", "Simulations", "", 0, 0, "band");
      myPlot.AddHisto("meanPt_4TeV", "Simulations", "", 0, 0, "band");
      myPlot.AddHisto("meanPt_4.5TeV", "Simulations", "", 0, 0, "band");
      myPlot.AddHisto("meanPt_5TeV", "Simulations", "", 0, 0, "band");
      myPlot.AddHisto("meanPt_5.5TeV", "Simulations", "", 0, 0, "band");
      myPlot.AddHisto("meanPt_6TeV", "Simulations", "", 0, 0, "band");
      myPlot.AddHisto("meanPt_6.5TeV", "Simulations", "", 0, 0, "band");
      myPlot.AddHisto("meanPt_7TeV", "Simulations", "7 TeV", 0, 0, "band");
      myPlot.AddHisto("meanPt_7.5TeV", "Simulations", "", 0, 0, "band");
      myPlot.AddHisto("meanPt_8TeV", "Simulations", "", 0, 0, "band");
      myPlot.AddHisto("meanPt_8.5TeV", "Simulations", "", 0, 0, "band");
      myPlot.AddHisto("meanPt_9TeV", "Simulations", "", 0, 0, "band");
      myPlot.AddHisto("meanPt_9.5TeV", "Simulations", "", 0, 0, "band");
      myPlot.AddHisto("meanPt_10TeV", "Simulations", "", 0, 0, "band");
      myPlot.AddHisto("meanPt_10.5TeV", "Simulations", "", 0, 0, "band");
      myPlot.AddHisto("meanPt_11TeV", "Simulations", "", 0, 0, "band");
      myPlot.AddHisto("meanPt_11.5TeV", "Simulations", "", 0, 0, "band");
      myPlot.AddHisto("meanPt_12TeV", "Simulations", "", 0, 0, "band");
      myPlot.AddHisto("meanPt_12.5TeV", "Simulations", "", 0, 0, "band");
      myPlot.AddHisto("meanPt_13TeV", "Simulations", "", 0, 0, "band");
      myPlot.AddHisto("meanPt_13.5TeV", "Simulations", "", 0, 0, "band");
      myPlot.AddHisto("meanPt_14TeV", "Simulations", "14 TeV", 0, 0, "band");


      //myPlot.AddHisto("meanPt_2.76TeV", "Simulations", "2.76 TeV", kFullSquare, kGreen+3, "", 40);
      //myPlot.AddHisto("momentGeneratedMC1", "pp_5TeV", "5.02 TeV", kFullSquare, kBlue+1);
      //myPlot.AddHisto("meanPt_7TeV", "Simulations", "7 TeV", kFullCircle, kMagenta+1);
      //myPlot.AddHisto("momentGeneratedMC1", "pp_13TeV", "13 TeV", kFullCross, kRed+1);
      myPlot.SetAxisRange("X", 0, 60);
      myPlot.SetAxisRange("Y", 0.45, 0.85);
      myPlot.AddLegendBox(0.35, 0.3, "",4);
      myPlot.AddTextBox(0.4, 0.5, energyLableMC);
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------

    { // -----------------------------------------------------------------------
      string plotName = "moment1MCGenerated";
      Plot myPlot(plotName, plotGroup);
      myPlot.AddHisto("momentGeneratedMC1", "pp_2TeV", "2.76 TeV", kFullSquare, kGreen+3, "", 40);
      myPlot.AddHisto("momentGeneratedMC1", "pp_5TeV", "5.02 TeV", kFullSquare, kBlue+1, "");
      myPlot.AddHisto("momentGeneratedMC1", "pp_7TeV", "7 TeV", kFullCircle, kMagenta+1, "");
      myPlot.AddHisto("momentGeneratedMC1", "pp_13TeV", "13 TeV", kFullCross, kRed+1, "");
      myPlot.SetAxisRange("X", 0, 60);
      myPlot.SetAxisRange("Y", 0.45, 0.85);
      myPlot.AddLegendBox(0.6, 0.5, "");
      myPlot.AddTextBox(0.15, 0.92, energyLable);
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------

    { // -----------------------------------------------------------------------
      string plotName = "moment2";
      Plot myPlot(plotName, plotGroup);
      myPlot.AddHisto("momentUnfolded2", "pp_2TeV", "", kFullSquare, kGreen+3, "", 40);
      myPlot.AddHisto("momentUnfolded2_Syst", "pp_2TeV", "2.76 TeV", kFullSquare, kGreen+3, "boxes", 40);
      myPlot.AddHisto("momentUnfolded2", "pp_5TeV", "", kFullSquare, kBlue+1);
      myPlot.AddHisto("momentUnfolded2_Syst", "pp_5TeV", "5.02 TeV", kFullSquare, kBlue+1, "boxes");
      myPlot.AddHisto("momentUnfolded2", "pp_7TeV", "", kFullCircle, kMagenta+1);
      myPlot.AddHisto("momentUnfolded2_Syst", "pp_7TeV", "7 TeV", kFullCircle, kMagenta+1, "boxes");
      myPlot.AddHisto("momentUnfolded2", "pp_13TeV", "", kFullCross, kRed+1);
      myPlot.AddHisto("momentUnfolded2_Syst", "pp_13TeV", "13 TeV", kFullCross, kRed+1, "boxes");
      myPlot.SetAxisRange("X", 0, 60);
      myPlot.SetAxisRange("Y", 0.2, 1.5);
      myPlot.AddLegendBox(0.2, 0.9, "");
      myPlot.AddTextBox(0.4, 0.3, energyLable);
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------

    { // -----------------------------------------------------------------------
      string plotName = "moment3";
      Plot myPlot(plotName, plotGroup);
      myPlot.AddHisto("momentUnfolded3", "pp_2TeV", "", kFullSquare, kGreen+3, "", 40);
      myPlot.AddHisto("momentUnfolded3_Syst", "pp_2TeV", "2.76 TeV", kFullSquare, kGreen+3, "boxes", 40);
      myPlot.AddHisto("momentUnfolded3", "pp_5TeV", "", kFullSquare, kBlue+1);
      myPlot.AddHisto("momentUnfolded3_Syst", "pp_5TeV", "5.02 TeV", kFullSquare, kBlue+1, "boxes");
      myPlot.AddHisto("momentUnfolded3", "pp_7TeV", "", kFullCircle, kMagenta+1);
      myPlot.AddHisto("momentUnfolded3_Syst", "pp_7TeV", "7 TeV", kFullCircle, kMagenta+1, "boxes");
      myPlot.AddHisto("momentUnfolded3", "pp_13TeV", "", kFullCross, kRed+1);
      myPlot.AddHisto("momentUnfolded3_Syst", "pp_13TeV", "13 TeV", kFullCross, kRed+1, "boxes");
      myPlot.SetAxisRange("X", 0, 60);
      myPlot.SetAxisRange("Y", 0.1, 7.0);
      myPlot.AddLegendBox(0.6, 0.4, "");
      myPlot.AddTextBox(0.15, 0.92, energyLable);
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------

    { // -----------------------------------------------------------------------
      string plotName = "multDensity";
      Plot myPlot(plotName, plotGroup);
      myPlot.SetDrawingProperties("logY");
      myPlot.AddHisto("multDensityUnfolded", "pp_2TeV", "", kFullDiamond, kGreen+3, "", 42);
      myPlot.AddHisto("multDensityUnfolded", "pp_13TeV", "", kFullSquare, kRed+1);
      myPlot.AddHisto("multDensityUnfolded_Syst", "pp_13TeV", "13 TeV", kFullSquare, kRed+1, "boxes");
      //myPlot.AddHisto("multDensityUnfolded", "pp_7TeV", "", kFullCircle, kMagenta+1);
      //myPlot.AddHisto("multDensityUnfolded_Syst", "pp_7TeV", "7 TeV", kFullCircle, kMagenta+1, "boxes");
      myPlot.AddHisto("multDist_pp_7TeV_Stat", "Publications", "", kFullCircle, kMagenta+1, "", 52);
      myPlot.AddHisto("multDist_pp_7TeV_Syst", "Publications", "7 TeV", kFullCircle, kMagenta+1, "boxes", 52);
      myPlot.AddHisto("multDensityUnfolded", "pp_5TeV", "", kFullCross, kBlue+1, "", 49);
      myPlot.AddHisto("multDensityUnfolded_Syst", "pp_5TeV", "5.02 TeV", kFullCross, kBlue+1, "boxes", 49);
      myPlot.AddHisto("multDensityUnfolded", "pp_2TeV", "", kFullDiamond, kGreen+3, "", 42);
      myPlot.AddHisto("multDensityUnfolded_Syst", "pp_2TeV", "2.76 TeV", kFullDiamond, kGreen+3, "boxes", 42);
      myPlot.AddHisto("multDist_pp_0.9TeV_Stat", "Publications", "", kFullStar, kBlack, "", 33);
      myPlot.AddHisto("multDist_pp_0.9TeV_Syst", "Publications", "0.9 TeV", kFullStar, kBlack, "boxes", 33);
      myPlot.SetAxisTitle("X", "#it{N}_{ch}");
      myPlot.SetAxisTitle("Y", "#it{P}(#it{N}_{ch})");
      myPlot.SetAxisRange("X", 0, 60);
      myPlot.SetAxisRange("Y", 1e-4, 3e-1);
      myPlot.AddLegendBox(0.18, 0.45);
      myPlot.AddTextBox(0.4, 0.9, energyLable);
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------
    { // -----------------------------------------------------------------------
      string plotName = "multDistKNO";
      Plot myPlot(plotName, plotGroup);
      myPlot.SetDrawingProperties("logY");
      myPlot.AddHisto("multDistUnfoldedKNO", "pp_5TeV", "", kOpenCross, kBlue+1);
      myPlot.AddHisto("multDistUnfoldedKNO", "pp_13TeV", "", kOpenSquare, kRed+1);
      myPlot.AddHisto("multDistUnfoldedKNO_Syst", "pp_13TeV", "13 TeV", kOpenSquare, kRed+1, "boxes");
      myPlot.AddHisto("multDistUnfoldedKNO", "pp_7TeV", "", kOpenCircle, kMagenta+1);
      myPlot.AddHisto("multDistUnfoldedKNO_Syst", "pp_7TeV", "7 TeV", kOpenCircle, kMagenta+1, "boxes");
      //myPlot.AddHisto("multDist_pp_7TeVKNO_Stat", "Publications", "", kOpenCircle, kMagenta+1, "");
      //myPlot.AddHisto("multDist_pp_7TeVKNO_Syst", "Publications", "7 TeV", kOpenCircle, kMagenta+1, "boxes");
      myPlot.AddHisto("multDistUnfoldedKNO", "pp_5TeV", "", kOpenCross, kBlue+1);
      myPlot.AddHisto("multDistUnfoldedKNO_Syst", "pp_5TeV", "5.02 TeV", kOpenCross, kBlue+1, "boxes");
      myPlot.AddHisto("multDistUnfoldedKNO", "pp_2TeV", "", kOpenDiamond, kGreen+3);
      myPlot.AddHisto("multDistUnfoldedKNO_Syst", "pp_2TeV", "2.76 TeV", kOpenDiamond, kGreen+3, "boxes");
      myPlot.AddHisto("multDist_pp_0.9TeVKNO_Stat", "Publications", "", kOpenStar, kBlack, "");
      myPlot.AddHisto("multDist_pp_0.9TeVKNO_Syst", "Publications", "0.9 TeV", kOpenStar, kBlack, "boxes");
      myPlot.SetAxisRange("X", 0, 5);
      //myPlot.SetAxisTitle("X", "#it{N}_{ch}");
      myPlot.AddLegendBox(0.7, 0.9);
      myPlot.AddTextBox(0.15, 0.3, energyLable);
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------

    { // -----------------------------------------------------------------------
      string plotName = "multDistKNORatio";
      Plot myPlot(plotName, plotGroup, "default ratio");
      myPlot.SetDrawingProperties("logY");
      myPlot.AddHisto("multDistUnfoldedKNO", "pp_5TeV", "5.02 TeV", 0, kBlue+1);
      myPlot.AddHisto("multDistUnfoldedKNO", "pp_2TeV", "2.76 TeV", 0, kGreen+3);
      myPlot.AddHisto("multDistUnfoldedKNO", "pp_7TeV", "7 TeV", 0, kMagenta+1);
      myPlot.AddHisto("multDistUnfoldedKNO", "pp_13TeV", "13 TeV", 0, kRed+1);
      myPlot.AddRatio("multDistUnfoldedKNO", "pp_2TeV", "multDistUnfoldedKNO", "pp_5TeV", "", 0, kGreen+3);
      myPlot.AddRatio("multDistUnfoldedKNO", "pp_7TeV", "multDistUnfoldedKNO", "pp_5TeV", "", 0, kMagenta+1);
      myPlot.AddRatio("multDistUnfoldedKNO", "pp_13TeV", "multDistUnfoldedKNO", "pp_5TeV", "", 0, kRed+1);
      myPlot.SetAxisRange("ratio", 0.45, 1.55);
      myPlot.SetAxisTitle("ratio", "ratio to 5");
      myPlot.SetAxisRange("X", 0, 7);
      myPlot.AddLegendBox(0.7, 0.8);
      myPlot.AddTextBox(0.15, 0.3, energyLable);
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------

  } //==========================================================================


  //if(plotEnv.ContainsDatasets({"Publications"}))
  { //==========================================================================
    string publicationLable = alice + ", " + chargedParticles +  newLine + eta03 + ", " + ptRange10GeV + newLine + "#it{N}_{ch} cut: #it{p}_{T} > 0 GeV/#it{c}";
    string plotGroup = "system_comparison";
    { // -----------------------------------------------------------------------
      string plotName = "publication";
      Plot myPlot(plotName, plotGroup);
      myPlot.SetDrawingProperties("logX");
      myPlot.AddHisto("meanPt_PbPb_2.76TeV_Stat", "Publications", "", kFullCross, kWhite);
      myPlot.AddHisto("meanPt_pp_7TeV_Stat", "Publications", "", kFullSquare, kBlue+1);
      myPlot.AddHisto("meanPt_pp_7TeV_Syst", "Publications", "pp #sqrt{s} = 7 TeV", kFullSquare, kBlue+1, "boxes");
      myPlot.AddHisto("meanPt_pPb_5.02TeV_Stat", "Publications", "", kFullCircle, kMagenta+1);
      myPlot.AddHisto("meanPt_pPb_5.02TeV_Syst", "Publications", "p-Pb #sqrt{#it{s}_{NN}} = 5.02 TeV", kFullCircle, kMagenta+1, "boxes");
      myPlot.AddHisto("meanPt_PbPb_2.76TeV_Stat", "Publications", "", kFullCross, kRed+1);
      myPlot.AddHisto("meanPt_PbPb_2.76TeV_Syst", "Publications", "Pb-Pb  #sqrt{#it{s}_{NN}} = 2.76 TeV", kFullCross, kRed+1, "boxes");
      myPlot.SetAxisRange("X", 0, 100);
      myPlot.SetAxisRange("Y", 0.45, 0.92);
//      myPlot.AddLegendBox(0.5, 0.92, "");
//      myPlot.AddTextBox(0.3, 0.3, publicationLable);
      myPlot.AddLegendBox(0.15, 0.7, "");
      myPlot.AddTextBox(0.15, 0.92, publicationLable);
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------

    { // -----------------------------------------------------------------------
      string plotName = "publicationdNchdEta";
      Plot myPlot(plotName, plotGroup);
      myPlot.AddHisto("meanPt_PbPb_2.76TeV_dNchdEta_Stat", "Publications", "", kFullCross, kWhite);
      myPlot.AddHisto("meanPt_pp_7TeV_dNchdEta_Stat", "Publications", "", kFullSquare, kBlue+1);
      myPlot.AddHisto("meanPt_pp_7TeV_dNchdEta_Syst", "Publications", "pp #sqrt{s} = 7 TeV", kFullSquare, kBlue+1, "boxes");
      myPlot.AddHisto("meanPt_pPb_5.02TeV_dNchdEta_Stat", "Publications", "", kFullCircle, kMagenta+1);
      myPlot.AddHisto("meanPt_pPb_5.02TeV_dNchdEta_Syst", "Publications", "p-Pb #sqrt{#it{s}_{NN}} = 5.02 TeV", kFullCircle, kMagenta+1, "boxes");
      myPlot.AddHisto("meanPt_PbPb_2.76TeV_dNchdEta_Stat", "Publications", "", kFullCross, kRed+1);
      myPlot.AddHisto("meanPt_PbPb_2.76TeV_dNchdEta_Syst", "Publications", "Pb-Pb #sqrt{#it{s}_{NN}} = 2.76 TeV", kFullCross, kRed+1, "boxes");
      myPlot.AddHisto("momentUnfolded1dNchDeta", "pp_5TeV", "pp", kOpenSquare, kBlue+1);
      myPlot.AddHisto("momentUnfolded1dNchDeta", "pPb_5TeV", "pPb", kOpenCircle, kMagenta+1);
      myPlot.AddHisto("momentUnfolded1dNchDeta", "PbPb_5TeV", "PbPb", kOpenCross, kRed+1);
//      myPlot.CropHisto("momentUnfolded1", "pp_5TeV", "X", 0, 40)
      myPlot.SetAxisRange("Y", 0.45, 0.92);
      myPlot.AddLegendBox(0.55, 0.3, "");
      myPlot.AddTextBox(0.5, 0.92, publicationLable);
    //  myPlot3.SetDrawingProperties("thick");
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------

    { // -----------------------------------------------------------------------
      string plotName = "multDistsComparisonPub";
      Plot myPlot(plotName, plotGroup, "default ratio");
      myPlot.SetDrawingProperties("logY");
      myPlot.AddHisto("multDist_pp_7TeV_Stat", "Publications", "", kFullSquare, kMagenta+1);
      myPlot.AddHisto("multDist_pp_7TeV_Syst", "Publications", "preliminary (ALICE-PUBLIC-2013-001)", kFullSquare, kMagenta+1, "boxes");
      myPlot.AddHisto("multDensityUnfolded", "pp_7TeV", "", kOpenCircle, kBlue+1);
      myPlot.AddHisto("multDensityUnfolded_Syst", "pp_7TeV", "this work", kOpenCircle, kBlue+1, "boxes");
      myPlot.AddHisto("multDensityGeneratedMC", "pp_7TeV", "Pythia6", kOpenCross, kGray+2);
//      myPlot.AddHisto("multDensityGeneratedMC", "Simulations", "Pythia8", kOpenCross, kGray+2);
      myPlot.AddRatio("multDensityUnfolded_Syst", "pp_7TeV", "multDist_pp_7TeV_Syst", "Publications", "", kOpenCircle, kBlue+1, "boxes");
//      myPlot.AddRatio("multDensityUnfolded", "pp_7TeV", "multDist_pp_7TeV_Stat", "Publications");
      myPlot.SetAxisRange("X", 0, 60);
      myPlot.SetAxisRange("ratio", 0.75, 1.25);
      myPlot.AddLegendBox(0.2, 0.25);
      string energyLable = alice + newLine + chargedParticles + ", " + "pp collisions" + newLine + eta08 + ", " + ptRange;
      myPlot.AddTextBox(0.42, 0.92, energyLable);
      plotEnv.AddPlot(myPlot);
    } // -----------------------------------------------------------------------


  } //==========================================================================
  if(false)
  {
    string plotGroup = "QA";
  { // -----------------------------------------------------------------------
    string plotName = "multDistMeasured13TeV";
    Plot myPlot(plotName, plotGroup);

    myPlot.SetDrawingProperties("logY");
  //  myPlot.SetDrawingProperties("logZ");
    myPlot.AddHisto("multDistMeasured", "pp_13TeV_old", "event selection before", kFullSquare, kRed+1);
    myPlot.AddHisto("multDistMeasuredMC", "pp_13TeV_old", "", kOpenSquare, kRed+1);
    myPlot.AddHisto("multDistMeasured", "pp_13TeV", "AilEventCuts and PhysSel plieup rejection", kFullCircle, kBlue+1);
    myPlot.AddHisto("multDistMeasuredMC", "pp_13TeV", "", kOpenCircle, kBlue+1);
    myPlot.AddLegendBox(0.3, 0.9);
    myPlot.SetAxisRange("X", 0, 60);
  //  myPlot.SetAxisRange("Y", 0.1, 1e6);
    plotEnv.AddPlot(myPlot);
  } // -----------------------------------------------------------------------

  { // -----------------------------------------------------------------------
    string plotName = "scaleEffectDDCMeas";
    Plot myPlot(plotName, plotGroup);
    myPlot.SetDrawingProperties("logZ");
    myPlot.AddHisto("scaleEffectMultMeas", "pp_13TeV_withDDC", "");
    myPlot.SetAxisTitle("Y", "#it{N}^{MC}_{acc}");
    myPlot.SetAxisTitle("X", "#it{N}^{scaled}_{acc}");
    myPlot.SetAxisRange("X", 0, 60);
    myPlot.SetAxisRange("Y", 0, 60);
    myPlot.AddLegendBox(0.2, 0.9, "");
    plotEnv.AddPlot(myPlot);
  } // -----------------------------------------------------------------------

  { // -----------------------------------------------------------------------
    string plotName = "scaleEffectDDCTrue";
    Plot myPlot(plotName, plotGroup);
    myPlot.SetDrawingProperties("logZ");
    myPlot.AddHisto("scaleEffectMultTrue", "pp_13TeV_withDDC", "");
    myPlot.SetAxisTitle("Y", "#it{N}^{MC}_{ch}");
    myPlot.SetAxisTitle("X", "#it{N}^{scaled}_{ch}");
    myPlot.SetAxisRange("X", 0, 60);
    myPlot.SetAxisRange("Y", 0, 60);
    myPlot.AddLegendBox(0.2, 0.9, "");
    plotEnv.AddPlot(myPlot);
  } // -----------------------------------------------------------------------
  { // -----------------------------------------------------------------------
    string plotName = "multDist_withDDC";
    Plot myPlot(plotName, plotGroup, "default ratio");
    myPlot.SetDrawingProperties("logY");
    myPlot.AddHisto("multDistUnfolded_Syst", "pp_13TeV", "without PCC and SecScaling", kFullSquare, kBlue+1, "boxes", 60);
    myPlot.AddHisto("multDistUnfolded", "pp_13TeV", "", kFullSquare, kBlue+1, "", 60);
    myPlot.AddHisto("multDistUnfolded", "pp_13TeV_withDDC", "with PCC and SecScaling", kFullSquare, kRed+1, "", 60);
    myPlot.AddRatio("multDistUnfolded_Syst", "pp_13TeV", "multDistUnfolded", "pp_13TeV_withDDC", "", kFullSquare, kBlue+1, "boxes", 60);
    myPlot.SetAxisRange("X", 0, 60);
//    myPlot.SetAxisRange("Y", 0.45, 0.85);
    myPlot.AddTextBox(0.2, 0.25, "data unfolded");
    myPlot.SetAxisRange("ratio", 0.8, 1.2);
    myPlot.AddLegendBox(0.4, 0.9, "");
    plotEnv.AddPlot(myPlot);
  } // -----------------------------------------------------------------------

  { // -----------------------------------------------------------------------
    string plotName = "multDistKNO_withDDC";
    Plot myPlot(plotName, plotGroup, "default ratio");
    myPlot.SetDrawingProperties("logY");
    myPlot.AddHisto("multDistUnfoldedKNO_Syst", "pp_13TeV", "without PCC and SecScaling", kFullSquare, kBlue+1, "boxes", 60);
    myPlot.AddHisto("multDistUnfoldedKNO", "pp_13TeV", "", kFullSquare, kBlue+1, "", 60);
    myPlot.AddHisto("multDistUnfoldedKNO", "pp_13TeV_withDDC", "with PCC and SecScaling", kFullSquare, kRed+1, "", 60);
    myPlot.AddRatio("multDistUnfoldedKNO_Syst", "pp_13TeV", "multDistUnfoldedKNO", "pp_13TeV_withDDC", "", kFullSquare, kBlue+1, "boxes", 60);
    myPlot.SetAxisRange("X", 0, 60);
//    myPlot.SetAxisRange("Y", 0.45, 0.85);
    myPlot.AddTextBox(0.2, 0.25, "data unfolded");
    myPlot.SetAxisRange("ratio", 0.8, 1.2);
    myPlot.AddLegendBox(0.4, 0.9, "");
    plotEnv.AddPlot(myPlot);
  } // -----------------------------------------------------------------------

  { // -----------------------------------------------------------------------
    string plotName = "multDensity_withDDC";
    Plot myPlot(plotName, plotGroup, "default ratio");
    myPlot.SetDrawingProperties("logY");
    myPlot.AddHisto("multDensityUnfolded_Syst", "pp_13TeV", "without PCC and SecScaling", kFullSquare, kBlue+1, "boxes", 60);
    myPlot.AddHisto("multDensityUnfolded", "pp_13TeV", "", kFullSquare, kBlue+1, "", 60);
    myPlot.AddHisto("multDensityUnfolded", "pp_13TeV_withDDC", "with PCC and SecScaling", kFullSquare, kRed+1, "", 60);
    myPlot.AddRatio("multDensityUnfolded_Syst", "pp_13TeV", "multDensityUnfolded", "pp_13TeV_withDDC", "", kFullSquare, kBlue+1, "boxes", 60);
    myPlot.SetAxisRange("X", 0, 60);
//    myPlot.SetAxisRange("Y", 0.45, 0.85);
    myPlot.AddTextBox(0.2, 0.25, "data unfolded");
    myPlot.SetAxisRange("ratio", 0.8, 1.2);
    myPlot.AddLegendBox(0.4, 0.9, "");
    plotEnv.AddPlot(myPlot);
  } // -----------------------------------------------------------------------

  { // -----------------------------------------------------------------------
    string plotName = "multDistMeasuredMC_withDDC";
    Plot myPlot(plotName, plotGroup, "default ratio");
    myPlot.SetDrawingProperties("logY");
    myPlot.AddHisto("multDistMeasuredMC", "pp_13TeV", "without PCC and SecScaling", kFullSquare, kBlue+1, "", 60);
    myPlot.AddHisto("multDistMeasuredMC", "pp_13TeV_withDDC", "with PCC and SecScaling", kFullSquare, kRed+1, "", 60);
    myPlot.AddRatio("multDistMeasuredMC", "pp_13TeV", "multDistMeasuredMC", "pp_13TeV_withDDC", "", kFullSquare, kBlue+1, "", 60);
    myPlot.SetAxisRange("X", 0, 60);
//    myPlot.SetAxisRange("Y", 0.45, 0.85);
    myPlot.AddTextBox(0.2, 0.25, "MC measured");
    myPlot.SetAxisRange("ratio", 0.8, 1.2);
    myPlot.AddLegendBox(0.4, 0.9, "");
    plotEnv.AddPlot(myPlot);
  } // -----------------------------------------------------------------------

  { // -----------------------------------------------------------------------
    string plotName = "multDistGeneratedMC_withDDC";
    Plot myPlot(plotName, plotGroup, "default ratio");
    myPlot.SetDrawingProperties("logY");
    myPlot.AddHisto("multDistGeneratedMC", "pp_13TeV", "without PCC and SecScaling", kFullSquare, kBlue+1, "", 60);
    myPlot.AddHisto("multDistGeneratedMC", "pp_13TeV_withDDC", "with PCC and SecScaling", kFullSquare, kRed+1, "", 60);
    myPlot.AddRatio("multDistGeneratedMC", "pp_13TeV", "multDistMeasuredMC", "pp_13TeV_withDDC", "", kFullSquare, kBlue+1, "", 60);
    myPlot.SetAxisRange("X", 0, 60);
//    myPlot.SetAxisRange("Y", 0.45, 0.85);
//    myPlot.SetAxisRange("ratio", 0.8, 1.2);
    myPlot.AddTextBox(0.2, 0.25, "MC truth");
    myPlot.AddLegendBox(0.4, 0.9, "");
    plotEnv.AddPlot(myPlot);
  } // -----------------------------------------------------------------------

  { // -----------------------------------------------------------------------
    string plotName = "multCorrelationMatrix_noDDC";
    Plot myPlot(plotName, plotGroup);
    myPlot.SetDrawingProperties("logZ");
    myPlot.AddHisto("multCorrelationMatrix", "pp_13TeV");
    myPlot.AddTextBox(0.4, 0.25, "no data-driven corrections");
    plotEnv.AddPlot(myPlot);
  } // -----------------------------------------------------------------------
  { // -----------------------------------------------------------------------
    string plotName = "multCorrelationMatrix_withDDC";
    Plot myPlot(plotName, plotGroup);
    myPlot.SetDrawingProperties("logZ");
    myPlot.AddHisto("multCorrelationMatrix", "pp_13TeV_withDDC");
    myPlot.AddTextBox(0.4, 0.25, "with data-driven corrections");
    plotEnv.AddPlot(myPlot);
  } // -----------------------------------------------------------------------

  { // -----------------------------------------------------------------------
    string plotName = "responseMatrix_Mean";
    Plot myPlot(plotName, plotGroup, "default ratio");
    myPlot.AddHisto("mean", "pp_13TeV", "without PCC and SecScaling", kFullSquare, kBlue+1);
    myPlot.AddHisto("mean", "pp_13TeV_withDDC", "with PCC and SecScaling", kFullSquare, kRed+1);
    myPlot.SetAxisTitle("Y", "<#it{N}_{acc}>");
    myPlot.SetAxisRange("Y", 0, 45);
    myPlot.SetAxisRange("X", 0, 60);
    myPlot.SetAxisRange("ratio", 0.95, 1.05);
    myPlot.AddRatio("mean", "pp_13TeV", "mean", "pp_13TeV_withDDC", "", kFullSquare, kBlue+1);
    myPlot.AddLegendBox(0.2, 0.9);
    plotEnv.AddPlot(myPlot);
  } // -----------------------------------------------------------------------

  { // -----------------------------------------------------------------------
    string plotName = "responseMatrix_Sigma";
    Plot myPlot(plotName, plotGroup, "default ratio");
    myPlot.AddHisto("sigma", "pp_13TeV", "without PCC and SecScaling", kFullSquare, kBlue+1);
    myPlot.AddHisto("sigma", "pp_13TeV_withDDC", "with PCC and SecScaling", kFullSquare, kRed+1);
    myPlot.SetAxisTitle("Y", "#sigma(#it{N}_{acc})");
    myPlot.SetAxisRange("Y", 0, 5);
    myPlot.SetAxisRange("X", 0, 60);
    myPlot.SetAxisRange("ratio", 0.5, 1.5);
    myPlot.AddRatio("sigma", "pp_13TeV", "sigma", "pp_13TeV_withDDC", "", kFullSquare, kBlue+1);
    myPlot.AddLegendBox(0.2, 0.9);
    plotEnv.AddPlot(myPlot);
  } // -----------------------------------------------------------------------

  { // -----------------------------------------------------------------------
    string plotName = "responseMatrix_noDDC";
    Plot myPlot(plotName, plotGroup);
    myPlot.SetDrawingProperties("logZ");
    myPlot.AddHisto("responseMatrix", "pp_13TeV");
    myPlot.AddTextBox(0.4, 0.25, "no data-driven corrections");
    plotEnv.AddPlot(myPlot);
  } // -----------------------------------------------------------------------
  { // -----------------------------------------------------------------------
    string plotName = "responseMatrix_withDDC";
    Plot myPlot(plotName, plotGroup);
    myPlot.SetDrawingProperties("logZ");
    myPlot.AddHisto("responseMatrix", "pp_13TeV_withDDC");
    myPlot.AddTextBox(0.4, 0.25, "with data-driven corrections");
    plotEnv.AddPlot(myPlot);
  } // -----------------------------------------------------------------------

  { // -----------------------------------------------------------------------
    string plotName = "moment1_withDDC";
    Plot myPlot(plotName, plotGroup, "default ratio");
    myPlot.AddHisto("momentUnfolded1", "pp_13TeV", "", kFullSquare, kBlue+1, "", 60);
    myPlot.AddHisto("momentUnfolded1_Syst", "pp_13TeV", "without PCC and SecScaling", kFullSquare, kBlue+1, "boxes", 60);
    myPlot.AddHisto("momentUnfolded1", "pp_13TeV_withDDC", "with PCC and SecScaling", kFullSquare, kRed+1, "", 60);
    myPlot.AddRatio("momentUnfolded1_Syst", "pp_13TeV", "momentUnfolded1", "pp_13TeV_withDDC", "", kFullSquare, kBlue+1, "boxes", 60);
    myPlot.SetAxisRange("X", 0, 60);
    myPlot.SetAxisRange("Y", 0.45, 0.85);
    myPlot.SetAxisRange("ratio", 0.99, 1.01);
    myPlot.AddLegendBox(0.2, 0.9, "");
    plotEnv.AddPlot(myPlot);
  } // -----------------------------------------------------------------------
  { // -----------------------------------------------------------------------
    string plotName = "variance_withDDC";
    Plot myPlot(plotName, plotGroup, "default ratio");
    myPlot.AddHisto("varianceUnfolded", "pp_13TeV", "", kFullSquare, kBlue+1, "", 60);
    myPlot.AddHisto("varianceUnfolded_Syst", "pp_13TeV", "without PCC and SecScaling", kFullSquare, kBlue+1, "boxes", 60);
    myPlot.AddHisto("varianceUnfolded", "pp_13TeV_withDDC", "with PCC and SecScaling", kFullSquare, kRed+1, "", 60);
    myPlot.AddRatio("varianceUnfolded_Syst", "pp_13TeV", "varianceUnfolded", "pp_13TeV_withDDC", "", kFullSquare, kBlue+1, "boxes", 60);
    myPlot.SetAxisRange("X", 0, 60);
    myPlot.SetAxisRange("Y", 0.05, 0.9);
    myPlot.SetAxisRange("ratio", 0.95, 1.05);
    myPlot.AddLegendBox(0.2, 0.9, "");
    plotEnv.AddPlot(myPlot);
  } // -----------------------------------------------------------------------
  }

  plotEnv.DumpPlots(plotConfig);
  //plotEnv.CreatePlots();

}


// helper functions
string GetPtString(int pTbin)
{
  double binsPtDefault[53] = {0.1,0.15,0.2,0.25,0.3,0.35,0.4,0.45,0.5,0.55,0.6,0.65,0.7,0.75,0.8,0.85,0.9,0.95,1.0,1.1,1.2,1.3,1.4,1.5,1.6,1.7,1.8,1.9,2.0,2.2,2.4,2.6,2.8,3.0,3.2,3.4,3.6,3.8,4.0,4.5,5.0,5.5,6.0,6.5,7.0,8.0,9.0,10.0,20.0,30.0,40.0,50.0,60.0};

  double pTlow = binsPtDefault[pTbin-1];
  double pThigh = binsPtDefault[pTbin];

  std::ostringstream out;
  out.precision(2);
  out << std::fixed << pTlow << " GeV/c - " << pThigh << " GeV/c";


  return out.str();
}
