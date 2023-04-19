#include <stdio.h>
#include <math.h>

int main()
{
    int max_dim = 3;
    printf("Number of nodes and edges in hypercube topology up to %d dimensions:\n", max_dim);
    printf("Dimension\tNodes\tEdges\n");
    for (int dim = 1; dim <= max_dim; dim++)
    {
        int nodes = pow(2, dim);
        int edges = dim * nodes / 2;
        printf("%d\t%d\t%d\n", dim, nodes, edges);
    }

    int target_dim = 8;
    printf("\nDistribution of distances between nodes in %d-dimensional hypercube:\n", target_dim);
    int num_nodes = pow(2, target_dim);
    int dist[num_nodes];
    for (int i = 0; i < num_nodes; i++)
    {
        dist[i] = 0;
        for (int j = 0; j < target_dim; j++)
        {
            if ((i >> j) & 1)
                dist[i]++;
        }
        printf("%d ", dist[i]);
    }

    printf("\n\nHistogram of distances between nodes in %d-dimensional hypercube:\n", target_dim);
    int max_dist = target_dim;
    int hist[max_dist + 1];

    for (int i = 0; i <= max_dist; i++)
        hist[i] = 0;

    for (int i = 0; i < num_nodes; i++)
        hist[dist[i]]++;

    for (int i = 0; i <= max_dist; i++)
    {
        printf("%d: ", i);
        for (int j = 0; j < hist[i]; j++)
            printf("*");
        printf("\n");
    }

    // Average distance
    max_dim = 20;
    printf("\nAverage distance between nodes in hypercube topology up to %d dimensions:\n", max_dim);
    printf("Dimension\tAverage Distance\n");

    for (int dim = 1; dim <= max_dim; dim++)
    {
        double avg_dist = 0.0;
        num_nodes = pow(2, dim);

        for (int node = 0; node < num_nodes / 2 + 1; ++node)
        {
            avg_dist += hist[node] * node * 2.0 / num_nodes;
        }

        avg_dist -= hist[0] / num_nodes;

        printf("%d\t%f\n", dim, avg_dist);
    }

    // Comparison with planar and cubic lattices
    double planar_avg[3] = {1.41421356237 / 2.0, sqrt(5) / 3.0, sqrt(10) / 4.5};
    double cubic_avg[3] = {sqrt(3) / 2.5, sqrt(6) / 4.5, sqrt(7) / 6};

    printf("\nComparison of average distances with planar and cubic lattices:\n");

    for (int d = 1; d < 4; ++d)
    {
        num_nodes = pow(2, d);

        double avg_dist = 0.0;

        if (d == 1)
            avg_dist = sqrt(hist[1]) / num_nodes * hist[1];
        else
        {
            for (int node = 1; node < num_nodes / 2 + 1; ++node)
            {
                avg_dist += hist[node] * node * 2.0 / num_nodes;
            }
        }

        if (d == 3)
            avg_dist -= hist[3] / num_nodes;

        if (d == 4)
            avg_dist -= hist[4] / num_nodes;

        if (d == 5)
            avg_dist -= hist[5] / num_nodes;

        if (d == 6)
            avg_dist -= hist[6] / num_nodes;

        if (d == 7)
            avg_dist -= hist[7] / num_nodes;

        if (d == 8)
            avg_dist -= hist[8] / num_nodes;

        printf("%dD Hypercube: %f\tPlanar: %f\tCubic: %f\n", d, avg_dist, planar_avg[d - 1], cubic_avg[d - 1]);
    }

    return 0;
}
