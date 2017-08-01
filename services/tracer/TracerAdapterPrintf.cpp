#include "services/tracer/TracerAdapterPrintf.hpp"

namespace services
{
    TracerAdapterPrintf::TracerAdapterPrintf(services::Tracer& tracer)
        : tracer(tracer)
    {}

    void TracerAdapterPrintf::Print(const char* format, va_list args)
    {
        for (; *format != 0; ++format)
        {
            if (*format == '%')
                HandleFormat(format, args);
            else if (*format == '\n')
                tracer.Trace();
            else
                tracer.Continue() << *format;
        }
    }

    void TracerAdapterPrintf::HandleFormat(const char*& format, va_list& args)
    {
        ++format;

        if (*format == '0')
            ++format;

        int lengthSpecifier = ReadLength(format);
        SkipSize(format);
        ParseFormat(*format, lengthSpecifier, args);
    }

    int TracerAdapterPrintf::ReadLength(const char*& format) const
    {
        int lengthSpecifier = 0;

        while (*format == 'l')
        {
            ++lengthSpecifier;
            ++format;
        }

        return lengthSpecifier;
    }

    void TracerAdapterPrintf::SkipSize(const char*& format) const
    {
        while (*format > '0' && *format <= '9')
            ++format;

        if (*format == '.')
        {
            ++format;
            while (*format > '0' && *format <= '9')
                ++format;
        }
    }

    void TracerAdapterPrintf::ParseFormat(char format, int lengthSpecifier, va_list& args)
    {
        switch (format)
        {
            case '\0': break;
            case '%':
                tracer.Continue() << format;
                break;
            case 'c':
                tracer.Continue() << static_cast<const char>(va_arg(args, int32_t));
                break;
            case 's':
            {
                const char* s = reinterpret_cast<const char*>(va_arg(args, int32_t));
                tracer.Continue() << (s != nullptr ? s : "(null)");
                break;
            }
            case 'd':
            case 'i':
                if (lengthSpecifier >= 2)
                    tracer.Continue() << va_arg(args, int64_t);
                else
                    tracer.Continue() << va_arg(args, int32_t);
                break;
            case 'u':
                if (lengthSpecifier >= 2)
                    tracer.Continue() << va_arg(args, uint64_t);
                else
                    tracer.Continue() << va_arg(args, uint32_t);
                break;
            case 'p':                                                                                               //TICS !CFL#001
            case 'X':
                tracer.Continue() << "0x";
            case 'x':
                if (lengthSpecifier >= 2)
                    tracer.Continue() << infra::hex << va_arg(args, uint64_t);
                else
                    tracer.Continue() << infra::hex << va_arg(args, uint32_t);
                break;
            case 'f':
                tracer.Continue() << static_cast<float>(va_arg(args, double));
                break;
            default:
                while (lengthSpecifier-- > 0)
                    tracer.Continue() << 'l';
                tracer.Continue() << format;
                break;
        }
    }
}
