/**
 *  @file   PandoraPFANew/Framework/include/Pandora/PandoraInternal.h
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
#include <vector>

namespace pandora
{

class BFieldCalculator;
class CaloHit;
class CartesianVector;
class Cluster;
class DetectorGap;
class Helix;
class Histogram;
class MCParticle;
class OrderedCaloHitList;
class ParticleFlowObject;
class PseudoLayerCalculator;
class ShowerProfileCalculator;
class Track;
class TrackState;
class TwoDHistogram;

// Macro allowing use of pandora monitoring to be quickly included/excluded via pre-processor flag
#ifdef MONITORING
    #define PANDORA_MONITORING_API(command)                                                                 \
    if (PandoraSettings::IsMonitoringEnabled())                                                             \
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

#define PANDORA_REGISTER_ENERGY_CORRECTION(a, b, c)                                                         \
{                                                                                                           \
    const pandora::StatusCode statusCode(PandoraApi::RegisterEnergyCorrectionFunction(pandora, a, b, c));   \
    if (pandora::STATUS_CODE_SUCCESS != statusCode)                                                         \
        return statusCode;                                                                                  \
}

#define PANDORA_REGISTER_PARTICLE_ID(a, b)                                                                  \
{                                                                                                           \
    const pandora::StatusCode statusCode(PandoraApi::RegisterParticleIdFunction(pandora, a, b));            \
    if (pandora::STATUS_CODE_SUCCESS != statusCode)                                                         \
        return statusCode;                                                                                  \
}

#define PANDORA_REGISTER_SETTINGS(a, b)                                                                     \
{                                                                                                           \
    const pandora::StatusCode statusCode(PandoraApi::RegisterSettingsFunction(pandora, a, b));              \
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
typedef std::set<Track *> TrackList;
typedef std::set<Cluster *> ClusterList;
typedef std::set<MCParticle *> MCParticleList;
typedef std::vector<TrackState *> TrackStateList;
typedef std::vector<CaloHit *> CaloHitVector;
typedef std::vector<Track *> TrackVector;
typedef std::vector<Cluster *> ClusterVector;
typedef std::vector<MCParticle *> MCParticleVector;
typedef std::vector<ParticleFlowObject *> PfoVector;
typedef std::set<ParticleFlowObject *> PfoList;

typedef unsigned int PseudoLayer;
typedef std::set<std::string> StringSet;
typedef std::vector<std::string> StringVector;
typedef std::vector<int> IntVector;
typedef std::vector<float> FloatVector;
typedef std::vector<CartesianVector> CartesianPointList;

typedef const void * Uid;
typedef std::map<Uid, MCParticle *> UidToMCParticleMap;
typedef std::map<Track *, Cluster *> TrackToClusterMap;

} // namespace pandora

#endif // #ifndef PANDORA_INTERNAL_H
