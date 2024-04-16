typedef struct __attribute__ ((packed)) order_s{
    int order[100];
    int n;
}order_t;

__kernel void occurance(__global order_t * o)
{
    int GID = get_global_id(0);
    if (GID < o->n){
      int count = 0;
      for(int i=0; i<o->n; i++){
        if (o->order[GID] == o->order[i])
          count++;
      }
      printf("Kernel %d with number %d has %d occurance(s)!\n",GID,o->order[GID],count);
    }
}