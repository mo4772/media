#include <iostream>

void merge(int *a,int first,int mid,int last,int *temp)
{
	int i = first,j = mid + 1;
	int m = mid,n = last;
	int k =0;
	while (i<=m&&j<=n)
	{
		if (a[i]<=a[j])
		{
			temp[k] = a[i++];
		}
		else
		{
			temp[k] = a[j++];
		}

		++k;
	}
	
	while (i <= m)  
        temp[k++] = a[i++];

	while (j <= n)  
        temp[k++] = a[j++];

	for (i = 0; i < k; i++)  
        a[first + i] = temp[i];
}

//归并排序，归并排序是先分再和
void mergesort(int *a,int first,int last,int *temp)
{
	if (first<last)
	{
		int mid = (first + last)/2;
		mergesort(a,first,mid,temp);
		mergesort(a,mid+1,last,temp);
		merge(a,first,mid,last,temp);
	}
}

void MergeSort(int *a,int n)
{
	int *p = new int[n];
	
	mergesort(a,0,n-1,p);
	delete[] p;
}

int main()
{
	int a[] = {2,1,9,0,5,6,3,8,17};
	MergeSort(a,9);

	for (int i=0; i<9; ++i)
	{
		std::cout<<a[i]<<" ";
	}

	system("pause");

}