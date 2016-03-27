#include <iostream>
void qicksort(int *a,int left, int right)
{
	if (left > right)
	{
		return;
	}

	int i = left;
	int j = right;

	//ѡȡ��׼��
	int tmp = a[left];
	while(i != j)
	{
		//˳�����Ҫ���ȴ��ұ߿�ʼ��С��tmp��
		while (a[j] >= tmp && i < j)
		{
			--j;
		}

		//������Ҵ���tmp��(�ȺŲ���ȥ�������ڵľͲ�С�ڵ���)
		while(a[i] <=tmp && i<j)
		{
			++i;
		}

// 		//˳�����Ҫ��Ҫ�ȴ��ұ߿�ʼ�� 
// 		while(a[j]>=tmp && i<j) 
// 			j--; 
// 		//�����ұߵ� 
// 		while(a[i]<=tmp && i<j) 
// 			i++; 

		//�ҵ��󽻻�
		if (i < j)
		{
			int t = a[i];
			a[i] = a[j];
			a[j] = t;
		}
	}

	//i��j����һ���ˣ������׼������
	a[left] = a[i];
	a[i] = tmp;

	//�ٵݹ�ʣ�������
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