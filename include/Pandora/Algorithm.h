/**
 *  @file   PandoraPFANew/Framework/include/Pandora/Algorithm.h
 * 
 *  @brief  Header file for the algorithm class.
 * 
 *  $Log: $
 */
#ifndef PANDORA_ALGORITHM_H
#define PANDORA_ALGORITHM_H 1

#include "Pandora/Pandora.h"
#include "Pandora/PandoraInternal.h"
#include "Pandora/PandoraInputTypes.h"
#include "Pandora/StatusCodes.h"

namespace pandora
{

class PandoraContentApiImpl;
class TiXmlHandle;

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  Algorithm class
 */
class Algorithm
{
public:
    /**
     *  @brief  Default constructor
     */
    Algorithm();

    /**
     *  @brief  Get the algorithm type
     * 
     *  @return The algorithm type name
     */
    std::string GetAlgorithmType() const;

    /**
     *  @brief  Get the pandora content api impl
     * 
     *  @return Address of the pandora content api impl
     */
    const PandoraContentApiImpl *GetPandoraContentApiImpl() const;

protected:
    /**
     *  @brief  Run the algorithm
     */
    virtual StatusCode Run() = 0;

    /**
     *  @brief  Read the algorithm settings
     * 
     *  @param  xmlHandle the relevant xml handle
     */
    virtual StatusCode ReadSettings(const TiXmlHandle xmlHandle) = 0;

    /**
     *  @brief  Perform any operations that must occur after reading settings, but before running the algorithm
     */
    virtual StatusCode Initialize();

    /**
     *  @brief  Get the address of the pandora object that will run the algorithm
     * 
     *  @return The address of the pandora object that will run the algorithm
     */
    const Pandora *GetPandora() const;

    /**
     *  @brief  Destructor
     */
    virtual ~Algorithm();

    /**
     *  @brief  Register the pandora object that will run the algorithm and the algorithm type
     *
     *  @param  pPandora address of the pandora object that will run the algorithm
     *  @param  algorithmType the algorithm type identifier
     */
    StatusCode RegisterDetails(Pandora *pPandora, const std::string &algorithmType);

    Pandora            *m_pPandora;             ///< The pandora object that will run the algorithm
    std::string         m_algorithmType;        ///< The type of algorithm

    friend class AlgorithmManager;
    friend class PandoraContentApiImpl;
};

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  Factory class for instantiating algorithms
 */
class AlgorithmFactory
{
public:
    /**
     *  @brief  Create an instance of an algorithm
     * 
     *  @return the address of the algorithm instance
     */
    virtual Algorithm *CreateAlgorithm() const = 0;

    /**
     *  @brief  Destructor
     */
    virtual ~AlgorithmFactory();
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline AlgorithmFactory::~AlgorithmFactory()
{
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

inline Algorithm::Algorithm() :
    m_pPandora(NULL)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline std::string Algorithm::GetAlgorithmType() const
{
    return m_algorithmType;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const PandoraContentApiImpl *Algorithm::GetPandoraContentApiImpl() const
{
    if (NULL == m_pPandora)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return m_pPandora->GetPandoraContentApiImpl();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode Algorithm::Initialize()
{
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const Pandora *Algorithm::GetPandora() const
{
    if (NULL == m_pPandora)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return m_pPandora;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline Algorithm::~Algorithm()
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode Algorithm::RegisterDetails(Pandora *pPandora, const std::string &algorithmType)
{
    if ((NULL == pPandora) || (algorithmType.empty()))
        return STATUS_CODE_FAILURE;

    m_pPandora = pPandora;
    m_algorithmType = algorithmType;

    return STATUS_CODE_SUCCESS;
}

} // namespace pandora

#endif // #ifndef PANDORA_ALGORITHM_H
