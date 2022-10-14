#include"Epoll/Epoll.h"
#include"Socket/Socket.h"
#include"util/util.h"
#include"InetAddr/InetAddr.h"
#include<stdio.h>
#include<string.h>
#include<unistd.h>
const int READ_BUFFER = 1024;
const int MAXSIZE = 1024;

void handleEvent(int sockfd,Epoll& poll);
int main()
{
	Socket serv_socket;
	InetAddr saddr(10000);
	serv_socket.bind(&saddr);
	serv_socket.listen();
	serv_socket.setNonblock();	//设置非阻塞

	Epoll poll;
	poll.update(serv_socket.fd(), EPOLLIN, EPOLL_CTL_ADD);


	while (1)
	{
		vector<epoll_event> active;
		poll.Epoll_wait(active);
		int nums = active.size();
		for (int i = 0; i < nums; ++i) {
			int curfd = active[i].data.fd;
			if (active[i].events & EPOLLIN) {
				if (curfd == serv_socket.fd()) {
					InetAddr caddr;
					Socket* cli_socket = new Socket(serv_socket.accept(&caddr));//没有delete,内存泄漏.因为析构函数会调用close(),之后的版本会解决内存泄漏问题

					cli_socket->setNonblock();
					poll.update(cli_socket->fd(), EPOLLIN, EPOLL_CTL_ADD);
				}
				else {
					handleEvent(curfd, poll);
				}
			}
			else if (active[i].events & EPOLLOUT) {
				//其他事件以后实现
			}
		}
	}

	return 0;
}

void handleEvent(int sockfd,Epoll& poll) {
	char buf[READ_BUFFER];
	memset(buf, 0, sizeof(buf));
	ssize_t bytes_read = read(sockfd, buf, sizeof(buf));
	if (bytes_read > 0) {
		printf("client fd %d says: %s\n", sockfd, buf);
		write(sockfd, buf, bytes_read);
	}
	else if (bytes_read == -1) {//出错
		perror_if(1, "read");
	}
	else if (bytes_read == 0) {  //客户端断开连接
		printf("client fd %d disconnected\n", sockfd);
		poll.epoll_delete(sockfd);
		close(sockfd); 
	}
}