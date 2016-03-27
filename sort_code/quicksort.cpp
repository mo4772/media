#include <iostream>
void qicksort(int *a,int left, int right)
{
	if (left > right)
	{
		return;
	}

	int i = left;
	int j = right;

	//选取基准数
	int tmp = a[left];
	while(i != j)
	{
		//顺序很重要，先从右边开始找小于tmp的
		while (a[j] >= tmp && i < j)
		{
			--j;
		}

		//从左边找大于tmp的(等号不能去掉，大于的就不小于等于)
		while(a[i] <=tmp && i<j)
		{
			++i;
		}

// 		//顺序很重要，要先从右边开始找 
// 		while(a[j]>=tmp && i<j) 
// 			j--; 
// 		//再找右边的 
// 		while(a[i]<=tmp && i<j) 
// 			i++; 

		//找到后交换
		if (i < j)
		{
			int t = a[i];
			a[i] = a[j];
			a[j] = t;
		}
	}

	//i与j碰到一起了，则与基准数交换
	a[left] = a[i];
	a[i] = tmp;

	//再递归剩余的区间
	qicksort(a,left,i-1);
	qicksort(a,i+1,right);
}

int main()
{
	int a[] = {2,6,1,5,3,9,5};
	qicksort(a,0,6);
	for (int i=0; i<6; ++i)
	{
		std::cout<<a[i]<<" ";
	}

	system("pause");
	return 0;
}