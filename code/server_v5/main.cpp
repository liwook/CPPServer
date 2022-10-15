#include"Epoll/Epoll.h"
#include"Socket/Socket.h"
#include"util/util.h"
#include"InetAddr/InetAddr.h"
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include"Channel/Channel.h"
const int READ_BUFFER = 1024;
const int MAXSIZE = 1024;

void handleEvent(Channel* ch,Epoll& poll);

int main()
{
	Socket serv_socket;
	InetAddr saddr(10000);
	serv_socket.bind(&saddr);
	serv_socket.listen();
	serv_socket.setNonblock();	//���÷�����

	Epoll poll;
	//poll.update(serv_socket.fd(), EPOLLIN, EPOLL_CTL_ADD);
	Channel serv_channel(&poll, serv_socket.fd());
	serv_channel.enableReading();	//����EPOLLIN,����ӵ�epoll��


	while (1)
	{
		/*vector<epoll_event> active;
		poll.Epoll_wait(active);
		int nums = active.size();*/

		vector<Channel*> activeChannel;
		poll.Epoll_wait(activeChannel);
		int nums = activeChannel.size();
		for (int i = 0; i < nums; ++i) {
			Channel* ch = activeChannel[i];
			if (ch->Fd() == serv_socket.fd()) {
				InetAddr caddr;
				Socket* cli_socket = new Socket(serv_socket.accept(&caddr));
				cli_socket->setNonblock();
				Channel* cliChannel = new Channel(&poll, cli_socket->fd());
				cliChannel->enableReading();//�ͻ��˵�channel����EPOLLIN,����ӵ�epoll��
			}
			else if(ch->Revent() & EPOLLIN){
				handleEvent(ch,poll);
			}

			/*int curfd = active[i].data.fd;

			if (curfd == serv_socket.fd()) {
				InetAddr* caddr = new InetAddr;
				Socket* cli_socket = new Socket(serv_socket.accept(caddr));
				cli_socket->setNonblock();
				poll.update(cli_socket->fd(), EPOLLIN,EPOLL_CTL_ADD);
			}
			else if (active[i].events & EPOLLIN) {
				handleEvent(curfd);
			}*/
		}
	}

	return 0;
}

void handleEvent(Channel* ch, Epoll& poll) {
	int fd = ch->Fd();
	char buf[READ_BUFFER];
	memset(buf, 0, sizeof(buf));
	ssize_t bytes_read = read(fd, buf, sizeof(buf));
	if (bytes_read > 0) {
		printf("client fd %d says: %s\n", fd, buf);
		write(fd, buf, bytes_read);
	}
	else if (bytes_read == -1) {
		perror("read");
	}
	else if (bytes_read == 0) {  //�ͻ��˶Ͽ�����
		printf("client fd %d disconnected\n", fd);
		poll.del(ch);
		close(fd);
	}
}