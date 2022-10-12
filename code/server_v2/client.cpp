#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<string.h>

//错误处理函数
void perror_if(bool condition, const char* errorMessage)
{
	if (condition) {
		perror(errorMessage);
		exit(1);
	}
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
	inet_pton(AF_INET, "192.168.226.128", &saddr.sin_addr.s_addr);

	int ret = connect(fd, (struct sockaddr*)&saddr, sizeof(saddr));
	perror_if(ret == -1, "connect");


	// 3. 和服务器端通信
	int n = 0;
	while (1)
	{
		// 发送数据
		char buf[512] = { 0 };
		sprintf(buf, "hi, I am client...%d\n", n++);
		write(fd, buf, strlen(buf));

		// 接收数据
		memset(buf, 0, sizeof(buf));
		int len = read(fd, buf, sizeof(buf));
		if (len > 0)
		{
			printf("server say: %s\n", buf);
		}
		else if (len == 0)
		{
			printf("server disconnect...\n");
			break;
		}
		else
		{
			perror("read");
			break;
		}
		sleep(1);   // 每隔1s发送一条数据
	}

	close(fd);

	return 0;
}
