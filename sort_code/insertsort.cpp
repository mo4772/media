#include <iostream>

//插入排序
void insertsort(int *a,int len)
{
	for (int i=1; i<len;++i)
	{
		int t = a[i];
		int j=i;
		//for (; j>0&&t<a[j-1];--j)
		for (; j>0&&a[j-1]<t; --j)
		{
			a[j] = a[j-1];
		}

		a[j] = t;
	}
}

int GetMax(int *a,int begin,int end)
{
	int max = 0;
	int index = 0;
	for (int i=begin;i<=end;++i)
	{
		if (a[i] >= max)
		{
			max = a[i];
		    index = i;
		}
	}

	return index;
}

//选择排序,从大到小
void selectsort1(int *a,int len)
{
	    for (int j=0; j<len;++j)
		{
			int i = GetMax1(a,j,len -1);
			int tmp = a[j];
			a[j] = a[i];
			a[i] = tmp;
		}
		
}

//选择排序,从小到大
void selectsort2(int *a,int len)
{
	for (int j=len-1;j>=0;--j)
	{
		int i = GetMax1(a,0,j);
		int tmp = a[j];
		a[j] = a[i];
		a[i] = tmp;
	}
}

int main()
{
	int a[] = {1,0,9,8,3,6,7,2,5};
	//insertsort(a,9);
	selectsort2(a,9);
	for (int ii=0; ii<9; ++ii)
	{
		std::cout<<a[ii]<<" ";
	}

	system("pause");
}