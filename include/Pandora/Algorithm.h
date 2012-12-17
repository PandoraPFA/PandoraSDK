/**
 *  @file   PandoraPFANew/Framework/include/Pandora/Algorithm.h
 * 
 *  @brief  Header file for the algorithm class.
 * 
 *  $Log: $
 */
#ifndef ALGORITHM_H
#define ALGORITHM_H 1

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
     *  @brief  Destructor
     */
    virtual ~Algorithm();

    /**
     *  @brief  Get the address of the pandora object that will run the algorithm
     * 
     *  @return The address of the pandora object that will run the algorithm
     */
    const Pandora *GetPandora() const;

    /**
     *  @brief  Get the pandora content api impl
     * 
     *  @return Address of the pandora content api impl
     */
    const PandoraContentApiImpl *GetPandoraContentApiImpl() const;

    /**
     *  @brief  Get the algorithm type
     * 
     *  @return The algorithm type name
     */
    const std::string GetAlgorithmType() const;

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
     *  @brief  Register the pandora object that will run the algorithm
     *
     *  @param  pPandora address of the pandora object that will run the algorithm
     */
    StatusCode RegisterPandora(Pandora *pPandora);

    Pandora            *m_pPandora;             ///< The pandora object that will run the algorithm
    std::string         m_algorithmType;        ///< The type of algorithm

    friend class AlgorithmManager;
    friend class FileReader;
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

inline Algorithm::~Algorithm()
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const Pandora *Algorithm::GetPandora() const
{
    if (NULL == m_pPandora)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return m_pPandora;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const PandoraContentApiImpl *Algorithm::GetPandoraContentApiImpl() const
{
    if (NULL == m_pPandora)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return m_pPandora->GetPandoraContentApiImpl();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const std::string Algorithm::GetAlgorithmType() const
{
    return m_algorithmType;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode Algorithm::Initialize()
{
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode Algorithm::RegisterPandora(Pandora *pPandora)
{
    if (NULL == pPandora)
        return STATUS_CODE_FAILURE;

    m_pPandora = pPandora;

    return STATUS_CODE_SUCCESS;
}

} // namespace pandora

#endif // #ifndef ALGORITHM_H
