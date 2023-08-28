#include <protobuf_parser/DelimitedMessagesStreamParser.hpp>
#include "message.pb.h"
#include <gtest/gtest.h>

TEST(DelimitedMessagesStreamParser, OneMessage)
{
    std::string currentDateTime = "19851019T050107.333";

    TestTask::Messages::WrapperMessage message;
    message.mutable_fast_response()->set_current_date_time(currentDateTime);

    PointerToConstData data = serializeDelimited(message);

    std::list<typename DelimitedMessagesStreamParser<TestTask::Messages::WrapperMessage>::PointerToConstValue> messages;
    DelimitedMessagesStreamParser<TestTask::Messages::WrapperMessage> parser;
    messages = parser.parse(std::string(data->begin(), data->end()));

    ASSERT_EQ(1, messages.size());
    for (const auto& message : messages) {
        ASSERT_EQ(message->fast_response().current_date_time(), currentDateTime);
    }
}

TEST(DelimitedMessagesStreamParser, TwoIdenticalMessages)
{
    std::string currentDateTimeFirst = "19851019T050107.333";
    std::string currentDateTimeSecond = "19851019T050107.666";

    TestTask::Messages::WrapperMessage messageFirst;
    messageFirst.mutable_fast_response()->set_current_date_time(currentDateTimeFirst);

    TestTask::Messages::WrapperMessage messageSecond;
    messageSecond.mutable_fast_response()->set_current_date_time(currentDateTimeSecond);

    PointerToConstData serializedFastResponseFirst = serializeDelimited(messageFirst);
    PointerToConstData serializedFastResponseSecond = serializeDelimited(messageSecond);

    std::string stream;
    stream.append(std::string(serializedFastResponseFirst->begin(), serializedFastResponseFirst->end()));
    stream.append(std::string(serializedFastResponseSecond->begin(), serializedFastResponseSecond->end()));

    std::list<typename DelimitedMessagesStreamParser<TestTask::Messages::WrapperMessage>::PointerToConstValue> messages;
    DelimitedMessagesStreamParser<TestTask::Messages::WrapperMessage> parser;
    messages = parser.parse(stream);

    ASSERT_EQ(2, messages.size());
    size_t cnt = 0;
    for (const auto& message : messages) {
        if (cnt == 0) {
            ASSERT_EQ(message->fast_response().current_date_time(), currentDateTimeFirst);
            ++cnt;
        }
        else
            ASSERT_EQ(message->fast_response().current_date_time(), currentDateTimeSecond);
    }
}

TEST(DelimitedMessagesStreamParser, TwoDifferentMessages)
{
    std::string currentDateTimeFirst = "19851019T050107.333";
    size_t timeInSecondsToSleep = 15;

    TestTask::Messages::WrapperMessage messageFirst;
    messageFirst.mutable_fast_response()->set_current_date_time(currentDateTimeFirst);

    TestTask::Messages::WrapperMessage messageSecond;
    messageSecond.mutable_request_for_slow_response()->set_time_in_seconds_to_sleep(timeInSecondsToSleep);

    PointerToConstData serializedFastResponseFirst = serializeDelimited(messageFirst);
    PointerToConstData serializedFastResponseSecond = serializeDelimited(messageSecond);

    std::string stream;
    stream.append(std::string(serializedFastResponseFirst->begin(), serializedFastResponseFirst->end()));
    stream.append(std::string(serializedFastResponseSecond->begin(), serializedFastResponseSecond->end()));

    std::list<typename DelimitedMessagesStreamParser<TestTask::Messages::WrapperMessage>::PointerToConstValue> messages;
    DelimitedMessagesStreamParser<TestTask::Messages::WrapperMessage> parser;
    messages = parser.parse(stream);

    ASSERT_EQ(2, messages.size());
    int cnt = 0;
    for (const auto& message : messages) {
        if (cnt == 0) {
            ASSERT_EQ(message->fast_response().current_date_time(), currentDateTimeFirst);
            ++cnt;
        }
        else
            ASSERT_EQ(message->request_for_slow_response().time_in_seconds_to_sleep(), timeInSecondsToSleep);
    }
}

TEST(DelimitedMessagesStreamParser, PartOfTheMessage)
{
    std::string currentDateTime = "19851019T050107.333";

    TestTask::Messages::WrapperMessage message;
    message.mutable_fast_response()->set_current_date_time(currentDateTime);

    PointerToConstData data = serializeDelimited(message);

    std::list<typename DelimitedMessagesStreamParser<TestTask::Messages::WrapperMessage>::PointerToConstValue> messages;
    DelimitedMessagesStreamParser<TestTask::Messages::WrapperMessage> parser;
    messages = parser.parse(std::string(data->begin(), data->begin() + currentDateTime.size() / 2));

    ASSERT_EQ(0, messages.size());
}

TEST(DelimitedMessagesStreamParser, OneAndAHalfMessages)
{
    std::string currentDateTimeFirst = "19851019T050107.333";
    std::string currentDateTimeSecond = "19851019T050107.666";

    TestTask::Messages::WrapperMessage messageFirst;
    messageFirst.mutable_fast_response()->set_current_date_time(currentDateTimeFirst);

    TestTask::Messages::WrapperMessage messageSecond;
    messageSecond.mutable_fast_response()->set_current_date_time(currentDateTimeSecond);

    PointerToConstData serializedFastResponseFirst = serializeDelimited(messageFirst);
    PointerToConstData serializedFastResponseSecond = serializeDelimited(messageSecond);

    std::string stream;
    stream.append(std::string(serializedFastResponseFirst->begin(), serializedFastResponseFirst->end()));
    stream.append(std::string(serializedFastResponseSecond->begin(), serializedFastResponseSecond->begin() + currentDateTimeSecond.size() / 2));

    std::list<typename DelimitedMessagesStreamParser<TestTask::Messages::WrapperMessage>::PointerToConstValue> messages;
    DelimitedMessagesStreamParser<TestTask::Messages::WrapperMessage> parser;
    messages = parser.parse(stream);

    ASSERT_EQ(1, messages.size());
    size_t cnt = 0;
    for (const auto& message : messages) {
        if (cnt == 0) {
            ASSERT_EQ(message->fast_response().current_date_time(), currentDateTimeFirst);
            ++cnt;
        }
    }
}

TEST(DelimitedMessagesStreamParser, EmptyMessage)
{
    std::string currentDateTime = "";

    std::list<typename DelimitedMessagesStreamParser<TestTask::Messages::WrapperMessage>::PointerToConstValue> messages;
    DelimitedMessagesStreamParser<TestTask::Messages::WrapperMessage> parser;
    messages = parser.parse(currentDateTime);

    EXPECT_EQ(0, messages.size());
}

TEST(DelimitedMessagesStreamParser, IncorrectMessageFormat)
{
    std::string currentDateTime = "19851019T050107.333";
    std::string wrongString = "\a";

    TestTask::Messages::WrapperMessage message;
    message.mutable_fast_response()->set_current_date_time(currentDateTime);

    PointerToConstData data = serializeDelimited(message);

    DelimitedMessagesStreamParser<TestTask::Messages::WrapperMessage> parser;

    EXPECT_THROW(parser.parse(wrongString + std::string(data->begin(), data->end())), std::runtime_error);
}