/**
 *  @file   PandoraSDK/include/Helpers/ClusterFitHelper.h
 * 
 *  @brief  Header file for the cluster fit helper class.
 * 
 *  $Log: $
 */
#ifndef PANDORA_CLUSTER_FIT_HELPER_H
#define PANDORA_CLUSTER_FIT_HELPER_H 1

#include "Pandora/PandoraInputTypes.h"
#include "Pandora/PandoraInternal.h"

namespace pandora
{

/**
 *  @brief  ClusterFitPoint class
 */
class ClusterFitPoint
{
public:
    /**
     *  @brief  Constructor
     * 
     *  @param  pCaloHit address of a calo hit on which to base the cluster fit point
     */
    ClusterFitPoint(const CaloHit *const pCaloHit);

    /**
     *  @brief  Constructor
     * 
     *  @param  position the position vector of the fit point
     *  @param  cellNormalVector the unit normal vector to the cell in which the point was recorded
     *  @param  cellSize the size of the cell in which the point was recorded
     *  @param  energy the energy deposited in the cell in which the point was recorded
     *  @param  pseudoLayer the pseudolayer in which the point was recorded
     */
    ClusterFitPoint(const CartesianVector &position, const CartesianVector &cellNormalVector, const float cellSize,
        const float energy, const unsigned int pseudoLayer);

    /**
     *  @brief  Get the position vector of the fit point
     * 
     *  @return the position vector of the fit point
     */
    const CartesianVector &GetPosition() const;

    /**
     *  @brief  Get the unit normal vector to the cell in which the point was recorded
     * 
     *  @return the unit normal vector to the cell in which the point was recorded
     */
    const CartesianVector &GetCellNormalVector() const;

    /**
     *  @brief  Get the size of the cell in which the point was recorded
     * 
     *  @return the size of the cell in which the point was recorded
     */
    float GetCellSize() const;

    /**
     *  @brief  Get the energy deposited in the cell in which the point was recorded
     * 
     *  @return energy deposited in the cell in which the point was recorded
     */
    float GetEnergy() const;

    /**
     *  @brief  Get the pseudolayer in which the point was recorded
     * 
     *  @return the pseudolayer in which the point was recorded
     */
    unsigned int GetPseudoLayer() const;

private:
    CartesianVector         m_position;              ///< The position vector of the fit point
    CartesianVector         m_cellNormalVector;      ///< The unit normal vector to the cell in which the point was recorded
    float                   m_cellSize;              ///< The size of the cell in which the point was recorded
    float                   m_energy;                ///< The energy deposited in the cell in which the point was recorded
    unsigned int            m_pseudoLayer;           ///< The pseudolayer in which the point was recorded
};

typedef std::vector<ClusterFitPoint> ClusterFitPointList;

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  ClusterFitResult class
 */
class ClusterFitResult
{
public:
    /**
     *  @brief  Default constructor
     */
    ClusterFitResult();

    /**
     *  @brief  Query whether fit was successful
     * 
     *  @return boolean
     */
    bool IsFitSuccessful() const;

    /**
     *  @brief  Get the fit direction
     * 
     *  @return the fit direction
     */
    const CartesianVector &GetDirection() const;

    /**
     *  @brief  Get the fit intercept
     * 
     *  @return the fit intercept
     */
    const CartesianVector &GetIntercept() const;

    /**
     *  @brief  Get the fit ch2
     * 
     *  @return the fit chi2
     */
    float GetChi2() const;

    /**
     *  @brief  Get the fit rms
     * 
     *  @return the fit rms
     */
    float GetRms() const;

    /**
     *  @brief  Get the fit direction cosine w.r.t. the radial direction
     * 
     *  @return the fit direction cosine w.r.t. the radial direction
     */
    float GetRadialDirectionCosine() const;

    /**
     *  @brief  Set the fit success flag
     * 
     *  @param  successFlag the fit success flag
     */
    void SetSuccessFlag(const bool successFlag);

    /**
     *  @brief  Set the fit direction
     * 
     *  @param  direction the fit direction
     */
    void SetDirection(const CartesianVector &direction);

    /**
     *  @brief  Set the fit intercept
     * 
     *  @param  intercept the fit intercept
     */
    void SetIntercept(const CartesianVector &intercept);

    /**
     *  @brief  Set the fit chi2
     * 
     *  @param  chi2 the fit chi2
     */
    void SetChi2(const float chi2);

    /**
     *  @brief  Set the fit rms
     * 
     *  @param  rms the fit rms
     */
    void SetRms(const float rms);

    /**
     *  @brief  Set the fit direction cosine w.r.t. the radial direction
     * 
     *  @param  radialDirectionCosine the fit direction cosine w.r.t. the radial direction
     */
    void SetRadialDirectionCosine(const float radialDirectionCosine);

    /**
     *  @brief  Reset the cluster fit result
     */
    void Reset();

private:
    bool                    m_isFitSuccessful;       ///< Whether the fit was successful
    CartesianVector         m_direction;             ///< The best fit direction
    CartesianVector         m_intercept;             ///< The best fit intercept
    InputFloat              m_chi2;                  ///< The chi2 value for the fit
    InputFloat              m_rms;                   ///< The rms of the fit
    InputFloat              m_dirCosR;               ///< The direction cosine wrt to the radial direction
};

typedef std::vector<ClusterFitResult> ClusterFitResultList;

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  ClusterFitHelper class
 */
class ClusterFitHelper
{
public:
    /**
     *  @brief  Fit points in first n occupied pseudolayers of a cluster
     * 
     *  @param  pCluster the cluster containing the ordered list of calo hits to fit
     *  @param  maxOccupiedLayers the maximum number of occupied pseudo layers to consider
     *  @param  clusterFitResult to receive the cluster fit result
     */
    static StatusCode FitStart(const Cluster *const pCluster, const unsigned int maxOccupiedLayers, ClusterFitResult &clusterFitResult);

    /**
     *  @brief  Fit points in last n occupied pseudolayers of a cluster
     * 
     *  @param  pCluster the cluster containing the ordered list of calo hits to fit
     *  @param  maxOccupiedLayers the maximum number of occupied pseudo layers to consider
     *  @param  clusterFitResult to receive the cluster fit result
     */
    static StatusCode FitEnd(const Cluster *const pCluster, const unsigned int maxOccupiedLayers, ClusterFitResult &clusterFitResult);

    /**
     *  @brief  Fit all points in a cluster
     * 
     *  @param  pCluster the cluster containing the ordered list of calo hits to fit
     *  @param  clusterFitResult to receive the cluster fit result
     */
    static StatusCode FitFullCluster(const Cluster *const pCluster, ClusterFitResult &clusterFitResult);

    /**
     *  @brief  Fit all cluster points within the specified (inclusive) pseudolayer range
     * 
     *  @param  pCluster the cluster containing the ordered list of calo hits to fit
     *  @param  startLayer the start of the pseudolayer range
     *  @param  endLayer the end of the pseudolayer range
     *  @param  clusterFitResult to receive the cluster fit result
     */
    static StatusCode FitLayers(const Cluster *const pCluster, const unsigned int startLayer, const unsigned int endLayer,
        ClusterFitResult &clusterFitResult);

    /**
     *  @brief  Fit all cluster centroids within the specified (inclusive) pseudolayer range
     * 
     *  @param  pCluster the cluster containing the ordered list of calo hits to fit
     *  @param  startLayer the start of the pseudolayer range
     *  @param  endLayer the end of the pseudolayer range
     *  @param  clusterFitResult to receive the cluster fit result
     */
    static StatusCode FitLayerCentroids(const Cluster *const pCluster, const unsigned int startLayer, const unsigned int endLayer,
        ClusterFitResult &clusterFitResult);

    /**
     *  @brief  Perform linear regression of x vs d and y vs d and z vs d (assuming same error on all hits)
     * 
     *  @param  clusterFitPointList list of cluster fit points
     *  @param  clusterFitResult to receive the cluster fit result
     */
    static StatusCode FitPoints(const ClusterFitPointList &clusterFitPointList, ClusterFitResult &clusterFitResult);

private:
    /**
     *  @brief  Perform linear fit to cluster fit points
     * 
     *  @param  clusterFitPointList list of cluster fit points
     *  @param  centralPosition central position of the cluster fit points
     *  @param  centralDirection central direction of normal to cluster fit calorimeter cells
     *  @param  clusterFitResult to receive the cluster fit result
     */
    static StatusCode PerformLinearFit(const ClusterFitPointList &clusterFitPointList, const CartesianVector &centralPosition,
        const CartesianVector &centralDirection, ClusterFitResult &clusterFitResult);
};

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

inline const CartesianVector &ClusterFitPoint::GetPosition() const
{
    return m_position;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const CartesianVector &ClusterFitPoint::GetCellNormalVector() const
{
    return m_cellNormalVector;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float ClusterFitPoint::GetCellSize() const
{
    return m_cellSize;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float ClusterFitPoint::GetEnergy() const
{
    return m_energy;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline unsigned int ClusterFitPoint::GetPseudoLayer() const
{
    return m_pseudoLayer;
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

inline ClusterFitResult::ClusterFitResult() :
    m_isFitSuccessful(false),
    m_direction(0.f, 0.f, 0.f),
    m_intercept(0.f, 0.f, 0.f)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline bool ClusterFitResult::IsFitSuccessful() const
{
    return m_isFitSuccessful;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const CartesianVector &ClusterFitResult::GetDirection() const
{
    if (!m_isFitSuccessful)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return m_direction;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const CartesianVector &ClusterFitResult::GetIntercept() const
{
    if (!m_isFitSuccessful)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return m_intercept;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float ClusterFitResult::GetChi2() const
{
    if (!m_isFitSuccessful)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return m_chi2.Get();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float ClusterFitResult::GetRms() const
{
    if (!m_isFitSuccessful)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return m_rms.Get();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float ClusterFitResult::GetRadialDirectionCosine() const
{
    if (!m_isFitSuccessful)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return m_dirCosR.Get();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void ClusterFitResult::SetSuccessFlag(bool successFlag)
{
    m_isFitSuccessful = successFlag;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void ClusterFitResult::SetDirection(const CartesianVector &direction)
{
    m_direction = direction;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void ClusterFitResult::SetIntercept(const CartesianVector &intercept)
{
    m_intercept = intercept;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void ClusterFitResult::SetChi2(const float chi2)
{
    if (!(m_chi2 = chi2))
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void ClusterFitResult::SetRms(const float rms)
{
    if (!(m_rms = rms))
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void ClusterFitResult::SetRadialDirectionCosine(const float radialDirectionCosine)
{
    if (!(m_dirCosR = radialDirectionCosine))
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void ClusterFitResult::Reset()
{
    m_isFitSuccessful = false;
    m_direction.SetValues(0.f, 0.f, 0.f);
    m_intercept.SetValues(0.f, 0.f, 0.f);
    m_chi2.Reset();
    m_rms.Reset();
    m_dirCosR.Reset();
}

} // namespace pandora

#endif // #ifndef PANDORA_CLUSTER_FIT_HELPER_H
