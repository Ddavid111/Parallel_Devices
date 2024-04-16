typedef struct __attribute__ ((packed)) order_s{
    int order[100];
    int n;
}order_t;

__kernel void swap(__global order_t * o)
{
    int GID = get_global_id(0);
    if (GID % 2 == 0){
        int temp = o->order[GID];
        o->order[GID] = o->order[GID+1];
        o->order[GID+1] = temp;
    }
}