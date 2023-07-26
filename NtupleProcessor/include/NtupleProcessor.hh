#ifndef GUARD_NtupleProcessor_h
#define GUARD_NtupleProcessor_h

/*------------------------------------------------------------------------------
   NtupleProcessor
 Created : 2022-09-02  okugawa
 Main class of NtupleProcessor program. Created to handle input and running of
 ntuple processing.
 Takes input variables (datasets, configurations, etc) and sets up the
 appropriate classes to handle each portion of the analysis process.
------------------------------------------------------------------------------*/
#include <TString.h>
#include "EventAnalyzer.hh"
#include "TreeIterator.hh"
#include "FileSelector.hh"

namespace QQbarAnalysis
{
  class NtupleProcessor
  {
    public:
      NtupleProcessor(TString input="", TString fnac="",TString o="", int me = -1);
      ~NtupleProcessor(){}

      EventAnalyzer  eAnalyzer;
      TreeIterator   tIter;
      TString        input_file;
      TString        options;        // Extra options for this processing.
      int            maxEvents;      // Max number of events to run over in this ntuple

      TFile          *ntupleFile;

    private: 

  };
}
#endif