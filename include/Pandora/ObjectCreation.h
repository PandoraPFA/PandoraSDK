/**
 *  @file   PandoraSDK/include/Pandora/ObjectCreation.h
 * 
 *  @brief  Header file for pandora object creation classes
 * 
 *  $Log: $
 */
#ifndef PANDORA_OBJECT_CREATION_H
#define PANDORA_OBJECT_CREATION_H 1

#include "Pandora/PandoraInternal.h"
#include "Pandora/PandoraInputTypes.h"
#include "Pandora/StatusCodes.h"

namespace pandora
{
    class Algorithm; class Pandora;
    template <typename Parameters, typename Object> class ObjectFactory;
    template <typename Parameters, typename Object> class PandoraObjectFactory;
}

//------------------------------------------------------------------------------------------------------------------------------------------

namespace object_creation
{

/**
 *  @brief  Object creation helper class
 * 
 *  @param  PARAMETERS the type of object parameters
 *  @param  METADATA the type of object metadata
 *  @param  OBJECT the type of object
 */
template <typename PARAMETERS, typename METADATA, typename OBJECT>
class ObjectCreationHelper
{
public:
    typedef PARAMETERS Parameters;
    typedef METADATA Metadata;
    typedef OBJECT Object;

    /**
     *  @brief  Create a new object from a user factory
     *
     *  @param  pandora the pandora instance to create the new object
     *  @param  parameters the object parameters
     *  @param  factory the factory that performs the object allocation
     */
    static pandora::StatusCode Create(const pandora::Pandora &pandora, const Parameters &parameters,
        const pandora::ObjectFactory<Parameters, Object> &factory = pandora::PandoraObjectFactory<Parameters, Object>());

    /**
     *  @brief  Create a new object from a user factory, receiving the address of the object created
     *
     *  @param  algorithm the algorithm calling this function
     *  @param  parameters the object parameters
     *  @param  pObject to receive the address of the object created
     *  @param  factory the factory that performs the object allocation
     */
    static pandora::StatusCode Create(const pandora::Algorithm &algorithm, const Parameters &parameters,
        const Object *&pObject, const pandora::ObjectFactory<Parameters, Object> &factory = pandora::PandoraObjectFactory<Parameters, Object>());

    /**
     *  @brief  Alter the metadata information stored in an object
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  pObject address of the object to modify
     *  @param  metaData the metadata (only populated metadata fields will be propagated to the object)
     */
    static pandora::StatusCode AlterMetadata(const pandora::Algorithm &algorithm, const Object *const pObject, const Metadata &metadata);
};

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  ObjectMetadata class
 */
class ObjectMetadata
{
public:
    /**
     *  @param  virtual destructor to allow polymorphic behaviour of api metadata classes
     */
    virtual ~ObjectMetadata();
};

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  ObjectParameters class
 */
class ObjectParameters
{
public:
    /**
     *  @param  virtual destructor to allow polymorphic behaviour of api parameters classes
     */
    virtual ~ObjectParameters();
};

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  CaloHitMetadata class
 */
class CaloHitMetadata : public ObjectMetadata
{
public:
    pandora::InputFloat                 m_x0;                       ///< For LArTPC usage, the x-coordinate shift associated with a drift time t0 shift, units mm
    pandora::InputBool                  m_isIsolated;               ///< The calo hit isolation flag
    pandora::InputBool                  m_isPossibleMip;            ///< The calo hit minimum ionising particle flag
    pandora::PropertiesMap              m_propertiesToAdd;          ///< The mapping from calo hit property names to new values
    pandora::StringVector               m_propertiesToRemove;       ///< The vector of calo hit property names to remove
};

/**
 *  @brief  CaloHitParameters class
 */
class CaloHitParameters : public ObjectParameters
{
public:
    pandora::InputCartesianVector       m_positionVector;           ///< Position vector of center of calorimeter cell, units mm
    pandora::InputCartesianVector       m_expectedDirection;        ///< Unit vector in direction of expected hit propagation
    pandora::InputCartesianVector       m_cellNormalVector;         ///< Unit normal to sampling layer, pointing outwards from the origin
    pandora::InputCellGeometry          m_cellGeometry;             ///< The cell geometry type, pointing or rectangular
    pandora::InputFloat                 m_cellSize0;                ///< Cell size 0 [pointing: eta, rectangular: up in ENDCAP, along beam in BARREL, units mm]
    pandora::InputFloat                 m_cellSize1;                ///< Cell size 1 [pointing: phi, rectangular: perpendicular to size 0 and thickness, units mm]
    pandora::InputFloat                 m_cellThickness;            ///< Cell thickness, units mm
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

typedef ObjectCreationHelper<CaloHitParameters, CaloHitMetadata, pandora::CaloHit> CaloHit;

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  MCParticleParameters class
 */
class MCParticleParameters : public ObjectParameters
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

typedef ObjectCreationHelper<MCParticleParameters, ObjectMetadata, pandora::MCParticle> MCParticle;

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  TrackParameters class
 */
class TrackParameters : public ObjectParameters
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

typedef ObjectCreationHelper<TrackParameters, ObjectMetadata, pandora::Track> Track;

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  Geometry class
 */
class Geometry
{
public:
    /**
     *  @brief  LayerParameters class
     */
    class LayerParameters : public ObjectParameters
    {
    public:
        pandora::InputFloat             m_closestDistanceToIp;      ///< Closest distance of the layer from the interaction point, units mm
        pandora::InputFloat             m_nRadiationLengths;        ///< Absorber material in front of layer, units radiation lengths
        pandora::InputFloat             m_nInteractionLengths;      ///< Absorber material in front of layer, units interaction lengths
    };

    typedef std::vector<LayerParameters> LayerParametersVector;

    /**
     *  @brief  SubDetectorParameters class
     */
    class SubDetectorParameters : public ObjectParameters
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
        LayerParametersVector           m_layerParametersVector;    ///< The vector of layer parameters for the detector section
    };

    typedef ObjectCreationHelper<SubDetectorParameters, ObjectMetadata, pandora::SubDetector> SubDetector;

    /**
     *  @brief  LArTPCParameters class
     */
    class LArTPCParameters : public ObjectParameters
    {
    public:
        pandora::InputUInt              m_larTPCVolumeId;           ///< The lar tpc volume id, must uniquely specify a single lar tpc
        pandora::InputFloat             m_centerX;                  ///< The center in x, units mm
        pandora::InputFloat             m_centerY;                  ///< The center in y, units mm
        pandora::InputFloat             m_centerZ;                  ///< The center in z, units mm
        pandora::InputFloat             m_widthX;                   ///< The width in x, units mm
        pandora::InputFloat             m_widthY;                   ///< The width in y, units mm
        pandora::InputFloat             m_widthZ;                   ///< The width in z, units mm
        pandora::InputFloat             m_wirePitchU;               ///< The u wire pitch, units mm
        pandora::InputFloat             m_wirePitchV;               ///< The v wire pitch, units mm
        pandora::InputFloat             m_wirePitchW;               ///< The w wire pitch, units mm
        pandora::InputFloat             m_wireAngleU;               ///< The u wire angle to the vertical, units radians
        pandora::InputFloat             m_wireAngleV;               ///< The v wire angle to the vertical, units radians
        pandora::InputFloat             m_wireAngleW;               ///< The w wire angle to the vertical, units radians
        pandora::InputFloat             m_sigmaUVW;                 ///< The u, v, w resolution, units mm
        pandora::InputBool              m_isDriftInPositiveX;       ///< Whether the electron drift is in the positive x direction
    };

    typedef ObjectCreationHelper<LArTPCParameters, ObjectMetadata, pandora::LArTPC> LArTPC;

    /**
     *  @brief  LineGapParameters class
     */
    class LineGapParameters : public ObjectParameters
    {
    public:
        pandora::InputLineGapType       m_lineGapType;              ///< The type of line gap, e.g. TPC wire-type gap (u, v, w), or drift-type gap
        pandora::InputFloat             m_lineStartX;               ///< The line start x coordinate, units mm
        pandora::InputFloat             m_lineEndX;                 ///< The line end x coordinate, units mm
        pandora::InputFloat             m_lineStartZ;               ///< The line start z coordinate, units mm
        pandora::InputFloat             m_lineEndZ;                 ///< The line end z coordinate, units mm
    };

    typedef ObjectCreationHelper<LineGapParameters, ObjectMetadata, pandora::LineGap> LineGap;

    /**
     *  @brief  BoxGapParameters class
     */
    class BoxGapParameters : public ObjectParameters
    {
    public:
        pandora::InputCartesianVector   m_vertex;                   ///< Cartesian coordinates of a gap vertex, units mm
        pandora::InputCartesianVector   m_side1;                    ///< Cartesian vector describing first side meeting vertex, units mm
        pandora::InputCartesianVector   m_side2;                    ///< Cartesian vector describing second side meeting vertex, units mm
        pandora::InputCartesianVector   m_side3;                    ///< Cartesian vector describing third side meeting vertex, units mm
    };

    typedef ObjectCreationHelper<BoxGapParameters, ObjectMetadata, pandora::BoxGap> BoxGap;

    /**
     *  @brief  ConcentricGapParameters class
     */
    class ConcentricGapParameters : public ObjectParameters
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

    typedef ObjectCreationHelper<ConcentricGapParameters, ObjectMetadata, pandora::ConcentricGap> ConcentricGap;
};

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  ClusterMetadata class
 */
class ClusterMetadata : public ObjectMetadata
{
public:
    pandora::InputInt                   m_particleId;               ///< The cluster id (PDG code)
};

/**
 *  @brief  ClusterParameters class. To build a cluster must provide at least one hit (which may be isolated) or a track address.
 */
class ClusterParameters : public ObjectParameters
{
public:
    pandora::CaloHitList                m_caloHitList;              ///< The calo hit(s) to include
    pandora::CaloHitList                m_isolatedCaloHitList;      ///< The isolated calo hit(s) to include
    pandora::InputTrackAddress          m_pTrack;                   ///< The address of the track seeding the cluster
};

typedef ObjectCreationHelper<ClusterParameters, ClusterMetadata, pandora::Cluster> Cluster;

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  ParticleFlowObjectMetadata class
 */
class ParticleFlowObjectMetadata : public ObjectMetadata
{
public:
    pandora::InputInt                   m_particleId;               ///< The particle flow object id (PDG code)
    pandora::InputInt                   m_charge;                   ///< The particle flow object charge
    pandora::InputFloat                 m_mass;                     ///< The particle flow object mass
    pandora::InputFloat                 m_energy;                   ///< The particle flow object energy
    pandora::InputCartesianVector       m_momentum;                 ///< The particle flow object momentum
    pandora::PropertiesMap              m_propertiesToAdd;          ///< The mapping from pfo property names to new values
    pandora::StringVector               m_propertiesToRemove;       ///< The vector of pfo property names to remove
};

/**
 *  @brief  ParticleFlowObjectCreation class
 */
class ParticleFlowObjectCreation : public ParticleFlowObjectMetadata, public ObjectParameters
{
public:
    pandora::ClusterList                m_clusterList;              ///< The clusters in the particle flow object
    pandora::TrackList                  m_trackList;                ///< The tracks in the particle flow object
    pandora::VertexList                 m_vertexList;               ///< The vertices in the particle flow object
};

typedef ObjectCreationHelper<ParticleFlowObjectCreation, ParticleFlowObjectMetadata, pandora::ParticleFlowObject> ParticleFlowObject;

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  VertexMetadata class
 */
class VertexMetadata : public ObjectMetadata
{
public:
    pandora::InputFloat                 m_x0;                       ///< For LArTPC usage, the x-coordinate shift associated with a drift time t0 shift, units mm
    pandora::InputVertexLabel           m_vertexLabel;              ///< The vertex label (interaction, start, end, etc.)
    pandora::InputVertexType            m_vertexType;               ///< The vertex type (3d, view u, v, w, etc.)
};

/**
 *  @brief  Vertex creation class
 */
class VertexParameters : public VertexMetadata, public ObjectParameters
{
public:
    pandora::InputCartesianVector       m_position;                 ///< The vertex position
};

typedef ObjectCreationHelper<VertexParameters, VertexMetadata, pandora::Vertex> Vertex;

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  Type definition helper class
 * 
 *  @param  PARAMETERS the type of object parameters
 *  @param  OBJECT the type of object
 */
template <typename PARAMETERS, typename OBJECT>
class TypedefHelper
{
public:
    typedef PARAMETERS Parameters;
    typedef OBJECT Object;
};

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  CaloHit fragment creation class
 */
class CaloHitFragmentParameters : public ObjectParameters
{
public:
    const pandora::CaloHit             *m_pOriginalCaloHit;         ///< The address of the original calo hit
    pandora::InputFloat                 m_weight;                   ///< The weight to be assigned to the fragment
};

typedef TypedefHelper<CaloHitFragmentParameters, pandora::CaloHit> CaloHitFragment;

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

inline ObjectMetadata::~ObjectMetadata()
{
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

inline ObjectParameters::~ObjectParameters()
{
}

} // namespace object_creation

#endif // #ifndef PANDORA_OBJECT_CREATION_H
