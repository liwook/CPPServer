#pragma once
#include<string>
class Timestamp
{
public:
	Timestamp()
		:micro_seconds_since_epoch_(0)
	{
	}
	explicit Timestamp(int64_t microSecondsSinceEpoch)
		:micro_seconds_since_epoch_(microSecondsSinceEpoch)
	{
	}
	static Timestamp now();
	static Timestamp invalid()
	{
		return Timestamp();
	}
	bool valid() const { return micro_seconds_since_epoch_ > 0; }

	std::string toString()const;
	std::string toFormattedString(bool showMicroseconds = true) const;

	int64_t microSecondsSinceEpoch() const { return micro_seconds_since_epoch_; }

	static const int kMicroSecondsPerSecond = 1000 * 1000;
private:
	int64_t micro_seconds_since_epoch_;
};

inline bool operator<(Timestamp lhs, Timestamp rhs)
{
	return lhs.microSecondsSinceEpoch() < rhs.microSecondsSinceEpoch();
}

inline bool operator==(Timestamp lhs, Timestamp rhs)
{
	return lhs.microSecondsSinceEpoch() == rhs.microSecondsSinceEpoch();
}

///
/// Gets time difference of two timestamps, result in seconds.
///
/// @param high, low
/// @return (high-low) in seconds
/// @c double has 52-bit precision, enough for one-microsecond
/// resolution for next 100 years.
inline double timeDifference(Timestamp high, Timestamp low)
{
	int64_t diff = high.microSecondsSinceEpoch() - low.microSecondsSinceEpoch();
	return static_cast<double>(diff) / Timestamp::kMicroSecondsPerSecond;
}


/// Add @c seconds to given timestamp.
///
/// @return timestamp+seconds as Timestamp
inline Timestamp addTime(Timestamp timestamp, double seconds)
{
	int64_t delta = static_cast<int64_t>(seconds * Timestamp::kMicroSecondsPerSecond);
	return Timestamp(timestamp.microSecondsSinceEpoch() + delta);
}