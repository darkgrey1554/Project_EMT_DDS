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
 * @file DdsDataPubSubTypes.cpp
 * This header file contains the implementation of the serialization functions.
 *
 * This file was generated by the tool fastcdrgen.
 */


#include <fastcdr/FastBuffer.h>
#include <fastcdr/Cdr.h>

#include "DdsDataPubSubTypes.h"

using SerializedPayload_t = eprosima::fastrtps::rtps::SerializedPayload_t;
using InstanceHandle_t = eprosima::fastrtps::rtps::InstanceHandle_t;

DdsDataPubSubType::DdsDataPubSubType()
{
    setName("DdsData");
    m_typeSize = static_cast<uint32_t>(DdsData::getMaxCdrSerializedSize()) + 4 /*encapsulation*/;
    m_isGetKeyDefined = DdsData::isKeyDefined();
    size_t keyLength = DdsData::getKeyMaxCdrSerializedSize() > 16 ?
        DdsData::getKeyMaxCdrSerializedSize() : 16;
    m_keyBuffer = reinterpret_cast<unsigned char*>(malloc(keyLength));
    memset(m_keyBuffer, 0, keyLength);
}

DdsDataPubSubType::~DdsDataPubSubType()
{
    if (m_keyBuffer != nullptr)
    {
        free(m_keyBuffer);
    }
}

bool DdsDataPubSubType::serialize(
    void* data,
    SerializedPayload_t* payload)
{
    DdsData* p_type = static_cast<DdsData*>(data);

    // Object that manages the raw buffer.
    eprosima::fastcdr::FastBuffer fastbuffer(reinterpret_cast<char*>(payload->data), payload->max_size);
    // Object that serializes the data.
    eprosima::fastcdr::Cdr ser(fastbuffer, eprosima::fastcdr::Cdr::DEFAULT_ENDIAN, eprosima::fastcdr::Cdr::DDS_CDR);
    payload->encapsulation = ser.endianness() == eprosima::fastcdr::Cdr::BIG_ENDIANNESS ? CDR_BE : CDR_LE;
    // Serialize encapsulation
    ser.serialize_encapsulation();

    try
    {
        // Serialize the object.
        p_type->serialize(ser);
    }
    catch (eprosima::fastcdr::exception::NotEnoughMemoryException& /*exception*/)
    {
        return false;
    }

    // Get the serialized length
    payload->length = static_cast<uint32_t>(ser.getSerializedDataLength());
    return true;
}

bool DdsDataPubSubType::deserialize(
    SerializedPayload_t* payload,
    void* data)
{
    //Convert DATA to pointer of your type
    DdsData* p_type = static_cast<DdsData*>(data);

    // Object that manages the raw buffer.
    eprosima::fastcdr::FastBuffer fastbuffer(reinterpret_cast<char*>(payload->data), payload->length);

    // Object that deserializes the data.
    eprosima::fastcdr::Cdr deser(fastbuffer, eprosima::fastcdr::Cdr::DEFAULT_ENDIAN, eprosima::fastcdr::Cdr::DDS_CDR);

    // Deserialize encapsulation.
    deser.read_encapsulation();
    payload->encapsulation = deser.endianness() == eprosima::fastcdr::Cdr::BIG_ENDIANNESS ? CDR_BE : CDR_LE;

    try
    {
        // Deserialize the object.
        p_type->deserialize(deser);
    }
    catch (eprosima::fastcdr::exception::NotEnoughMemoryException& /*exception*/)
    {
        return false;
    }

    return true;
}

std::function<uint32_t()> DdsDataPubSubType::getSerializedSizeProvider(
    void* data)
{
    return [data]() -> uint32_t
    {
        return static_cast<uint32_t>(type::getCdrSerializedSize(*static_cast<DdsData*>(data))) +
            4u /*encapsulation*/;
    };
}

void* DdsDataPubSubType::createData()
{
    return reinterpret_cast<void*>(new DdsData());
}

void DdsDataPubSubType::deleteData(
    void* data)
{
    delete(reinterpret_cast<DdsData*>(data));
}

bool DdsDataPubSubType::getKey(
    void* data,
    InstanceHandle_t* handle,
    bool force_md5)
{
    if (!m_isGetKeyDefined)
    {
        return false;
    }

    DdsData* p_type = static_cast<DdsData*>(data);

    // Object that manages the raw buffer.
    eprosima::fastcdr::FastBuffer fastbuffer(reinterpret_cast<char*>(m_keyBuffer),
        DdsData::getKeyMaxCdrSerializedSize());

    // Object that serializes the data.
    eprosima::fastcdr::Cdr ser(fastbuffer, eprosima::fastcdr::Cdr::BIG_ENDIANNESS);
    p_type->serializeKey(ser);
    if (force_md5 || DdsData::getKeyMaxCdrSerializedSize() > 16)
    {
        m_md5.init();
        m_md5.update(m_keyBuffer, static_cast<unsigned int>(ser.getSerializedDataLength()));
        m_md5.finalize();
        for (uint8_t i = 0; i < 16; ++i)
        {
            handle->value[i] = m_md5.digest[i];
        }
    }
    else
    {
        for (uint8_t i = 0; i < 16; ++i)
        {
            handle->value[i] = m_keyBuffer[i];
        }
    }
    return true;
}

DdsAlarmPubSubType::DdsAlarmPubSubType()
{
    setName("DdsAlarm");
    m_typeSize = static_cast<uint32_t>(DdsAlarm::getMaxCdrSerializedSize()) + 4 /*encapsulation*/;
    m_isGetKeyDefined = DdsAlarm::isKeyDefined();
    size_t keyLength = DdsAlarm::getKeyMaxCdrSerializedSize() > 16 ?
        DdsAlarm::getKeyMaxCdrSerializedSize() : 16;
    m_keyBuffer = reinterpret_cast<unsigned char*>(malloc(keyLength));
    memset(m_keyBuffer, 0, keyLength);
}

DdsAlarmPubSubType::~DdsAlarmPubSubType()
{
    if (m_keyBuffer != nullptr)
    {
        free(m_keyBuffer);
    }
}

bool DdsAlarmPubSubType::serialize(
    void* data,
    SerializedPayload_t* payload)
{
    DdsAlarm* p_type = static_cast<DdsAlarm*>(data);

    // Object that manages the raw buffer.
    eprosima::fastcdr::FastBuffer fastbuffer(reinterpret_cast<char*>(payload->data), payload->max_size);
    // Object that serializes the data.
    eprosima::fastcdr::Cdr ser(fastbuffer, eprosima::fastcdr::Cdr::DEFAULT_ENDIAN, eprosima::fastcdr::Cdr::DDS_CDR);
    payload->encapsulation = ser.endianness() == eprosima::fastcdr::Cdr::BIG_ENDIANNESS ? CDR_BE : CDR_LE;
    // Serialize encapsulation
    ser.serialize_encapsulation();

    try
    {
        // Serialize the object.
        p_type->serialize(ser);
    }
    catch (eprosima::fastcdr::exception::NotEnoughMemoryException& /*exception*/)
    {
        return false;
    }

    // Get the serialized length
    payload->length = static_cast<uint32_t>(ser.getSerializedDataLength());
    return true;
}

bool DdsAlarmPubSubType::deserialize(
    SerializedPayload_t* payload,
    void* data)
{
    //Convert DATA to pointer of your type
    DdsAlarm* p_type = static_cast<DdsAlarm*>(data);

    // Object that manages the raw buffer.
    eprosima::fastcdr::FastBuffer fastbuffer(reinterpret_cast<char*>(payload->data), payload->length);

    // Object that deserializes the data.
    eprosima::fastcdr::Cdr deser(fastbuffer, eprosima::fastcdr::Cdr::DEFAULT_ENDIAN, eprosima::fastcdr::Cdr::DDS_CDR);

    // Deserialize encapsulation.
    deser.read_encapsulation();
    payload->encapsulation = deser.endianness() == eprosima::fastcdr::Cdr::BIG_ENDIANNESS ? CDR_BE : CDR_LE;

    try
    {
        // Deserialize the object.
        p_type->deserialize(deser);
    }
    catch (eprosima::fastcdr::exception::NotEnoughMemoryException& /*exception*/)
    {
        return false;
    }

    return true;
}

std::function<uint32_t()> DdsAlarmPubSubType::getSerializedSizeProvider(
    void* data)
{
    return [data]() -> uint32_t
    {
        return static_cast<uint32_t>(type::getCdrSerializedSize(*static_cast<DdsAlarm*>(data))) +
            4u /*encapsulation*/;
    };
}

void* DdsAlarmPubSubType::createData()
{
    return reinterpret_cast<void*>(new DdsAlarm());
}

void DdsAlarmPubSubType::deleteData(
    void* data)
{
    delete(reinterpret_cast<DdsAlarm*>(data));
}

bool DdsAlarmPubSubType::getKey(
    void* data,
    InstanceHandle_t* handle,
    bool force_md5)
{
    if (!m_isGetKeyDefined)
    {
        return false;
    }

    DdsAlarm* p_type = static_cast<DdsAlarm*>(data);

    // Object that manages the raw buffer.
    eprosima::fastcdr::FastBuffer fastbuffer(reinterpret_cast<char*>(m_keyBuffer),
        DdsAlarm::getKeyMaxCdrSerializedSize());

    // Object that serializes the data.
    eprosima::fastcdr::Cdr ser(fastbuffer, eprosima::fastcdr::Cdr::BIG_ENDIANNESS);
    p_type->serializeKey(ser);
    if (force_md5 || DdsAlarm::getKeyMaxCdrSerializedSize() > 16)
    {
        m_md5.init();
        m_md5.update(m_keyBuffer, static_cast<unsigned int>(ser.getSerializedDataLength()));
        m_md5.finalize();
        for (uint8_t i = 0; i < 16; ++i)
        {
            handle->value[i] = m_md5.digest[i];
        }
    }
    else
    {
        for (uint8_t i = 0; i < 16; ++i)
        {
            handle->value[i] = m_keyBuffer[i];
        }
    }
    return true;
}

DdsStatusPubSubType::DdsStatusPubSubType()
{
    setName("DdsStatus");
    m_typeSize = static_cast<uint32_t>(DdsStatus::getMaxCdrSerializedSize()) + 4 /*encapsulation*/;
    m_isGetKeyDefined = DdsStatus::isKeyDefined();
    size_t keyLength = DdsStatus::getKeyMaxCdrSerializedSize() > 16 ?
        DdsStatus::getKeyMaxCdrSerializedSize() : 16;
    m_keyBuffer = reinterpret_cast<unsigned char*>(malloc(keyLength));
    memset(m_keyBuffer, 0, keyLength);
}

DdsStatusPubSubType::~DdsStatusPubSubType()
{
    if (m_keyBuffer != nullptr)
    {
        free(m_keyBuffer);
    }
}

bool DdsStatusPubSubType::serialize(
    void* data,
    SerializedPayload_t* payload)
{
    DdsStatus* p_type = static_cast<DdsStatus*>(data);

    // Object that manages the raw buffer.
    eprosima::fastcdr::FastBuffer fastbuffer(reinterpret_cast<char*>(payload->data), payload->max_size);
    // Object that serializes the data.
    eprosima::fastcdr::Cdr ser(fastbuffer, eprosima::fastcdr::Cdr::DEFAULT_ENDIAN, eprosima::fastcdr::Cdr::DDS_CDR);
    payload->encapsulation = ser.endianness() == eprosima::fastcdr::Cdr::BIG_ENDIANNESS ? CDR_BE : CDR_LE;
    // Serialize encapsulation
    ser.serialize_encapsulation();

    try
    {
        // Serialize the object.
        p_type->serialize(ser);
    }
    catch (eprosima::fastcdr::exception::NotEnoughMemoryException& /*exception*/)
    {
        return false;
    }

    // Get the serialized length
    payload->length = static_cast<uint32_t>(ser.getSerializedDataLength());
    return true;
}

bool DdsStatusPubSubType::deserialize(
    SerializedPayload_t* payload,
    void* data)
{
    //Convert DATA to pointer of your type
    DdsStatus* p_type = static_cast<DdsStatus*>(data);

    // Object that manages the raw buffer.
    eprosima::fastcdr::FastBuffer fastbuffer(reinterpret_cast<char*>(payload->data), payload->length);

    // Object that deserializes the data.
    eprosima::fastcdr::Cdr deser(fastbuffer, eprosima::fastcdr::Cdr::DEFAULT_ENDIAN, eprosima::fastcdr::Cdr::DDS_CDR);

    // Deserialize encapsulation.
    deser.read_encapsulation();
    payload->encapsulation = deser.endianness() == eprosima::fastcdr::Cdr::BIG_ENDIANNESS ? CDR_BE : CDR_LE;

    try
    {
        // Deserialize the object.
        p_type->deserialize(deser);
    }
    catch (eprosima::fastcdr::exception::NotEnoughMemoryException& /*exception*/)
    {
        return false;
    }

    return true;
}

std::function<uint32_t()> DdsStatusPubSubType::getSerializedSizeProvider(
    void* data)
{
    return [data]() -> uint32_t
    {
        return static_cast<uint32_t>(type::getCdrSerializedSize(*static_cast<DdsStatus*>(data))) +
            4u /*encapsulation*/;
    };
}

void* DdsStatusPubSubType::createData()
{
    return reinterpret_cast<void*>(new DdsStatus());
}

void DdsStatusPubSubType::deleteData(
    void* data)
{
    delete(reinterpret_cast<DdsStatus*>(data));
}

bool DdsStatusPubSubType::getKey(
    void* data,
    InstanceHandle_t* handle,
    bool force_md5)
{
    if (!m_isGetKeyDefined)
    {
        return false;
    }

    DdsStatus* p_type = static_cast<DdsStatus*>(data);

    // Object that manages the raw buffer.
    eprosima::fastcdr::FastBuffer fastbuffer(reinterpret_cast<char*>(m_keyBuffer),
        DdsStatus::getKeyMaxCdrSerializedSize());

    // Object that serializes the data.
    eprosima::fastcdr::Cdr ser(fastbuffer, eprosima::fastcdr::Cdr::BIG_ENDIANNESS);
    p_type->serializeKey(ser);
    if (force_md5 || DdsStatus::getKeyMaxCdrSerializedSize() > 16)
    {
        m_md5.init();
        m_md5.update(m_keyBuffer, static_cast<unsigned int>(ser.getSerializedDataLength()));
        m_md5.finalize();
        for (uint8_t i = 0; i < 16; ++i)
        {
            handle->value[i] = m_md5.digest[i];
        }
    }
    else
    {
        for (uint8_t i = 0; i < 16; ++i)
        {
            handle->value[i] = m_keyBuffer[i];
        }
    }
    return true;
}

DdsConfigPubSubType::DdsConfigPubSubType()
{
    setName("DdsConfig");
    m_typeSize = static_cast<uint32_t>(DdsConfig::getMaxCdrSerializedSize()) + 4 /*encapsulation*/;
    m_isGetKeyDefined = DdsConfig::isKeyDefined();
    size_t keyLength = DdsConfig::getKeyMaxCdrSerializedSize() > 16 ?
        DdsConfig::getKeyMaxCdrSerializedSize() : 16;
    m_keyBuffer = reinterpret_cast<unsigned char*>(malloc(keyLength));
    memset(m_keyBuffer, 0, keyLength);
}

DdsConfigPubSubType::~DdsConfigPubSubType()
{
    if (m_keyBuffer != nullptr)
    {
        free(m_keyBuffer);
    }
}

bool DdsConfigPubSubType::serialize(
    void* data,
    SerializedPayload_t* payload)
{
    DdsConfig* p_type = static_cast<DdsConfig*>(data);

    // Object that manages the raw buffer.
    eprosima::fastcdr::FastBuffer fastbuffer(reinterpret_cast<char*>(payload->data), payload->max_size);
    // Object that serializes the data.
    eprosima::fastcdr::Cdr ser(fastbuffer, eprosima::fastcdr::Cdr::DEFAULT_ENDIAN, eprosima::fastcdr::Cdr::DDS_CDR);
    payload->encapsulation = ser.endianness() == eprosima::fastcdr::Cdr::BIG_ENDIANNESS ? CDR_BE : CDR_LE;
    // Serialize encapsulation
    ser.serialize_encapsulation();

    try
    {
        // Serialize the object.
        p_type->serialize(ser);
    }
    catch (eprosima::fastcdr::exception::NotEnoughMemoryException& /*exception*/)
    {
        return false;
    }

    // Get the serialized length
    payload->length = static_cast<uint32_t>(ser.getSerializedDataLength());
    return true;
}

bool DdsConfigPubSubType::deserialize(
    SerializedPayload_t* payload,
    void* data)
{
    //Convert DATA to pointer of your type
    DdsConfig* p_type = static_cast<DdsConfig*>(data);

    // Object that manages the raw buffer.
    eprosima::fastcdr::FastBuffer fastbuffer(reinterpret_cast<char*>(payload->data), payload->length);

    // Object that deserializes the data.
    eprosima::fastcdr::Cdr deser(fastbuffer, eprosima::fastcdr::Cdr::DEFAULT_ENDIAN, eprosima::fastcdr::Cdr::DDS_CDR);

    // Deserialize encapsulation.
    deser.read_encapsulation();
    payload->encapsulation = deser.endianness() == eprosima::fastcdr::Cdr::BIG_ENDIANNESS ? CDR_BE : CDR_LE;

    try
    {
        // Deserialize the object.
        p_type->deserialize(deser);
    }
    catch (eprosima::fastcdr::exception::NotEnoughMemoryException& /*exception*/)
    {
        return false;
    }

    return true;
}

std::function<uint32_t()> DdsConfigPubSubType::getSerializedSizeProvider(
    void* data)
{
    return [data]() -> uint32_t
    {
        return static_cast<uint32_t>(type::getCdrSerializedSize(*static_cast<DdsConfig*>(data))) +
            4u /*encapsulation*/;
    };
}

void* DdsConfigPubSubType::createData()
{
    return reinterpret_cast<void*>(new DdsConfig());
}

void DdsConfigPubSubType::deleteData(
    void* data)
{
    delete(reinterpret_cast<DdsConfig*>(data));
}

bool DdsConfigPubSubType::getKey(
    void* data,
    InstanceHandle_t* handle,
    bool force_md5)
{
    if (!m_isGetKeyDefined)
    {
        return false;
    }

    DdsConfig* p_type = static_cast<DdsConfig*>(data);

    // Object that manages the raw buffer.
    eprosima::fastcdr::FastBuffer fastbuffer(reinterpret_cast<char*>(m_keyBuffer),
        DdsConfig::getKeyMaxCdrSerializedSize());

    // Object that serializes the data.
    eprosima::fastcdr::Cdr ser(fastbuffer, eprosima::fastcdr::Cdr::BIG_ENDIANNESS);
    p_type->serializeKey(ser);
    if (force_md5 || DdsConfig::getKeyMaxCdrSerializedSize() > 16)
    {
        m_md5.init();
        m_md5.update(m_keyBuffer, static_cast<unsigned int>(ser.getSerializedDataLength()));
        m_md5.finalize();
        for (uint8_t i = 0; i < 16; ++i)
        {
            handle->value[i] = m_md5.digest[i];
        }
    }
    else
    {
        for (uint8_t i = 0; i < 16; ++i)
        {
            handle->value[i] = m_keyBuffer[i];
        }
    }
    return true;
}

DdsDataCollectionPubSubType::DdsDataCollectionPubSubType()
{
    setName("DdsDataCollection");
    m_typeSize = static_cast<uint32_t>(DdsDataCollection::getMaxCdrSerializedSize()) + 4 /*encapsulation*/;
    m_isGetKeyDefined = DdsDataCollection::isKeyDefined();
    size_t keyLength = DdsDataCollection::getKeyMaxCdrSerializedSize() > 16 ?
        DdsDataCollection::getKeyMaxCdrSerializedSize() : 16;
    m_keyBuffer = reinterpret_cast<unsigned char*>(malloc(keyLength));
    memset(m_keyBuffer, 0, keyLength);
}

DdsDataCollectionPubSubType::~DdsDataCollectionPubSubType()
{
    if (m_keyBuffer != nullptr)
    {
        free(m_keyBuffer);
    }
}

bool DdsDataCollectionPubSubType::serialize(
    void* data,
    SerializedPayload_t* payload)
{
    DdsDataCollection* p_type = static_cast<DdsDataCollection*>(data);

    // Object that manages the raw buffer.
    eprosima::fastcdr::FastBuffer fastbuffer(reinterpret_cast<char*>(payload->data), payload->max_size);
    // Object that serializes the data.
    eprosima::fastcdr::Cdr ser(fastbuffer, eprosima::fastcdr::Cdr::DEFAULT_ENDIAN, eprosima::fastcdr::Cdr::DDS_CDR);
    payload->encapsulation = ser.endianness() == eprosima::fastcdr::Cdr::BIG_ENDIANNESS ? CDR_BE : CDR_LE;
    // Serialize encapsulation
    ser.serialize_encapsulation();

    try
    {
        // Serialize the object.
        p_type->serialize(ser);
    }
    catch (eprosima::fastcdr::exception::NotEnoughMemoryException& /*exception*/)
    {
        return false;
    }

    // Get the serialized length
    payload->length = static_cast<uint32_t>(ser.getSerializedDataLength());
    return true;
}

bool DdsDataCollectionPubSubType::deserialize(
    SerializedPayload_t* payload,
    void* data)
{
    //Convert DATA to pointer of your type
    DdsDataCollection* p_type = static_cast<DdsDataCollection*>(data);

    // Object that manages the raw buffer.
    eprosima::fastcdr::FastBuffer fastbuffer(reinterpret_cast<char*>(payload->data), payload->length);

    // Object that deserializes the data.
    eprosima::fastcdr::Cdr deser(fastbuffer, eprosima::fastcdr::Cdr::DEFAULT_ENDIAN, eprosima::fastcdr::Cdr::DDS_CDR);

    // Deserialize encapsulation.
    deser.read_encapsulation();
    payload->encapsulation = deser.endianness() == eprosima::fastcdr::Cdr::BIG_ENDIANNESS ? CDR_BE : CDR_LE;

    try
    {
        // Deserialize the object.
        p_type->deserialize(deser);
    }
    catch (eprosima::fastcdr::exception::NotEnoughMemoryException& /*exception*/)
    {
        return false;
    }

    return true;
}

std::function<uint32_t()> DdsDataCollectionPubSubType::getSerializedSizeProvider(
    void* data)
{
    return [data]() -> uint32_t
    {
        return static_cast<uint32_t>(type::getCdrSerializedSize(*static_cast<DdsDataCollection*>(data))) +
            4u /*encapsulation*/;
    };
}

void* DdsDataCollectionPubSubType::createData()
{
    return reinterpret_cast<void*>(new DdsDataCollection());
}

void DdsDataCollectionPubSubType::deleteData(
    void* data)
{
    delete(reinterpret_cast<DdsDataCollection*>(data));
}

bool DdsDataCollectionPubSubType::getKey(
    void* data,
    InstanceHandle_t* handle,
    bool force_md5)
{
    if (!m_isGetKeyDefined)
    {
        return false;
    }

    DdsDataCollection* p_type = static_cast<DdsDataCollection*>(data);

    // Object that manages the raw buffer.
    eprosima::fastcdr::FastBuffer fastbuffer(reinterpret_cast<char*>(m_keyBuffer),
        DdsDataCollection::getKeyMaxCdrSerializedSize());

    // Object that serializes the data.
    eprosima::fastcdr::Cdr ser(fastbuffer, eprosima::fastcdr::Cdr::BIG_ENDIANNESS);
    p_type->serializeKey(ser);
    if (force_md5 || DdsDataCollection::getKeyMaxCdrSerializedSize() > 16)
    {
        m_md5.init();
        m_md5.update(m_keyBuffer, static_cast<unsigned int>(ser.getSerializedDataLength()));
        m_md5.finalize();
        for (uint8_t i = 0; i < 16; ++i)
        {
            handle->value[i] = m_md5.digest[i];
        }
    }
    else
    {
        for (uint8_t i = 0; i < 16; ++i)
        {
            handle->value[i] = m_keyBuffer[i];
        }
    }
    return true;
}

DdsAlarmCollectionPubSubType::DdsAlarmCollectionPubSubType()
{
    setName("DdsAlarmCollection");
    m_typeSize = static_cast<uint32_t>(DdsAlarmCollection::getMaxCdrSerializedSize()) + 4 /*encapsulation*/;
    m_isGetKeyDefined = DdsAlarmCollection::isKeyDefined();
    size_t keyLength = DdsAlarmCollection::getKeyMaxCdrSerializedSize() > 16 ?
        DdsAlarmCollection::getKeyMaxCdrSerializedSize() : 16;
    m_keyBuffer = reinterpret_cast<unsigned char*>(malloc(keyLength));
    memset(m_keyBuffer, 0, keyLength);
}

DdsAlarmCollectionPubSubType::~DdsAlarmCollectionPubSubType()
{
    if (m_keyBuffer != nullptr)
    {
        free(m_keyBuffer);
    }
}

bool DdsAlarmCollectionPubSubType::serialize(
    void* data,
    SerializedPayload_t* payload)
{
    DdsAlarmCollection* p_type = static_cast<DdsAlarmCollection*>(data);

    // Object that manages the raw buffer.
    eprosima::fastcdr::FastBuffer fastbuffer(reinterpret_cast<char*>(payload->data), payload->max_size);
    // Object that serializes the data.
    eprosima::fastcdr::Cdr ser(fastbuffer, eprosima::fastcdr::Cdr::DEFAULT_ENDIAN, eprosima::fastcdr::Cdr::DDS_CDR);
    payload->encapsulation = ser.endianness() == eprosima::fastcdr::Cdr::BIG_ENDIANNESS ? CDR_BE : CDR_LE;
    // Serialize encapsulation
    ser.serialize_encapsulation();

    try
    {
        // Serialize the object.
        p_type->serialize(ser);
    }
    catch (eprosima::fastcdr::exception::NotEnoughMemoryException& /*exception*/)
    {
        return false;
    }

    // Get the serialized length
    payload->length = static_cast<uint32_t>(ser.getSerializedDataLength());
    return true;
}

bool DdsAlarmCollectionPubSubType::deserialize(
    SerializedPayload_t* payload,
    void* data)
{
    //Convert DATA to pointer of your type
    DdsAlarmCollection* p_type = static_cast<DdsAlarmCollection*>(data);

    // Object that manages the raw buffer.
    eprosima::fastcdr::FastBuffer fastbuffer(reinterpret_cast<char*>(payload->data), payload->length);

    // Object that deserializes the data.
    eprosima::fastcdr::Cdr deser(fastbuffer, eprosima::fastcdr::Cdr::DEFAULT_ENDIAN, eprosima::fastcdr::Cdr::DDS_CDR);

    // Deserialize encapsulation.
    deser.read_encapsulation();
    payload->encapsulation = deser.endianness() == eprosima::fastcdr::Cdr::BIG_ENDIANNESS ? CDR_BE : CDR_LE;

    try
    {
        // Deserialize the object.
        p_type->deserialize(deser);
    }
    catch (eprosima::fastcdr::exception::NotEnoughMemoryException& /*exception*/)
    {
        return false;
    }

    return true;
}

std::function<uint32_t()> DdsAlarmCollectionPubSubType::getSerializedSizeProvider(
    void* data)
{
    return [data]() -> uint32_t
    {
        return static_cast<uint32_t>(type::getCdrSerializedSize(*static_cast<DdsAlarmCollection*>(data))) +
            4u /*encapsulation*/;
    };
}

void* DdsAlarmCollectionPubSubType::createData()
{
    return reinterpret_cast<void*>(new DdsAlarmCollection());
}

void DdsAlarmCollectionPubSubType::deleteData(
    void* data)
{
    delete(reinterpret_cast<DdsAlarmCollection*>(data));
}

bool DdsAlarmCollectionPubSubType::getKey(
    void* data,
    InstanceHandle_t* handle,
    bool force_md5)
{
    if (!m_isGetKeyDefined)
    {
        return false;
    }

    DdsAlarmCollection* p_type = static_cast<DdsAlarmCollection*>(data);

    // Object that manages the raw buffer.
    eprosima::fastcdr::FastBuffer fastbuffer(reinterpret_cast<char*>(m_keyBuffer),
        DdsAlarmCollection::getKeyMaxCdrSerializedSize());

    // Object that serializes the data.
    eprosima::fastcdr::Cdr ser(fastbuffer, eprosima::fastcdr::Cdr::BIG_ENDIANNESS);
    p_type->serializeKey(ser);
    if (force_md5 || DdsAlarmCollection::getKeyMaxCdrSerializedSize() > 16)
    {
        m_md5.init();
        m_md5.update(m_keyBuffer, static_cast<unsigned int>(ser.getSerializedDataLength()));
        m_md5.finalize();
        for (uint8_t i = 0; i < 16; ++i)
        {
            handle->value[i] = m_md5.digest[i];
        }
    }
    else
    {
        for (uint8_t i = 0; i < 16; ++i)
        {
            handle->value[i] = m_keyBuffer[i];
        }
    }
    return true;
}

