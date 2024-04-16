typedef struct order_s{
    int order[100];
    int n;
    __global int * minimum;
    __global int * maximum;
}order_t;

__kernel void minmax(__global order_t * o) {
    int GID = get_global_id(0);
    if (GID >= o->n) return;
    atomic_min(o->minimum, o->order[GID]);
    atomic_max(o->maximum, o->order[GID]);
    //printf("o->order[%d]: %d\n",GID,o->order[GID]);
    printf("o->min: %d\n",*(o->minimum));
    printf("o->max: %d\n",*(o->maximum));

}