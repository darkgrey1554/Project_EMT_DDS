// Copyright 2016 Proyectos y Sistemas de Mantenimiento SL (eProsima).
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/*!
 * @file DdsData.h
 * This header file contains the declaration of the described types in the IDL file.
 *
 * This file was generated by the tool gen.
 */

#ifndef _DDSDATA_H_
#define _DDSDATA_H_


#include <stdint.h>
#include <array>
#include <string>
#include <vector>
#include <map>
#include <bitset>

#if defined(_WIN32)
#if defined(EPROSIMA_USER_DLL_EXPORT)
#define eProsima_user_DllExport __declspec( dllexport )
#else
#define eProsima_user_DllExport
#endif  // EPROSIMA_USER_DLL_EXPORT
#else
#define eProsima_user_DllExport
#endif  // _WIN32

#if defined(_WIN32)
#if defined(EPROSIMA_USER_DLL_EXPORT)
#if defined(DdsData_SOURCE)
#define DdsData_DllAPI __declspec( dllexport )
#else
#define DdsData_DllAPI __declspec( dllimport )
#endif // DdsData_SOURCE
#else
#define DdsData_DllAPI
#endif  // EPROSIMA_USER_DLL_EXPORT
#else
#define DdsData_DllAPI
#endif // _WIN32

namespace eprosima {
    namespace fastcdr {
        class Cdr;
    } // namespace fastcdr
} // namespace eprosima


/*!
 * @brief This class represents the structure DdsData defined by the user in the IDL file.
 * @ingroup DDSDATA
 */
class DdsData
{
public:

    /*!
     * @brief Default constructor.
     */
    eProsima_user_DllExport DdsData();

    /*!
     * @brief Default destructor.
     */
    eProsima_user_DllExport ~DdsData();

    /*!
     * @brief Copy constructor.
     * @param x Reference to the object DdsData that will be copied.
     */
    eProsima_user_DllExport DdsData(
        const DdsData& x);

    /*!
     * @brief Move constructor.
     * @param x Reference to the object DdsData that will be copied.
     */
    eProsima_user_DllExport DdsData(
        DdsData&& x);

    /*!
     * @brief Copy assignment.
     * @param x Reference to the object DdsData that will be copied.
     */
    eProsima_user_DllExport DdsData& operator =(
        const DdsData& x);

    /*!
     * @brief Move assignment.
     * @param x Reference to the object DdsData that will be copied.
     */
    eProsima_user_DllExport DdsData& operator =(
        DdsData&& x);

    /*!
     * @brief This function sets a value in member id_tag
     * @param _id_tag New value for member id_tag
     */
    eProsima_user_DllExport void id_tag(
        uint32_t _id_tag);

    /*!
     * @brief This function returns the value of member id_tag
     * @return Value of member id_tag
     */
    eProsima_user_DllExport uint32_t id_tag() const;

    /*!
     * @brief This function returns a reference to member id_tag
     * @return Reference to member id_tag
     */
    eProsima_user_DllExport uint32_t& id_tag();

    /*!
     * @brief This function sets a value in member dt_opc
     * @param _dt_opc New value for member dt_opc
     */
    eProsima_user_DllExport void dt_opc(
        int64_t _dt_opc);

    /*!
     * @brief This function returns the value of member dt_opc
     * @return Value of member dt_opc
     */
    eProsima_user_DllExport int64_t dt_opc() const;

    /*!
     * @brief This function returns a reference to member dt_opc
     * @return Reference to member dt_opc
     */
    eProsima_user_DllExport int64_t& dt_opc();

    /*!
     * @brief This function sets a value in member dt_app
     * @param _dt_app New value for member dt_app
     */
    eProsima_user_DllExport void dt_app(
        int64_t _dt_app);

    /*!
     * @brief This function returns the value of member dt_app
     * @return Value of member dt_app
     */
    eProsima_user_DllExport int64_t dt_app() const;

    /*!
     * @brief This function returns a reference to member dt_app
     * @return Reference to member dt_app
     */
    eProsima_user_DllExport int64_t& dt_app();

    /*!
     * @brief This function sets a value in member v
     * @param _v New value for member v
     */
    eProsima_user_DllExport void v(
        double _v);

    /*!
     * @brief This function returns the value of member v
     * @return Value of member v
     */
    eProsima_user_DllExport double v() const;

    /*!
     * @brief This function returns a reference to member v
     * @return Reference to member v
     */
    eProsima_user_DllExport double& v();

    /*!
     * @brief This function sets a value in member q
     * @param _q New value for member q
     */
    eProsima_user_DllExport void q(
        int16_t _q);

    /*!
     * @brief This function returns the value of member q
     * @return Value of member q
     */
    eProsima_user_DllExport int16_t q() const;

    /*!
     * @brief This function returns a reference to member q
     * @return Reference to member q
     */
    eProsima_user_DllExport int16_t& q();


    /*!
     * @brief This function returns the maximum serialized size of an object
     * depending on the buffer alignment.
     * @param current_alignment Buffer alignment.
     * @return Maximum serialized size.
     */
    eProsima_user_DllExport static size_t getMaxCdrSerializedSize(
        size_t current_alignment = 0);

    /*!
     * @brief This function returns the serialized size of a data depending on the buffer alignment.
     * @param data Data which is calculated its serialized size.
     * @param current_alignment Buffer alignment.
     * @return Serialized size.
     */
    eProsima_user_DllExport static size_t getCdrSerializedSize(
        const DdsData& data,
        size_t current_alignment = 0);


    /*!
     * @brief This function serializes an object using CDR serialization.
     * @param cdr CDR serialization object.
     */
    eProsima_user_DllExport void serialize(
        eprosima::fastcdr::Cdr& cdr) const;

    /*!
     * @brief This function deserializes an object using CDR serialization.
     * @param cdr CDR serialization object.
     */
    eProsima_user_DllExport void deserialize(
        eprosima::fastcdr::Cdr& cdr);



    /*!
     * @brief This function returns the maximum serialized size of the Key of an object
     * depending on the buffer alignment.
     * @param current_alignment Buffer alignment.
     * @return Maximum serialized size.
     */
    eProsima_user_DllExport static size_t getKeyMaxCdrSerializedSize(
        size_t current_alignment = 0);

    /*!
     * @brief This function tells you if the Key has been defined for this type
     */
    eProsima_user_DllExport static bool isKeyDefined();

    /*!
     * @brief This function serializes the key members of an object using CDR serialization.
     * @param cdr CDR serialization object.
     */
    eProsima_user_DllExport void serializeKey(
        eprosima::fastcdr::Cdr& cdr) const;

private:

    uint32_t m_id_tag;
    int64_t m_dt_opc;
    int64_t m_dt_app;
    double m_v;
    int16_t m_q;
};
/*!
 * @brief This class represents the structure DdsAlarm defined by the user in the IDL file.
 * @ingroup DDSDATA
 */
class DdsAlarm
{
public:

    /*!
     * @brief Default constructor.
     */
    eProsima_user_DllExport DdsAlarm();

    /*!
     * @brief Default destructor.
     */
    eProsima_user_DllExport ~DdsAlarm();

    /*!
     * @brief Copy constructor.
     * @param x Reference to the object DdsAlarm that will be copied.
     */
    eProsima_user_DllExport DdsAlarm(
        const DdsAlarm& x);

    /*!
     * @brief Move constructor.
     * @param x Reference to the object DdsAlarm that will be copied.
     */
    eProsima_user_DllExport DdsAlarm(
        DdsAlarm&& x);

    /*!
     * @brief Copy assignment.
     * @param x Reference to the object DdsAlarm that will be copied.
     */
    eProsima_user_DllExport DdsAlarm& operator =(
        const DdsAlarm& x);

    /*!
     * @brief Move assignment.
     * @param x Reference to the object DdsAlarm that will be copied.
     */
    eProsima_user_DllExport DdsAlarm& operator =(
        DdsAlarm&& x);

    /*!
     * @brief This function sets a value in member id_tag
     * @param _id_tag New value for member id_tag
     */
    eProsima_user_DllExport void id_tag(
        uint32_t _id_tag);

    /*!
     * @brief This function returns the value of member id_tag
     * @return Value of member id_tag
     */
    eProsima_user_DllExport uint32_t id_tag() const;

    /*!
     * @brief This function returns a reference to member id_tag
     * @return Reference to member id_tag
     */
    eProsima_user_DllExport uint32_t& id_tag();

    /*!
     * @brief This function sets a value in member dt_opc
     * @param _dt_opc New value for member dt_opc
     */
    eProsima_user_DllExport void dt_opc(
        int64_t _dt_opc);

    /*!
     * @brief This function returns the value of member dt_opc
     * @return Value of member dt_opc
     */
    eProsima_user_DllExport int64_t dt_opc() const;

    /*!
     * @brief This function returns a reference to member dt_opc
     * @return Reference to member dt_opc
     */
    eProsima_user_DllExport int64_t& dt_opc();

    /*!
     * @brief This function sets a value in member dt_app
     * @param _dt_app New value for member dt_app
     */
    eProsima_user_DllExport void dt_app(
        int64_t _dt_app);

    /*!
     * @brief This function returns the value of member dt_app
     * @return Value of member dt_app
     */
    eProsima_user_DllExport int64_t dt_app() const;

    /*!
     * @brief This function returns a reference to member dt_app
     * @return Reference to member dt_app
     */
    eProsima_user_DllExport int64_t& dt_app();

    /*!
     * @brief This function sets a value in member state
     * @param _state New value for member state
     */
    eProsima_user_DllExport void state(
        uint32_t _state);

    /*!
     * @brief This function returns the value of member state
     * @return Value of member state
     */
    eProsima_user_DllExport uint32_t state() const;

    /*!
     * @brief This function returns a reference to member state
     * @return Reference to member state
     */
    eProsima_user_DllExport uint32_t& state();


    /*!
     * @brief This function returns the maximum serialized size of an object
     * depending on the buffer alignment.
     * @param current_alignment Buffer alignment.
     * @return Maximum serialized size.
     */
    eProsima_user_DllExport static size_t getMaxCdrSerializedSize(
        size_t current_alignment = 0);

    /*!
     * @brief This function returns the serialized size of a data depending on the buffer alignment.
     * @param data Data which is calculated its serialized size.
     * @param current_alignment Buffer alignment.
     * @return Serialized size.
     */
    eProsima_user_DllExport static size_t getCdrSerializedSize(
        const DdsAlarm& data,
        size_t current_alignment = 0);


    /*!
     * @brief This function serializes an object using CDR serialization.
     * @param cdr CDR serialization object.
     */
    eProsima_user_DllExport void serialize(
        eprosima::fastcdr::Cdr& cdr) const;

    /*!
     * @brief This function deserializes an object using CDR serialization.
     * @param cdr CDR serialization object.
     */
    eProsima_user_DllExport void deserialize(
        eprosima::fastcdr::Cdr& cdr);



    /*!
     * @brief This function returns the maximum serialized size of the Key of an object
     * depending on the buffer alignment.
     * @param current_alignment Buffer alignment.
     * @return Maximum serialized size.
     */
    eProsima_user_DllExport static size_t getKeyMaxCdrSerializedSize(
        size_t current_alignment = 0);

    /*!
     * @brief This function tells you if the Key has been defined for this type
     */
    eProsima_user_DllExport static bool isKeyDefined();

    /*!
     * @brief This function serializes the key members of an object using CDR serialization.
     * @param cdr CDR serialization object.
     */
    eProsima_user_DllExport void serializeKey(
        eprosima::fastcdr::Cdr& cdr) const;

private:

    uint32_t m_id_tag;
    int64_t m_dt_opc;
    int64_t m_dt_app;
    uint32_t m_state;
};
/*!
 * @brief This class represents the structure DdsStatus defined by the user in the IDL file.
 * @ingroup DDSDATA
 */
class DdsStatus
{
public:

    /*!
     * @brief Default constructor.
     */
    eProsima_user_DllExport DdsStatus();

    /*!
     * @brief Default destructor.
     */
    eProsima_user_DllExport ~DdsStatus();

    /*!
     * @brief Copy constructor.
     * @param x Reference to the object DdsStatus that will be copied.
     */
    eProsima_user_DllExport DdsStatus(
        const DdsStatus& x);

    /*!
     * @brief Move constructor.
     * @param x Reference to the object DdsStatus that will be copied.
     */
    eProsima_user_DllExport DdsStatus(
        DdsStatus&& x);

    /*!
     * @brief Copy assignment.
     * @param x Reference to the object DdsStatus that will be copied.
     */
    eProsima_user_DllExport DdsStatus& operator =(
        const DdsStatus& x);

    /*!
     * @brief Move assignment.
     * @param x Reference to the object DdsStatus that will be copied.
     */
    eProsima_user_DllExport DdsStatus& operator =(
        DdsStatus&& x);

    /*!
     * @brief This function sets a value in member id
     * @param _id New value for member id
     */
    eProsima_user_DllExport void id(
        uint32_t _id);

    /*!
     * @brief This function returns the value of member id
     * @return Value of member id
     */
    eProsima_user_DllExport uint32_t id() const;

    /*!
     * @brief This function returns a reference to member id
     * @return Reference to member id
     */
    eProsima_user_DllExport uint32_t& id();

    /*!
     * @brief This function sets a value in member status
     * @param _status New value for member status
     */
    eProsima_user_DllExport void status(
        uint32_t _status);

    /*!
     * @brief This function returns the value of member status
     * @return Value of member status
     */
    eProsima_user_DllExport uint32_t status() const;

    /*!
     * @brief This function returns a reference to member status
     * @return Reference to member status
     */
    eProsima_user_DllExport uint32_t& status();

    /*!
     * @brief This function sets a value in member code
     * @param _code New value for member code
     */
    eProsima_user_DllExport void code(
        uint32_t _code);

    /*!
     * @brief This function returns the value of member code
     * @return Value of member code
     */
    eProsima_user_DllExport uint32_t code() const;

    /*!
     * @brief This function returns a reference to member code
     * @return Reference to member code
     */
    eProsima_user_DllExport uint32_t& code();

    /*!
     * @brief This function copies the value in member message
     * @param _message New value to be copied in member message
     */
    eProsima_user_DllExport void message(
        const std::string& _message);

    /*!
     * @brief This function moves the value in member message
     * @param _message New value to be moved in member message
     */
    eProsima_user_DllExport void message(
        std::string&& _message);

    /*!
     * @brief This function returns a constant reference to member message
     * @return Constant reference to member message
     */
    eProsima_user_DllExport const std::string& message() const;

    /*!
     * @brief This function returns a reference to member message
     * @return Reference to member message
     */
    eProsima_user_DllExport std::string& message();

    /*!
     * @brief This function returns the maximum serialized size of an object
     * depending on the buffer alignment.
     * @param current_alignment Buffer alignment.
     * @return Maximum serialized size.
     */
    eProsima_user_DllExport static size_t getMaxCdrSerializedSize(
        size_t current_alignment = 0);

    /*!
     * @brief This function returns the serialized size of a data depending on the buffer alignment.
     * @param data Data which is calculated its serialized size.
     * @param current_alignment Buffer alignment.
     * @return Serialized size.
     */
    eProsima_user_DllExport static size_t getCdrSerializedSize(
        const DdsStatus& data,
        size_t current_alignment = 0);


    /*!
     * @brief This function serializes an object using CDR serialization.
     * @param cdr CDR serialization object.
     */
    eProsima_user_DllExport void serialize(
        eprosima::fastcdr::Cdr& cdr) const;

    /*!
     * @brief This function deserializes an object using CDR serialization.
     * @param cdr CDR serialization object.
     */
    eProsima_user_DllExport void deserialize(
        eprosima::fastcdr::Cdr& cdr);



    /*!
     * @brief This function returns the maximum serialized size of the Key of an object
     * depending on the buffer alignment.
     * @param current_alignment Buffer alignment.
     * @return Maximum serialized size.
     */
    eProsima_user_DllExport static size_t getKeyMaxCdrSerializedSize(
        size_t current_alignment = 0);

    /*!
     * @brief This function tells you if the Key has been defined for this type
     */
    eProsima_user_DllExport static bool isKeyDefined();

    /*!
     * @brief This function serializes the key members of an object using CDR serialization.
     * @param cdr CDR serialization object.
     */
    eProsima_user_DllExport void serializeKey(
        eprosima::fastcdr::Cdr& cdr) const;

private:

    uint32_t m_id;
    uint32_t m_status;
    uint32_t m_code;
    std::string m_message;
};
/*!
 * @brief This class represents the structure DdsConfig defined by the user in the IDL file.
 * @ingroup DDSDATA
 */
class DdsConfig
{
public:

    /*!
     * @brief Default constructor.
     */
    eProsima_user_DllExport DdsConfig();

    /*!
     * @brief Default destructor.
     */
    eProsima_user_DllExport ~DdsConfig();

    /*!
     * @brief Copy constructor.
     * @param x Reference to the object DdsConfig that will be copied.
     */
    eProsima_user_DllExport DdsConfig(
        const DdsConfig& x);

    /*!
     * @brief Move constructor.
     * @param x Reference to the object DdsConfig that will be copied.
     */
    eProsima_user_DllExport DdsConfig(
        DdsConfig&& x);

    /*!
     * @brief Copy assignment.
     * @param x Reference to the object DdsConfig that will be copied.
     */
    eProsima_user_DllExport DdsConfig& operator =(
        const DdsConfig& x);

    /*!
     * @brief Move assignment.
     * @param x Reference to the object DdsConfig that will be copied.
     */
    eProsima_user_DllExport DdsConfig& operator =(
        DdsConfig&& x);

    /*!
     * @brief This function sets a value in member id
     * @param _id New value for member id
     */
    eProsima_user_DllExport void id(
        uint32_t _id);

    /*!
     * @brief This function returns the value of member id
     * @return Value of member id
     */
    eProsima_user_DllExport uint32_t id() const;

    /*!
     * @brief This function returns a reference to member id
     * @return Reference to member id
     */
    eProsima_user_DllExport uint32_t& id();

    /*!
     * @brief This function sets a value in member config_size
     * @param _config_size New value for member config_size
     */
    eProsima_user_DllExport void config_size(
        uint32_t _config_size);

    /*!
     * @brief This function returns the value of member config_size
     * @return Value of member config_size
     */
    eProsima_user_DllExport uint32_t config_size() const;

    /*!
     * @brief This function returns a reference to member config_size
     * @return Reference to member config_size
     */
    eProsima_user_DllExport uint32_t& config_size();

    /*!
     * @brief This function copies the value in member message
     * @param _message New value to be copied in member message
     */
    eProsima_user_DllExport void message(
        const std::string& _message);

    /*!
     * @brief This function moves the value in member message
     * @param _message New value to be moved in member message
     */
    eProsima_user_DllExport void message(
        std::string&& _message);

    /*!
     * @brief This function returns a constant reference to member message
     * @return Constant reference to member message
     */
    eProsima_user_DllExport const std::string& message() const;

    /*!
     * @brief This function returns a reference to member message
     * @return Reference to member message
     */
    eProsima_user_DllExport std::string& message();

    /*!
     * @brief This function returns the maximum serialized size of an object
     * depending on the buffer alignment.
     * @param current_alignment Buffer alignment.
     * @return Maximum serialized size.
     */
    eProsima_user_DllExport static size_t getMaxCdrSerializedSize(
        size_t current_alignment = 0);

    /*!
     * @brief This function returns the serialized size of a data depending on the buffer alignment.
     * @param data Data which is calculated its serialized size.
     * @param current_alignment Buffer alignment.
     * @return Serialized size.
     */
    eProsima_user_DllExport static size_t getCdrSerializedSize(
        const DdsConfig& data,
        size_t current_alignment = 0);


    /*!
     * @brief This function serializes an object using CDR serialization.
     * @param cdr CDR serialization object.
     */
    eProsima_user_DllExport void serialize(
        eprosima::fastcdr::Cdr& cdr) const;

    /*!
     * @brief This function deserializes an object using CDR serialization.
     * @param cdr CDR serialization object.
     */
    eProsima_user_DllExport void deserialize(
        eprosima::fastcdr::Cdr& cdr);



    /*!
     * @brief This function returns the maximum serialized size of the Key of an object
     * depending on the buffer alignment.
     * @param current_alignment Buffer alignment.
     * @return Maximum serialized size.
     */
    eProsima_user_DllExport static size_t getKeyMaxCdrSerializedSize(
        size_t current_alignment = 0);

    /*!
     * @brief This function tells you if the Key has been defined for this type
     */
    eProsima_user_DllExport static bool isKeyDefined();

    /*!
     * @brief This function serializes the key members of an object using CDR serialization.
     * @param cdr CDR serialization object.
     */
    eProsima_user_DllExport void serializeKey(
        eprosima::fastcdr::Cdr& cdr) const;

    static uint32_t getSize() { return m_size; }
    static void setSize(uint32_t size) { m_size = size; }

private:

    uint32_t m_id;
    uint32_t m_config_size;
    std::string m_message;
    static  uint32_t m_size;
};
/*!
 * @brief This class represents the structure DdsDataCollection defined by the user in the IDL file.
 * @ingroup DDSDATA
 */
class DdsDataCollection
{
public:

    /*!
     * @brief Default constructor.
     */
    eProsima_user_DllExport DdsDataCollection();

    /*!
     * @brief Default destructor.
     */
    eProsima_user_DllExport ~DdsDataCollection();

    /*!
     * @brief Copy constructor.
     * @param x Reference to the object DdsDataCollection that will be copied.
     */
    eProsima_user_DllExport DdsDataCollection(
        const DdsDataCollection& x);

    /*!
     * @brief Move constructor.
     * @param x Reference to the object DdsDataCollection that will be copied.
     */
    eProsima_user_DllExport DdsDataCollection(
        DdsDataCollection&& x);

    /*!
     * @brief Copy assignment.
     * @param x Reference to the object DdsDataCollection that will be copied.
     */
    eProsima_user_DllExport DdsDataCollection& operator =(
        const DdsDataCollection& x);

    /*!
     * @brief Move assignment.
     * @param x Reference to the object DdsDataCollection that will be copied.
     */
    eProsima_user_DllExport DdsDataCollection& operator =(
        DdsDataCollection&& x);

    /*!
     * @brief This function copies the value in member sensor_array
     * @param _sensor_array New value to be copied in member sensor_array
     */
    eProsima_user_DllExport void sensor_array(
        const std::vector<DdsData>& _sensor_array);

    /*!
     * @brief This function moves the value in member sensor_array
     * @param _sensor_array New value to be moved in member sensor_array
     */
    eProsima_user_DllExport void sensor_array(
        std::vector<DdsData>&& _sensor_array);

    /*!
     * @brief This function returns a constant reference to member sensor_array
     * @return Constant reference to member sensor_array
     */
    eProsima_user_DllExport const std::vector<DdsData>& sensor_array() const;

    /*!
     * @brief This function returns a reference to member sensor_array
     * @return Reference to member sensor_array
     */
    eProsima_user_DllExport std::vector<DdsData>& sensor_array();

    /*!
     * @brief This function returns the maximum serialized size of an object
     * depending on the buffer alignment.
     * @param current_alignment Buffer alignment.
     * @return Maximum serialized size.
     */
    eProsima_user_DllExport static size_t getMaxCdrSerializedSize(
        size_t current_alignment = 0);

    /*!
     * @brief This function returns the serialized size of a data depending on the buffer alignment.
     * @param data Data which is calculated its serialized size.
     * @param current_alignment Buffer alignment.
     * @return Serialized size.
     */
    eProsima_user_DllExport static size_t getCdrSerializedSize(
        const DdsDataCollection& data,
        size_t current_alignment = 0);


    /*!
     * @brief This function serializes an object using CDR serialization.
     * @param cdr CDR serialization object.
     */
    eProsima_user_DllExport void serialize(
        eprosima::fastcdr::Cdr& cdr) const;

    /*!
     * @brief This function deserializes an object using CDR serialization.
     * @param cdr CDR serialization object.
     */
    eProsima_user_DllExport void deserialize(
        eprosima::fastcdr::Cdr& cdr);



    /*!
     * @brief This function returns the maximum serialized size of the Key of an object
     * depending on the buffer alignment.
     * @param current_alignment Buffer alignment.
     * @return Maximum serialized size.
     */
    eProsima_user_DllExport static size_t getKeyMaxCdrSerializedSize(
        size_t current_alignment = 0);

    /*!
     * @brief This function tells you if the Key has been defined for this type
     */
    eProsima_user_DllExport static bool isKeyDefined();

    /*!
     * @brief This function serializes the key members of an object using CDR serialization.
     * @param cdr CDR serialization object.
     */
    eProsima_user_DllExport void serializeKey(
        eprosima::fastcdr::Cdr& cdr) const;

    static uint32_t getSize() { return m_size; }
    static void setSize(uint32_t size) { m_size = size; }

private:

    std::vector<DdsData> m_sensor_array;
    static  uint32_t m_size;
};
/*!
 * @brief This class represents the structure DdsAlarmCollection defined by the user in the IDL file.
 * @ingroup DDSDATA
 */
class DdsAlarmCollection
{
public:

    /*!
     * @brief Default constructor.
     */
    eProsima_user_DllExport DdsAlarmCollection();

    /*!
     * @brief Default destructor.
     */
    eProsima_user_DllExport ~DdsAlarmCollection();

    /*!
     * @brief Copy constructor.
     * @param x Reference to the object DdsAlarmCollection that will be copied.
     */
    eProsima_user_DllExport DdsAlarmCollection(
        const DdsAlarmCollection& x);

    /*!
     * @brief Move constructor.
     * @param x Reference to the object DdsAlarmCollection that will be copied.
     */
    eProsima_user_DllExport DdsAlarmCollection(
        DdsAlarmCollection&& x);

    /*!
     * @brief Copy assignment.
     * @param x Reference to the object DdsAlarmCollection that will be copied.
     */
    eProsima_user_DllExport DdsAlarmCollection& operator =(
        const DdsAlarmCollection& x);

    /*!
     * @brief Move assignment.
     * @param x Reference to the object DdsAlarmCollection that will be copied.
     */
    eProsima_user_DllExport DdsAlarmCollection& operator =(
        DdsAlarmCollection&& x);

    /*!
     * @brief This function copies the value in member alarms_array
     * @param _alarms_array New value to be copied in member alarms_array
     */
    eProsima_user_DllExport void alarms_array(
        const std::vector<DdsAlarm>& _alarms_array);

    /*!
     * @brief This function moves the value in member alarms_array
     * @param _alarms_array New value to be moved in member alarms_array
     */
    eProsima_user_DllExport void alarms_array(
        std::vector<DdsAlarm>&& _alarms_array);

    /*!
     * @brief This function returns a constant reference to member alarms_array
     * @return Constant reference to member alarms_array
     */
    eProsima_user_DllExport const std::vector<DdsAlarm>& alarms_array() const;

    /*!
     * @brief This function returns a reference to member alarms_array
     * @return Reference to member alarms_array
     */
    eProsima_user_DllExport std::vector<DdsAlarm>& alarms_array();

    /*!
     * @brief This function returns the maximum serialized size of an object
     * depending on the buffer alignment.
     * @param current_alignment Buffer alignment.
     * @return Maximum serialized size.
     */
    eProsima_user_DllExport static size_t getMaxCdrSerializedSize(
        size_t current_alignment = 0);

    /*!
     * @brief This function returns the serialized size of a data depending on the buffer alignment.
     * @param data Data which is calculated its serialized size.
     * @param current_alignment Buffer alignment.
     * @return Serialized size.
     */
    eProsima_user_DllExport static size_t getCdrSerializedSize(
        const DdsAlarmCollection& data,
        size_t current_alignment = 0);


    /*!
     * @brief This function serializes an object using CDR serialization.
     * @param cdr CDR serialization object.
     */
    eProsima_user_DllExport void serialize(
        eprosima::fastcdr::Cdr& cdr) const;

    /*!
     * @brief This function deserializes an object using CDR serialization.
     * @param cdr CDR serialization object.
     */
    eProsima_user_DllExport void deserialize(
        eprosima::fastcdr::Cdr& cdr);



    /*!
     * @brief This function returns the maximum serialized size of the Key of an object
     * depending on the buffer alignment.
     * @param current_alignment Buffer alignment.
     * @return Maximum serialized size.
     */
    eProsima_user_DllExport static size_t getKeyMaxCdrSerializedSize(
        size_t current_alignment = 0);

    /*!
     * @brief This function tells you if the Key has been defined for this type
     */
    eProsima_user_DllExport static bool isKeyDefined();

    /*!
     * @brief This function serializes the key members of an object using CDR serialization.
     * @param cdr CDR serialization object.
     */
    eProsima_user_DllExport void serializeKey(
        eprosima::fastcdr::Cdr& cdr) const;

    static uint32_t getSize() { return m_size; }
    static void setSize(uint32_t size) { m_size = size; }

private:

    std::vector<DdsAlarm> m_alarms_array;
    static  uint32_t m_size;
};

#endif // _DDSDATA_H_