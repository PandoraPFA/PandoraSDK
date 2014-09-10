/**
 *  @file   LCContent/include/LCMonitoring/MCParticlesMonitoringAlgorithm.h
 * 
 *  @brief  monitoring of the MC Particles
 * 
 *  $Log: $
 */
#ifndef LC_MC_PARTICLES_MONITORING_ALGORITHM_H
#define LC_MC_PARTICLES_MONITORING_ALGORITHM_H 1

#include "Pandora/Algorithm.h"

namespace lc_content
{

/**
 *  @brief MCParticlesMonitoringAlgorithm class
 */
class MCParticlesMonitoringAlgorithm : public pandora::Algorithm
{
public:
    /**
     *  @brief  Factory class for instantiating algorithm
     */
    class Factory : public pandora::AlgorithmFactory
    {
    public:
        pandora::Algorithm *CreateAlgorithm() const;
    };

    virtual pandora::StatusCode Initialize();
    virtual ~MCParticlesMonitoringAlgorithm();

private:
    pandora::StatusCode Run();
    pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);

    /**
     *  @brief  
     * 
     *  @param  mcParticleList
     */
    void MonitorMCParticleList(const pandora::MCParticleList& mcParticleList);

    /**
     *  @brief  
     */
    pandora::StatusCode FillListOfUsedMCParticles();

    /**
     *  @brief  
     * 
     *  @param  pMCParticle
     *  @param  caloHitEnergy
     *  @param  trackEnergy
     */
    bool TakeMCParticle(const pandora::MCParticle *pMCParticle, float &caloHitEnergy, float &trackEnergy);

    /**
     *  @brief  Print the MCParticle's parameters
     * 
     *  @param  pMCParticle mcparticle of which the information is printed
     *  @param  caloHitEnergy
     *  @param  trackEnergy
     *  @param  o output-stream where the information is written to
     */
    void PrintMCParticle(const pandora::MCParticle *pMCParticle, float &caloHitEnergy, float &trackEnergy, std::ostream &o);

    pandora::StringVector   m_clusterListNames;                     ///< list of strings denoting clusternames 
    std::string             m_monitoringFileName;                   ///< filename for storing the monitoring information (ROOT)
    std::string             m_treeName;                             ///< tree name for the monitoring output
    bool                    m_print;                                ///< print the monitoring info
    bool                    m_sort;                                 ///< sort mc particles according to their energy
    bool                    m_indent;                               ///< indent mc particles at printout according to their outer radius

    bool                    m_onlyFinal;                            ///< monitor only the final PFOs (those without daughters)
    bool                    m_excludeRootParticles;                 ///< exclude MC particles with no parents from monitoring
    bool                    m_onlyRootParticles;                    ///< monitor only MC particles with no parents (root of the mcparticle tree)
    bool                    m_haveCaloHits;                         ///< monitor PFOs which have calohits
    bool                    m_haveTracks;                           ///< monitor PFOs which have tracks

    pandora::FloatVector   *m_energy;                               ///< energy of mc particle
    pandora::FloatVector   *m_momentumX;                            ///< x component of momentum of mc particle
    pandora::FloatVector   *m_momentumY;                            ///< y component of momentum of mc particle
    pandora::FloatVector   *m_momentumZ;                            ///< z component of momentum of mc particle
    pandora::IntVector     *m_particleId;                           ///< particle id of mc particle
    pandora::FloatVector   *m_outerRadius;                          ///< outer radius of mcparticle
    pandora::FloatVector   *m_innerRadius;                          ///< inner radius of mcparticle

    pandora::FloatVector   *m_caloHitEnergy;                        ///< outer radius of mcparticle
    pandora::FloatVector   *m_trackEnergy;                          ///< inner radius of mcparticle

    typedef std::map<const pandora::MCParticle*, std::pair<float,float> > ConstMCParticleToEnergyMap;

    ConstMCParticleToEnergyMap      m_mcParticleToEnergyMap;        ///< list of mc particles from calohits and tracks
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *MCParticlesMonitoringAlgorithm::Factory::CreateAlgorithm() const
{
    return new MCParticlesMonitoringAlgorithm();
}

} // namespace lc_content

#endif // #ifndef LC_MC_PARTICLES_MONITORING_ALGORITHM_H
