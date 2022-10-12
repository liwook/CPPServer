#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<string.h>
#include<thread>


//错误处理函数
void perror_if(bool condition, const char* errorMessage)
{
	if (condition) {
		perror(errorMessage);
		exit(1);
	}
}

//子线程函数，负责通信
void working(int clientfd) 
{
	char buf[512];
	while (1) {
		memset(buf, 0, sizeof(buf));
		int len = read(clientfd, buf, sizeof(buf));
		if (len > 0) {
			printf("client says: %s\n", buf);
			write(clientfd, buf, len);
		}
		else if (len == 0) {
			printf("client is disconnect..\n");
			break;
		}
		else {
			//perror("read");这里是多线程环境了，不可再用perror,可以使用strerror(int)
			printf("read error..\n");
			break;
		}
	}
	close(clientfd);
}

int main()
{
	//1.创建监听的套接字
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	perror_if(fd == -1, "socket");

	//2.绑定ip地址和端口
	struct sockaddr_in saddr;
	memset(&saddr, 0, sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(10000);
	saddr.sin_addr.s_addr = INADDR_ANY;

	int ret = bind(fd, (struct sockaddr*)&saddr, sizeof(saddr));
	perror_if(ret == -1, "bind");

	//3.设置监听
	ret = listen(fd, 64);
	perror_if(ret == -1, "listen");


	while (1) {
		//4.建立连接
		struct sockaddr_in cliaddr;	//保存客户端ip地址信息
		socklen_t len = sizeof(cliaddr);

		int cfd = accept(fd, (struct sockaddr*)&cliaddr, &len);
		if (cfd == -1) {
			perror("accpet");
			continue;
		}

		char ip[64] = { 0 };
		printf("new client fd:%d ip:%s, port:%d\n", cfd,
			inet_ntop(AF_INET, &cliaddr.sin_addr.s_addr, ip, sizeof(ip)),
			htons(cliaddr.sin_port));

		std::thread t(working, cfd);
		t.detach();
	}

	close(fd);
	return 0;
}
