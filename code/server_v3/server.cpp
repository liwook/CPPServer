#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<string.h>
#include<sys/epoll.h>
#include<fcntl.h>

const int SIZE = 1024;

//错误处理函数
void perror_if(bool condition, const char* errorMessage)
{
	if (condition) {
		perror(errorMessage);
		exit(1);
	}
}

void handleReadevent(int fd, int epfd);

//把套接字设置为非阻塞
void setNonblocking(int fd) {
	int flag = fcntl(fd, F_GETFL);
	flag |= O_NONBLOCK;
	fcntl(fd, F_SETFL, flag);
}

int main()
{
	//1.创建监听的套接字
	int lfd = socket(AF_INET, SOCK_STREAM, 0);
	perror_if(lfd == -1, "socket");

	//2.绑定ip地址和端口
	struct sockaddr_in saddr;
	memset(&saddr, 0, sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(10000);
	saddr.sin_addr.s_addr = INADDR_ANY;

	int ret = bind(lfd, (struct sockaddr*)&saddr, sizeof(saddr));
	perror_if(ret == -1, "bind");

	//3.设置监听
	ret = listen(lfd, 64);
	perror_if(ret == -1, "listen");

	//创建epoll实例
	int epfd = epoll_create(1);
	perror_if(epfd == -1, "epoll_create");

	//设置非阻塞
	setNonblocking(lfd);

	//把监听的套接字绑定到epoll实例上
	struct epoll_event ev;
	memset(&ev, 0, sizeof(ev));
	ev.data.fd = lfd;
	ev.events = EPOLLIN;
	ret=epoll_ctl(epfd, EPOLL_CTL_ADD, lfd, &ev);
	perror_if(ret == -1, "epoll_ctl add");


	epoll_event evs[SIZE];
	while (1) {
		int nums = epoll_wait(epfd, evs, SIZE, -1);
		if (nums == -1) {
			perror("epoll_wait");
			continue;
		}
		
		for (int i = 0; i < nums; ++i) {
			if (evs[i].events && EPOLLIN) {
				if (evs[i].data.fd == lfd) {
					//4.建立连接

					//这次不打印客户端的ip，不需要客户端的sockaddr
					int cfd = accept(lfd, nullptr, nullptr);
					if (cfd == -1) {
						perror("accpet");
						continue;
					}

					setNonblocking(cfd);

					memset(&ev, 0, sizeof(ev));
					ev.data.fd = cfd;
					ev.events = EPOLLIN;
					int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &ev);
					perror_if(ret == -1, "epoll_ctl add");
				}
				else {
					handleReadevent(evs[i].data.fd, epfd);
				}
			}
			else {
				//其他事件以后再实现
			}
		}
	}

	close(lfd);
	return 0;
}
void handleReadevent(int cfd, int epfd)
{
	char buf[512] = { 0 };

	int len = read(cfd, buf, sizeof(buf));
	if (len > 0) {
		printf("client says: %s\n", buf);
		write(cfd, buf, len);
	}
	else if (len == 0) {
		printf("client is disconnect..\n");
		epoll_ctl(epfd, EPOLL_CTL_DEL, cfd, nullptr);
		close(cfd);
	}
	else {
		perror("read");
		exit(1);
	}
}