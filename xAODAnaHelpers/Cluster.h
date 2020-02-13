#ifndef xAODAnaHelpers_Cluster_H
#define xAODAnaHelpers_Cluster_H

#include "xAODAnaHelpers/Particle.h"


namespace xAH {

  class Cluster : public Particle {
  public:
    float rawE;
    float rawEta;
    float rawPhi;
    float rawM;
    float calE;
    float calEta;
    float calPhi;
    float calM;
  };

}//xAH
#endif // xAODAnaHelpers_Cluster_H
