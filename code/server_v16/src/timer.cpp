#include"timer.h"

std::atomic_int64_t Timer::num_created_;

void Timer::restart(Timestamp now)
{
	if (repeat_) {
		expiration_ = addTime(now, interval_);
	}
	else {
		expiration_ = Timestamp::invalid();
	}
}

