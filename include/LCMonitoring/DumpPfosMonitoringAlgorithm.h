/**
 *  @file   LCContent/include/LCMonitoring/DumpPfosMonitoringAlgorithm.h
 * 
 *  @brief  Header file for the dump pfos monitoring algorithm class
 * 
 *  $Log: $
 */
#ifndef LC_DUMP_PFOS_MONITORING_ALGORITHM_H
#define LC_DUMP_PFOS_MONITORING_ALGORITHM_H 1

#include "Pandora/Algorithm.h"

namespace lc_content
{

/**
 *  @brief  the track error types enum
 */
enum TrackErrorTypes
{
    TRACK_STATUS_OK,
    TRACK_STATUS_UNKNOWN,
    TRACK_STATUS_SPLIT_TRACK,
    TRACK_STATUS_MISSED_CONVERSION,
    TRACK_STATUS_MISSED_KSHORT
};

//------------------------------------------------------------------------------------------------------------------------------------------

#define FORMATTED_OUTPUT_PFO_HEADER(N1, E2)                                                     \
    std::cout  <<                                                                               \
    std::right << std::setw(widthInt)      <<    N1        <<                                   \
    std::right << std::setw(widthFloat)    <<    E2

#define FORMATTED_OUTPUT_PFO_HEADER_TITLE()                                                     \
    std::cout  <<                                                                               \
    std::right << std::setw(widthInt)      <<    "Pfo"     <<                                   \
    std::right << std::setw(widthFloat)    <<    "EPfo"

#define FORMATTED_PFO_PADDING(TITLE1, TITLE2)                                                   \
    std::cout  <<                                                                               \
    std::right << std::setw(widthInt)      <<    TITLE1    <<                                   \
    std::right << std::setw(widthFloat)    <<    TITLE2

#define FORMATTED_OUTPUT_CONFUSION(E1, E2, E3, E4, E5, E6, E7, E8, E9)                          \
    std::cout  <<                                                                               \
      std::right << std::setw(16) << " Generated as   "    <<                                   \
      std::right << std::setw(widthFloat)  <<    "track"   <<                                   \
      std::right << std::setw(widthFloat)  <<    "gamma"   <<                                   \
      std::right << std::setw(widthFloat)  <<    "hadron"  <<  std::endl <<                     \
      std::right << std::setw(16) << "Reco as track : "    <<                                   \
      std::right << std::setw(widthFloat)  <<    E1        <<                                   \
      std::right << std::setw(widthFloat)  <<    E2        <<                                   \
      std::right << std::setw(widthFloat)  <<    E3        <<  std::endl <<                     \
      std::right << std::setw(16) <<  "Reco as gamma : "   <<                                   \
      std::right << std::setw(widthFloat)  <<    E4        <<                                   \
      std::right << std::setw(widthFloat)  <<    E5        <<                                   \
      std::right << std::setw(widthFloat)  <<    E6        << std::endl <<                      \
      std::right << std::setw(16) <<  "Reco as hadron: "   <<                                   \
      std::right << std::setw(widthFloat)  <<    E7        <<                                   \
      std::right << std::setw(widthFloat)  <<    E8        <<                                   \
      std::right << std::setw(widthFloat)  <<    E9        << std::endl

#define FORMATTED_OUTPUT_TRACK_TITLE()                                                          \
    std::cout  <<                                                                               \
    std::right << std::setw(widthInt)      <<    "Track"   <<                                   \
    std::right << std::setw(widthFlag)     <<    "("       <<                                   \
    std::right << std::setw(widthInt)      <<    "mc"      <<                                   \
    std::left  << std::setw(widthFlag)     <<    ")"       <<                                   \
    std::right << std::setw(widthFlag)     <<    "C"       <<                                   \
    std::right << std::setw(widthFlag)     <<    "R"       <<                                   \
    std::right << std::setw(widthFloat)    <<    "Mom"     <<                                   \
    std::right << std::setw(widthFlag)     <<    "("       <<                                   \
    std::right << std::setw(widthFloat)    <<    "mc"      <<                                   \
    std::left  << std::setw(widthFlag)     <<    ")"       <<                                   \
    std::right << std::setw(widthFloat)    <<    "Eclust"  <<                                   \
    std::right << std::setw(widthFloat)    <<    "tclust"  <<                                   \
    std::right << std::setw(widthFloat)    <<    "chi"     <<                                   \
    std::right << std::setw(widthFlag)     <<    "L"       <<                                   \
    std::right << std::setw(widthSmallFloat)    <<    "fC" <<                                   \
    std::right << std::setw(widthSmallFloat)    <<    "fP" <<                                   \
    std::right << std::setw(widthSmallFloat)    <<    "fN" <<                                   \
    std::endl

#define FORMATTED_OUTPUT_TRACK(N1, N2, FLAG1, FLAG2, E1, E2, E3, E4, E5, FLAG3, E6, E7, E8)     \
    std::cout  <<                                                                               \
    std::right << std::setw(widthInt)      <<    N1        <<                                   \
    std::right << std::setw(widthFlag)     <<    "("       <<                                   \
    std::right << std::setw(widthInt)      <<    N2        <<                                   \
    std::left  << std::setw(widthFlag)     <<    ")"       <<                                   \
    std::right << std::setw(widthFlag)     <<    FLAG1     <<                                   \
    std::right << std::setw(widthFlag)     <<    FLAG2     <<                                   \
    std::right << std::setw(widthFloat)    <<    E1        <<                                   \
    std::right << std::setw(widthFlag)     <<    "("       <<                                   \
    std::right << std::setw(widthFloat)    <<    E2        <<                                   \
    std::left  << std::setw(widthFlag)     <<    ")"       <<                                   \
    std::right << std::setw(widthFloat)    <<    E3        <<                                   \
    std::right << std::setw(widthFloat)    <<    E4        <<                                   \
    std::right << std::setw(widthFloat)    <<    E5        <<                                   \
    std::right << std::setw(widthFlag)     <<    FLAG3     <<                                   \
    std::right << std::setw(widthSmallFloat) <<  E6        <<                                   \
    std::right << std::setw(widthSmallFloat) <<  E7        <<                                   \
    std::right << std::setw(widthSmallFloat) <<  E8

#define FORMATTED_OUTPUT_NEUTRAL(E1, E2, E3, E4, E5, N1, N2, E6, E7)                            \
    std::cout  <<                                                                               \
    std::right << std::setw(widthFloat)      <<  E1        <<                                   \
    std::right << std::setw(widthFloat)      <<  E2        <<                                   \
    std::right << std::setw(widthInt4)       <<  "     "   <<                                   \
    std::right << std::setw(widthSmallFloat) <<  E3        <<                                   \
    std::right << std::setw(widthSmallFloat) <<  E4        <<                                   \
    std::right << std::setw(widthSmallFloat) <<  E5        <<                                   \
    std::right << std::setw(widthInt4)     <<    N1        <<                                   \
    std::left  << std::setw(widthFlag)     <<    "-"       <<                                   \
    std::left  << std::setw(widthInt4)     <<    N2        <<                                   \
    std::right << std::setw(widthFloat)    <<    E6        <<                                   \
    std::right << std::setw(widthFloat)    <<    E7

#define FORMATTED_OUTPUT_NEUTRAL_TITLE()                                                        \
    std::cout  <<                                                                               \
    std::right << std::setw(widthFloat)    <<    "Eclust"  <<                                   \
    std::right << std::setw(widthFloat)    <<    "tclust"  <<                                   \
    std::right << std::setw(widthInt4)     <<    "     "   <<                                   \
    std::right << std::setw(widthSmallFloat) <<  "fC"      <<                                   \
    std::right << std::setw(widthSmallFloat) <<  "fP"      <<                                   \
    std::right << std::setw(widthSmallFloat) <<  "fN"      <<                                   \
    std::left  << std::setw(widthInt4+widthFlag+widthInt4) <<  " Layers " <<                    \
    std::right << std::setw(widthFloat)      <<   "sStart" <<                                   \
    std::right << std::setw(widthFloat)      <<   "sDisc"  <<                                   \
    std::endl


#define FORMATTED_OUTPUT_GOODENERGY(E1, E2, E3, E4)                                             \
    std::cout  <<                                                                               \
    std::right << std::setw(22) <<  "Unconfused energy : "    <<                                \
    std::right << std::setw(widthBigFloat)      <<  E1        <<                                \
    std::right << std::setw(widthBigFloat)      <<  E2        <<                                \
    std::right << std::setw(widthBigFloat)      <<  E3        <<                                \
    std::right << std::setw(4)  <<  " : "       <<                                              \
    std::right << std::setw(widthBigFloat)      <<  E4        <<                                \
    std::endl;


#define FORMATTED_OUTPUT_BADENERGY(E1, E2, E3, E4)                                              \
    std::cout  <<                                                                               \
    std::right << std::setw(22) <<  "Confused   energy : " <<                                   \
    std::right << std::setw(widthBigFloat)      <<  E1        <<                                \
    std::right << std::setw(widthBigFloat)      <<  E2        <<                                \
    std::right << std::setw(widthBigFloat)      <<  E3        <<                                \
    std::right << std::setw(4)  <<  " : "       <<                                              \
    std::right << std::setw(widthBigFloat)      <<  E4        <<                                \
    std::endl;


//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief DumpPfosMonitoringAlgorithm class
 */
class DumpPfosMonitoringAlgorithm : public pandora::Algorithm
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

    /**
     *  @brief Default constructor
     */
    DumpPfosMonitoringAlgorithm();

    /**
     *  @brief  Destructor
     */
    ~DumpPfosMonitoringAlgorithm();

private:
    pandora::StatusCode Run();
    pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);

    /**
     *  @brief  DumpChargedPfo
     * 
     *  @param  pPfo
     */
    void DumpChargedPfo(const pandora::ParticleFlowObject *const pPfo);

    /**
     *  @brief  DumpNeutralPfo
     * 
     *  @param  pPfo
     */
    void DumpNeutralPfo(const pandora::ParticleFlowObject *const pPfo);

    /**
     *  @brief  DumpNeutralOrPhotonPfo
     * 
     *  @param  pPfo
     *  @param  isPhotonPfo
     */
    void DumpNeutralOrPhotonPfo(const pandora::ParticleFlowObject *const pPfo, bool isPhotonPfo);

    /**
     *  @brief  DumpPhotonPfo
     * 
     *  @param  pPfo
     */
    void DumpPhotonPfo(const pandora::ParticleFlowObject *const pPfo);

    /**
     *  @brief  ClusterEnergyFractions
     * 
     *  @param  pCluster
     *  @param  fCharged
     *  @param  fPhoton
     *  @param  fneutral
     *  @param  pBestMatchedMcPfo
     */
    void ClusterEnergyFractions(const pandora::Cluster *const pCluster, float &fCharged, float &fPhoton, float &fneutral,
        const pandora::MCParticle *&pBestMatchedMcPfo) const;

    /**
     *  @brief  ClusterTime
     * 
     *  @param  pCluster
     * 
     *  @return the cluster time
     */
    float ClusterTime(const pandora::Cluster *const pCluster) const;

    typedef std::set<const pandora::MCParticle*> MCParticleList;
    typedef std::vector<pandora::ParticleFlowObject*> ParticleFlowObjectVector;

    typedef std::map<const pandora::MCParticle*, float> MCParticleToFloatMap;
    typedef std::map<const pandora::MCParticle*, const pandora::Track*> MCParticleToTrackMap;
    typedef std::map<const pandora::Track*, TrackErrorTypes> TrackToErrorTypeMap;

    MCParticleList          m_trackMcPfoTargets;                        ///< 
    MCParticleToTrackMap    m_mcParticleToTrackMap;                     ///< 
    TrackToErrorTypeMap     m_trackToErrorTypeMap;                      ///< 

    float                   m_trackRecoAsTrackEnergy;                   ///< 
    float                   m_trackRecoAsPhotonEnergy;                  ///< 
    float                   m_trackRecoAsNeutralEnergy;                 ///< 
    float                   m_photonRecoAsTrackEnergy;                  ///< 
    float                   m_photonRecoAsPhotonEnergy;                 ///< 
    float                   m_photonRecoAsNeutralEnergy;                ///< 
    float                   m_neutralRecoAsTrackEnergy;                 ///< 
    float                   m_neutralRecoAsPhotonEnergy;                ///< 
    float                   m_neutralRecoAsNeutralEnergy;               ///< 

    int                     m_count;                                    ///<
    float                   m_photonOrNeutralRecoAsTrackEnergySum;      ///<
    float                   m_photonOrNeutralRecoAsTrackEnergySum2;     ///<
    float                   m_trackRecoAsPhotonOrNeutralEnergySum;      ///<
    float                   m_trackRecoAsPhotonOrNeutralEnergySum2;     ///<
    float                   m_confusionCorrelation;                     ///<
                    
    float                   m_minPfoEnergyToDisplay;                    ///< 
    float                   m_minAbsChiToDisplay;                       ///< 
    float                   m_minConfusionEnergyToDisplay;              ///< 
    float                   m_minFragmentEnergyToDisplay;               ///< 
    float                   m_totalPfoEnergyDisplayLessThan;            ///< 
    float                   m_totalPfoEnergyDisplayGreaterThan;         ///< 
    float                   m_fragmentEnergyToDisplay;                  ///< 
    float                   m_photonIdEnergyToDisplay;                  ///< 
                    
    float                   m_trackRecoAsTrackEnergySum;                ///< 
    float                   m_trackRecoAsPhotonEnergySum;               ///< 
    float                   m_trackRecoAsNeutralEnergySum;              ///< 
    float                   m_photonRecoAsTrackEnergySum;               ///< 
    float                   m_photonRecoAsPhotonEnergySum;              ///< 
    float                   m_photonRecoAsNeutralEnergySum;             ///< 
    float                   m_neutralRecoAsTrackEnergySum;              ///< 
    float                   m_neutralRecoAsPhotonEnergySum;             ///< 
    float                   m_neutralRecoAsNeutralEnergySum;            ///< 

    float                   m_goodTrackEnergy;                          ///< 
    float                   m_goodPhotonEnergy;                         ///< 
    float                   m_goodIdedPhotonEnergy;                     ///< 
    float                   m_goodNeutralEnergy;                        ///< 
    float                   m_goodIdedNeutralEnergy;                    ///< 
    float                   m_badTrackEnergy;                           ///< 
    float                   m_badPhotonEnergy;                          ///< 
    float                   m_badNeutralEnergy;                         ///< 
    float                   m_goodTrackEnergySum;                       ///< 
    float                   m_goodPhotonEnergySum;                      ///< 
    float                   m_goodIdedPhotonEnergySum;                  ///< 
    float                   m_goodNeutralEnergySum;                     ///< 
    float                   m_goodIdedNeutralEnergySum;                 ///< 
    float                   m_badTrackEnergySum;                        ///< 
    float                   m_badPhotonEnergySum;                       ///< 
    float                   m_badNeutralEnergySum;                      ///< 
    float                   m_goodFractionCut;                          ///< 

    bool                    m_firstChargedPfoToPrint;                   ///< 
    bool                    m_firstNeutralPfoToPrint;                   ///< 
    bool                    m_firstPhotonPfoToPrint;                    ///< 
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline void DumpPfosMonitoringAlgorithm::DumpPhotonPfo(const pandora::ParticleFlowObject *pPfo)
{
    return this->DumpNeutralOrPhotonPfo(pPfo, true);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void DumpPfosMonitoringAlgorithm::DumpNeutralPfo(const pandora::ParticleFlowObject *pPfo)
{
    return this->DumpNeutralOrPhotonPfo(pPfo, false);
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *DumpPfosMonitoringAlgorithm::Factory::CreateAlgorithm() const
{
    return new DumpPfosMonitoringAlgorithm();
}

} // namespace lc_content

#endif // #ifndef LC_DUMP_PFOS_MONITORING_ALGORITHM_H
