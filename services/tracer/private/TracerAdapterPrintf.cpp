#include "services/tracer/public/TracerAdapterPrintf.hpp"

namespace services
{
    TracerAdapterPrintf::TracerAdapterPrintf(services::Tracer& tracer)
        : tracer(tracer)
    {}

    int TracerAdapterPrintf::Print(const char* format, va_list args)
    {
        if (format == nullptr)
            return 0;

        for (; *format != 0; ++format)
        {
            if (*format == '%')
            {
                int lengthSpecifier = 0;
                char paddingCharacter = ' ';
                infra::Optional<int> precision;

                ++format;

                if (*format == '0')
                {
                    paddingCharacter = '0';
                    ++format;
                }

                while (*format == 'l')
                {
                    ++lengthSpecifier;
                    ++format;
                }

                while (*format > '0' && *format <= '9')
                    ++format;

                if (*format == '.')
                {
                    ++format;
                    while (*format > '0' && *format <= '9')
                        ++format;
                }

                switch (*format)
                {
                case '\0': break;
                case '%':
                    tracer.Continue() << *format;
                    break;
                case 'c':
                    tracer.Continue() << static_cast<const char>(va_arg(args, int32_t));
                    break;
                case 's':
                {
                    const char* s = reinterpret_cast<const char*>(va_arg(args, int32_t));
                    tracer.Continue() << (s ? s : "(null)");
                    break;
                }
                case 'd':
                case 'i':
                    if (lengthSpecifier >= 2)
                        tracer.Continue() << va_arg(args, int64_t);
                    else if (lengthSpecifier == 1)
                        tracer.Continue() << va_arg(args, int32_t);
                    else
                        tracer.Continue() << va_arg(args, int32_t);
                    break;
                case 'u':
                    if (lengthSpecifier >= 2)
                        tracer.Continue() << va_arg(args, uint64_t);
                    else if (lengthSpecifier == 1)
                        tracer.Continue() << va_arg(args, uint32_t);
                    else
                        tracer.Continue() << va_arg(args, uint32_t);
                    break;
                case 'p':
                case 'X':
                    tracer.Continue() << "0x";
                case 'x':
                    if (lengthSpecifier >= 2)
                        tracer.Continue() << infra::hex << va_arg(args, uint64_t);
                    else if (lengthSpecifier == 1)
                        tracer.Continue() << infra::hex << va_arg(args, uint32_t);
                    else
                        tracer.Continue() << infra::hex << va_arg(args, uint32_t);
                    break;
                case 'f':
                    tracer.Continue() << static_cast<float>(va_arg(args, double));
                    break;
                default:
                    while (lengthSpecifier-- > 0)
                        tracer.Continue() << 'l';
                    tracer.Continue() << *format;
                    break;
                }
            }
            else
            {
                if (*format == '\n')
                    tracer.Trace();
                else
                    tracer.Continue() << *format;
            }
        }

        return 0;
    }
}
