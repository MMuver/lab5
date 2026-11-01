#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

void generate_adj_matrix(int** adj, int n, int is_directed, int prob)
{
    int loop_prob = prob * 5;
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
            adj[i][j] = 0;
    }

    if (!is_directed)
    {
        for (int i = 0; i < n; i++)
        {
            adj[i][i] = (rand() % 100 < loop_prob) ? 1 : 0;
            for (int j = i + 1; j < n; j++)
            {
                int edge = (rand() % 100 < prob) ? 1 : 0;
                adj[i][j] = edge;
                adj[j][i] = edge;
            }
        }
    }
    else
    {
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                int p = (i == j) ? loop_prob : prob;
                adj[i][j] = (rand() % 100 < p) ? 1 : 0;
            }
        }
    }
}

void print_adj_matrix(int** adj, int n)
{
    printf("\nМатрица смежности:\n     ");
    for (int j = 0; j < n; j++)
        printf("v%-3d", j + 1);
    printf("\n    +");
    for (int j = 0; j < n; j++)
        printf("----");
    printf("\n");

    for (int i = 0; i < n; i++)
    {
        printf("v%-2d | ", i + 1);
        for (int j = 0; j < n; j++)
            printf("%-4d", adj[i][j]);
        printf("\n");
    }
}

int get_graph_size_adj(int** adj, int n, int is_directed)
{
    int m = 0;
    if (!is_directed)
    {
        for (int i = 0; i < n; i++)
        {
            for (int j = i; j < n; j++)
            {
                if (adj[i][j])
                    m++;
            }
        }
    }
    else
    {
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                if (adj[i][j])
                    m++;
            }
        }
    }
    return m;
}

void build_inc_matrix(int** adj, int n, int m, int is_directed, int** inc)
{
    int edge_idx = 0;
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m; j++)
            inc[i][j] = 0;
    }

    if (!is_directed)
    {
        for (int i = 0; i < n; i++)
        {
            if (adj[i][i] && edge_idx < m)
                inc[i][edge_idx++] = 2;
            for (int j = i + 1; j < n; j++)
            {
                if (adj[i][j] && edge_idx < m)
                {
                    inc[i][edge_idx] = 1;
                    inc[j][edge_idx++] = 1;
                }
            }
        }
    }
    else
    {
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                if (adj[i][j] && edge_idx < m)
                {
                    if (i == j)
                    {
                        inc[i][edge_idx++] = 2;
                    }
                    else
                    {
                        inc[i][edge_idx] = -1;
                        inc[j][edge_idx++] = 1;
                    }
                }
            }
        }
    }
}

void print_inc_matrix(int** inc, int n, int m)
{
    printf("\nМатрица инцидентности:\n     ");
    for (int j = 0; j < m; j++)
        printf("e%-3d", j + 1);
    printf("\n    +");
    for (int j = 0; j < m; j++)
        printf("----");
    printf("\n");

    for (int i = 0; i < n; i++)
    {
        printf("v%-2d | ", i + 1);
        for (int j = 0; j < m; j++)
            printf("%-4d", inc[i][j]);
        printf("\n");
    }
}

void print_analysis_results(int* deg, int* is_iso, int* is_pend, int* is_dom, int n)
{
    printf("\nХарактеристики вершин:\n");
    for (int i = 0; i < n; i++)
    {
        printf("Вершина v%d: deg = %d", i + 1, deg[i]);
        if (is_iso[i])
            printf(" -> [ИЗОЛИРОВАННАЯ]");
        if (is_pend[i])
            printf(" -> [КОНЦЕВАЯ]");
        if (is_dom[i])
            printf(" -> [ДОМИНИРУЮЩАЯ]");
        printf("\n");
    }

    printf("\nСводка особых вершин:\n");
    printf("- Изолированные вершины: ");
    int found = 0;
    for (int i = 0; i < n; i++)
    {
        if (is_iso[i])
        {
            printf("v%d ", i + 1);
            found = 1;
        }
    }
    if (!found) printf("нет");
    printf("\n");

    printf("- Концевые вершины: ");
    found = 0;
    for (int i = 0; i < n; i++)
    {
        if (is_pend[i])
        {
            printf("v%d ", i + 1);
            found = 1;
        }
    }
    if (!found) printf("нет");
    printf("\n");

    printf("- Доминирующие вершины: ");
    found = 0;
    for (int i = 0; i < n; i++)
    {
        if (is_dom[i])
        {
            printf("v%d ", i + 1);
            found = 1;
        }
    }
    if (!found) printf("нет");
    printf("\n");
}

void analyze_adj(int** adj, int n, int is_directed)
{
    int m = get_graph_size_adj(adj, n, is_directed);
    printf("\nРазмер графа: %d\n", m);

    int* deg = (int*)calloc(n, sizeof(int));
    int* is_iso = (int*)calloc(n, sizeof(int));
    int* is_pend = (int*)calloc(n, sizeof(int));
    int* is_dom = (int*)calloc(n, sizeof(int));

    for (int i = 0; i < n; i++)
    {
        int deg_others = 0;
        int connected_others = 0;
        int has_loop = adj[i][i];

        for (int j = 0; j < n; j++)
        {
            if (i == j) continue;

            if (!is_directed)
            {
                if (adj[i][j])
                {
                    deg_others++;
                    connected_others++;
                }
            }
            else
            {
                if (adj[i][j]) deg_others++;
                if (adj[j][i]) deg_others++;
                if (adj[i][j] || adj[j][i]) connected_others++;
            }
        }

        deg[i] = deg_others + (is_directed ? 2 * has_loop : has_loop);
        is_iso[i] = (deg_others == 0);
        is_pend[i] = (deg_others == 1 && !has_loop);
        is_dom[i] = (connected_others == n - 1 && n > 1);
    }

    print_analysis_results(deg, is_iso, is_pend, is_dom, n);

    free(deg);
    free(is_iso);
    free(is_pend);
    free(is_dom);
}

void analyze_inc(int** inc, int n, int m, int is_directed)
{
    int total_sum = 0;
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m; j++)
            total_sum += (inc[i][j] < 0) ? -inc[i][j] : inc[i][j];
    }
    printf("\nРазмер графа: %d\n", total_sum / 2);

    int* deg = (int*)calloc(n, sizeof(int));
    int* is_iso = (int*)calloc(n, sizeof(int));
    int* is_pend = (int*)calloc(n, sizeof(int));
    int* is_dom = (int*)calloc(n, sizeof(int));

    for (int i = 0; i < n; i++)
    {
        int deg_others = 0;
        int has_loop = 0;
        int* connected = (int*)calloc(n, sizeof(int));

        for (int j = 0; j < m; j++)
        {
            if (inc[i][j] == 2)
            {
                has_loop = 1;
            }
            else if (inc[i][j] != 0)
            {
                deg_others++;
                for (int other = 0; other < n; other++)
                {
                    if (other != i && inc[other][j] != 0)
                        connected[other] = 1;
                }
            }
        }

        int conn_count = 0;
        for (int other = 0; other < n; other++)
        {
            if (other != i && connected[other])
                conn_count++;
        }
        free(connected);

        deg[i] = deg_others + (is_directed ? 2 * has_loop : has_loop);
        is_iso[i] = (deg_others == 0);
        is_pend[i] = (deg_others == 1 && !has_loop);
        is_dom[i] = (conn_count == n - 1 && n > 1);
    }

    print_analysis_results(deg, is_iso, is_pend, is_dom, n);

    free(deg);
    free(is_iso);
    free(is_pend);
    free(is_dom);
}

int main(void)
{
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    srand((unsigned int)time(NULL));

    int n = 0;
    int is_directed = 0;
    int prob = 50;
    int** adj = NULL;

    while (1)
    {
        int op = 0;
        printf("\nМеню:\n");
        printf("1 - Построить неориентированный граф\n");
        printf("2 - Построить ориентированный граф\n");
        printf("3 - Анализ по матрице смежности\n");
        printf("4 - Анализ по матрице инцидентности\n");
        printf("0 - Выход из программы\n");
        printf("Ваш выбор: ");
        if (scanf("%d", &op) != 1 || op == 0)
            break;

        if (op == 1 || op == 2)
        {
            if (adj != NULL)
            {
                for (int i = 0; i < n; i++)
                    free(adj[i]);
                free(adj);
                adj = NULL;
            }

            is_directed = (op == 2) ? 1 : 0;

            printf("Введите количество вершин: ");
            if (scanf("%d", &n) != 1 || n < 2)
            {
                printf("Некорректный размер!\n");
                n = 0;
                continue;
            }

            printf("Введите вероятность ребра: ");
            if (scanf("%d", &prob) != 1 || prob < 0 || prob > 100)
            {
                if (prob < 0) prob = 0;
                if (prob > 100) prob = 100;
            }

            adj = (int**)malloc(n * sizeof(int*));
            for (int i = 0; i < n; i++)
                adj[i] = (int*)malloc(n * sizeof(int));

            generate_adj_matrix(adj, n, is_directed, prob);
            print_adj_matrix(adj, n);
        }
        else if (op == 3)
        {
            if (adj == NULL)
            {
                printf("Сначала сгенерируйте граф!\n");
                continue;
            }
            analyze_adj(adj, n, is_directed);
        }
        else if (op == 4)
        {
            if (adj == NULL)
            {
                printf("Сначала сгенерируйте граф!\n");
                continue;
            }

            int m = get_graph_size_adj(adj, n, is_directed);
            if (m == 0)
            {
                printf("Граф не содержит связей.\n");
                continue;
            }

            int** inc = (int**)malloc(n * sizeof(int*));
            for (int i = 0; i < n; i++)
                inc[i] = (int*)malloc(m * sizeof(int));

            build_inc_matrix(adj, n, m, is_directed, inc);
            print_inc_matrix(inc, n, m);
            analyze_inc(inc, n, m, is_directed);

            for (int i = 0; i < n; i++)
                free(inc[i]);
            free(inc);
        }
        else
        {
            printf("Неверный пункт меню!\n");
        }
    }

    if (adj != NULL)
    {
        for (int i = 0; i < n; i++)
            free(adj[i]);
        free(adj);
    }

    return 0;
}
