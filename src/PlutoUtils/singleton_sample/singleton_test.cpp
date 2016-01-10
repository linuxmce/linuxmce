// singleton_test.cpp : Defines the entry point for the console application.
//
#include "../singleton.h"
#include "../threads.h"
#include <iostream>

using namespace cpp;
using namespace cpp::Threading;
using namespace std;

class MyClassImpl : public Singleton<MyClassImpl>
{
public:
	void DoSomethingFromMain() { cout << "MainThread: I'm doin' something" << endl; }
	void DoSomethingFromThread() { cout << "Thread: I'm doin' something" << endl; }
};

typedef Singleton<MyClassImpl, CreateUsingNew<MyClassImpl>, DefaultLifetime, MultiThreaded> MyClass; //safe threaded

void *MyThread(void *)
{
	int i = 1000;
	while(i-- > 0)
	{
		MyClass::Instance().DoSomethingFromThread();
	}

	return NULL;
}

int main(int argc, char* argv[])
{
	pthread_t id;
	pthread_create(&id, NULL, MyThread, NULL);
	
	int i = 1000;
	while(i-- > 0)
	{
		MyClass::Instance().DoSomethingFromMain();
	}

	pthread_join(id, NULL);

	return 0;
}

