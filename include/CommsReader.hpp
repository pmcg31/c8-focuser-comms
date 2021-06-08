#pragma once

namespace ELS
{

    class CommsReader
    {
    public:
        virtual const char *readLine() = 0;
        virtual void close() = 0;
    };

}