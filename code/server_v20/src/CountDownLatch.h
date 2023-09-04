#include<mutex>
#include<condition_variable>

class CountDownLatch
{
public:
	explicit CountDownLatch(int count);

	void wait();

	void countDown();

	int getCount();
private:
	std::mutex mutex_;
	std::condition_variable cond_;
	int count_;
};