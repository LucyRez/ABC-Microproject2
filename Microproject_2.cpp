//ВАРИАНТ 3
/*
Задача о читателях и писателях.Базу данных разделяют два типа
процессов – читатели и писатели.Читатели выполняют транзакции, которые
просматривают записи базы данных, транзакции писателей и просматривают
и изменяют записи.Предполагается, что в начале БД находится в
непротиворечивом состоянии(т.е.отношения между данными имеют смысл).
Каждая отдельная транзакция переводит БД из одного непротиворечивого
состояния в другое.Для предотвращения взаимного влияния транзакций
процесс - писатель должен иметь исключительный доступ к БД.Если к БД не
обращается ни один из процессов - писателей, то выполнять транзакции могут
одновременно сколько угодно читателей.Создать многопоточное
приложение с потоками - писателями и потоками - читателями.Реализовать
решение, используя семафоры.
*/

#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <semaphore.h>
#define HAVE_STRUCT_TIMESPEC
#include <pthread.h>


using namespace std;

class Data {
public:
	std::vector<std::string> data;

	Data() {}

	void add(std::string str) {
		data.push_back(str);
		std::cout << "Писатель записал в базу строку '" << str << "'\n";
	}

	void remove() {
		if (data.size() == 0)
		{
			std::cout << "Писатель пытался удалить из базы что-то, но в ней ещё ничего не записано.\n";
			return;
		}
		srand(time(0));
		int randIndex = 0 + (rand() % data.size());
		std::string remove = data[randIndex];
		data.erase(data.begin() + randIndex);
		std::cout << "Писатель удалил из базы строку '" << remove << "'\n";
	}

	void read() {
		if (data.size() == 0)
		{
			std::cout << "Читатель пытался прочитать данные из базы, но там ничего не записано.\n\n";
			return;
		}
		std::cout << "Читатель прочитал все данные из базы:\n";
		for (int i = 0; i < data.size(); i++)
		{
			std::cout << data[i] << "\n";
		}
		std::cout << "\n";
	}

};


Data dataBase = Data();
sem_t semaphore;


std::string generateString() {
	int length = 1 + (rand() % 20);
	std::string str = "";

	for (int i = 0; i < length; i++)
	{
		str += 'a' + rand() % 26;
	}
	return str;
}

void* reader(void* i) {

	sem_wait(&semaphore);

	std::cout << "Читатель " << (int)i << " осуществляет чтение из базы...\n";
	this_thread::sleep_for(chrono::milliseconds(3000));
	dataBase.read();

	sem_post(&semaphore);


	return NULL;
}

void* writer(void* i) {

	sem_wait(&semaphore);
	std::cout << "Писатель " << (int)i << " осуществляет добавление строки в базу...\n";
	this_thread::sleep_for(chrono::milliseconds(3000));
	srand(time(0));
	dataBase.add(generateString());
	if ((rand()%2) == 1)
	{
		std::cout << "Писатель " << (int)i << " осуществляет удаление строки из базы...\n";
		this_thread::sleep_for(chrono::milliseconds(3000));
		dataBase.remove();
	}
	std::cout << "\n";
	sem_post(&semaphore);
	return NULL;


}

int main()
{
	setlocale(LC_ALL, "rus");
	sem_init(&semaphore, 0, 1);

	pthread_t readers[10];
	pthread_t writers[10];

	for (size_t i = 0; i < 10; i++)
	{
		pthread_create(&(writers[i]), NULL, writer, (void*)i);
		pthread_create(&(readers[i]), NULL, reader, (void*)i);
	}

	for (size_t i = 0; i < 10; i++)
	{
		pthread_join(writers[i], NULL);
		pthread_join(readers[i], NULL);
	}


}

