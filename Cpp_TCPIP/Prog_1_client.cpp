#include <netdb.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> 
#include <unistd.h> 

#include <cstdio>

#include <iostream>
#include <sstream>
#include <fstream> //для файлового ввода/вывода
#include <algorithm> //для sort()

#include <thread>

#define PORT 8080 //Назначаем порт 8080 для передачи данных через сокеты
#define SA struct sockaddr 

const int LIMIT = 80;

using namespace std;

class Thread_1
    {
        

        private:
            string numbers; //строка для цифр
            int wlen; //Размер строки
            //int arr[LIMIT];
            ofstream filestr = ofstream("test.txt"); //создание текстового файла

        public:
        Thread_1() : numbers("0")
        {};

        //Получение строки от пользователя
        //После ввода get_string запускает метод check для проверки строки,
        //который в случае ошибки предлагает повторно ввести строку
        //Метод поучает параметр sockfd, хранящий данные о сокете. Параметр перемщается до метода
        //write_again(), где должен подать сигнал прекртатить работу второй программы, в случае отказа пользоавтеля повторно ввести строку.
            void get_string(int sockfd) 
            {
                cout<<"\nВведите строку, состоящую из цифр.\n Максимальный размер 64 символа\n Ввод: ";
                cin>>ws; //Необходим в случае повторного ввода строки
                getline(cin, numbers); //Ввод строки
                check(sockfd); //Проверка введенной строки
            }
        //Проверка строки. 
        //1. Размер Строки не должен быть больше 64 символов
        //2. Строка должна состоять только из цифр.
            void check(int sockfd)
            {
                wlen = numbers.length(); // Определяем длину строки
                int error = 0; // 0 - нет ошибок, 1 - есть ошибки
                if(wlen>64) //проверка на превышение размера строки
                {
                    cout<<"\nВы ввели более 64 символов.\n";
                    error = 1;
                }
                for (int i = 0; i<wlen; i++)
                {
                    if(isdigit(numbers[i]))
                    {
                        continue;
                        //cout<<numbers[i];
                    }
                    else
                    {
                        cout<<"\nСтрока должна состоять только из цифр.\n";
                        error = 1;
                        break;
                    }
                }
                //Если проверка не была пройдена, предоставляется возможность повторного ввода строки
                if (error == 1) 
                {
                    error = 0;
                    write_again(sockfd); 
                }    
            }
        //Возможность повторного ввода строки
            void write_again(int sockfd)
            {
                int rwtsockfd = sockfd;
                char yesno;
                cout<<"\nВвести число заново? (y/n): ";
                cin>>yesno;
                if(yesno=='y')
                        get_string(rwtsockfd);//Повторяем ввод строки и заново передаем данные о сокете
                else
                {
                    char ch[LIMIT] = "exit";
                    write(sockfd, ch, sizeof(ch));//Посылаем сигнал для прекращения работы второй программы
                    return exit(0);
                }
            }
        //Вывод строки
            void put_string() const
            {
                cout<<"\n"<<numbers<<"\n";
            }
        //Конвертация string в массив int, сортировка элементов по убыванию
        //и обратная передача в строку 
            void sortString()
            {
                int arr[wlen] = { 0 }; //Создаем массив целых чисел
                int i, arr_size; //индекс массива и размер массива
                for (i = 0; i<wlen; i++) //конвертация строки в массив целых чисел
                    {
                        arr[i] = arr[i] * 10 + (numbers[i] - 48); 
                    }
                arr_size = sizeof(arr)/sizeof(arr[0]);
                sort(arr, arr+arr_size, greater<int>()); //сортировка эл-в массива по убыванию

            /*  
            //Вывод отстортированной строки
                cout<<"SORT: ";
                for (i=0; i<wlen; i++)
                    cout<<arr[i]<<" ";
            */
                ostringstream os; //Создаем поток, для преобразования массива int в строку
                for (int j: arr)
                {
                    os<<j; //передача массива в поток ostringstream
                }
                string rev_numbers(os.str()); //Запись потока в строку
                // cout<<"\n"<<rev_numbers;
                numbers = rev_numbers;
            }
        //Метод для Замены четных элементов на "KB"
            void changeToKB()
            {
                string new_numbers; //Строка для преобразований
                new_numbers.resize(1); //Задаем размер строки
                string kb = "KB"; //Строка с символами для вставки
                   
                
                new_numbers[0]=numbers[0]; //Первый элемент строки не изменяется
                int j = 1, i = 1; //j - Индекс строки numbers; i - Индекс строки new_numbers
                for (int z=1;z<wlen; z++)
                {
                    
                    if((z+1)%2==0)
                    {
                        new_numbers.insert(i,kb);
                        i++;//увеличиваем индекс new_number на 2 т.к. добавили 2 символа
                        i++;
                        j++;//т.к. заменили цифру на символы, увеличиваем индекс на 1
                    }
                    else
                    {
                        new_numbers.insert(i++,numbers,j++,1);//Вставляем цифры в new_numbers на место нечетных эл-в
                    }
                    
                }
                numbers = new_numbers; //Присваиваем строке numbers данные строки new_numbers
                //cout<<"\nfinal: "<<numbers;
            }
        //Передача строки в файл
            void strToFile()
            {
                string line=" ";
                if (filestr.is_open())
                {
                    
                    filestr<<numbers;
                    filestr.close();
                }
                else
                {
                    filestr.open("./test.txt");
                    filestr<<numbers;
                    filestr.close();
                }
            }
    };
class Thread_2
{
    private:
        string numbers2; //строка, в которую скопируем данные из файла
        int wlen2; //длина строки
        char buff[LIMIT]; //сюда скопируем строку и передадим в программу 2
    protected:
        int sum;
    public:
    //Перемещаем данные из файла в строку для дальнейшей обработки данных
        void getStringFromFile()
        {
            ifstream infile("./test.txt");
            infile>>numbers2;
            infile.close();
            ofstream toclearfile = ofstream("./test.txt");
            toclearfile.close();
        }
    //Производим рассчет суммы чисел строки
        void get_summ()
        {
            wlen2 = numbers2.length();
            int arr[wlen2] = { 0 }; 
            
                int i, j=0;
                sum = 0;

                for (i = 0; i<wlen2; i++) 
                    {
                        if(isdigit(numbers2[i]))
                        {
                            arr[j] = arr[j] * 10 + (numbers2[i] - 48);
                            j++;
                        }
                    }     
                //cout << "\narr[] = "; 
                for (i = 0; i < j; i++) 
                { 
                    sum += arr[i]; 
                } 
                //cout << "\nSum of array is = " << sum << endl; 
        }
    //Посылаем полученную сумму в программу 2 
        void sendToServer(int sockfd)
        {
            bzero(buff, sizeof(buff)); 
            sprintf(buff,"%d",sum);
            write(sockfd,buff,sizeof(buff)); //передаем данные в программу 2

        }
};
        

int main()
{
    int sockfd, connfd; //параметры сокета
	struct sockaddr_in servaddr, cli; 

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
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
	servaddr.sin_port = htons(PORT); 

// Подключение сокета-клиента к сокету-серверу
	if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) { 
		printf("connection with the server failed...\n"); 
		exit(0); 
	} 
	else
		printf("connected to the server..\n"); 


    Thread_1 t1; //Объект 1-го потока
    Thread_2 t2; //Объект 2-го потока
    char yesno;

//Выполнение программы
    //for (int i=0; i<3; i++)
    do
    {
    t1.get_string(sockfd); // получаем строку. Функция также скрыто вызывает метод для проверки строки
    t1.sortString(); // сортируем строку по убыванию
    //t1.put_string(); // вывод строки
    t1.changeToKB(); // меняем четные элементы строки на символы "KB"
    t1.strToFile();  // переносим строку в файл ./test.txt
    t2.getStringFromFile(); // получаем строку из файла
    t2.get_summ(); // производим рассчет суммы численных элементов строки
    t2.sendToServer(sockfd);
    cout<<"\nВыполнить еще раз? (y/n): "; cin>>yesno;
    }while (yesno == 'y');
//По завершении выполнения основных функций 1-й программы,
// посылаем сигнал, завершающий работу 2-й программы
    char ch[LIMIT] = "exit";
    write(sockfd, ch, sizeof(ch)); 

// Завершаем работу сокета-клиента
	close(sockfd); 
}
