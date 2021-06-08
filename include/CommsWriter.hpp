#pragma once

namespace ELS
{

    class CommsWriter
    {
    public:
        virtual bool writeLine(const char *line) = 0;
        virtual void close() = 0;
    };

}