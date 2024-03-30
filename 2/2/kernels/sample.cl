typedef struct __attribute__ ((packed)) vector{
    int a[5];
    int b[5];
    int c[5];
    int n;
}Vector;

__kernel void vector_sum(__global Vector * vs)
{
    int GID = get_global_id(0);
    if (GID < vs->n){
        vs->c[GID] = vs->a[GID] + vs->b[GID];
        printf("kernel %d: %d + %d = %d\n",GID,vs->a[GID],vs->b[GID],vs->c[GID]);
    }
}