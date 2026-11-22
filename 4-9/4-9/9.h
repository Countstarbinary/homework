#pragma once
#include <iostream>


template <class M>
void Swap(M x, M y)
{
	M temp = (M)malloc(sizeof(M));
	if (!temp)return;

	*temp = *x;
	*x = *y;
	*y = *temp;

	free(temp);
}

template<class T>
void Sort(T arr, int nLength)
{
	int i;
	int k;
	T temp = (T)malloc(sizeof(T));

	for (i = 0; i < nLength - 1; i++)
	{
		for (k = 0; k < nLength - 1 - i; k++)
		{
			if (arr[k] > arr[k + 1])
			{
				*temp = arr[k];
				arr[k] = arr[k + 1];
				arr[k + 1] = *temp;
			}
		}
	}
}