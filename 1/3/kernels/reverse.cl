typedef struct __attribute__ ((packed)) order_s{
    int order[100];
    int n;
}order_t;

__kernel void reverse(__global order_t * o)
{
    int GID = get_global_id(0);
    if (GID < o->n){
        o->order[GID] = o->order[100-(GID+1)];
    }
}