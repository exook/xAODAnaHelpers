// c++ include(s):
#include <iostream>
#include <typeinfo>
#include <sstream>

// EL include(s):
#include <EventLoop/Job.h>
#include <EventLoop/StatusCode.h>
#include <EventLoop/Worker.h>

// EDM include(s):
#include "xAODJet/JetContainer.h"
#include <xAODJet/JetAuxContainer.h>
#include <xAODCore/AuxContainerBase.h>
#include "xAODCore/ShallowCopy.h"
#include "AthContainers/ConstDataVector.h"
#include "PATInterfaces/SystematicVariation.h"
#include "PATInterfaces/SystematicRegistry.h"
#include "PATInterfaces/SystematicCode.h"

// package include(s):
#include "xAODEventInfo/EventInfo.h"
#include "xAODAnaHelpers/FatJetTrimmer.h"
#include "xAODAnaHelpers/HelperClasses.h"
#include "xAODAnaHelpers/HelperFunctions.h"

// external tools include(s):
#include "JetJvtEfficiency/JetJvtEfficiency.h"
#include "JetMomentTools/JetForwardJvtTool.h"
#include "xAODBTaggingEfficiency/BTaggingSelectionTool.h"

// ROOT include(s):
#include "TFile.h"
#include "TObjArray.h"
#include "TObjString.h"

// this is needed to distribute the algorithm to the workers
ClassImp(FatJetTrimmer)


FatJetTrimmer :: FatJetTrimmer () :
    Algorithm("FatJetTrimmer")
{
}

EL::StatusCode FatJetTrimmer :: setupJob (EL::Job& job)
{
  // Here you put code that sets up the job on the submission object
  // so that it is ready to work with your algorithm, e.g. you can
  // request the D3PDReader service or add output files.  Any code you
  // put here could instead also go into the submission script.  The
  // sole advantage of putting it here is that it gets automatically
  // activated/deactivated when you add/remove the algorithm from your
  // job, which may or may not be of value to you.

  ANA_MSG_DEBUG( "Calling setupJob");

  job.useXAOD ();
  xAOD::Init( "FatJetTrimmer" ).ignore(); // call before opening first file

  return EL::StatusCode::SUCCESS;
}



EL::StatusCode FatJetTrimmer :: histInitialize ()
{
  // Here you do everything that needs to be done at the very
  // beginning on each worker node, e.g. create histograms and output
  // trees.  This method gets called before any input files are
  // connected.

  ANA_MSG_DEBUG( "Calling histInitialize");
  ANA_CHECK( xAH::Algorithm::algInitialize());

  return EL::StatusCode::SUCCESS;
}



EL::StatusCode FatJetTrimmer :: fileExecute ()
{
  // Here you do everything that needs to be done exactly once for every
  // single file, e.g. collect a list of all lumi-blocks processed

  ANA_MSG_DEBUG( "Calling fileExecute");

  return EL::StatusCode::SUCCESS;
}



EL::StatusCode FatJetTrimmer :: changeInput (bool /*firstFile*/)
{
  // Here you do everything you need to do when we change input files,
  // e.g. resetting branch addresses on trees.  If you are using
  // D3PDReader or a similar service this method is not needed.

  ANA_MSG_DEBUG( "Calling changeInput");

  return EL::StatusCode::SUCCESS;
}



EL::StatusCode FatJetTrimmer :: initialize ()
{
  // Here you do everything that you need to do after the first input
  // file has been connected and before the first event is processed,
  // e.g. create additional histograms based on which variables are
  // available in the input files.  You can also create all of your
  // histograms and trees in here, but be aware that this method
  // doesn't get called if no events are processed.  So any objects
  // you create here won't be available in the output if you have no
  // input events.

  ANA_MSG_DEBUG( "Calling initialize");

  m_event = wk()->xaodEvent();
  m_store = wk()->xaodStore();

  if ( m_inContainerName.empty() ) {
    ANA_MSG_ERROR( "InputContainer is empty!");
    return EL::StatusCode::FAILURE;
  }
  
  
  if ( m_decorateSelectedObjects ) {
    ANA_MSG_INFO(" Decorate Jets with " << m_decor);
  }

  //  Add the chosen WP to the string labelling the vector<SF> decoration
  // m_outputSystNamesJVT = m_outputSystNamesJVT + "_JVT_" + m_WorkingPointJVT;
  
  ANA_MSG_DEBUG( "Number of events in file: " << m_event->getEntries() );

  ANA_MSG_DEBUG( "FatJetTrimmer Interface succesfully initialized!" );

  return EL::StatusCode::SUCCESS;
}



EL::StatusCode FatJetTrimmer :: execute ()
{
  // Here you do everything that needs to be done on every single
  // events, e.g. read input variables, apply cuts, and fill
  // histograms and trees.  This is where most of your actual analysis
  // code will go.

  ANA_MSG_DEBUG( "Applying Fat Jet Trimming... " << m_name);

  // set trimming algo
  if(m_subjetalgname == "kt") {
    m_s_alg = fastjet::kt_algorithm;
  }
  else {
    ANA_MSG_ERROR("m_subjetalgname " << m_subjetalgname << " not recognised");
  }
  
  // retrieve event
  const xAOD::EventInfo* eventInfo(nullptr);
  ANA_CHECK( HelperFunctions::retrieve(eventInfo, m_eventInfoContainerName, m_event, m_store, msg()) );

  // get jets to trim
  const xAOD::JetContainer* inJets(nullptr);
  ANA_CHECK( HelperFunctions::retrieve(inJets, m_inContainerName, m_event, m_store, msg()) );

  // apply trimming jet by jet and modify container
  std::pair< xAOD::JetContainer*, xAOD::ShallowAuxContainer* > trimmedJets_shallowCopy = xAOD::shallowCopyContainer( *inJets );
  xAOD::JetContainer::iterator jetSC_itr = (trimmedJets_shallowCopy.first)->begin();
  xAOD::JetContainer::iterator jetSC_end = (trimmedJets_shallowCopy.first)->end();
  for( ; jetSC_itr != jetSC_end; ++jetSC_itr ) {
    TLorentzVector trimmedJetTLV = HelperFunctions::jetTrimming((*jetSC_itr), m_radius, m_fcut, m_s_alg);
    xAOD::JetFourMom_t newp4(trimmedJetTLV.Pt(), trimmedJetTLV.Eta(), trimmedJetTLV.Phi(), trimmedJetTLV.M());
    (*jetSC_itr)->setJetP4( newp4);
  }  
  
  // to do in the future: add various substructure variables as auxdata

  // store in outContainerName
  ANA_CHECK( m_store->record( trimmedJets_shallowCopy.first, m_outContainerName));
  ANA_CHECK( m_store->record( trimmedJets_shallowCopy.second, m_outContainerName+"Aux."));

  // look what we have in TStore
  if(msgLvl(MSG::VERBOSE)) m_store->print();  
  
  ANA_MSG_DEBUG( "Leave Fat Jet Trimming... ");
  
  return EL::StatusCode::SUCCESS;
  
}




EL::StatusCode FatJetTrimmer :: postExecute ()
{
  // Here you do everything that needs to be done after the main event
  // processing.  This is typically very rare, particularly in user
  // code.  It is mainly used in implementing the NTupleSvc.

  ANA_MSG_DEBUG("Calling postExecute");

  return EL::StatusCode::SUCCESS;
}



EL::StatusCode FatJetTrimmer :: finalize ()
{
  // This method is the mirror image of initialize(), meaning it gets
  // called after the last event has been processed on the worker node
  // and allows you to finish up any objects you created in
  // initialize() before they are written to disk.  This is actually
  // fairly rare, since this happens separately for each worker node.
  // Most of the time you want to do your post-processing on the
  // submission node after all your histogram outputs have been
  // merged.  This is different from histFinalize() in that it only
  // gets called on worker nodes that processed input events.

  ANA_MSG_DEBUG( m_name );

  return EL::StatusCode::SUCCESS;
}



EL::StatusCode FatJetTrimmer :: histFinalize ()
{
  // This method is the mirror image of histInitialize(), meaning it
  // gets called after the last event has been processed on the worker
  // node and allows you to finish up any objects you created in
  // histInitialize() before they are written to disk.  This is
  // actually fairly rare, since this happens separately for each
  // worker node.  Most of the time you want to do your
  // post-processing on the submission node after all your histogram
  // outputs have been merged.  This is different from finalize() in
  // that it gets called on all worker nodes regardless of whether
  // they processed input events.

  ANA_MSG_DEBUG( "Calling histFinalize");
  ANA_CHECK( xAH::Algorithm::algFinalize());

  return EL::StatusCode::SUCCESS;
}

