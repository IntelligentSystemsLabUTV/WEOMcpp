#ifndef CORE_CONNECTION_DATALINKUART_H
#define CORE_CONNECTION_DATALINKUART_H

#include "core/connection/asiodatalinkwithbaudrateandstreamsource.h"
#include "core/device.h"
#include "core/connection/serialportinfo.h"

#include <memory>
#include <chrono>
#include <atomic>

struct AVFormatContext;
struct AVInputFormat;
struct AVCodecContext;
struct AVCodec;
struct SwsContext;
struct AVPacket;

namespace core
{
namespace connection
{

class DataLinkUart final : public AsioDataLinkWithBaudrateAndStreamSource
{
private:
    explicit DataLinkUart(const SerialPortInfo& portInfo);

public:
    virtual ~DataLinkUart() override;

    virtual bool isOpened() const override;
    virtual size_t getMaxDataSize() const override;
    [[nodiscard]] virtual ValueResult<Baudrate::Item> getBaudrate() const override;
    [[nodiscard]] virtual VoidResult setBaudrate(Baudrate::Item baudrate) override;

    const SerialPortInfo& getPortInfo() const;

    [[nodiscard]] static ValueResult<std::shared_ptr<DataLinkUart>> createConnection(const SerialPortInfo& portInfo, Baudrate::Item baudrate);

private:
    class UartStream : public IStream
    {
    public:
        explicit UartStream(const std::string& deviceName, const std::string inputFormat);
        virtual ~UartStream();
        [[nodiscard]] virtual VoidResult startStream(ImageData::Type dataType) override;
        [[nodiscard]] virtual VoidResult stopStream() override;
        [[nodiscard]] virtual bool isRunning() const override;
        [[nodiscard]] virtual VoidResult readImageData(ImageData& imageData) override;
        void setTimeouts(std::chrono::milliseconds openTimeout, std::chrono::milliseconds readTimeout) override;
        void requestStop() override;

        [[nodiscard]] static ValueResult<std::shared_ptr<UartStream>> createStream(const std::string& deviceName, const std::string inputFormat);
        static constexpr uint16_t WIDTH_INPUT_STREAM = 640;
        static constexpr uint16_t HEIGHT_INPUT_STREAM = 480;

    private:
        [[nodiscard]] VoidResult getFrameData(std::vector<uint8_t>& data);
        [[nodiscard]] VoidResult getRgbFrameData(std::vector<uint8_t>& data);
        int readPacket(AVPacket* packet);
        void setDeadline(std::chrono::milliseconds timeout);
        static int interruptCallback(void* opaque);

        std::string m_deviceName;
        std::string m_inputFormat;
        std::mutex m_streamMutex;
        AVFormatContext* m_inputContext{nullptr};
        AVCodecContext* m_codecContext{nullptr};
        SwsContext* m_convertContext{nullptr};
        ImageData::Type m_dataType;
        std::atomic<bool> m_stopRequested{false};
        std::atomic<int64_t> m_deadlineMs{0};
        std::chrono::milliseconds m_openTimeout{3000};
        std::chrono::milliseconds m_readTimeout{1000};
    };

    void closeConnectionImpl() override;
    bool isConnectionLostIndicator(boost::system::error_code errorCode) const override;
    ValueResult<std::shared_ptr<IStream>> createNewStream() override;

    bool readAsync(std::chrono::steady_clock::duration timeout,
                   std::span<uint8_t> buffer,
                   boost::system::error_code& errorCode,
                   size_t& transferedSize) override;

    bool writeAsync(std::chrono::steady_clock::duration timeout,
                    std::span<const uint8_t> buffer,
                    boost::system::error_code& errorCode,
                    size_t& transferedSize) override;

    template<typename T>
    bool doAsync(std::chrono::steady_clock::duration timeout,
                 std::span<T> buffer,
                 boost::system::error_code& errorCode,
                 size_t& transferedSize);


    std::pair<std::string, std::string> getVideoDeviceNameWithFormat() const;

private:
    SerialPortInfo m_portInfo;
    std::unique_ptr<boost::asio::serial_port> m_serialPort;
};

} // namespace connection
} // namespace core

#endif // CORE_CONNECTION_DATALINKUART_H
