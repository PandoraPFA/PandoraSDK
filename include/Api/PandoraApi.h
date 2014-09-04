/**
 *  @file   PandoraSDK/include/Api/PandoraApi.h
 *
 *  @brief  Header file for the pandora api class.
 *
 *  $Log: $
 */
#ifndef PANDORA_API_H
#define PANDORA_API_H 1

#include "Pandora/Pandora.h"
#include "Pandora/PandoraInputTypes.h"

namespace pandora { class AlgorithmFactory; class AlgorithmToolFactory; }

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  PandoraApi class
 */
class PandoraApi
{
public:
    /**
     *  @brief  Object creation helper class
     * 
     *  @param  PARAMETERS the type of object parameters
     */
    template <typename PARAMETERS>
    class ObjectCreationHelper
    {
    public:
        typedef PARAMETERS Parameters;

        /**
         *  @brief  Create a new object
         * 
         *  @param  pandora the pandora instance to create the new object
         *  @param  parameters the object parameters
         */
        static pandora::StatusCode Create(const pandora::Pandora &pandora, const Parameters &parameters);
    };

    /**
     *  @brief  MCParticleParameters class
     */
    class MCParticleParameters
    {
    public:
        pandora::InputFloat                 m_energy;                   ///< The energy of the MC particle, units GeV
        pandora::InputCartesianVector       m_momentum;                 ///< The momentum of the MC particle, units GeV
        pandora::InputCartesianVector       m_vertex;                   ///< The production vertex of the MC particle, units mm
        pandora::InputCartesianVector       m_endpoint;                 ///< The endpoint of the MC particle, units mm
        pandora::InputInt                   m_particleId;               ///< The MC particle's ID (PDG code)
        pandora::InputMCParticleType        m_mcParticleType;           ///< The type of mc particle, e.g. vertex, 2D-projection, etc.
        pandora::InputAddress               m_pParentAddress;           ///< Address of the parent MC particle in the user framework
    };

    /**
     *  @brief  TrackParameters class
     */
    class TrackParameters
    {
    public:
        pandora::InputFloat                 m_d0;                       ///< The 2D impact parameter wrt (0,0), units mm
        pandora::InputFloat                 m_z0;                       ///< The z coordinate at the 2D distance of closest approach, units mm
        pandora::InputInt                   m_particleId;               ///< The PDG code of the tracked particle
        pandora::InputInt                   m_charge;                   ///< The charge of the tracked particle
        pandora::InputFloat                 m_mass;                     ///< The mass of the tracked particle, units GeV
        pandora::InputCartesianVector       m_momentumAtDca;            ///< Track momentum at the 2D distance of closest approach, units GeV
        pandora::InputTrackState            m_trackStateAtStart;        ///< Track state at the start of the track, units mm and GeV
        pandora::InputTrackState            m_trackStateAtEnd;          ///< Track state at the end of the track, units mm and GeV
        pandora::InputTrackState            m_trackStateAtCalorimeter;  ///< The (sometimes projected) track state at the calorimeter, units mm and GeV
        pandora::InputFloat                 m_timeAtCalorimeter;        ///< The (sometimes projected) time at the calorimeter, units ns
        pandora::InputBool                  m_reachesCalorimeter;       ///< Whether the track actually reaches the calorimeter
        pandora::InputBool                  m_isProjectedToEndCap;      ///< Whether the calorimeter projection is to an endcap
        pandora::InputBool                  m_canFormPfo;               ///< Whether track should form a pfo, if it has an associated cluster
        pandora::InputBool                  m_canFormClusterlessPfo;    ///< Whether track should form a pfo, even if it has no associated cluster
        pandora::InputAddress               m_pParentAddress;           ///< Address of the parent track in the user framework
    };

    /**
     *  @brief  CaloHitBaseParameters class
     */
    class CaloHitBaseParameters
    {
    public:
        pandora::InputCartesianVector       m_positionVector;           ///< Position vector of center of calorimeter cell, units mm
        pandora::InputCartesianVector       m_expectedDirection;        ///< Unit vector in direction of expected hit propagation
        pandora::InputCartesianVector       m_cellNormalVector;         ///< Unit normal to sampling layer, pointing outwards from the origin
        pandora::InputFloat                 m_cellThickness;            ///< Thickness of cell, units mm
        pandora::InputFloat                 m_nCellRadiationLengths;    ///< Absorber material in front of cell, units radiation lengths
        pandora::InputFloat                 m_nCellInteractionLengths;  ///< Absorber material in front of cell, units interaction lengths
        pandora::InputFloat                 m_time;                     ///< Time of (earliest) energy deposition in this cell, units ns
        pandora::InputFloat                 m_inputEnergy;              ///< Corrected energy of calorimeter cell in user framework, units GeV
        pandora::InputFloat                 m_mipEquivalentEnergy;      ///< The calibrated mip equivalent energy, units mip
        pandora::InputFloat                 m_electromagneticEnergy;    ///< The calibrated electromagnetic energy measure, units GeV
        pandora::InputFloat                 m_hadronicEnergy;           ///< The calibrated hadronic energy measure, units GeV
        pandora::InputBool                  m_isDigital;                ///< Whether cell should be treated as digital
        pandora::InputHitType               m_hitType;                  ///< The type of calorimeter hit
        pandora::InputHitRegion             m_hitRegion;                ///< Region of the detector in which the calo hit is located
        pandora::InputUInt                  m_layer;                    ///< The subdetector readout layer number
        pandora::InputBool                  m_isInOuterSamplingLayer;   ///< Whether cell is in one of the outermost detector sampling layers
        pandora::InputAddress               m_pParentAddress;           ///< Address of the parent calo hit in the user framework
    };

    /**
     *  @brief  RectangularCaloHitParameters class
     */
    class RectangularCaloHitParameters : public CaloHitBaseParameters
    {
    public:
        pandora::InputFloat                 m_cellSizeU;                ///< Dimension of cell (up in ENDCAP, along beam in BARREL), units mm
        pandora::InputFloat                 m_cellSizeV;                ///< Dimension of cell (perpendicular to u and thickness), units mm
    };

    /**
     *  @brief  PointingCaloHitParameters class
     */
    class PointingCaloHitParameters : public CaloHitBaseParameters
    {
    public:
        pandora::InputFloat                 m_cellSizeEta;              ///< Dimension of cell, as measured by change in pseudo rapidity, eta
        pandora::InputFloat                 m_cellSizePhi;              ///< Dimension of cell, as measured by change in azimuthal angle, phi
    };

    /**
     *  @brief  Geometry class
     */
    class Geometry
    {
    public:
        /**
         *  @brief  LayerParameters class
         */
        class LayerParameters
        {
        public:
            pandora::InputFloat             m_closestDistanceToIp;      ///< Closest distance of the layer from the interaction point, units mm
            pandora::InputFloat             m_nRadiationLengths;        ///< Absorber material in front of layer, units radiation lengths
            pandora::InputFloat             m_nInteractionLengths;      ///< Absorber material in front of layer, units interaction lengths
        };

        typedef std::vector<LayerParameters> LayerParametersList;

        /**
         *  @brief  SubDetectorParameters class
         */
        class SubDetectorParameters
        {
        public:
            pandora::InputString            m_subDetectorName;          ///< The sub detector name, must uniquely specify a single sub detector
            pandora::InputSubDetectorType   m_subDetectorType;          ///< The sub detector type, e.g. ECAL_BARREL, HCAL_ENDCAP, TPC, etc.
            pandora::InputFloat             m_innerRCoordinate;         ///< Inner cylindrical polar r coordinate, origin interaction point, units mm
            pandora::InputFloat             m_innerZCoordinate;         ///< Inner cylindrical polar z coordinate, origin interaction point, units mm
            pandora::InputFloat             m_innerPhiCoordinate;       ///< Inner cylindrical polar phi coordinate (angle wrt cartesian x axis)
            pandora::InputUInt              m_innerSymmetryOrder;       ///< Order of symmetry of the innermost edge of subdetector
            pandora::InputFloat             m_outerRCoordinate;         ///< Outer cylindrical polar r coordinate, origin interaction point, units mm
            pandora::InputFloat             m_outerZCoordinate;         ///< Outer cylindrical polar z coordinate, origin interaction point, units mm
            pandora::InputFloat             m_outerPhiCoordinate;       ///< Outer cylindrical polar phi coordinate (angle wrt cartesian x axis)
            pandora::InputUInt              m_outerSymmetryOrder;       ///< Order of symmetry of the outermost edge of subdetector
            pandora::InputBool              m_isMirroredInZ;            ///< Whether to construct a second subdetector, via reflection in z=0 plane
            pandora::InputUInt              m_nLayers;                  ///< The number of layers in the detector section
            LayerParametersList             m_layerParametersList;      ///< The list of layer parameters for the detector section
        };

        /**
         *  @brief  BoxGapParameters class
         */
        class BoxGapParameters
        {
        public:
            pandora::InputCartesianVector   m_vertex;                   ///< Cartesian coordinates of a gap vertex, units mm
            pandora::InputCartesianVector   m_side1;                    ///< Cartesian vector describing first side meeting vertex, units mm
            pandora::InputCartesianVector   m_side2;                    ///< Cartesian vector describing second side meeting vertex, units mm
            pandora::InputCartesianVector   m_side3;                    ///< Cartesian vector describing third side meeting vertex, units mm
        };

        /**
         *  @brief  ConcentricGapParameters class
         */
        class ConcentricGapParameters
        {
        public:
            pandora::InputFloat             m_minZCoordinate;           ///< Min cylindrical polar z coordinate, origin interaction point, units mm
            pandora::InputFloat             m_maxZCoordinate;           ///< Max cylindrical polar z coordinate, origin interaction point, units mm
            pandora::InputFloat             m_innerRCoordinate;         ///< Inner cylindrical polar r coordinate, origin interaction point, units mm
            pandora::InputFloat             m_innerPhiCoordinate;       ///< Inner cylindrical polar phi coordinate (angle wrt cartesian x axis)
            pandora::InputUInt              m_innerSymmetryOrder;       ///< Order of symmetry of the innermost edge of gap
            pandora::InputFloat             m_outerRCoordinate;         ///< Outer cylindrical polar r coordinate, origin interaction point, units mm
            pandora::InputFloat             m_outerPhiCoordinate;       ///< Outer cylindrical polar phi coordinate (angle wrt cartesian x axis)
            pandora::InputUInt              m_outerSymmetryOrder;       ///< Order of symmetry of the outermost edge of gap
        };

        typedef ObjectCreationHelper<SubDetectorParameters> SubDetector;
        typedef ObjectCreationHelper<BoxGapParameters> BoxGap;
        typedef ObjectCreationHelper<ConcentricGapParameters> ConcentricGap;
    };

    typedef ObjectCreationHelper<MCParticleParameters> MCParticle;
    typedef ObjectCreationHelper<TrackParameters> Track;
    typedef ObjectCreationHelper<RectangularCaloHitParameters> CaloHit;
    typedef ObjectCreationHelper<RectangularCaloHitParameters> RectangularCaloHit;
    typedef ObjectCreationHelper<PointingCaloHitParameters> PointingCaloHit;

    /**
     *  @brief  Process an event
     * 
     *  @param  pandora the pandora instance to process event
     */
    static pandora::StatusCode ProcessEvent(const pandora::Pandora &pandora);

    /**
     *  @brief  Read pandora settings
     * 
     *  @param  pandora the pandora instance to run the algorithms initialize
     *  @param  xmlFileName the name of the xml file containing the settings
     */
    static pandora::StatusCode ReadSettings(const pandora::Pandora &pandora, const std::string &xmlFileName);

    /**
     *  @brief  Register an algorithm factory with pandora
     * 
     *  @param  pandora the pandora instance to register the algorithm factory with
     *  @param  algorithmType the type of algorithm that the factory will create
     *  @param  pAlgorithmFactory the address of an algorithm factory instance
     */
    static pandora::StatusCode RegisterAlgorithmFactory(const pandora::Pandora &pandora, const std::string &algorithmType,
        pandora::AlgorithmFactory *const pAlgorithmFactory);

    /**
     *  @brief  Register an algorithm tool factory with pandora
     * 
     *  @param  pandora the pandora instance to register the algorithm tool factory with
     *  @param  algorithmTool Type the type of algorithm tool that the factory will create
     *  @param  pAlgorithmToolFactory the address of an algorithm tool factory instance
     */
    static pandora::StatusCode RegisterAlgorithmToolFactory(const pandora::Pandora &pandora, const std::string &algorithmToolType,
        pandora::AlgorithmToolFactory *const pAlgorithmToolFactory);

    /**
     *  @brief  Set parent-daughter mc particle relationship
     * 
     *  @param  pandora the pandora instance to register the relationship with
     *  @param  pParentAddress address of parent mc particle in the user framework
     *  @param  pDaughterAddress address of daughter mc particle in the user framework
     */
    static pandora::StatusCode SetMCParentDaughterRelationship(const pandora::Pandora &pandora, const void *pParentAddress,
        const void *pDaughterAddress);

    /**
     *  @brief  Set parent-daughter track relationship
     * 
     *  @param  pandora the pandora instance to register the relationship with
     *  @param  pParentAddress address of parent track in the user framework
     *  @param  pDaughterAddress address of daughter track in the user framework
     */
    static pandora::StatusCode SetTrackParentDaughterRelationship(const pandora::Pandora &pandora, const void *pParentAddress,
        const void *pDaughterAddress);

    /**
     *  @brief  Set sibling track relationship
     * 
     *  @param  pandora the pandora instance to register the relationship with
     *  @param  pFirstSiblingAddress address of first sibling track in the user framework
     *  @param  pSecondSiblingAddress address of second sibling track in the user framework
     */
    static pandora::StatusCode SetTrackSiblingRelationship(const pandora::Pandora &pandora, const void *pFirstSiblingAddress,
        const void *pSecondSiblingAddress);

    /**
     *  @brief  Set calo hit to mc particle relationship
     * 
     *  @param  pandora the pandora instance to register the relationship with
     *  @param  pCaloHitParentAddress address of calo hit in the user framework
     *  @param  pMCParticleParentAddress address of mc particle in the user framework
     *  @param  mcParticleWeight weighting to assign to the mc particle
     */
    static pandora::StatusCode SetCaloHitToMCParticleRelationship(const pandora::Pandora &pandora, const void *pCaloHitParentAddress,
        const void *pMCParticleParentAddress, const float mcParticleWeight = 1);

    /**
     *  @brief  Set track to mc particle relationship
     * 
     *  @param  pandora the pandora instance to register the relationship with
     *  @param  pTrackParentAddress address of track in the user framework
     *  @param  pMCParticleParentAddress address of mc particle in the user framework
     *  @param  mcParticleWeight weighting to assign to the mc particle
     */
    static pandora::StatusCode SetTrackToMCParticleRelationship(const pandora::Pandora &pandora, const void *pTrackParentAddress,
        const void *pMCParticleParentAddress, const float mcParticleWeight = 1);

    /**
     *  @brief  Get the current pfo list
     * 
     *  @param  pandora the pandora instance to get the objects from
     *  @param  pPfoList to receive the address of the particle flow objects
     */
    static pandora::StatusCode GetCurrentPfoList(const pandora::Pandora &pandora, const pandora::PfoList *&pPfoList);

    /**
     *  @brief  Get a named pfo list
     * 
     *  @param  pandora the pandora instance to get the objects from
     *  @param  pfoListName the name of the pfo list
     *  @param  pPfoList to receive the address of the pfo list
     */
    static pandora::StatusCode GetPfoList(const pandora::Pandora &pandora, const std::string &pfoListName, const pandora::PfoList *&pPfoList);

    /**
     *  @brief  Set the granularity level to be associated with a specified hit type
     * 
     *  @param  pandora the pandora instance to register the hit type to granularity relationship
     *  @param  hitType the specified hit type
     *  @param  granularity the specified granularity
     */
    static pandora::StatusCode SetHitTypeGranularity(const pandora::Pandora &pandora, const pandora::HitType hitType,
        const pandora::Granularity granularity);

    /**
     *  @brief  Set the bfield plugin used by pandora
     * 
     *  @param  pandora the pandora instance to register the bfield calculator with
     *  @param  pBFieldPlugin address of the bfield plugin (will pass ownership to pandora)
     */
    static pandora::StatusCode SetBFieldPlugin(const pandora::Pandora &pandora, pandora::BFieldPlugin *pBFieldPlugin);

    /**
     *  @brief  Set the pseudo layer plugin used by pandora
     * 
     *  @param  pandora the pandora instance to register the pseudo layer calculator with
     *  @param  pPseudoLayerPlugin address of the pseudo layer plugin (will pass ownership to pandora)
     */
    static pandora::StatusCode SetPseudoLayerPlugin(const pandora::Pandora &pandora, pandora::PseudoLayerPlugin *pPseudoLayerPlugin);

    /**
     *  @brief  Set the shower profile plugin used by pandora
     * 
     *  @param  pandora the pandora instance to register the shower profile calculator with
     *  @param  pPseudoLayerPlugin address of the pseudo layer plugin (will pass ownership to pandora)
     */
    static pandora::StatusCode SetShowerProfilePlugin(const pandora::Pandora &pandora, pandora::ShowerProfilePlugin *pShowerProfilePlugin);

    /**
     *  @brief  Register an energy correction plugin
     * 
     *  @param  pandora the pandora instance with which to register the energy correction plugin
     *  @param  name the name/label associated with the energy correction plugin
     *  @param  energyCorrectionType the energy correction type
     *  @param  pEnergyCorrectionPlugin address of the energy correction plugin (will pass ownership to pandora)
     */
    static pandora::StatusCode RegisterEnergyCorrectionPlugin(const pandora::Pandora &pandora, const std::string &name,
        const pandora::EnergyCorrectionType energyCorrectionType, pandora::EnergyCorrectionPlugin *pEnergyCorrectionPlugin);

    /**
     *  @brief  Register a particle id plugin
     * 
     *  @param  pandora the pandora instance with which to register the particle id plugin
     *  @param  functionName the name/label associated with the particle id plugin
     *  @param  pParticleIdPlugin address of the particle id plugin (will pass ownership to pandora)
     */
    static pandora::StatusCode RegisterParticleIdPlugin(const pandora::Pandora &pandora, const std::string &name,
        pandora::ParticleIdPlugin *pParticleIdPlugin);

    /**
     *  @brief  Reset pandora to process another event
     * 
     *  @param  pandora the pandora instance to reset
     */
    static pandora::StatusCode Reset(const pandora::Pandora &pandora);
};

#endif // #ifndef PANDORA_API_H
