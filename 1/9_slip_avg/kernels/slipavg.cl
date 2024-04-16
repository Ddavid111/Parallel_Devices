typedef struct __attribute__ ((packed)) order_s{
    int order[100];
    float avgs[100];
    int n;
    int sugar;
}order_t;

__kernel void slipavg(__global order_t * o)
{
    int GID = get_global_id(0);
    if(GID - (o->sugar) < 0){
      float avg = 0;
      int sum = 0;
      for(int i=0; i<GID+o->sugar+1; i++){
        sum += o->order[i];
      }
      avg = (float)sum / (float)(GID+o->sugar+1);
      o->avgs[GID] = avg;
    } else
    if(GID + (o->sugar) > o->n-1){
      float avg = 0;
      int sum = 0;
      for(int i=o->n-1; i>GID-o->sugar-1; i--){
        sum += o->order[i];
      }
      avg = (float)sum / (float)(100 - GID+2);
      o->avgs[GID] = avg;
    } else {
      float avg = 0;
      int sum = 0;
      for(int i=GID-o->sugar; i<GID+o->sugar+1; i++){
        sum += o->order[i];
      }
      avg = (float)sum / ((float)(2*o->sugar)+1.0);
      o->avgs[GID] = avg;
    }
}