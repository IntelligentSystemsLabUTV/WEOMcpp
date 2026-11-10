#ifndef CORE_ISTREAM_H
#define CORE_ISTREAM_H

#include "core/stream/imagedata.h"
#include "core/misc/result.h"
#include <chrono>

namespace core
{

template <typename T>
class StreamRequirements
{
protected:
    StreamRequirements()
    {
        static_assert(T::WIDTH_INPUT_STREAM > 0, "Derived class must define WIDTH_INPUT_STREAM as a positive constexpr.");
        static_assert(T::HEIGHT_INPUT_STREAM > 0, "Derived class must define HEIGHT_INPUT_STREAM as a positive constexpr.");
    }
};

class IStream : public StreamRequirements<IStream>
{
public:
    virtual ~IStream() {}

    [[nodiscard]] virtual VoidResult startStream(ImageData::Type type) = 0;
    [[nodiscard]] virtual VoidResult stopStream() = 0;

    virtual void setPathToExecutableFolder(const std::string& path){};

    [[nodiscard]] virtual bool isRunning() const = 0;

    [[nodiscard]] virtual VoidResult readImageData(ImageData& imageData) = 0;

    // Configure before startStream. Implemented by the USB/UART stream.
    virtual void setTimeouts(std::chrono::milliseconds, std::chrono::milliseconds) {}
    // May be called from another thread to interrupt opening or reading a stream.
    virtual void requestStop() {}

    static constexpr uint16_t WIDTH_INPUT_STREAM = 640;
    static constexpr uint16_t HEIGHT_INPUT_STREAM = 480;
};

} // namespace core

#endif // CORE_ISTREAM_H
