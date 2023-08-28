#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/message_lite.h>
#include <vector>
#include <list>

//вспомогательная функция, которая сериализует сообщение в потом байт с добавлением длины перед сообщением 
//нужна для тестов
#if GOOGLE_PROTOBUF_VERSION >= 3012004
#define PROTOBUF_MESSAGE_BYTE_SIZE(message) (message.ByteSizeLong())
#else
#define PROTOBUF_MESSAGE_BYTE_SIZE(message) (message.ByteSize())
#endif

typedef std::vector<char> Data;
typedef std::shared_ptr<const Data> PointerToConstData;
typedef std::shared_ptr<Data> PointerToData;

template <typename Message> 
PointerToConstData serializeDelimited(const Message& msg)
{
    const size_t messageSize = PROTOBUF_MESSAGE_BYTE_SIZE(msg);
    const size_t headerSize = google::protobuf::io::CodedOutputStream::VarintSize32(messageSize);

    const PointerToData& result = std::make_shared<Data>(headerSize + messageSize);
    google::protobuf::uint8* buffer = reinterpret_cast<google::protobuf::uint8*>(&*result->begin());

    google::protobuf::io::CodedOutputStream::WriteVarint32ToArray(messageSize, buffer);
    msg.SerializeWithCachedSizesToArray(buffer + headerSize);

    return result;
}

template<typename Message>
std::shared_ptr<Message> parseDelimited(const void* data, size_t size, size_t* bytesConsumed = 0) {

    if (size == 0 || !data) {
        return nullptr;
    }

    //Преобразуем указатель на данные в указатель на const uint8_t
    const uint8_t* buffer = static_cast<const uint8_t*>(data);

    uint32_t messageSize = 0; //размер самого сообщения (без учета дополнительных байтов для длины-префикса)
    size_t headerSize = 0; //размер (length-prefixed)

    // Создаем CodedInputStream, который будет использоваться для разбора сообщения
    google::protobuf::io::CodedInputStream inputStream(buffer, size);

    // Читаем размер сообщения (без учёта длины-префикса) из потока данных 
    if (!inputStream.ReadVarint32(&messageSize)) {
        // Если не удалось прочитать размер сообщения, возможно поток данных слишком короткий
        if (bytesConsumed) 
            *bytesConsumed = 0;
        return nullptr;
    }

    //если прочитали размер без префикса, вычисляем длину-префикса - это количество байт, 
    //необходимых для хранения размера сообщения (Varint-переменной длины), 
    //которое записывается перед самим сообщением.
    headerSize = google::protobuf::io::CodedOutputStream::VarintSize32(messageSize);

    // Проверяем, что есть достаточно данных для прочтения всего сообщения
    if (messageSize > inputStream.BytesUntilLimit()) {
        // Недостаточно данных, откатываем читаемый поток на начало
        inputStream.PopLimit(inputStream.PushLimit(size));
        if (bytesConsumed) 
            *bytesConsumed = 0;
        return nullptr;
    }

    // Разбираем сообщение из потока данных
    std::shared_ptr<Message> parsedMessage = std::make_shared<Message>(Message());
    std::string bytes;

    if (inputStream.ReadString(&bytes, messageSize) && parsedMessage->ParseFromString(bytes)) {
        //если удалось разобрать сообщение
        if (bytesConsumed)
            *bytesConsumed = messageSize + headerSize;
        std::cout << "Parsing done." << std::endl;
        return parsedMessage; //возвращаем распарсенное сообщение
    }
    else {
        // Если не удалось разобрать сообщение, возвращаем пустой указатель
        inputStream.PopLimit(inputStream.PushLimit(size));
        if (bytesConsumed)
            *bytesConsumed = 0;
        throw std::runtime_error("Failed to parse message");
    }
}