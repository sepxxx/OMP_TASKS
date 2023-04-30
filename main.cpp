#include <omp.h>
#include <iostream>
#include <time.h> 

using namespace std;
void t2() {
    int n = 3;
#pragma omp parallel if(n>2) num_threads(n)
    printf("Threads: %d num_thread: %d\n", omp_get_num_threads(), omp_get_thread_num());
    n = 2;
#pragma omp parallel if(n>2) num_threads(n)
    printf("Threads: %d num_thread: %d\n", omp_get_num_threads(), omp_get_thread_num());
}
void t1() {
    int N = 8;
#pragma omp parallel num_threads(N)
#pragma omp for 
    for (int i = 0; i < N; i++) {
        printf("Threads: %d num_thread: %d %s\n", omp_get_num_threads(), omp_get_thread_num(), "Hello world!");
    }
}

void t3() {
    int a = 0, b = 0;
    printf("*FIRST SECTION*\n");
    printf("BEFORE a=%d b=%d\n", a, b);
#pragma omp parallel private(a) firstprivate(b) num_threads(2) 
    {
        int n = omp_get_thread_num();
        a += n;
        b += n;
        printf("IN thread %d a=%d b=%d \n",n, a, b);
    }
    printf("AFTER a=%d b=%d\n", a, b);

    printf("*SECOND SECTION*\n");
    printf("BEFORE a=%d b=%d\n", a, b);
#pragma omp parallel shared(a) private(b) num_threads(4) 
    {
        int n = omp_get_thread_num();
        a -= n;
        b -= n;
        printf("IN thread %d a=%d b=%d \n", n, a, b);
    }
    printf("AFTER a=%d b=%d\n", a, b);
}

void t4() {
    int a[10] = {1,2,3,4,5,6,7,8,9,0};
    int b[10] = {1,2,3,4,5,6,7,8,9,0};
    int max =b[0], min = a[0];
#pragma omp parallel shared(max, min,a,b) num_threads(2) 
    {
        int n = omp_get_thread_num();
        //omp master
        if(n==0){
            for (int i = 1; i < 10; i++)
                if (a[i] < min)
                    min = a[i];
        }//omp signle
        else {
            for (int i = 1; i < 10; i++)
                if (b[i] > max)
                    max = b[i];
        }
    }
    printf("max b = %d min a = %d \n", max, min);
}

void t5() {
    const int m = 6, n = 8;
    int d[m][n];
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            d[i][j] = rand() % 10;
        }
    }
    printf("MAS\n");
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            printf(" %d ", d[i][j]);
        }
        printf("\n");
    }
#pragma omp parallel sections 
    {
        #pragma omp section
        {
            int mean = 0;
            for (int i = 0; i < m; i++)
                for (int j = 0; j < n; j++) {
                    mean += d[i][j];
                }
                mean /= m * n;
            printf("THREAD %d MEAN=%d\n",omp_get_thread_num(), mean);

        }

        #pragma omp section
        {
            int max = d[0][0], min = d[0][0];
            for (int i = 0; i < m; i++)
                for (int j = 0; j < n; j++) {
                    if (max < d[i][j])
                        max = d[i][j];
                    if (min > d[i][j])
                        min = d[i][j];
                }
            printf("THREAD %d min=%d max=%d\n", omp_get_thread_num(), min, max);

        }

        #pragma omp section
        {
            int nk3 = 0;
            for (int i = 0; i < m; i++)
                for (int j = 0; j < n; j++) {
                    if (d[i][j] % 3 == 0)
                        nk3++;
                }
            printf("THREAD %d nk3=%d\n", omp_get_thread_num(), nk3);

        }


    }
}

void t6() {
    const int n = 100;
    int a[n], m1 = 0, m2=0;
    for (int i = 0; i < n; i++)
        a[i] = 4;
#pragma omp parallel for reduction(+:m1) shared(m2)
    for (int i = 0; i < n; i++) {
        m1 += a[i];
        m2 += a[i];
    }
    m1 /= n;
    m2 /= n;
    printf("m1=%d m2=%d\n", m1, m2);

}

void t7() {
    const int n = 12;
    int a[n], b[n], c[n];

#pragma omp parallel num_threads(3)
    {
#pragma omp for  schedule(static, 4)
        for (int i = 0; i < n; i++) {
            a[i] = 4;
            b[i] = 4;
            printf("Threads: %d num_thread: %d a[i]=%d b[i]=%d\n", omp_get_num_threads(), omp_get_thread_num(), a[i], b[i]);

        }

    }

#pragma omp parallel num_threads(4)
    {
#pragma omp for schedule(dynamic, 3)
        for (int i = 0; i < n; i++) {
            c[i] = a[i] + b[i];
            printf("Threads: %d num_thread: %d c[i]=%d \n", omp_get_num_threads(), omp_get_thread_num(),c[i]);

        }
    }
}

void t8() {
    const int n = 16000;
    int* a;
    a = new int[n];
    double* b;
    b = new double[n];
    for (int i = 0; i < n; i++)
        a[i] = i;

    double start = omp_get_wtime();
#pragma omp parallel num_threads(8)
    {
#pragma omp for schedule(dynamic)
        for (int i = 1; i < n-1; i++)
            b[i] = (a[i - 1] + a[i] + a[i + 1]) / 3.0;
    }
    double end = omp_get_wtime();
    double seconds = (end - start);
    printf("The time: %f seconds\n", seconds);
    delete []a;
    delete []b;
    //не ставил порции
    // static 0.001105
    //dynamic 0.001446
    //guided 0.001010
    //runtime 0.001214

    //100
    //0.000935
    //0.001109
    //0.001111
    //

}


void t9() {
    int n = 960;
    int m = n;

    int** a = new int* [n];
    for (int i = 0; i < n; i++)
        a[i] = new int[m];

    int* vect = new int[m];

    //cout << "Автоматически сгенерированная матрица: " << endl;  
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m; j++)
        {
            a[i][j] = rand() % 10;
            //cout << a[i][j] << " ";
        }
        //cout << endl;
    }

    //генерация вектора.
    //cout << "Автоматически сгенерированный вектор: " << endl;
    for (int i = 0; i < m; i++)
    {
        vect[i] = rand() % 10;
        //cout << vect[i] << endl;
    }

    // Главный расчет......
    int* result = new int[n];
    double start = omp_get_wtime();

    #pragma omp parallel
        {
    #pragma omp for schedule(guided, 40)
            for (int i = 0; i < n; i++)
            {
                result[i] = 0;
                for (int j = 0; j < m; j++)
                {
                    result[i] += vect[j] * a[i][j];
                }
            }
        }
        //for (int i = 0; i < n; i++)
        //{
        //    result[i] = 0;
        //    for (int j = 0; j < m; j++)
        //    {
        //        result[i] += vect[j] * a[i][j];
        //    }
        //}

    double end = omp_get_wtime();
    double seconds = (end - start);
    printf("The time: %f seconds\n", seconds);


    //cout << "Ответ: " << endl;
    //for (int i = 0; i < n; i++)
    //{
    //    cout << result[i] << endl;
    //}

    // Удалить память
    for (int i = 0; i < n; i++)
        delete[] a[i];
    delete[] a;

    delete[] vect;
    delete[] result;
    //POSLED
    //15x15 0,000002 0,000006
    //30x30 0,000007 0,000010
    //60x60 0,000019 0,000022
    //120x120 0,000067 0,000059
    //240x240 0,000189 0,000190
    //480x480 0,000320 0,000220
    //960x960 0,002910 
    //1920x1920 0,010754 0,011044
    //3840x3840 0,044021 0,047779

    //PARALLEL STATIC
    //240x240 0,000562
    //480x480 0,000689 0,000890
    // 
    //960x960 0,001803 0,001676
    //1920x1920 0,004121
    //3840x3840 0,017244

    //PARALLEL DYNAMIC
    //960x960
    //none 0,006109
    //20 0,001597
    //40 0,001216 <-------------------------

    //PARALLEL guided
    //960x960
    //none 0,001681
    //20 0,001641
    //40 0,001783

}

void t10() {
    const int m = 6, n = 8;
    int d[m][n];
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            d[i][j] = rand() % 10;
        }
    }
    printf("MAS\n");
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            printf(" %d ", d[i][j]);
        }
        printf("\n");
    }

    int max = d[0][0], min = d[0][0];
#pragma omp parallel num_threads(8)
    {
#pragma omp for 
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                if (max < d[i][j]) {
#pragma omp critical(m1)
                    {
                        if (max < d[i][j])
                            max = d[i][j];

                    }
                }
                if (min > d[i][j]) {
#pragma omp critical(m2)
                    {
                        if (min > d[i][j])
                            min = d[i][j];
                    }
                }
            
            
            
            
            }


        }
    }

    
//#pragma omp parallel num_threads(8)
//    {
//#pragma omp for schedule(static)
//        for (int i = 0; i < m; i++) {
//            for (int j = 0; j < n; j++) {
//                if (max < d[i][j])
//                    max = d[i][j];
//                if (min > d[i][j])
//                    min = d[i][j];
//                printf("THREAD %d min=%d max=%d\n", omp_get_thread_num(), min, max);
//            }
//            //printf("THREAD %d \n", omp_get_thread_num());
//        }
//    }

    printf("THREAD %d min=%d max=%d\n", omp_get_thread_num(), min, max);

}


void t11() {
    const int n = 30;
    int a[n];
    for (int i = 0; i < n; i++){
        a[i] = rand() % 10;
        printf(" %d ", a[i]);
    }
    printf("\n");
    int nk9 = 0;
#pragma omp parallel num_threads(8)
#pragma omp for schedule(static)
    for (int i = 0; i < n; i++) {
        if (a[i] % 9 == 0) {
#pragma omp atomic
            nk9++;
            //printf("THREAD %d a[i]\n", omp_get_thread_num(), a[i]);
        }

    }
    printf("NK9: %d \b", nk9);
}


void ta12() {
    const int n = 300;
    int a[n];
    for (int i = 0; i < n; i++) {
        a[i] = rand() % 10;
        //printf(" %d ", a[i]);
    }
    printf("\n");
    int nk7 = 0;

    omp_lock_t writelock;
    omp_init_lock(&writelock);

#pragma omp parallel num_threads(8)
#pragma omp for schedule(static)
    for (int i = 0; i < n; i++) {
        if (a[i] % 7 == 0) {
            omp_set_lock(&writelock);
            nk7++;
            omp_unset_lock(&writelock);
            //printf("THREAD %d a[i]\n", omp_get_thread_num(), a[i]);
        }

    }
    printf("NK7: %d \b", nk7);
    omp_destroy_lock(&writelock);
}

void t13_1() {
    int N = 8;
#pragma omp parallel num_threads(N) 
    {
        for (int i = 0; i < (N - omp_get_thread_num()) * 1e8; i++) {}
        printf("Threads: %d num_thread: %d %s\n", omp_get_num_threads(), omp_get_thread_num(), "Hello world!");
    }
}
void t13_2() {
    int N = 8;
#pragma omp parallel num_threads(N) 
    {
        for (int i = 7; i>=0 ; i--) {
#pragma omp barrier
            if(i == omp_get_thread_num())
                printf("Threads: %d num_thread: %d %s\n", omp_get_num_threads(), omp_get_thread_num(), "Hello world!");

        }
    }
}

void t13_3() {
    int N = 8;
    int i = 7;
#pragma omp parallel num_threads(N) 
    {
        while (i >= 0) {
#pragma omp critical
            {
                if (i == omp_get_thread_num()) {
                    printf("Threads: %d num_thread: %d %s\n", omp_get_num_threads(), omp_get_thread_num(), "Hello world!");
                    i--;
                }
            }
        }
    }
    
}

void t13_4() {
    int N = 8;
#pragma omp parallel num_threads(N) 
    {
#pragma omp  for ordered
        for (int i = 7; i >= 0; i--)
        {
        printf("Threads: %d num_thread: %d %s\n", omp_get_num_threads(), omp_get_thread_num(), "Hello world!");
        }
    }

}

int main() {
    setlocale(LC_ALL, "Ru");
    /*srand(time(NULL));*/
    //printf("-------------------\n");
    //t1();
    //printf("-------------------\n");
    //t2();
    //printf("-------------------\n");
    //t3();
    //printf("-------------------\n");
    //t4();
    //printf("-------------------\n");
    //t5();
    //printf("-------------------\n");
    //t6();
    //printf("-------------------\n");
    //t7();
    //printf("-------------------\n");
    //t8();
    //printf("-------------------\n");
    // 
    // 
    //t9();
    printf("-------------------\n");
    t10();
    printf("-------------------\n");
    //t11();
    //printf("-------------------\n");
    //ta12();
    //printf("-------------------\n");
    //t13_1();
    //printf("-------------------\n");
    //t13_2();
    //printf("-------------------\n");
    //t13_3();
    //printf("-------------------\n");
	return 0;
}