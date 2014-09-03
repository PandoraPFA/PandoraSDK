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
inline bool StringToType(const std::string &s, void *&t)
{
    uintptr_t address;
    std::istringstream iss(s);
    iss >> std::hex >> address;
    t = reinterpret_cast<void*>(address);
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
inline std::string TypeToString(void *const &t)
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

typedef std::set<CaloHit *> CaloHitList;
typedef std::set<Cluster *> ClusterList;
typedef std::set<DetectorGap *> DetectorGapList;
typedef std::set<MCParticle *> MCParticleList;
typedef std::set<ParticleFlowObject *> ParticleFlowObjectList;
typedef std::set<ParticleFlowObject *> PfoList;
typedef std::set<Track *> TrackList;
typedef std::set<Vertex *> VertexList;

typedef std::vector<CaloHit *> CaloHitVector;
typedef std::vector<Cluster *> ClusterVector;
typedef std::vector<DetectorGap *> DetectorGapVector;
typedef std::vector<MCParticle *> MCParticleVector;
typedef std::vector<ParticleFlowObject *> ParticleFlowObjectVector;
typedef std::vector<ParticleFlowObject *> PfoVector;
typedef std::vector<Track *> TrackVector;
typedef std::vector<Vertex *> VertexVector;

typedef std::vector<AlgorithmTool *> AlgorithmToolList;

typedef std::set<std::string> StringSet;
typedef std::vector<std::string> StringVector;
typedef std::vector<int> IntVector;
typedef std::vector<float> FloatVector;
typedef std::vector<CartesianVector> CartesianPointList;

typedef const void * Uid;
typedef std::map<Uid, MCParticle *> UidToMCParticleMap;

typedef std::map<MCParticle *, float> MCParticleWeightMap;
typedef std::map<Uid, MCParticleWeightMap> UidToMCParticleWeightMap;

typedef std::map<Track *, Cluster *> TrackToClusterMap;

typedef std::map<std::string, SubDetector*> SubDetectorMap;

} // namespace pandora

#endif // #ifndef PANDORA_INTERNAL_H
