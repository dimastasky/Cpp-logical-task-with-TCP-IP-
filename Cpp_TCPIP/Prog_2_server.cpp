#include <stdio.h> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <arpa/inet.h>
#include <sys/socket.h> 
#include <sys/types.h>
#include <unistd.h>

#define MAX 80 
#define PORT 8080 
#define SA struct sockaddr 

class Receiver
{
    private:
        char buff[MAX];
        int converted;
		int a;
    public:
        void get_message(int sockfd) 
        { 
			for(;;)
			{
				a=0;
                bzero(buff, MAX); 
            // Чтение сообщения от программы 1 (клиента)
                read(sockfd, buff, sizeof(buff)); 
            // Вывод содержимого из полученного сообщения
            // Если сообщение содержит "exit", завершаем работу псервера
				printf("Server get: %s",buff);
                
				if (strncmp("exit", buff, 4) != 0)
				{
					converted = atoi(buff);
				//Если полученное число имеет более 2-х разрядов и кратно 32,
				// выводим сообщение о получении данных
					if (converted>99 && converted%32==0)
					{
						printf("  Данные получены\n");
					}
				//При невыполнении условий выводим сообщение о ошибке
					else if(converted <99 || converted%32!=0)
					{
						printf("  ОШИБКА. Данные отклонены.\n");
					}
				}/* code */
				else if (strncmp("exit", buff, 4) == 0)
				{
					break;
				//a=1;
				}
				

			}//while(a!=1);
		}
    
};


int main() 
{ 
	int sockfd, connfd, len; 
	struct sockaddr_in servaddr, cli; 

    Receiver r1; //Объект для получения данных

// Создание и верификация сокета, 
// в программах будем использовать сокеты TCP
// 1-я программа - Клиент; 2-я программа - сервер.
	sockfd = socket(AF_INET, SOCK_STREAM, 0); 
	if (sockfd == -1) { 
		printf("socket creation failed...\n"); 
		exit(0); 
	} 
	else
		printf("Socket successfully created..\n"); 
	bzero(&servaddr, sizeof(servaddr)); 

// Назначение IP-адреса и Порта
	servaddr.sin_family = AF_INET; 
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY); //любой адрес
	servaddr.sin_port = htons(PORT);

// Привязка только-что созданного сокета с заданным IP и верификация
	if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) { 
		printf("socket bind failed...\n"); 
		exit(0); 
	} 
	else
		printf("Socket successfully binded..\n"); 

// Теперь сервер готов к прослушиванию и к верификации
	if ((listen(sockfd, 5)) != 0) { 
		printf("Listen failed...\n"); 
		exit(0); 
	} 
	else
		printf("Server listening..\n"); 
	len = sizeof(cli); 

// Принятие пакета данных от клиента и верификация
	connfd = accept(sockfd, (SA*)&cli, (socklen_t*)&len); 
	if (connfd < 0) { 
		printf("server acccept failed...\n"); 
		exit(0); 
	} 
	else
		printf("server acccept the client...\n"); 

//Получение данных от клиента(программа 1) вплоть до команды "exit" для
//завершения цикла метода get_message
	r1.get_message(connfd); 


// После получения команды "exit", закрываем сокет-сервер
	close(sockfd); 
} 
