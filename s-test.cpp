#include <iostream>
#include <list>
#include <thread>
#include <mutex>
#include <chrono>
#include <ctime>

// Данная функция заполняет список list случайными значениями типа unsigned int
void initListByRandItems(std::list<int>& list);

// Данная функция подсчитывает количество нулевых или единичных бит (в зависимости от передаваемого в функцию параметра) в элементах
template <typename T>
int countOfBit(T element, int numberOfThread);

// Данная функция подсчитывает количество нулевых или единичных битов в элементах потока (в зависимости от передаваемых параметров),
// учитывает количество пройденных элементов, а также освобождает учтённые элементы сразу после учёта
void find(std::list<int>& list, int& bitCounter, int numberOfThread, int& removedItemCounter);

std::mutex mtx;

int main()
{
    setlocale(LC_ALL, "ru");

    srand(static_cast<unsigned int>(time(0)));

    // Данная переменная отвечает за размер списка list
    const int SIZE = 21;

    // Счетчики нулевых и единичных битов
    int zeroBitsCounter = 0;
    int singleBitsCounter = 0;

    // Количество освобожденных элементов списка list первым и вторым потоками
    int deletedByTh1 = 0;
    int deletedByTh2 = 0;

    // Номера потоков
    int thread1 = 0;
    int thread2 = 1;

    std::list<int> list(SIZE);

    // Заполняем список случайными числами
    initListByRandItems(list);

    for (auto i = list.begin(); i != list.end(); ++i)
    {
        std::cout << *i << " ";
    }
    std::cout << "\n";

    // С данными параметрами функция, начиная с головы списка, считает количество нулевых бит в элементах,
    // а также количество пройденных элементов первым потоком
    std::thread th1(find, std::ref(list), std::ref(zeroBitsCounter), thread1, std::ref(deletedByTh1));

    // С данными параметрами функция, начиная с хвоста списка, считает количество единичных бит в элементах,
    // а также количество пройденных элементов вторым потоком
    std::thread th2(find, std::ref(list), std::ref(singleBitsCounter), thread2, std::ref(deletedByTh2));

    th1.join();
    th2.join();

    std::cout << "Количество нулевых бит: " << zeroBitsCounter << "\n";
    std::cout << "Количество единичных бит: " << singleBitsCounter << "\n";
    std::cout << "Количество пройденных первым потоком элементов: " << deletedByTh1 << "\n";
    std::cout << "Количество пройденных вторым потоком элементов: " << deletedByTh2 << "\n";

    return 0;
}

void initListByRandItems(std::list<int>& list)
{
    for (auto i = list.begin(); i != list.end(); ++i)
    {
        *i = rand();
    }
}

template <typename T>
int countOfBit(T element, int numberOfThread) {
    int result{ 0 };
    int shiftBySize = ((sizeof(T) * 8) - 1);
    if (element < 0) {
        result++;
        element &= ((1 << shiftBySize) - 1);
    }
    while (element) {
        result += (element & 1);
        element >>= 1;
    }

    switch (numberOfThread)
    {
    case 0:
        return (sizeof(T) * 8) - result;

    case 1:
        return result;
    }
}

void find(std::list<int>& list, int& bitCounter, int numberOfThread, int& removedItemCounter)
{
    while (true)
    {
        mtx.lock();
        if (list.size() > 0)
        {
            switch (numberOfThread)
            {
            case 0:
                bitCounter += countOfBit(*(list.begin()), numberOfThread);
                list.pop_front();
                removedItemCounter++;
                break;

            case 1:
                bitCounter += countOfBit(*(list.rbegin()), numberOfThread);
                list.pop_back();
                removedItemCounter++;
                break;
            }
        }
        mtx.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        if (list.size() == 0)
            break;
    }
}