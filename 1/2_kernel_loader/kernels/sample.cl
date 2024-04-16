__kernel void sample_kernel(__global int* buffer, int n)
{
    int value = 11;
    // get_global_id(0) mert 1 dim a probléma, 1 lenne ha 2, 2 is lehetne ha 3 dim
    printf("%d\n", get_global_id(0));
    if (get_global_id(0) < n) {
        buffer[get_global_id(0)] = value;
    }
}