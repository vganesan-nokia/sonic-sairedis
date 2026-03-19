#pragma once

#include "Channel.h"

#include "swss/producertable.h"
#include "swss/consumertable.h"
#include "swss/notificationconsumer.h"
#include "swss/selectableevent.h"
#include "swss/asyncdbupdater.h"
#include "swss/dbconnector.h"

#include <memory>
#include <functional>

#define ZMQ_RESPONSE_DEFAULT_BUFFER_SIZE (4*1024*1024)

namespace sairedis
{
    class ZeroMQChannel:
        public Channel
    {
        public:

            ZeroMQChannel(
                    _In_ const std::string& endpoint,
                    _In_ const std::string& ntfEndpoint,
                    _In_ Channel::Callback callback,
                    _In_ long zmqResponseBufferSize = ZMQ_RESPONSE_DEFAULT_BUFFER_SIZE,
                    _In_ const std::string& dbName = "ASIC_DB",
                    _In_ bool dbPersistence = true);

            virtual ~ZeroMQChannel();

        public:

            virtual void setBuffered(
                    _In_ bool buffered) override;

            virtual void flush() override;

            virtual void set(
                    _In_ const std::string& key,
                    _In_ const std::vector<swss::FieldValueTuple>& values,
                    _In_ const std::string& command) override;

            virtual void del(
                    _In_ const std::string& key,
                    _In_ const std::string& command) override;

            virtual sai_status_t wait(
                    _In_ const std::string& command,
                    _Out_ swss::KeyOpFieldsValuesTuple& kco) override;

        protected:

            virtual void notificationThreadFunction() override;

        private:

            std::string m_endpoint;

            std::string m_ntfEndpoint;

            std::vector<uint8_t> m_buffer;

            void* m_context;

            void* m_socket;

            void* m_ntfContext;

            void* m_ntfSocket;

            long m_zmqResponseBufferSize;

            bool m_dbPersistence;

            std::unique_ptr<swss::DBConnector> m_db;

            std::unique_ptr<swss::AsyncDBUpdater> m_asyncDBUpdater;
    };
}
