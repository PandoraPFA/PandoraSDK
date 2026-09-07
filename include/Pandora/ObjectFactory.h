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

#include "Persistency/FieldMap.h"

namespace pandora
{

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  ObjectFactory class responsible for extended pandora object creation
 *
 *  Subclasses override Read(Parameters &, const FieldMap &) and
 *  Write(const Object *, FieldMap &) to persist experiment-specific fields.
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
     *  @brief  Create new parameters instance on the heap (caller takes ownership)
     *
     *  @return the address of the new parameters instance
     */
    virtual Parameters *NewParameters() const = 0;

    /**
     *  @brief  Read any additional (derived class only) object parameters from the supplied FieldMap.
     *
     *  @param  parameters the parameters to pass in constructor
     *  @param  fields the field map from which to read the parameters
     */
    virtual StatusCode Read(Parameters &parameters, const FieldMap &fields) const;

    /**
     *  @brief  Persist any additional (derived class only) object parameters into the supplied FieldMap.
     *
     *  @param  pObject the address of the object to persist
     *  @param  fields the field map into which to write the parameters
     */
    virtual StatusCode Write(const Object *const pObject, FieldMap &fields) const;

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
inline StatusCode ObjectFactory<PARAMETERS, OBJECT>::Read(Parameters & /*parameters*/, const FieldMap & /*fields*/) const
{
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename PARAMETERS, typename OBJECT>
inline StatusCode ObjectFactory<PARAMETERS, OBJECT>::Write(const OBJECT *const /*pObject*/, FieldMap & /*fields*/) const
{
    return STATUS_CODE_SUCCESS;
}

} // namespace pandora

#endif // #ifndef PANDORA_OBJECT_FACTORY_H
