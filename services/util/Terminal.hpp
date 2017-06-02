#ifndef SERVICES_TERMINAL_HPP
#define SERVICES_TERMINAL_HPP

#include "hal/interfaces/SerialCommunication.hpp"
#include "infra/event/QueueForOneReaderOneIrqWriter.hpp"
#include "infra/util/BoundedDeque.hpp"
#include "infra/util/BoundedString.hpp"

namespace services
{
    class Terminal
    {
    public:
        Terminal(hal::SerialCommunication& communication);

        void Print(const char* message);
        virtual void OnData(infra::BoundedString data) {}

    private:
        void HandleBytes();
        void HandleByte(uint8_t byte);

        bool ProcessEscapeSequence(uint8_t byte);
        void ProcessEnter();
        void ProcessBackspace();
        void ProcessDelete();

        void EraseCharacterUnderCursor();
        void MoveCursorHome();
        void MoveCursorEnd();
        void MoveCursorLeft();
        void MoveCursorRight();

        void OverwriteBuffer(infra::BoundedConstString element);
        void StoreHistory(infra::BoundedString element);
        void HistoryForward();
        void HistoryBackward();

        void SendBell();

    private:
        struct TerminalState
        {
            const char* prompt = "> ";
            bool processingEscapeSequence = false;
            uint32_t cursorPosition = 0;
            uint32_t historyIndex = 0;
        };

    private:
        TerminalState state;
        hal::SerialCommunication& communication;
        infra::QueueForOneReaderOneIrqWriter::WithStorage<32> queue;
        infra::BoundedString::WithStorage<256> buffer;
        infra::BoundedDeque<decltype(buffer)>::WithMaxSize<4> history;
    };
}

#endif
