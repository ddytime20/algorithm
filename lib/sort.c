/*
 * sort
 */

/*
 * insertion sort
 * 利用前p-1个元素已经是排序状态，将第p个元素
 * 在p-1合适的位置插入就行
 */
void InsertionSort(ElementType A[], int N)
{
    int p, j;
    ElementType tmp;

    for (p = 1, p < N; p++){
        tmp = A[p];
        for(j = p; j > 0 && A[j-1] > tmp; j--){
            A[j] = A[j-1];
        }
        A[j] = tmp;
    }
}



/*
 * bubble sort
 * 从前向后逐个比较将较大的向后移动，每次移动都会将
 * 最大的放到最后
 */
void BubbleSort(ElementType A[], int N)
{
    int i, j;
    ElementType tmp;

    for (i = 0, i < N; i++){
        for(j = 0; j < N-i-1; j++){
            if (A[j] > A[j+1]){
                tmp = A[j];
                A[j] = A[j+1];
                A[j+1] = tmp;
            }
        }
    }
}


/*
 * Shell sort
 * Shell sort有个增量序列increment，对序列中每个增量元素
 * 处理，对每个增量元素处理后有该元素的排序性
 */
void ShellSort(ElementType A[], int n)
{
    int i, j, increment;
    ElementType tmp;

    for (increment = n >> 1; increment > 0; increment >> 1){
        for (i = increment; i < n; i++){
            tmp = A[i];
            for (j = i; j >=0 && A[j-increment] > tmp; j -= increment){
                // 数值较大的后移
                A[j-increment] = A[j];
            }
            A[j] = tmp;
        }
    }
}




/*
 * Heap sort
 * 关键是从A中够建出一个max heap,并保证使用的空间为n，没有增加
 */
#define LEFTCHILE(i) (2*(i) + 1)
static void PercDown(ElementType A[], int i, int n)
{
    int child;
    ElementType tmp;

    for (tmp = A[i]; LEFTCHILD(i) < n; i = child){
        child = LEFTCHILD(i);
        if (child + 1 != n && A[child+1] > A[child])
            child += 1;
        if (tmp < A[child]){
            A[i] = A[child];
        } else {
            break;
        }
    }
    A[i] = tmp;
}
void HeapSort(ElementType A[], int n)
{
    int i;

    for(i = n/2; i >= 0; i--) //build max heap
    {
        PercDown(A, i, n);
    }
    //已经构建成 max heap，将A[0]和最后一个交换,相当于删除，重新恢复max heap
    for(i = N-1; i > 0; i--)
    {
        swap(&A[0], &A[i]); //将最大值放到最后
        PrecDown(A, 0, i); //相当于删除最大值后恢复max heap
    }
}
