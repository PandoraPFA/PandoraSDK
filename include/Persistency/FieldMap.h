/**
 *  @file   PandoraSDK/include/Persistency/FieldMap.h
 *
 *  @brief  Header file for the FieldMap class used for self-describing,
 *          forward/backward-compatible component serialisation.
 *
 *  $Log: $
 */
#ifndef PANDORA_FIELD_MAP_H
#define PANDORA_FIELD_MAP_H 1

#include "Pandora/StatusCodes.h"

#include "Objects/CartesianVector.h"
#include "Objects/TrackState.h"

#include <algorithm>
#include <cstring>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>

namespace pandora
{

/**
 *  @brief  The value type recorded against every field, alongside its raw bytes.
 *
 *  Determined automatically at Set<T>() time from the compile-time type T, so callers never specify it explicitly. Ensures the XML writer
 *  renders each field correctly and lets the XML reader parse it correctly.
 */
enum class FieldValueType : uint8_t
{
    UNKNOWN,
    FLOAT,
    DOUBLE,
    INT8,
    INT16,
    INT32,
    INT64,
    UINT8,
    UINT16,
    UINT32,
    UINT64,
    BOOL,
    STRING,
    CARTESIAN_VECTOR,
    TRACK_STATE
};

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  FieldMap
 *
 *  A flat key/value store that holds all serialised fields for a single component, keyed by their string tag names. This decouples the file
 *  format (binary / XML) from the component deserialisation logic.
 *
 *  Design contract
 *  ---------------
 *  - Writers populate a FieldMap by calling Set<T>(tag, value) for every field.
 *  - Readers call Get<T>(tag, value) to retrieve fields by name.
 *  - If a field is absent (old file, new reader), Get returns STATUS_CODE_NOT_FOUND and the caller supplies its own default.
 *  - If a field is present but its stored byte count does not match sizeof(T), Get returns STATUS_CODE_INVALID_PARAMETER.
 *  - Unknown fields encountered in a file (new file, old reader) are stored in the map but never queried.
 *
 *  Supported value types
 *  ---------------------
 *  Any trivially-copyable T is supported by the generic Get/Set pair. Specialisations are provided for std::string, CartesianVector, and
 *  TrackState so that these compound types serialise consistently with the binary format.
 *
 *  Type tracking
 *  -------------
 *  Set<T>() records a FieldValueType alongside the raw bytes, inferred from T at compile time (see GetFieldType()). This is used by
 *  XmlFileWriter to format each field correctly and by XmlFileReader to parse it back.
 *
 *  The recorded type is exact in both width and signedness, so that the byte count implied by the type always equals the byte count
 *  actually stored. XmlFileWriter relies on this: it copies sizeof(recorded type) bytes out of the field, and a type narrower or wider
 *  than the stored data would read past the end of the buffer or silently truncate.
 *
 *  Field order
 *  -----------
 *  GetAllFields() returns fields in the order they were first Set(), so XML output groups fields the way the writing code orders them.
 *
 *  The raw byte accessors (GetRawBytes / SetRawBytes) are used by format-specific readers; component code should always use the typed
 *  accessors.
 */
class FieldMap
{
public:
    /**
     *  @brief  Set a field value of type T, recording its raw bytes and FieldValueType.
     *
     *  @param  tag the field tag name
     *  @param  value the field value
     */
    template <typename T>
    void Set(const std::string &tag, const T &value);

    /**
     *  @brief  Get a field value of type T, reading its raw bytes and checking its FieldValueType.
     *
     *  @param  tag the field tag name
     *  @param  value the field value to populate
     *
     *  @return STATUS_CODE_SUCCESS if the field was found and read successfully, STATUS_CODE_NOT_FOUND if the field was absent, or
     *          STATUS_CODE_INVALID_PARAMETER if the field was present but its stored byte count did not match sizeof(T).
     */
    template <typename T>
    StatusCode Get(const std::string &tag, T &value) const;

    /**
     *  @brief  Get a field value of type T, returning a default if the field is absent.
     *
     *  @param  tag the field tag name
     *  @param  defaultValue the value to return if the field is absent
     *
     *  @return the field value if present, or defaultValue if absent. Throws StatusCodeException if the field is present but its stored byte
     *          count does not match sizeof(T).
     */
    template <typename T>
    T GetOrDefault(const std::string &tag, const T &defaultValue) const;

    /**
     *  @brief  Check whether a field is present in the map.
     *
     *  @param  tag the field tag name
     *
     *  @return true if the field is present, false otherwise
     */
    bool Has(const std::string &tag) const;

    /**
     *  @brief  Store raw bytes directly (used by format readers during deserialisation). The recorded FieldValueType is UNKNOWN, since
     *          the caller is not going through Set<T> and so no compile-time type is available.
     */
    void SetRawBytes(const std::string &tag, std::vector<unsigned char> bytes);

    /**
     *  @brief  Retrieve raw bytes directly (used by format writers during serialisation).
     *
     *  @param  tag the field tag name
     *  @param  bytes the vector to populate with the raw bytes
     *
     *  @return STATUS_CODE_SUCCESS if the field was found and read successfully, STATUS_CODE_NOT_FOUND if the field was absent.
     */
    StatusCode GetRawBytes(const std::string &tag, std::vector<unsigned char> &bytes) const;

    /**
     *  @brief  Return the value type recorded for a tag (UNKNOWN if absent, or if the field was populated via SetRawBytes rather than Set<T>).
     */
    FieldValueType GetFieldType(const std::string &tag) const;

    /**
     *  @brief  Remove a field from the map (used by migration functions to rename fields).
     */
    void Remove(const std::string &tag);

    /**
     *  @brief  Return all (tag, rawBytes) pairs in first-Set order. Used by writers to serialise the populated map to the output stream.
     *
     *  @return a vector of (tag, rawBytes) pairs in the order they were first Set() into the map.
     */
    std::vector<std::pair<std::string, std::vector<unsigned char>>> GetAllFields() const;

private:
    struct Entry
    {
        FieldValueType              m_type;
        std::vector<unsigned char>  m_bytes;
    };

    /**
     *  @brief  Infer the FieldValueType for T at compile time.
     *
     *  @return the FieldValueType corresponding to T, or UNKNOWN if T is not recognised.
     */
    template <typename T>
    static FieldValueType DeduceFieldType();

    /**
     *  @brief  Record the order in which tags are first Set() into the map.
     *
     *  @param  tag the field tag name
     */
    void RecordOrder(const std::string &tag);

    std::vector<std::string>                    m_tagOrder; ///< First-Set order of tags
    std::unordered_map<std::string, Entry>      m_fields;   ///< Tag -> (type, bytes)
};

//------------------------------------------------------------------------------------------------------------------------------------------

namespace field_map_detail
{

/**
 *  @brief  Yields the underlying type of an enum, or T itself for any non-enum T. std::underlying_type is ill-formed for non-enum types, so
 *          it cannot be named directly in a branch that also compiles for integrals.
 */
template <typename T, bool IsEnum = std::is_enum<T>::value>
struct UnderlyingOrSelf
{
    typedef T type;
};

template <typename T>
struct UnderlyingOrSelf<T, true>
{
    typedef typename std::underlying_type<T>::type type;
};

} // namespace field_map_detail


//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
inline FieldValueType FieldMap::DeduceFieldType()
{
    if (std::is_same<T, bool>::value)
        return FieldValueType::BOOL;

    if (std::is_floating_point<T>::value)
        return (sizeof(T) == 8) ? FieldValueType::DOUBLE : FieldValueType::FLOAT;

    // Pointer values (e.g. const void *) are rendered the same way as an unsigned integer of the same width
    if (std::is_pointer<T>::value)
        return (sizeof(T) == 8) ? FieldValueType::UINT64 : FieldValueType::UINT32;

    if (std::is_integral<T>::value || std::is_enum<T>::value)
    {
        // For an enum, signedness is that of the underlying type, not of the enum itself: std::is_signed is false for every enum, which
        // would otherwise render a negative enumerator as a large unsigned value.
        typedef typename field_map_detail::UnderlyingOrSelf<T>::type U;

        if (std::is_signed<U>::value)
        {
            return (sizeof(T) == 1) ? FieldValueType::INT8  :
                   (sizeof(T) == 2) ? FieldValueType::INT16 :
                   (sizeof(T) == 4) ? FieldValueType::INT32 : FieldValueType::INT64;
        }

        return (sizeof(T) == 1) ? FieldValueType::UINT8  :
               (sizeof(T) == 2) ? FieldValueType::UINT16 :
               (sizeof(T) == 4) ? FieldValueType::UINT32 : FieldValueType::UINT64;
    }

    return FieldValueType::UNKNOWN;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void FieldMap::RecordOrder(const std::string &tag)
{
    if (m_fields.find(tag) == m_fields.end())
        m_tagOrder.push_back(tag);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
inline void FieldMap::Set(const std::string &tag, const T &value)
{
    static_assert(std::is_trivially_copyable<T>::value,
        "FieldMap::Set requires trivially-copyable T; provide a specialisation for compound types");

    std::vector<unsigned char> bytes(sizeof(T));
    std::memcpy(bytes.data(), &value, sizeof(T));

    this->RecordOrder(tag);
    m_fields[tag] = Entry{DeduceFieldType<T>(), std::move(bytes)};
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
inline StatusCode FieldMap::Get(const std::string &tag, T &value) const
{
    static_assert(std::is_trivially_copyable<T>::value,
        "FieldMap::Get requires trivially-copyable T; provide a specialisation for compound types");
    auto it = m_fields.find(tag);
    if (it == m_fields.end()) return STATUS_CODE_NOT_FOUND;
    if (it->second.m_bytes.size() != sizeof(T)) return STATUS_CODE_INVALID_PARAMETER;
    std::memcpy(&value, it->second.m_bytes.data(), sizeof(T));
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
inline T FieldMap::GetOrDefault(const std::string &tag, const T &defaultValue) const
{
    // Check presence before attempting construction of T, so that types without a default constructor (e.g. CartesianVector, TrackState)
    // are never default-constructed — we return defaultValue directly.
    auto it = m_fields.find(tag);

    if (it == m_fields.end())
        return defaultValue;

    T value{defaultValue};   // copy-construct from default so compound types are always in a valid state before Get overwrites
    const StatusCode sc = this->Get(tag, value);

    if (STATUS_CODE_SUCCESS != sc)
        throw StatusCodeException(sc);

    return value;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline bool FieldMap::Has(const std::string &tag) const
{
    return m_fields.count(tag) > 0;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void FieldMap::SetRawBytes(const std::string &tag, std::vector<unsigned char> bytes)
{
    this->RecordOrder(tag);
    m_fields[tag] = Entry{FieldValueType::UNKNOWN, std::move(bytes)};
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode FieldMap::GetRawBytes(const std::string &tag, std::vector<unsigned char> &bytes) const
{
    auto it = m_fields.find(tag);
    if (it == m_fields.end()) return STATUS_CODE_NOT_FOUND;
    bytes = it->second.m_bytes;
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline FieldValueType FieldMap::GetFieldType(const std::string &tag) const
{
    auto it = m_fields.find(tag);
    if (it == m_fields.end()) return FieldValueType::UNKNOWN;
    return it->second.m_type;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void FieldMap::Remove(const std::string &tag)
{
    m_fields.erase(tag);
    m_tagOrder.erase(std::remove(m_tagOrder.begin(), m_tagOrder.end(), tag), m_tagOrder.end());
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline std::vector<std::pair<std::string, std::vector<unsigned char>>> FieldMap::GetAllFields() const
{
    std::vector<std::pair<std::string, std::vector<unsigned char>>> result;
    result.reserve(m_tagOrder.size());

    for (const std::string &tag : m_tagOrder)
    {
        auto it = m_fields.find(tag);
        if (it != m_fields.end())
            result.emplace_back(tag, it->second.m_bytes);
    }

    return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <>
inline void FieldMap::Set(const std::string &tag, const std::string &value)
{
    const uint32_t length = static_cast<uint32_t>(value.size());
    std::vector<unsigned char> bytes(sizeof(uint32_t) + length);
    std::memcpy(bytes.data(), &length, sizeof(uint32_t));
    if (length > 0)
        std::memcpy(bytes.data() + sizeof(uint32_t), value.data(), length);

    this->RecordOrder(tag);
    m_fields[tag] = Entry{FieldValueType::STRING, std::move(bytes)};
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <>
inline StatusCode FieldMap::Get(const std::string &tag, std::string &value) const
{
    auto it = m_fields.find(tag);
    if (it == m_fields.end())
        return STATUS_CODE_NOT_FOUND;

    const std::vector<unsigned char> &bytes = it->second.m_bytes;
    if (bytes.size() < sizeof(uint32_t))
        return STATUS_CODE_INVALID_PARAMETER;

    uint32_t length = 0;
    std::memcpy(&length, bytes.data(), sizeof(uint32_t));
    if (bytes.size() != sizeof(uint32_t) + length)
        return STATUS_CODE_INVALID_PARAMETER;

    value.assign(reinterpret_cast<const char *>(bytes.data() + sizeof(uint32_t)), length);

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <>
inline void FieldMap::Set(const std::string &tag, const CartesianVector &value)
{
    const float x = value.GetX(), y = value.GetY(), z = value.GetZ();
    std::vector<unsigned char> bytes(3 * sizeof(float));
    std::memcpy(bytes.data() + 0 * sizeof(float), &x, sizeof(float));
    std::memcpy(bytes.data() + 1 * sizeof(float), &y, sizeof(float));
    std::memcpy(bytes.data() + 2 * sizeof(float), &z, sizeof(float));

    this->RecordOrder(tag);
    m_fields[tag] = Entry{FieldValueType::CARTESIAN_VECTOR, std::move(bytes)};
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <>
inline StatusCode FieldMap::Get(const std::string &tag, CartesianVector &value) const
{
    auto it = m_fields.find(tag);
    if (it == m_fields.end())
        return STATUS_CODE_NOT_FOUND;

    if (it->second.m_bytes.size() != 3 * sizeof(float))
        return STATUS_CODE_INVALID_PARAMETER;

    float x = 0.f, y = 0.f, z = 0.f;
    std::memcpy(&x, it->second.m_bytes.data() + 0 * sizeof(float), sizeof(float));
    std::memcpy(&y, it->second.m_bytes.data() + 1 * sizeof(float), sizeof(float));
    std::memcpy(&z, it->second.m_bytes.data() + 2 * sizeof(float), sizeof(float));
    value = CartesianVector(x, y, z);

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <>
inline void FieldMap::Set(const std::string &tag, const TrackState &value)
{
    const float px = value.GetPosition().GetX(), py = value.GetPosition().GetY(), pz = value.GetPosition().GetZ();
    const float mx = value.GetMomentum().GetX(), my = value.GetMomentum().GetY(), mz = value.GetMomentum().GetZ();
    std::vector<unsigned char> bytes(6 * sizeof(float));
    std::memcpy(bytes.data() + 0 * sizeof(float), &px, sizeof(float));
    std::memcpy(bytes.data() + 1 * sizeof(float), &py, sizeof(float));
    std::memcpy(bytes.data() + 2 * sizeof(float), &pz, sizeof(float));
    std::memcpy(bytes.data() + 3 * sizeof(float), &mx, sizeof(float));
    std::memcpy(bytes.data() + 4 * sizeof(float), &my, sizeof(float));
    std::memcpy(bytes.data() + 5 * sizeof(float), &mz, sizeof(float));

    this->RecordOrder(tag);
    m_fields[tag] = Entry{FieldValueType::TRACK_STATE, std::move(bytes)};
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <>
inline StatusCode FieldMap::Get(const std::string &tag, TrackState &value) const
{
    auto it = m_fields.find(tag);
    if (it == m_fields.end())
        return STATUS_CODE_NOT_FOUND;

    if (it->second.m_bytes.size() != 6 * sizeof(float))
        return STATUS_CODE_INVALID_PARAMETER;

    float px = 0.f, py = 0.f, pz = 0.f, mx = 0.f, my = 0.f, mz = 0.f;
    std::memcpy(&px, it->second.m_bytes.data() + 0 * sizeof(float), sizeof(float));
    std::memcpy(&py, it->second.m_bytes.data() + 1 * sizeof(float), sizeof(float));
    std::memcpy(&pz, it->second.m_bytes.data() + 2 * sizeof(float), sizeof(float));
    std::memcpy(&mx, it->second.m_bytes.data() + 3 * sizeof(float), sizeof(float));
    std::memcpy(&my, it->second.m_bytes.data() + 4 * sizeof(float), sizeof(float));
    std::memcpy(&mz, it->second.m_bytes.data() + 5 * sizeof(float), sizeof(float));
    value = TrackState(CartesianVector(px, py, pz), CartesianVector(mx, my, mz));

    return STATUS_CODE_SUCCESS;
}

} // namespace pandora

#endif // #ifndef PANDORA_FIELD_MAP_H
