/**
 *  @file   PandoraSDK/include/Pandora/PandoraInternal.h
 * 
 *  @brief  Header file defining relevant internal typedefs, sort and string conversion functions
 * 
 *  $Log: $
 */
#ifndef PANDORA_INTERNAL_H
#define PANDORA_INTERNAL_H 1

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <iomanip>
#include <list>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace pandora
{

class Algorithm;
class AlgorithmTool;
class BFieldPlugin;
class BoxGap;
class CaloHit;
class CartesianVector;
class Cluster;
class ConcentricGap;
class DetectorGap;
class EnergyCorrectionPlugin;
class ExternalParameters;
class Helix;
class Histogram;
class LArTPC;
class LArTransformationPlugin;
class LineGap;
class MCParticle;
class OrderedCaloHitList;
class ParticleFlowObject;
class ParticleIdPlugin;
class PandoraSettings;
class PseudoLayerPlugin;
class ShowerProfilePlugin;
class SubDetector;
class Track;
class TrackState;
class TwoDHistogram;
class Vertex;

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

template <class T>
inline std::string TypeToStringPrecision(const T &t, const unsigned int precision = 12)
{
    const std::streamsize ss(std::cout.precision());
    std::ostringstream oss;

    if ((oss << std::setprecision(precision) << t << std::setprecision(ss)).fail())
        throw;

    return oss.str();
}

template <>
inline std::string TypeToStringPrecision(const void *const &t, const unsigned int precision)
{
    const uintptr_t address(reinterpret_cast<uintptr_t>(t));
    return TypeToStringPrecision(address, precision);
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
//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  Wrapper around std::list
 */
template <typename T>
class MyList
{
public:
    typedef typename std::list<T> TheList;
    typedef typename TheList::const_iterator const_iterator;
    typedef typename TheList::const_iterator iterator;
    typedef typename TheList::value_type value_type;

    /**
     *  @brief  Default constructor
     */
    MyList();

    /**
     *  @brief  Copy constructor
     * 
     *  @param  rhs
     */
    MyList(const MyList &rhs);

    /**
     *  @brief  Constructor
     * 
     *  @param  n
     *  @param  val
     */
    MyList(size_t n, const value_type &val = value_type());

    /**
     *  @brief  Constructor
     * 
     *  @param  first
     *  @param  last
     */
    template <class InputIterator>
    MyList(InputIterator first, InputIterator last);

    /**
     *  @brief  Destructor
     */
    ~MyList();

    /**
     *  @brief  operator=
     * 
     *  @param  rhs
     */
    void operator= (const MyList &rhs);

    /**
     *  @brief  begin
     */
    const_iterator begin() const;

    /**
     *  @brief  end
     */
    const_iterator end() const;

    /**
     *  @brief  push_back
     * 
     *  @param  val
     */
    void push_back(const value_type &val);

    /**
     *  @brief  erase
     * 
     *  @param  position
     */
    iterator erase(const_iterator position);

    /**
     *  @brief  insert
     * 
     *  @param  position
     *  @param  first
     *  @param  last
     */
    template <class InputIterator>
    void insert(const_iterator position, InputIterator first, InputIterator last);

    /**
     *  @brief  sort
     * 
     *  @param  comp
     */
    template <class Compare>
    void sort (Compare comp);

    /**
     *  @brief  size
     * 
     *  @return size
     */
    unsigned int size() const;

    /**
     *  @brief  empty
     * 
     *  @return boolean
     */
    bool empty() const;

    /**
     *  @brief  clear
     */
    void clear();

private:
    TheList     m_theList;      ///< The list
};

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
inline MyList<T>::MyList()
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
inline MyList<T>::MyList(const MyList &rhs) :
    m_theList(rhs.m_theList)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
inline MyList<T>::MyList(size_t n, const value_type &val) :
    m_theList(n, val)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
template <class InputIterator>
inline MyList<T>::MyList(InputIterator first, InputIterator last) :
    m_theList(first, last)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
inline MyList<T>::~MyList()
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
inline void MyList<T>::operator= (const MyList &rhs)
{
    m_theList = rhs.m_theList;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
inline typename MyList<T>::const_iterator MyList<T>::begin() const
{
    return m_theList.begin();
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
inline typename MyList<T>::const_iterator MyList<T>::end() const
{
    return m_theList.end();
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
inline void MyList<T>::push_back(const value_type &val)
{
    if (m_theList.end() != std::find(m_theList.begin(), m_theList.end(), val))
    {
        std::cout << "push_back duplicate, in file:     " << __FILE__ << " line#: " << __LINE__ << std::endl;
        throw;
    }

    m_theList.push_back(val);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
inline typename MyList<T>::iterator MyList<T>::erase(const_iterator position)
{
    return m_theList.erase(position);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
template <class InputIterator>
inline void MyList<T>::insert(const_iterator position, InputIterator first, InputIterator last)
{
    for (InputIterator iter = first; iter != last; ++iter)
    {
        if (m_theList.end() != std::find(m_theList.begin(), m_theList.end(), *iter))
        {
            std::cout << "insert duplicate, in file:     " << __FILE__ << " line#: " << __LINE__ << std::endl;
            throw;
        }
    }

    m_theList.insert(position, first, last);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
template <class Compare>
inline void MyList<T>::sort(Compare comp)
{
    m_theList.sort(comp);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
inline unsigned int MyList<T>::size() const
{
    return m_theList.size();
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
inline bool MyList<T>::empty() const
{
    return m_theList.empty();
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
inline void MyList<T>::clear()
{
    m_theList.clear();
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

#define MANAGED_CONTAINER std::list

typedef MANAGED_CONTAINER<const CaloHit *> CaloHitList;
typedef MANAGED_CONTAINER<const Cluster *> ClusterList;
typedef MANAGED_CONTAINER<const DetectorGap *> DetectorGapList;
typedef MANAGED_CONTAINER<const LArTPC *> LArTPCList;
typedef MANAGED_CONTAINER<const MCParticle *> MCParticleList;
typedef MANAGED_CONTAINER<const ParticleFlowObject *> ParticleFlowObjectList;
typedef MANAGED_CONTAINER<const ParticleFlowObject *> PfoList;
typedef MANAGED_CONTAINER<const SubDetector *> SubDetectorList;
typedef MANAGED_CONTAINER<const Track *> TrackList;
typedef MANAGED_CONTAINER<const Vertex *> VertexList;

typedef std::vector<const CaloHit *> CaloHitVector;
typedef std::vector<const Cluster *> ClusterVector;
typedef std::vector<const DetectorGap *> DetectorGapVector;
typedef std::vector<const LArTPC *> LArTPCVector;
typedef std::vector<const MCParticle *> MCParticleVector;
typedef std::vector<const ParticleFlowObject *> ParticleFlowObjectVector;
typedef std::vector<const ParticleFlowObject *> PfoVector;
typedef std::vector<const SubDetector *> SubDetectorVector;
typedef std::vector<const Track *> TrackVector;
typedef std::vector<const Vertex *> VertexVector;

typedef std::unordered_set<const CaloHit *> CaloHitSet;
typedef std::unordered_set<const Cluster *> ClusterSet;
typedef std::unordered_set<const DetectorGap *> DetectorGapSet;
typedef std::unordered_set<const LArTPC *> LArTPCSet;
typedef std::unordered_set<const MCParticle *> MCParticleSet;
typedef std::unordered_set<const ParticleFlowObject *> ParticleFlowObjectSet;
typedef std::unordered_set<const ParticleFlowObject *> PfoSet;
typedef std::unordered_set<const SubDetector *> SubDetectorSet;
typedef std::unordered_set<const Track *> TrackSet;
typedef std::unordered_set<const Vertex *> VertexSet;

typedef std::vector<int> IntVector;
typedef std::vector<float> FloatVector;
typedef std::vector<std::string> StringVector;
typedef std::vector<CartesianVector> CartesianPointVector;
typedef std::vector<TrackState> TrackStateVector;
typedef std::vector<AlgorithmTool *> AlgorithmToolVector;

typedef const void * Uid;
typedef std::unordered_map<Uid, const MCParticle *> UidToMCParticleMap;
typedef std::unordered_map<const MCParticle *, float> MCParticleWeightMap;
typedef std::unordered_map<Uid, MCParticleWeightMap> UidToMCParticleWeightMap;
typedef std::unordered_map<const Cluster *, const Track * > ClusterToTrackMap;
typedef std::unordered_map<const Track *, const Cluster * > TrackToClusterMap;

typedef std::set<std::string> StringSet;
typedef std::map<std::string, float> PropertiesMap;
typedef std::map<std::string, const SubDetector *> SubDetectorMap;
typedef std::map<unsigned int, const LArTPC *> LArTPCMap;

} // namespace pandora

#endif // #ifndef PANDORA_INTERNAL_H
