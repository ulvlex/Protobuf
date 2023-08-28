#include "Helpers.hpp"

template<typename MessageType>
class DelimitedMessagesStreamParser
{
public:
    typedef std::shared_ptr<const MessageType> PointerToConstValue;

    std::list<PointerToConstValue> parse(const std::string& data) {
        std::list<PointerToConstValue> parsedMessages;

        m_buffer.insert(m_buffer.end(), data.begin(), data.end());
        size_t bytesProcessed = 0; //байты, которые потребовались на разбор сообщения/сообщений

        // Проходим по временному буферу и пытаемся разобрать сообщения
        while (bytesProcessed < m_buffer.size()) {
            size_t bytesConsumed = 0;
            std::shared_ptr<MessageType> parsedMessage = parseDelimited<MessageType>(
                m_buffer.data() + bytesProcessed, //прибавление bytesProcessed (сдвиг) нужно в том случае, если в буфере хранится больше одного сообщения
                m_buffer.size() - bytesProcessed, //аналогично с уменьшением размера
                &bytesConsumed);

            if (parsedMessage) {
                // Если сообщение успешно разобрано, добавляем его в список
                parsedMessages.push_back(parsedMessage);
                bytesProcessed += bytesConsumed; //добавляем количество потребовавшихся для разбора одного сообщения байтов
            }
            else {
                // Если разбор не удался, прерываем проход
                break;
            }
        }

        // Удаляем из временного буфера уже обработанные данные
        m_buffer.erase(m_buffer.begin(), m_buffer.begin() + bytesProcessed);

        return parsedMessages;
    }

private:
    std::vector<char> m_buffer; //временный буфер
};
