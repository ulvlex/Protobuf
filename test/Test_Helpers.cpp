#include <protobuf_parser/helpers.hpp>
#include "message.pb.h"
#include <gtest/gtest.h>

TEST(ParseDelimited, FastResponse)
{
    std::string currentDateTime = "19851019T050107.333";

    TestTask::Messages::WrapperMessage message;
    message.mutable_fast_response()->set_current_date_time(currentDateTime);

    PointerToConstData data = serializeDelimited(message);

    std::shared_ptr<TestTask::Messages::WrapperMessage> parsedMessage = std::make_shared<TestTask::Messages::WrapperMessage>(TestTask::Messages::WrapperMessage());

    size_t bytesConsumed = 0;
    parsedMessage = parseDelimited<TestTask::Messages::WrapperMessage>(
        data->data(),
        data->size(),
        &bytesConsumed
        );
    
    ASSERT_EQ(bytesConsumed, data->size());
    ASSERT_EQ(parsedMessage->fast_response().current_date_time(), currentDateTime);
}

TEST(ParseDelimited, SlowResponse)
{
    uint32_t connectedClientCount = 15;

    TestTask::Messages::WrapperMessage message;
    message.mutable_slow_response()->set_connected_client_count(connectedClientCount);

    PointerToConstData data = serializeDelimited(message);

    std::shared_ptr<TestTask::Messages::WrapperMessage> parsedMessage = std::make_shared<TestTask::Messages::WrapperMessage>(TestTask::Messages::WrapperMessage());

    size_t bytesConsumed = 0;
    parsedMessage = parseDelimited<TestTask::Messages::WrapperMessage>(
        data->data(),
        data->size(),
        &bytesConsumed
        );

    ASSERT_EQ(bytesConsumed, data->size());
    ASSERT_EQ(parsedMessage->slow_response().connected_client_count(), connectedClientCount);
}

TEST(ParseDelimited, RequestForFastResponse)
{

    TestTask::Messages::WrapperMessage message;
    message.mutable_request_for_fast_response();

    PointerToConstData data = serializeDelimited(message);

    std::shared_ptr<TestTask::Messages::WrapperMessage> parsedMessage = std::make_shared<TestTask::Messages::WrapperMessage>(TestTask::Messages::WrapperMessage());

    size_t bytesConsumed = 0;
    parsedMessage = parseDelimited<TestTask::Messages::WrapperMessage>(
        data->data(),
        data->size(),
        &bytesConsumed
        );

    ASSERT_EQ(bytesConsumed, data->size());
    EXPECT_TRUE(parsedMessage->has_request_for_fast_response());
}

TEST(ParseDelimited, RequestForSlowResponse)
{
    uint32_t timeInSecondToSleep = 15;

    TestTask::Messages::WrapperMessage message;
    message.mutable_request_for_slow_response()->set_time_in_seconds_to_sleep(timeInSecondToSleep);

    PointerToConstData data = serializeDelimited(message);

    std::shared_ptr<TestTask::Messages::WrapperMessage> parsedMessage = std::make_shared<TestTask::Messages::WrapperMessage>(TestTask::Messages::WrapperMessage());

    size_t bytesConsumed = 0;
    parsedMessage = parseDelimited<TestTask::Messages::WrapperMessage>(
        data->data(),
        data->size(),
        &bytesConsumed
        );

    ASSERT_EQ(bytesConsumed, data->size());
    ASSERT_EQ(parsedMessage->request_for_slow_response().time_in_seconds_to_sleep(), timeInSecondToSleep);
}

TEST(ParseDelimited, NullData) {
    std::shared_ptr<TestTask::Messages::WrapperMessage> parsedMessage = std::make_shared<TestTask::Messages::WrapperMessage>(TestTask::Messages::WrapperMessage());

    size_t bytesConsumed = 0;
    parsedMessage = parseDelimited<TestTask::Messages::WrapperMessage>(
        nullptr,
        0,
        &bytesConsumed
        );

    ASSERT_EQ(bytesConsumed, 0);
    ASSERT_TRUE(parsedMessage == nullptr);
}

TEST(ParseDelimited, WrongMessageSize)
{
    uint32_t timeInSecondToSleep = 15;

    TestTask::Messages::WrapperMessage message;
    message.mutable_request_for_slow_response()->set_time_in_seconds_to_sleep(timeInSecondToSleep);

    PointerToConstData data = serializeDelimited(message);

    std::shared_ptr<TestTask::Messages::WrapperMessage> parsedMessage = std::make_shared<TestTask::Messages::WrapperMessage>(TestTask::Messages::WrapperMessage());

    size_t bytesConsumed = 0;
    parsedMessage = parseDelimited<TestTask::Messages::WrapperMessage>(
        data->data(),
        data->size()*2,
        &bytesConsumed
        );

    ASSERT_EQ(bytesConsumed, data->size());
    ASSERT_EQ(parsedMessage->request_for_slow_response().time_in_seconds_to_sleep(), timeInSecondToSleep);

    bytesConsumed = 0;
    parsedMessage = parseDelimited<TestTask::Messages::WrapperMessage>(
        data->data(),
        data->size() / 2,
        &bytesConsumed
        );

    ASSERT_EQ(bytesConsumed, 0);
    ASSERT_TRUE(parsedMessage == nullptr);
}