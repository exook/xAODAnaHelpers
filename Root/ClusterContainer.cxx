#include "xAODAnaHelpers/ClusterContainer.h"

#include <iostream>

using namespace xAH;
using std::vector;
using std::string;

ClusterContainer::ClusterContainer(const std::string& name, const std::string& detailStr, float units, bool mc)
  : ParticleContainer(name, detailStr, units, mc, true)
{
  m_rawE = new std::vector<float> ();
  m_rawEta = new std::vector<float> ();
  m_rawPhi = new std::vector<float> ();
  m_rawM = new std::vector<float> ();

  m_calE = new std::vector<float> ();
  m_calEta = new std::vector<float> ();
  m_calPhi = new std::vector<float> ();
  m_calM = new std::vector<float> ();
}

ClusterContainer::~ClusterContainer()
{
  delete m_rawE;
  delete m_rawEta;
  delete m_rawPhi;
  delete m_rawM;

  delete m_calE;
  delete m_calEta;
  delete m_calPhi;
  delete m_calM;
}

void ClusterContainer::setTree(TTree *tree)
{
  // Connect branches
  ParticleContainer::setTree(tree);
  tree->SetBranchStatus  ("nclus" , 1);
  tree->SetBranchAddress ("nclus" , &m_n);
  connectBranch<float>(tree, "rawE", &m_rawE);
  connectBranch<float>(tree, "rawEta", &m_rawEta);
  connectBranch<float>(tree, "rawPhi", &m_rawPhi);
  connectBranch<float>(tree, "rawM", &m_rawM);
  connectBranch<float>(tree, "calE", &m_calE);
  connectBranch<float>(tree, "calEta", &m_calEta);
  connectBranch<float>(tree, "calPhi", &m_calPhi);
  connectBranch<float>(tree, "calM", &m_calM);
}

void ClusterContainer::updateParticle(uint idx, Cluster& cluster)
{
  ParticleContainer::updateParticle(idx,cluster);
  cluster.rawE = m_rawE ->at(idx);
  cluster.rawE = m_rawEta ->at(idx);
  cluster.rawE = m_rawPhi ->at(idx);
  cluster.rawE = m_rawM ->at(idx);
  cluster.rawE = m_calE ->at(idx);
  cluster.rawE = m_calEta ->at(idx);
  cluster.rawE = m_calPhi ->at(idx);
  cluster.rawE = m_calM ->at(idx);
}


void ClusterContainer::setBranches(TTree *tree)
{
  ParticleContainer::setBranches(tree);
  setBranch<float>(tree, "rawE", m_rawE);
  setBranch<float>(tree, "rawEta", m_rawEta);
  setBranch<float>(tree, "rawPhi", m_rawPhi);
  setBranch<float>(tree, "rawM", m_rawM);
  setBranch<float>(tree, "calE", m_calE);
  setBranch<float>(tree, "calEta", m_calEta);
  setBranch<float>(tree, "calPhi", m_calPhi);
  setBranch<float>(tree, "calM", m_calM);
  return;
}



void ClusterContainer::clear()
{
  ParticleContainer::clear();
  m_rawE->clear();
  m_rawEta->clear();
  m_rawPhi->clear();
  m_rawM->clear();
  m_calE->clear();
  m_calEta->clear();
  m_calPhi->clear();
  m_calM->clear();
}


void ClusterContainer::FillCluster( const xAOD::CaloCluster* cluster ){
  m_rawE->push_back( cluster->rawE() /m_units );
  m_rawEta->push_back( cluster->rawEta() );
  m_rawPhi->push_back( cluster->rawPhi() );
  m_rawM->push_back( cluster->rawM() /m_units );
  m_calE->push_back( cluster->calE() /m_units );
  m_calEta->push_back( cluster->calEta() );
  m_calPhi->push_back( cluster->calPhi() );
  m_calM->push_back( cluster->calM() /m_units );
  return FillCluster(static_cast<const xAOD::IParticle*>(cluster));
}

void ClusterContainer::FillCluster( const xAOD::IParticle* particle )
{
  ParticleContainer::FillParticle(particle);
  const xAOD::CaloCluster* cluster=dynamic_cast<const xAOD::CaloCluster*>(particle);
  return;
}
