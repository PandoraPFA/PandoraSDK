/**
 *  @file   PandoraSDK/include/Pandora/PandoraInternal.h
 * 
 *  @brief  Header file defining relevant internal typedefs, sort and string conversion functions
 * 
 *  $Log: $
 */
#ifndef PANDORA_INTERNAL_H
#define PANDORA_INTERNAL_H 1

#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#if __cplusplus > 199711L
    #include <unordered_set>
    #define MANAGED_CONTAINER unordered_set
#else
    #define MANAGED_CONTAINER set
#endif

#include <stdint.h>

namespace pandora
{

class Algorithm;
class AlgorithmTool;
class BFieldPlugin;
class CaloHit;
class CartesianVector;
class Cluster;
class DetectorGap;
class EnergyCorrectionPlugin;
class Helix;
class Histogram;
class MCParticle;
class OrderedCaloHitList;
class ParticleFlowObject;
class ParticleIdPlugin;
class PseudoLayerPlugin;
class ShowerProfilePlugin;
class SubDetector;
class Track;
class TrackState;
class TwoDHistogram;
class Vertex;

// Macro allowing use of pandora monitoring to be quickly included/excluded via pre-processor flag; only works within algorithms
#ifdef MONITORING
    #define PANDORA_MONITORING_API(command)                                                                 \
    if (this->GetPandora().GetSettings()->IsMonitoringEnabled())                                            \
    {                                                                                                       \
        PandoraMonitoringApi::command;                                                                      \
    }
#else
    #define PANDORA_MONITORING_API(command)
#endif

//------------------------------------------------------------------------------------------------------------------------------------------

// Macros for registering lists of algorithms, energy corrections functions, particle id functions or settings functions
#define PANDORA_REGISTER_ALGORITHM(a, b)                                                                    \
{                                                                                                           \
    const pandora::StatusCode statusCode(PandoraApi::RegisterAlgorithmFactory(pandora, a, new b));          \
    if (pandora::STATUS_CODE_SUCCESS != statusCode)                                                         \
        return statusCode;                                                                                  \
}

#define PANDORA_REGISTER_ALGORITHM_TOOL(a, b)                                                               \
{                                                                                                           \
    const pandora::StatusCode statusCode(PandoraApi::RegisterAlgorithmToolFactory(pandora, a, new b));      \
    if (pandora::STATUS_CODE_SUCCESS != statusCode)                                                         \
        return statusCode;                                                                                  \
}

#define PANDORA_REGISTER_ENERGY_CORRECTION(a, b, c)                                                         \
{                                                                                                           \
    const pandora::StatusCode statusCode(PandoraApi::RegisterEnergyCorrectionPlugin(pandora, a, b, new c)); \
    if (pandora::STATUS_CODE_SUCCESS != statusCode)                                                         \
        return statusCode;                                                                                  \
}

#define PANDORA_REGISTER_PARTICLE_ID(a, b)                                                                  \
{                                                                                                           \
    const pandora::StatusCode statusCode(PandoraApi::RegisterParticleIdPlugin(pandora, a, new b));          \
    if (pandora::STATUS_CODE_SUCCESS != statusCode)                                                         \
        return statusCode;                                                                                  \
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <class T>
inline bool StringToType(const std::string &s, T &t)
{
    std::istringstream iss(s);
    return !(iss >> t).fail();
}

template <>
inline bool StringToType(const std::string &s, const void *&t)
{
    uintptr_t address;
    std::istringstream iss(s);
    iss >> std::hex >> address;
    t = reinterpret_cast<const void*>(address);
    return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

template <class T>
inline std::string TypeToString(const T &t)
{
    std::ostringstream oss;

    if ((oss << t).fail())
        throw;

    return oss.str();
}

template <>
inline std::string TypeToString(const void *const &t)
{
    const uintptr_t address(reinterpret_cast<uintptr_t>(t));
    return TypeToString(address);
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  Enable ordering of pointers based on properties of target objects
 */
template <typename T>
class PointerLessThan
{
public:
    bool operator()(const T *lhs, const T *rhs) const;
};

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
inline bool PointerLessThan<T>::operator()(const T *lhs, const T *rhs) const
{
    return (*lhs < *rhs);
}

//------------------------------------------------------------------------------------------------------------------------------------------

typedef std::MANAGED_CONTAINER<const CaloHit *> CaloHitList;
typedef std::MANAGED_CONTAINER<const Cluster *> ClusterList;
typedef std::MANAGED_CONTAINER<const DetectorGap *> DetectorGapList;
typedef std::MANAGED_CONTAINER<const MCParticle *> MCParticleList;
typedef std::MANAGED_CONTAINER<const ParticleFlowObject *> ParticleFlowObjectList;
typedef std::MANAGED_CONTAINER<const ParticleFlowObject *> PfoList;
typedef std::MANAGED_CONTAINER<const Track *> TrackList;
typedef std::MANAGED_CONTAINER<const Vertex *> VertexList;

typedef std::vector<const CaloHit *> CaloHitVector;
typedef std::vector<const Cluster *> ClusterVector;
typedef std::vector<const DetectorGap *> DetectorGapVector;
typedef std::vector<const MCParticle *> MCParticleVector;
typedef std::vector<const ParticleFlowObject *> ParticleFlowObjectVector;
typedef std::vector<const ParticleFlowObject *> PfoVector;
typedef std::vector<const Track *> TrackVector;
typedef std::vector<const Vertex *> VertexVector;

typedef std::vector<AlgorithmTool *> AlgorithmToolList;

typedef std::set<std::string> StringSet;
typedef std::vector<std::string> StringVector;
typedef std::vector<int> IntVector;
typedef std::vector<float> FloatVector;
typedef std::vector<CartesianVector> CartesianPointList;
typedef std::vector<TrackState> TrackStateList;

typedef const void * Uid;
typedef std::map<Uid, const MCParticle *> UidToMCParticleMap;

typedef std::map<const MCParticle *, float> MCParticleWeightMap;
typedef std::map<Uid, MCParticleWeightMap> UidToMCParticleWeightMap;

typedef std::map<const Cluster *, const Track * > ClusterToTrackMap;
typedef std::map<const Track *, const Cluster * > TrackToClusterMap;

typedef std::map<std::string, const SubDetector *> SubDetectorMap;

} // namespace pandora

#endif // #ifndef PANDORA_INTERNAL_H
