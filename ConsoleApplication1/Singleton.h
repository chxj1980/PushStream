#ifndef _SINGLETON_H
#define _SINGLETON_H

template <class T>
class Singleton
{
public:
	~Singleton() {}

	static T* Instance()
	{
		if (ms_pInstance == 0)
		{
			ms_pInstance = new T;
		}
		return ms_pInstance;
	}

	static void DestroyInstance()
	{
		if (ms_pInstance)
		{
			delete ms_pInstance;
			ms_pInstance = NULL;
		}
	}

private:
	static T* ms_pInstance;
};

template <class T>
T* Singleton<T>::ms_pInstance = 0;
#endif