#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<string.h>

int main()
{
	//1.创建监听的套接字
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd == -1) {
		perror("socket");
		exit(1);
	}

	//2.绑定ip地址和端口
	struct sockaddr_in saddr;
	memset(&saddr, 0, sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(10000);
	saddr.sin_addr.s_addr = INADDR_ANY;

	int ret = bind(fd, (struct sockaddr*)&saddr, sizeof(saddr));
	if (ret == -1) {
		perror("bind");
		exit(1);
	}

	//3.设置监听
	ret = listen(fd, 128);
	if (ret == -1) {
		perror("listen");
		exit(1);
	}

	//4.建立连接
	struct sockaddr_in cliaddr;	//保存客户端ip地址信息
	socklen_t len = sizeof(cliaddr);

	int cfd = accept(fd, (struct sockaddr*)&cliaddr, &len);
	if (cfd == -1) {
		perror("accpet");
		exit(1);
	}
	
	char ip[64] = { 0 };
	printf("new client fd:%d ip:%s, port:%d\n", cfd,
		inet_ntop(AF_INET, &cliaddr.sin_addr.s_addr, ip, sizeof(ip)),
		htons(cliaddr.sin_port));

	//5.进行通信
	char buf[512];
	while (1) {
		memset(buf, 0, sizeof(buf));
		int len = read(cfd, buf, sizeof(buf));
		if (len > 0) {
			printf("client says: %s\n", buf);
			write(cfd, buf, len);
		}
		else if (len == 0) {
			printf("client is disconnect..\n");
			break;
		}
		else {
			perror("read");
			break;
		}
	}

	close(fd);
	close(cfd);
	return 0;
}
