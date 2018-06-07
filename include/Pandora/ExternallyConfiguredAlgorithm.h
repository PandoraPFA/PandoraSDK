/**
 *  @file   PandoraSDK/include/Pandora/ExternallyConfiguredAlgorithm.h
 *
 *  @brief  Header file for the externally configured algorithm class.
 *
 *  $Log: $
 */
#ifndef PANDORA_EXTERNALLY_CONFIGURED_ALGORITHM_H
#define PANDORA_EXTERNALLY_CONFIGURED_ALGORITHM_H 1

#include "Pandora/Algorithm.h"

#include <map>
#include <unordered_map>

namespace pandora
{

/**
 *  @brief  External parameters class
 */
class ExternalParameters
{
public:
    /**
     *  @brief  Default constructor
     */
    ExternalParameters();

    /**
     *  @brief  Destructor
     */
    virtual ~ExternalParameters();

private:
    /**
     *  @brief  Register an attempt to access the external parameters during algorithm configuration
     */
    void RegisterParameterAccessAttempt();

    unsigned int m_nParameterAccessAttempts;            ///< The number of attempts made to access the external parameters

    friend class ExternallyConfiguredAlgorithm;
};

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  Externally configured algorithm class. Provides ability for external specification of algorithm-defined parameter blocks.
 *          A single call to GetExternalParameters per algorithm type, per Pandora instance is enforced to prevent misuse.
 *          In the case of multiple instances of a given algorithm type, must revert to standard xml-based configuration.
 */
class ExternallyConfiguredAlgorithm : public Algorithm
{
public:
    /**
     *  @brief  Destructor
     */
    virtual ~ExternallyConfiguredAlgorithm();

    /**
     *  @brief  Set the external parameters associated with an algorithm instance of a specific type, created by the given Pandora instance
     *
     *  @param  pandora the pandora instance
     *  @param  algorithmType the algorithm type
     *  @param  pExternalParameters the address of the external parameters instance
     */
    static StatusCode SetExternalParameters(const Pandora &pandora, const std::string &algorithmType, ExternalParameters *const pExternalParameters);

protected:
    /**
     *  @brief  Whether external parameters are present
     *
     *  @return boolean
     */
    bool ExternalParametersPresent() const;

    /**
     *  @brief  Get the external parameters associated with algorithm instances created by a given Pandora instance.
     *          A single call to this function per algorithm type, per Pandora instance is enforced to prevent misuse.
     *
     *  @return the address of the external parameters
     */
    ExternalParameters *GetExternalParameters() const;

private:
    /**
     *  @brief  External parameters map class
     */
    class ExternalParametersMap
    {
    public:
        /**
         *  @brief  Destructor
         */
        ~ExternalParametersMap();

        /**
         *  @brief  Whether external parameters are present
         *
         *  @param  pandora the pandora instance
         *  @param  algorithmType the algorithm type
         *
         *  @return boolean
         */
        bool ExternalParametersPresent(const pandora::Pandora &pandora, const std::string &algorithmType) const;

        /**
         *  @brief  Get the external parameters associated with algorithm instances created by a given Pandora instance.
         *          A single call to this function per algorithm type, per Pandora instance is enforced to prevent misuse.
         *
         *  @param  pandora the pandora instance
         *  @param  algorithmType the algorithm type
         *
         *  @return the address of the external parameters
         */
        ExternalParameters *GetExternalParameters(const pandora::Pandora &pandora, const std::string &algorithmType) const;

        /**
         *  @brief  Set the external parameters associated with an algorithm instance of a specific type, created by the given Pandora instance
         *
         *  @param  pandora the pandora instance
         *  @param  algorithmType the algorithm type
         *  @param  pExternalParameters the address of the external parameters instance
         */
        pandora::StatusCode SetExternalParameters(const pandora::Pandora &pandora, const std::string &algorithmType, ExternalParameters *const pExternalParameters);

        /**
         *  @brief  Remove the external parameters associated with an algorithm instance of a specific type, created by the given Pandora instance
         *
         *  @param  pandora the pandora instance
         *  @param  algorithmType the algorithm type
         */
        void RemoveExternalParameters(const pandora::Pandora &pandora, const std::string &algorithmType);

    private:
        typedef std::map<const std::string, ExternalParameters*> AlgTypeToParametersMap;
        typedef std::unordered_map<const Pandora*, AlgTypeToParametersMap> TheMap;

        TheMap  m_theMap;   ///< The underlying external parameters map
    };

    static ExternalParametersMap m_externalParametersMap;   ///< The external parameters map
};

} // namespace pandora

#endif // #ifndef PANDORA_EXTERNALLY_CONFIGURED_ALGORITHM_H
