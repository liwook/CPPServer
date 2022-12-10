// log1.cpp: 定义应用程序的入口点。
//

#include"log/logger.h"
#include<thread>
using namespace std;

int main()
{
	std::thread t1([]() {
		for (int i = 0; i < 1000; ++i)
			LOG_INFO << "1111woshisdfsd " << 23 << 34 << "buox";
		});
	t1.detach();
	std::thread t2([]() {
		for (int i = 0; i < 1000; ++i)
			LOG_INFO << "22woshisdfsd " << 23 << 34 << "buox";
		});
	t2.detach();
	for(int i=0;i<100;++i)
		LOG_INFO << 11 << "232" << 23;
	std::this_thread::sleep_for(std::chrono::seconds(2));
	return 0;
}
