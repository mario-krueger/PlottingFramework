#ifndef PlottingFramework_h
#define PlottingFramework_h

// std headers
#include <iostream>
#include <map>
#include <set>
#include <algorithm>

// boost headers
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>

// root headers
#include "TCanvas.h"
#include "TLegendEntry.h"
#include "TFile.h"
#include "TIterator.h"
#include "TKey.h"
#include "TObjArray.h"
#include "TObject.h"
#include "TStyle.h"
#include "TGraph.h"
#include "TF1.h"
#include "TH1.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TH3D.h"
#include "THn.h"
#include "TSystem.h"
#include <TROOT.h>
#include <TObjectTable.h>
#include "TExec.h"
#include "TTimeStamp.h"
#include "TLatex.h"
#include "TPaveText.h"
#include "TLegend.h"
#include "TText.h"
#include "TLatex.h"
#include "TError.h"
#include "TSpline.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TPaletteAxis.h"
#include "TView.h"
#include "TApplication.h"
#include "TGWindow.h"

namespace PlottingFramework {


  // aliases
  using std::cout;
  using std::endl;
  using std::flush;
  using std::string;
  using std::vector;
  using std::tuple;
  using std::pair;
  using std::map;
  using std::set;
  using std::array;
  using std::shared_ptr;
  
  using boost::property_tree::ptree;
  using boost::property_tree::write_json;
  using boost::property_tree::read_json;
  using boost::property_tree::write_xml;
  using boost::property_tree::read_xml;


  const string gNameGroupSeparator = "_IN_";

  // functions wrapping the ugly std functionality for basic operations

  // simple unity of two sets
  // simple intersection of two sets.

  // simple case insensitive contains method for strings
  // todo: some variant like template to specify which options are allwoed for specific variable



}

#endif /* PlottingFramework_h */
