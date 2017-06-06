/**
 * @file   FatJetTrimmer.h
 *
 * @author Will Kalderon <william.kalderon@cern.ch>
 *
 * @brief  Apply trimming to fat jets. Copied from JetSelector.h
 *
 */

#ifndef xAODAnaHelpers_FatJetTrimmer_H
#define xAODAnaHelpers_FatJetTrimmer_H

// EDM include(s):
#include "xAODJet/Jet.h"
#include "xAODJet/JetContainer.h"

// ROOT include(s):
#include "TH1D.h"

// algorithm wrapper
#include "xAODAnaHelpers/Algorithm.h"
#include "xAODAnaHelpers/HelperFunctions.h"

// external tools include(s):
#include "AsgTools/AnaToolHandle.h"
#include "JetInterface/IJetModifier.h"
#include <fastjet/JetDefinition.hh>

class FatJetTrimmer : public xAH::Algorithm
{
  // put your configuration variables here as public variables.
  // that way they can be set directly from CINT and python.
public:
  bool m_useCutFlow = true;

  // configuration variables
  /// @brief input container name
  std::string m_inContainerName = "AntiKt10LCTopoJets";
  /// @brief output container name
  std::string m_outContainerName = "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets";

  /// @brief radius of subjets to form
  float m_radius = 0.3;
  /// @brief trimming cut to apply on subjets
  float m_fcut = 0.05;
  /// @brief algorithm for building subjets
  std::string m_subjetalgname = "kt";

  /// @brief The decoration key written to passing objects
  std::string m_decor = "nothing";
  /// @brief decorate selected objects? defaul passSel
  bool m_decorateSelectedObjects = false;

private:

  fastjet::JetAlgorithm m_s_alg = fastjet::kt_algorithm; //!

  // variables that don't get filled at submission time should be
  // protected from being send from the submission node to the worker
  // node (done by the //!)
public:
  // Tree *myTree; //!
  // TH1 *myHist; //!



  // this is a standard constructor
  FatJetTrimmer ();

  // these are the functions inherited from Algorithm
  virtual EL::StatusCode setupJob (EL::Job& job);
  virtual EL::StatusCode fileExecute ();
  virtual EL::StatusCode histInitialize ();
  virtual EL::StatusCode changeInput (bool firstFile);
  virtual EL::StatusCode initialize ();
  virtual EL::StatusCode execute ();
  virtual EL::StatusCode postExecute ();
  virtual EL::StatusCode finalize ();
  virtual EL::StatusCode histFinalize ();

  /// @cond
  // this is needed to distribute the algorithm to the workers
  ClassDef(FatJetTrimmer, 1);
  /// @endcond

};

#endif
