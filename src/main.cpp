#include <iostream>
#include "protobuf_parser/DelimitedMessagesStreamParser.hpp" 
#include "message.pb.h"

int main() {
    typedef std::vector Data; 
    typedef std::shared_ptr PointerToConstData; 
    typedef std::shared_ptr PointerToData;

    //Создаём два вида сообщения SlowResponse для примера
    TestTask::Messages::SlowResponse slowResponse1;
    slowResponse1.set_connected_client_count(11); //заполняем данными 

    TestTask::Messages::SlowResponse slowResponse2;
    slowResponse2.set_connected_client_count(12);

    //сериализуем в поток байт
    PointerToConstData serializedSlow1Response = serializeDelimited(slowResponse1);
    PointerToConstData serializedSlow2Response = serializeDelimited(slowResponse2);

    //добавляем в строку, чтобы отправить в parse
    std::string stream;
    stream.append(std::string(serializedSlow1Response->begin(), serializedSlow1Response->end()));
    stream.append(std::string(serializedSlow2Response->begin(), serializedSlow2Response->end()));

    //создаём объект класса парсинг
    typedef DelimitedMessagesStreamParser<TestTask::Messages::WrapperMessage> Parser;
    Parser parser;

    // идём по одному байту по входному потоку сообщений
    for (const char byte : stream) {
        const std::list<Parser::PointerToConstValue>& parsedMessages = parser.parse(std::string(1, byte));
        for (const Parser::PointerToConstValue& value : parsedMessages)
        {
            //выводим информацию для проверки
            std::cout << value->DebugString();
        }
    }
}