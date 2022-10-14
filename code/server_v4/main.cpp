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
	serv_socket.setNonblock();	//���÷�����

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
					Socket* cli_socket = new Socket(serv_socket.accept(&caddr));//û��delete,�ڴ�й©.��Ϊ�������������close(),֮��İ汾�����ڴ�й©����

					cli_socket->setNonblock();
					poll.update(cli_socket->fd(), EPOLLIN, EPOLL_CTL_ADD);
				}
				else {
					handleEvent(curfd, poll);
				}
			}
			else if (active[i].events & EPOLLOUT) {
				//�����¼��Ժ�ʵ��
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
	else if (bytes_read == -1) {//����
		perror_if(1, "read");
	}
	else if (bytes_read == 0) {  //�ͻ��˶Ͽ�����
		printf("client fd %d disconnected\n", sockfd);
		poll.epoll_delete(sockfd);
		close(sockfd); 
	}
}