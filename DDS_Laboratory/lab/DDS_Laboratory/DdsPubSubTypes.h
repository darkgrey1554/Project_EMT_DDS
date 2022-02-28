#ifndef _DDS_PUBSUBTYPES_H_
#define _DDS_PUBSUBTYPES_H_
#define quote(x) #x

#include <fastrtps/config.h>
#include <fastrtps/TopicDataType.h>
#include <fastrtps/utils/md5.h>
#include <fastcdr/FastBuffer.h>
#include <fastcdr/Cdr.h>



#if !defined(GEN_API_VER) || (GEN_API_VER != 1)
#error Generated HelloWorld is not compatible with current installed Fast-RTPS. Please, regenerate it with fastrtpsgen.
#endif

using SerializedPayload_t = eprosima::fastrtps::rtps::SerializedPayload_t;
using InstanceHandle_t = eprosima::fastrtps::rtps::InstanceHandle_t;

using namespace eprosima::fastrtps;
using namespace eprosima::fastrtps::rtps;

template <typename T>
class DdsPubSubType : public eprosima::fastrtps::TopicDataType {
public:
    typedef T type;
   const char* getNameOfType() {

       std::string s = typeid(T).name();
       std::string delimiter = " ";
       size_t pos = 0;
       std::string token;
       while ((pos = s.find(delimiter)) != std::string::npos) {
           token = s.substr(0, pos);
           std::cout << token << std::endl;
           s.erase(0, pos + delimiter.length());
       }
       return s.c_str();
   }
    eProsima_user_DllExport DdsPubSubType()
    {
     
        setName(getNameOfType());
        m_typeSize = static_cast<uint32_t>(T::getMaxCdrSerializedSize()) + 4 /*encapsulation*/;
        m_isGetKeyDefined = T::isKeyDefined();
        size_t keyLength = T::getKeyMaxCdrSerializedSize() > 16 ? T::getKeyMaxCdrSerializedSize() : 16;
        m_keyBuffer = reinterpret_cast<unsigned char*>(malloc(keyLength));
        memset(m_keyBuffer, 0, keyLength);
    }


    eProsima_user_DllExport virtual ~DdsPubSubType()
    {
        if (m_keyBuffer != nullptr)
            free(m_keyBuffer);
    }
    eProsima_user_DllExport virtual bool serialize(void* data, eprosima::fastrtps::rtps::SerializedPayload_t* payload)
    {
        T* p_type = static_cast<T*>(data);
        eprosima::fastcdr::FastBuffer fastbuffer(reinterpret_cast<char*>(payload->data), payload->max_size); // Object that manages the raw buffer.
        eprosima::fastcdr::Cdr ser(fastbuffer, eprosima::fastcdr::Cdr::DEFAULT_ENDIAN,
            eprosima::fastcdr::Cdr::DDS_CDR); // Object that serializes the data.
        payload->encapsulation = ser.endianness() == eprosima::fastcdr::Cdr::BIG_ENDIANNESS ? CDR_BE : CDR_LE;
        // Serialize encapsulation
        ser.serialize_encapsulation();

        try
        {
            p_type->serialize(ser); // Serialize the object:
        }
        catch (eprosima::fastcdr::exception::NotEnoughMemoryException& /*exception*/)
        {
            return false;
        }

        payload->length = static_cast<uint32_t>(ser.getSerializedDataLength()); //Get the serialized length
        return true;
    }
    eProsima_user_DllExport virtual bool deserialize(eprosima::fastrtps::rtps::SerializedPayload_t* payload, void* data)
    {
        //Convert DATA to pointer of your type
        T* p_type = static_cast<T*>(data);

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

    eProsima_user_DllExport virtual std::function<uint32_t()> getSerializedSizeProvider(void* data) override
    {
        return [data]() -> uint32_t
        {
            return static_cast<uint32_t>(type::getCdrSerializedSize(*static_cast<T*>(data))) +
                4u /*encapsulation*/;
        };
    }
    eProsima_user_DllExport  bool getKey(void* data, eprosima::fastrtps::rtps::InstanceHandle_t* handle, bool force_md5 = false)
    {
        if (!m_isGetKeyDefined)
        {
            return false;
        }

        T* p_type = static_cast<T*>(data);

        // Object that manages the raw buffer.
        eprosima::fastcdr::FastBuffer fastbuffer(reinterpret_cast<char*>(m_keyBuffer), T::getKeyMaxCdrSerializedSize());

        // Object that serializes the data.
        eprosima::fastcdr::Cdr ser(fastbuffer, eprosima::fastcdr::Cdr::BIG_ENDIANNESS);
        p_type->serializeKey(ser);
        if (force_md5 || T::getKeyMaxCdrSerializedSize() > 16)
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
    eProsima_user_DllExport virtual void* createData()
    {
        return reinterpret_cast<void*>(new T());
    }

    eProsima_user_DllExport virtual void deleteData(void* data)
    {
        delete(reinterpret_cast<T*>(data));
    }
#ifdef TOPIC_DATA_TYPE_API_HAS_IS_BOUNDED
    eProsima_user_DllExport inline bool is_bounded() const
    {
        return false;
    }

#endif  // TOPIC_DATA_TYPE_API_HAS_IS_BOUNDED

#ifdef TOPIC_DATA_TYPE_API_HAS_IS_PLAIN
    eProsima_user_DllExport inline bool is_plain() const override
    {
        return false;
    }

#endif  // TOPIC_DATA_TYPE_API_HAS_IS_PLAIN

#ifdef TOPIC_DATA_TYPE_API_HAS_CONSTRUCT_SAMPLE
    eProsima_user_DllExport inline bool construct_sample(void* memory) const override
    {
        (void)memory;
        return false;
    }

#endif  // TOPIC_DATA_TYPE_API_HAS_CONSTRUCT_SAMPLE

    MD5 m_md5;
    unsigned char* m_keyBuffer;
};


#endif // _DDS_PUBSUBTYPES_H_
