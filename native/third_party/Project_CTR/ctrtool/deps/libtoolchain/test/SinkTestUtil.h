#pragma once

#include <tc/io/ISink.h>
#include <tc/NotImplementedException.h>
#include <tc/ArgumentOutOfRangeException.h>

class SinkTestUtil
{
public:
	static void testSinkLength(tc::io::ISink& source, int64_t expected_len);
	
	class DummySinkBase : public tc::io::ISink
	{
	public:
		DummySinkBase();
		DummySinkBase(int64_t length);
		DummySinkBase(int64_t length, bool canSetLength);

		void init(int64_t length, bool canSetLength);

		int64_t length();
		void setLength(int64_t length);
		virtual size_t pushData(const tc::ByteData& data, int64_t offset);
	private:
		static const std::string kClassName;
		bool mCanSetLength;
		int64_t mLength;
	};

	class DummySinkTestablePushData : public DummySinkBase
	{
	public:
		DummySinkTestablePushData();

		void setExpectedPushDataCfg(const tc::ByteData& data, int64_t offset);

		size_t pushData(const tc::ByteData& data, int64_t offset);
	private:
		std::shared_ptr<tc::ByteData> expected_data;
		std::shared_ptr<int64_t> expected_offset;
	};
};
