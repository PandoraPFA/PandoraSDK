/**
 *  @file   PandoraSDK/include/Pandora/ObjectFactory.h
 * 
 *  @brief  Header file for the object factory class.
 * 
 *  $Log: $
 */
#ifndef PANDORA_OBJECT_FACTORY_H
#define PANDORA_OBJECT_FACTORY_H 1

#include "Pandora/StatusCodes.h"

namespace pandora
{

class FileReader;
class FileWriter;

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  ObjectFactory class responsible for extended pandora object creation
 */
template <typename PARAMETERS, typename OBJECT>
class ObjectFactory
{
public:
    typedef PARAMETERS Parameters;
    typedef OBJECT Object;

    /**
     *  @brief  Default constructor
     */
    ObjectFactory();

    /**
     *  @brief  Destructor
     */
    virtual ~ObjectFactory();

    /**
     *  @brief  Read any additional (derived class only) object parameters from file using the specified file reader
     *
     *  @param  parameters the parameters to pass in constructor
     *  @param  fileReader the file reader, used to extract any additional parameters from file
     */
    virtual StatusCode Read(Parameters &parameters, FileReader &fileReader) const;

    /**
     *  @brief  Persist any additional (derived class only) object parameters using the specified file writer
     *
     *  @param  pObject the address of the object to persist
     *  @param  fileWriter the file writer
     */
    virtual StatusCode Write(const Object *const pObject, FileWriter &fileWriter) const;

protected:
    /**
     *  @brief  Create an object with the given parameters
     *
     *  @param  parameters the parameters to pass in constructor
     *  @param  pObject to receive the address of the object created
     */
    virtual StatusCode Create(const Parameters &parameters, const Object *&pObject) const = 0;

    friend class CaloHitManager;
    friend class TrackManager;
    friend class MCManager;
    friend class ClusterManager;
    friend class VertexManager;
    friend class ParticleFlowObjectManager;
    friend class GeometryManager;
};

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename PARAMETERS, typename OBJECT>
inline ObjectFactory<PARAMETERS, OBJECT>::ObjectFactory()
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename PARAMETERS, typename OBJECT>
inline ObjectFactory<PARAMETERS, OBJECT>::~ObjectFactory()
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename PARAMETERS, typename OBJECT>
inline StatusCode ObjectFactory<PARAMETERS, OBJECT>::Read(Parameters &/*parameters*/, FileReader &/*fileReader*/) const
{
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename PARAMETERS, typename OBJECT>
inline StatusCode ObjectFactory<PARAMETERS, OBJECT>::Write(const Object *const /*pObject*/, FileWriter &/*fileWriter*/) const
{
    return STATUS_CODE_SUCCESS;
}

} // namespace pandora

#endif // #ifndef PANDORA_OBJECT_FACTORY_H
